#include "cmdcoder.h"
#include "remoter_sender_RF.h"
#include "stm32f4xx.h"


#include "m_flash.h"
#include "driver_xtend900.h"
#include "bsp_xtend900_uart.h"
#include "bsp_xtend900_rssi_timer3.h"
#include "protocol.h"

#define USE_CMDCODER 0

#if USE_CMDCODER
cmdcoder_t encode_packget;
cmdcoder_t decode_packget;
#else
protocol_t encoder;
protocol_t decoder;

#endif

//当前船发过来的状态与数据
remoter_sender_data_t _sender_data;
//本地的配置参数
xtend900_config_t _sender_rf_config;
xTaskHandle remoter_sender_RF_Task_Handle;

#define JOSTICK_PACKGET_ID 1
#define HEART_PACKGET_ID 0
#define SET_CONFIG_PACKGET_ID 2
#define ACK_PACKGET_ID 3



volatile char _rf_mode = _RF_NORMAL_MODE; 
volatile char _rf_status = 0;

//在UI上获到状态并显示给用户看
char remoter_sender_RF_getMode()
{
	return _rf_mode;
}

char remoter_sender_RF_getStatus()
{
	return _rf_status;
}

int remoter_sender_RF_getDecodeRate()
{
#if USE_CMDCODER
	// return cmdcoder_update_Decode_Byte_Rate(&decode_packget);
	return decode_packget.decode_rate;
#else
	return decoder.decode_rate;
#endif
}

/*
cmdcoder Frame : 
{
	[0xff, 0xff, id , len....(可变长0-4Byte) , data....(可变长), crc ]
	len: 是data的实际长度
	crc: id和len和data 区段编码后的字节相加的和
	id : JOSTICK_PACKGET_ID =1 ，data数据是遥控器的通道与按键值； HEART_PACKGET_ID =0 心跳包id
	
	data:
	{
		1，通道包
		cmdcoder id = 1 , data[]={channel1(16bit),channel2,channle3,channle4,channel5,modebtn(8bit),sampleB-8bit,alarmBtn-8bit}
		channel1 left-jostick-x; [Byte0][Byte1] 小端, 向从右到左摇摇杆，值从0-4095
		channel2 left-jostick-y;[Byte0][Byte1] 小端, 向从下到上摇摇杆，值从0-4095
		channel3 right-jostick-x;[Byte0][Byte1] 小端, 向从右到左摇摇杆，值从0-4095
		channel4 right-jostick-y;[Byte0][Byte1] 小端, 向从下到上摇摇杆，值从0-4095
		channel5 middle-knob 顺时针方向，值从0-4095
    modebtn:0-1-2
    sampleBtn: 1-0  1:press key
    alarmbtn: 1-0   1:press key
	
		2，心跳包, 监听主控发过来的心跳包数据，用来获取状态
		cmdcoder id = 0 , data[]={电量-8bit}
		
		byte0 : 0-100 , 电量
		
		3, 信道， 连接参数， 要回发 ack 包
		cmdcoder id = 2 , data[]={ hp8Bit, id_Low8Bit, id_Hight8Bit} 
		hp;  0-9  信道 ;   
		id 模块ID 0x11-0x7fff
		
		4, ack 包
		cmdcoder id = 3 , data[]={ ack-1Byte } 
		ack=0 : false  ;   ack=1 ok 
		
	}
}


*/




///*****************************  port 

int remoter_sender_RF_putchar(unsigned char c)
{
	xtend900_putchar(c);
	
	return 1;
}
void remoter_sender_RF_sendChannel()
{
	unsigned char i;
	unsigned char data[24];
	remoter_sender_jostick_t *jostick;
	jostick = remoter_sender_jostick_get_data();
	
	i=0;
	
#if !USE_CMDCODER
	data[i++] = JOSTICK_PACKGET_ID;
#endif
	
	data[i++]= jostick->left_x & 0xff;
	data[i++]= (jostick->left_x>>8)&0xff;
	
	data[i++]= jostick->left_y & 0xff;
	data[i++]= (jostick->left_y>>8)&0xff;
	
	data[i++]= jostick->right_x & 0xff;
	data[i++]= (jostick->right_x>>8)&0xff;
	
	data[i++]= jostick->right_y & 0xff;
	data[i++]= (jostick->right_y>>8)&0xff;
	
	data[i++]= jostick->knob & 0xff;
	data[i++]= (jostick->knob>>8)&0xff;
	
	data[i++]= jostick->key_mode;
	data[i++]= jostick->key_sample;
	data[i++]= jostick->key_alarm;
	
	data[i++]= jostick->button_menu;
	data[i++]= jostick->button_ok;
	data[i++]= jostick->button_cancel;

#if USE_CMDCODER
	remoter_sender_RF_sendPackget(JOSTICK_PACKGET_ID, data , i);
#else
	if( 0 == protocol_encode(&encoder,data,i) )
	{
		printf("remoter_sender_RF_sendChannel: protocol encoder error\r\n");
		return;
	}
	for( i=0; i< encoder.len; i++)
		remoter_sender_RF_putchar(encoder.data[i]);
#endif


}

int _remoter_sender_RF_loadParam()
{
	int res;
	
	_rf_mode = _RF_LOADING_MODE ;
	res = xtend900_load_param(&_sender_rf_config);
	if( res == 0 ){
		_rf_status = _RF_STATUS_LOAD_PARAM_ERROR;
	}else
		_rf_status = _RF_STATUS_OK;
	_rf_mode = _RF_NORMAL_MODE ;
	
	return res;
}

