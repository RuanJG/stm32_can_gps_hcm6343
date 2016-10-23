#ifndef _REMOTER_SENDER_RF_H
#define _REMOTER_SENDER_RF_H


#include "FreeRTOS.h"
#include "task.h"

#include "driver_xtend900.h"

/*------------------------------------------------------------------------------------------------
������			����										����											ȡֵ��Χ
0 					Hopping_Channel					����ģ���ŵ� (HP) 				0 ~ 9
1 					Modem_VID								����ģ��VID	(ID)					0x11 ~ 0x7fff
2 					Source_Address					���ص�ַ		(MY)					0 ~ 0xffff
3						Destination_Address			Ŀ�ĵ�ַ		(DT)					0 ~ 0xffff   0xffffΪ�㲥��ַ
4						Address_Mask						�����ַ		(MK)					0 ~ 0xffff
5						TX_Power_Level					���书��		(PL)					0 ~ 4    0Ϊ1mW��1Ϊ10mW��2Ϊ100mW��3Ϊ500mW��4Ϊ1W
6						API_Enable							APIģʽ			(AP)					0 ~ 2		0Ϊ��ֹAPI��1Ϊ����escaped��APIģʽ��2Ϊ��escaped��APIģʽ
7						Board_Temperature				����ģ�鵱ǰ�¶� (TP)			0 ~ 0x7f	        
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