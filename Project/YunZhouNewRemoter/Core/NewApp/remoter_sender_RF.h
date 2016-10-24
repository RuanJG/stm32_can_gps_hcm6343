#ifndef _REMOTER_SENDER_RF_H
#define _REMOTER_SENDER_RF_H


#include "FreeRTOS.h"
#include "task.h"

#include "driver_xtend900.h"

/*------------------------------------------------------------------------------------------------
索引号			名称										描述											取值范围
0 					Hopping_Channel					数传模块信道 (HP) 				0 ~ 9
1 					Modem_VID								数传模块VID	(ID)					0x11 ~ 0x7fff
2 					Source_Address					本地地址		(MY)					0 ~ 0xffff
3						Destination_Address			目的地址		(DT)					0 ~ 0xffff   0xffff为广播地址
4						Address_Mask						掩码地址		(MK)					0 ~ 0xffff
5						TX_Power_Level					发射功率		(PL)					0 ~ 4    0为1mW，1为10mW，2为100mW，3为500mW，4为1W
6						API_Enable							API模式			(AP)					0 ~ 2		0为禁止API，1为不带escaped的API模式，2为带escaped的API模式
7						Board_Temperature				数传模块当前温度 (TP)			0 ~ 0x7f	        
------------------------------------------------------------------------------------------------*/


typedef struct _remoter_sender_data_t {
	
	unsigned char updated;
	unsigned char powerLevel;
	xtend900_config_t rf_config;
	
}remoter_sender_data_t;



void remoter_sender_RF_init();
int remoter_sender_RF_putchar(unsigned char c);
void remoter_sender_RF_sendChannel();
void remoter_sender_RF_parase(unsigned char c);
int remoter_sender_RF_sendPackget(unsigned char id, unsigned char *data, int len);

int remoter_sender_get_boat_powerLevel();
void remoter_sender_RF_Task(void * pvParameters);
xtend900_config_t * remoter_sender_get_rf_config();

extern xTaskHandle remoter_sender_RF_Task_Handle;












#endif