/*-------------------------------------------------------------------------
�������ƣ�ң��������Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UI_APP_H
#define __UI_APP_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "bsp.h"
#include "UI_element.h"
#include "keyboard_app.h"
#include "message_app.h"


/* Exported define ------------------------------------------------------------*/
#define Graphic_Interface_Task_PRIO    ( tskIDLE_PRIORITY  + 10 )				//��ֵԽС���ȼ�Խ��
#define Graphic_Interface_Task_STACK   ( 2048 )


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Graphic_Interface_Task_Handle;


/* Exported function prototypes -----------------------------------------------*/
void Graphic_Interface_Task(void * pvParameters);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);


#endif
