#include "cmdcoder.h"
#include "remoter_sender_RF.h"
#include "stm32f4xx.h"


#include "m_flash.h"
#include "driver_xtend900.h"



cmdcoder_t encode_packget;
cmdcoder_t decode_packget;
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
	// return cmdcoder_update_Decode_Byte_Rate(&decode_packget);
	return decode_packget.decode_rate;
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
		cmdcoder id = 1 , data[]={channel1-16bit,channel2,channle3,channle4,channel5,modebtn-8bit,sampleB-8bit,alarmBtn-8bit}
		channel1 left-jostick-x; [Byte0][Byte1] 小端
		channel2 left-jostick-y;
		channel3 right-jostick-x;
		channel4 right-jostick-y;
		channel5 middle-knob 
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
	unsigned char data[16];
	remoter_sender_jostick_t *jostick;
	jostick = remoter_sender_jostick_get_data();
	
	data[0]= jostick->left_x & 0xff;
	data[1]= (jostick->left_x>>8)&0xff;
	
	data[2]= jostick->left_y & 0xff;
	data[3]= (jostick->left_y>>8)&0xff;
	
	data[4]= jostick->right_x & 0xff;
	data[5]= (jostick->right_x>>8)&0xff;
	
	data[6]= jostick->right_y & 0xff;
	data[7]= (jostick->right_y>>8)&0xff;
	
	data[8]= jostick->knob & 0xff;
	data[9]= (jostick->knob>>8)&0xff;
	
	data[10]= jostick->key_mode;
	data[11]= jostick->key_sample;
	data[12]= jostick->key_alarm;
	
	data[13]= jostick->button_menu;
	data[14]= jostick->button_ok;
	data[15]= jostick->button_cancel;
	
	remoter_sender_RF_sendPackget(JOSTICK_PACKGET_ID, data , 16);

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


void remoter_sender_RF_init()
{
	int res;
	
	cmdcoder_init(&encode_packget,JOSTICK_PACKGET_ID, remoter_sender_RF_putchar);
	cmdcoder_init(&decode_packget,JOSTICK_PACKGET_ID, CMD_CODER_CALL_BACK_NULL);
	xtend900_set_reciver_handler(remoter_sender_RF_parase);
	//init _sender_data
	_sender_data.powerLevel = 0;
	_sender_data.updated = 0;
	
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
	
	if( cmdcoder_Parse_byte(&decode_packget,c) ){
		//TODO 
		switch (decode_packget.id)
		{
			case HEART_PACKGET_ID:
					_sender_data.powerLevel = decode_packget.data[0];
				break;
			case SET_CONFIG_PACKGET_ID:
				if( decode_packget.data[0] >= XTEND900_MIN_CHANNEL && decode_packget.data[0] <= XTEND900_MAX_CHANNEL )
				{
					if( _sender_data.rf_config.hp != decode_packget.data[0] )
					{
						_sender_data.rf_config.hp = decode_packget.data[0];
						_sender_data.rf_config.updated = 1;
					}
				}
				
				tmp = (decode_packget.data[1]|(decode_packget.data[2]<<8));
				if( tmp >= XTEND900_MIN_ID && tmp <= XTEND900_MAX_ID && tmp != _sender_data.rf_config.id )
				{
					_sender_data.rf_config.updated = 1;
					_sender_data.rf_config.id = tmp;
				}
				
				break;
			default:
				break;
		}
	}
}

int remoter_sender_RF_sendPackget(unsigned char id, unsigned char *data, int len)
{
	encode_packget.id = id;
	cmdcoder_send_bytes( &encode_packget, data, len);
}


void _remter_sender_RF_send_Ack_packget()
{
	unsigned char ack = 1;
	
	remoter_sender_RF_sendPackget(ACK_PACKGET_ID,&ack,1);
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
			// ack boat
			_remter_sender_RF_send_Ack_packget();
			
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

