/*-------------------------------------------------------------------------
�������ƣ�ҡ�ˡ���ť����ش���Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __JOYSTICK_APP_H
#define __JOYSTICK_APP_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "SetADC.h"


/* Exported define ------------------------------------------------------------*/
#define Joystick_Task_PRIO    ( tskIDLE_PRIORITY  + 14 )								//ҡ�˴�������
#define Joystick_Task_STACK   ( 256 )


/* Exported variables ---------------------------------------------------------*/
extern 	xTaskHandle                   Joystick_Task_Handle;


/* Exported function prototypes -----------------------------------------------*/
void Joystick_Task(void * pvParameters);


#endif