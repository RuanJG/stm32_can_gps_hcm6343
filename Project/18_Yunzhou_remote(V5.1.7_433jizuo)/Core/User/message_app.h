/*-------------------------------------------------------------------------
�������ƣ���Ϣ���շ��ʹ�������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MESSAGE_APP_H
#define __MESSAGE_APP_H


/* Includes ------------------------------------------------------------------*/
#include "SetUART.h"
#include "global_includes.h"


/* Exported define ------------------------------------------------------------*/
#define Message_Task_PRIO    ( tskIDLE_PRIORITY  + 12 )								//��Ϣ���շ��ʹ�������
#define Message_Task_STACK   ( 128 )


/* Exported variables ---------------------------------------------------------*/
extern xTaskHandle                   Message_Task_Handle;

extern char show_message[52];			//��ʾ���յ���Ϣ


/* Exported function prototypes -----------------------------------------------*/
void Message_Task(void * pvParameters);

#endif