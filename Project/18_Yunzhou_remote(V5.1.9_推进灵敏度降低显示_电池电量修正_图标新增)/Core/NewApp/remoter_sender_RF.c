#include "cmdcoder.h"
#include "remoter_sender_RF.h"
#include "stm32f4xx.h"


#include "m_flash.h"
#include "driver_xtend900.h"



cmdcoder_t encode_packget;
cmdcoder_t decode_packget;
//��ǰ����������״̬������
remoter_sender_data_t _sender_data;
//���ص����ò���
xtend900_config_t _sender_rf_config;
xTaskHandle remoter_sender_RF_Task_Handle;

#define JOSTICK_PACKGET_ID 1
#define HEART_PACKGET_ID 0
#define SET_CONFIG_PACKGET_ID 2
#define ACK_PACKGET_ID 3


// ��ǰ���߳����״̬�� _RF_NORMAL_MODE ����״̬�����Է���ͨ�����ݣ� _RF_SETTING_MODE ����״̬��������ģ���У����ܷ����κ�����
#define _RF_NORMAL_MODE 0
#define _RF_SETTING_MODE 1
volatile char _rf_mode = _RF_NORMAL_MODE; 

/*
cmdcoder Frame : 
{
	[0xff, 0xff, id , len....(�ɱ䳤0-4Byte) , data....(�ɱ䳤), crc ]
	len: ��data��ʵ�ʳ���
	crc: id��len��data ���α������ֽ���ӵĺ�
	id : JOSTICK_PACKGET_ID =1 ��data������ң������ͨ���밴��ֵ�� HEART_PACKGET_ID =0 ������id
	
	data:
	{
		1��ͨ����
		cmdcoder id = 1 , data[]={channel1-16bit,channel2,channle3,channle4,channel5,modebtn-8bit,sampleB-8bit,alarmBtn-8bit}
		channel1 left-jostick-x; [Byte0][Byte1] С��
		channel2 left-jostick-y;
		channel3 right-jostick-x;
		channel4 right-jostick-y;
		channel5 middle-knob 
    modebtn:0-1-2
    sampleBtn: 1-0  1:press key
    alarmbtn: 1-0   1:press key
	
		2��������, �������ط����������������ݣ�������ȡ״̬
		cmdcoder id = 0 , data[]={����-8bit}
		
		byte0 : 0-100 , ����
		
		3, �ŵ��� ���Ӳ����� Ҫ�ط� ack ��
		cmdcoder id = 2 , data[]={ hp8Bit, id_Low8Bit, id_Hight8Bit} 
		hp;  0-9  �ŵ� ;   
		id ģ��ID 0x11-0x7fff
		
		4, ack ��
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

	return xtend900_load_param(&_sender_rf_config);

}

int _remoter_sender_RF_saveParam()
{
	return xtend900_save_param(&_sender_data.rf_config, &_sender_rf_config);
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
	
	_rf_mode = _RF_SETTING_MODE ;
	
	//init _sender_rf_config
	vTaskDelay(300); // wait xtend900 running
	res = _remoter_sender_RF_loadParam();
	if( res == 1 )
	{
		_sender_data.rf_config = _sender_rf_config;
	}
	
	_rf_mode = _RF_NORMAL_MODE ;
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
				if( _sender_data.rf_config.hp != decode_packget.data[0] ){
					_sender_data.rf_config.hp = decode_packget.data[0];
					_sender_data.rf_config.updated = 1;
				}
				tmp = (decode_packget.data[1]|(decode_packget.data[2]<<8));
				if( tmp != _sender_data.rf_config.id ){
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
	remoter_sender_RF_init();
	
	while(1)
	{

		//each 200 ms , send jostick data
		_rf_peroid_send_channel_counter++;
		if( _rf_peroid_send_channel_counter>= 20 ){
			remoter_sender_RF_sendChannel();
			_rf_peroid_send_channel_counter = 0;
		}
		
		
		//check setting�� ��remoter_sender_RF_parase ����յ���������ø��£�����ִ�и���
		if( _sender_data.rf_config.updated == 1 )
		{
			// ack boat
			_remter_sender_RF_send_Ack_packget();
			
			//start set xtend900
			_remoter_sender_RF_saveParam(); //block for setting xten900
			
			//set status
			_sender_data.rf_config.updated = 0;
			
			//restart count , as setting just over
			_rf_peroid_send_channel_counter = 0;
		}

		
		vTaskDelay(10);
	}
}


int remoter_sender_get_boat_powerLevel()
{
	return _sender_data.powerLevel;
}
xtend900_config_t * remoter_sender_get_rf_config()
{
	return &_sender_rf_config;
}

