/*-------------------------------------------------------------------------
�������ƣ���������Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEYBOARD_APP_H
#define __KEYBOARD_APP_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"


/* Exported define ------------------------------------------------------------*/
#define Keyboard_Task_PRIO    ( tskIDLE_PRIORITY  + 14 )								//������ť�ӿ��ز�������
#define Keyboard_Task_STACK   ( 128 )


/* Exported variables ---------------------------------------------------------*/
extern xTaskHandle                   Keyboard_Task_Handle;

//������ť�ӿ������ȫ�ֱ���
extern uint8_t MENU_Button;				//MENU �˵�����0��ʾû�а��£�1��ʾ����
extern uint8_t OK_Button;					//OK ȷ�ϼ���0��ʾû�а��£�1��ʾ����
extern uint8_t CANCEL_Button;			//CANCEL ȡ������0��ʾû�а��£�1��ʾ����
extern uint8_t ALARM_Button;				//ALARM ��������ť�ӿ��أ�0��ʾû�а��£�1��ʾ����
extern uint8_t SAMPLE_Button;			//SAMPLE �������أ�0��ʾû�а��£�1��ʾ����
extern uint8_t MODE_Button;				//MODE ģʽ���أ�0��ʾģʽ0��1��ʾģʽ1��2��ʾģʽ2


/* Exported function prototypes -----------------------------------------------*/
void Keyboard_Task(void * pvParameters);


#endif