int _remoter_sender_RF_saveParam()
{
	int res;
	
	_rf_mode = _RF_SETTING_MODE ;
	res = xtend900_save_param(&_sender_data.rf_config, &_sender_rf_config);
	if( res == 0 ){
		_rf_status = _RF_STATUS_SAVE_PARAM_ERROR;
	}else
		_rf_status = _RF_STATUS_OK;
	_rf_mode = _RF_NORMAL_MODE ;
	
	return res;
}

//******************************  command 
void _remter_sender_RF_send_Ack_packget(unsigned char ack);

void remoter_sender_RF_init()
{
	int res;
	
	//init _sender_data
	_sender_data.powerLevel = 0;
	_sender_data.updated = 0;
	
	//init coder 
#if USE_CMDCODER
	cmdcoder_init(&encode_packget,JOSTICK_PACKGET_ID, remoter_sender_RF_putchar);
	cmdcoder_init(&decode_packget,JOSTICK_PACKGET_ID, CMD_CODER_CALL_BACK_NULL);
#else
	protocol_init(&encoder);
	protocol_init(&decoder);
#endif
	xtend900_set_reciver_handler(remoter_sender_RF_parase);
	
	//uart should be last init
	bsp_xtend900_uart_init();
	
	//RSSI信号PWM捕捉初始化
	bsp_xtend900_rssi_timer3_init();
	
	//init _sender_rf_config
	vTaskDelay(300); // wait xtend900 running
	res = _remoter_sender_RF_loadParam();
	if( res == 1 )
	{
		_sender_data.rf_config = _sender_rf_config;
	}
}


void remoter_sender_RF_parase(unsigned char c)
{
	unsigned short tmp;
	unsigned int id;
	unsigned char * data;
	unsigned char ack=1;
	
#if USE_CMDCODER
	if( cmdcoder_Parse_byte(&decode_packget,c) ){
		id = decode_packget.id;
		data = decode_packget.data;
#else
	if( protocol_parse(&decoder,c) ){
		id = decoder.data[0];
		data = &decoder.data[1];
#endif
		//TODO 
		switch (id)
		{
			case HEART_PACKGET_ID:
					_sender_data.powerLevel = data[0];
				break;
			case SET_CONFIG_PACKGET_ID:
				ack = 1;
				if( data[0] >= XTEND900_MIN_CHANNEL && data[0] <= XTEND900_MAX_CHANNEL )
				{
					if( _sender_data.rf_config.hp != data[0] )
					{
						_sender_data.rf_config.hp = data[0];
						_sender_data.rf_config.updated = 1;
					}
				}else ack=0;
				
				tmp = (data[1]|(data[2]<<8));
				if( tmp >= XTEND900_MIN_ID && tmp <= XTEND900_MAX_ID )
				{
					if( tmp != _sender_data.rf_config.id)
					{
						_sender_data.rf_config.updated = 1;
						_sender_data.rf_config.id = tmp;
					}
				}else ack = 0;
				
				if( ack == 0 ) _sender_data.rf_config.updated = 0; // error param , do not update
				
				// error param ack 0;     difference param ack 1;     the same param ack 1  
				_remter_sender_RF_send_Ack_packget(ack);

				break;
			default:
				break;
		}
	}
}

#if USE_CMDCODER
int remoter_sender_RF_sendPackget(unsigned char id, unsigned char *data, int len)
{
	encode_packget.id = id;
	cmdcoder_send_bytes( &encode_packget, data, len);
}
#endif


void _remter_sender_RF_send_Ack_packget(unsigned char ack)
{
	int i;

#if USE_CMDCODER
	remoter_sender_RF_sendPackget(ACK_PACKGET_ID,&ack,1);
#else
	unsigned char data[2]={ACK_PACKGET_ID,0};
	data[1]=ack;
	if( 0 == protocol_encode(&encoder,data,2) )
	{
		printf("_remter_sender_RF_send_Ack_packget: protocol encoder error\r\n");
		return;
	}
	for( i=0; i< encoder.len; i++)
		remoter_sender_RF_putchar(encoder.data[i]);
#endif
}



volatile int _rf_peroid_send_channel_counter = 0;

void remoter_sender_RF_Task(void * pvParameters)
{
	int res;
	
	remoter_sender_RF_init();
	
	while(1)
	{

		//each 200 ms , send jostick data
		_rf_peroid_send_channel_counter++;
		if( _rf_peroid_send_channel_counter>= 20 ){
			remoter_sender_RF_sendChannel();
			_rf_peroid_send_channel_counter = 0;
		}
		
		
		//check setting： 在remoter_sender_RF_parase 里接收到命令后，设置更新，这里执行更新
		if( _sender_data.rf_config.updated == 1 )
		{
			
			//start set xtend900
			res = _remoter_sender_RF_saveParam(); //block for setting xten900
			if( res != 1 )
			{
				//printf("set config false\n");
				if( _sender_data.rf_config.id != _sender_rf_config.id || _sender_data.rf_config.hp != _sender_rf_config.hp)
				{
					printf("set config false \n");
				}
			}
			
			//set status
			_sender_data.rf_config.updated = 0;
			
			//restart count , as setting just over
			_rf_peroid_send_channel_counter = 0;
		}

		
		vTaskDelay(10);
	}
}


int remoter_sender_RF_get_boat_powerLevel()
{
	return _sender_data.powerLevel;
}



xtend900_config_t * remoter_sender_RF_get_config()
{
	return &_sender_rf_config;
}

