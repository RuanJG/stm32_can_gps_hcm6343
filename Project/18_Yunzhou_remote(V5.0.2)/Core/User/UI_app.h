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
#define Graphic_Interface_Task_STACK   ( 2000 )

#define DEBUG_MAIN_DIVISION			10						//������������ʾ��Ƶ         10���� * 50 = 500����	
#define DEBUG_PARAMETER_DIVISION	50					//���Բ�����ʾ��Ƶ         10���� * 50 = 500����
#define ATCOMMAND_DELAY		6										//����AT����ģʽ�ȴ�ʱ��  6 * 500���� = 3��


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Graphic_Interface_Task_Handle;

extern uint8_t interface_index;			//����������1Ϊ������
																		//					2Ϊ�������ý���
																		//					3Ϊ�������ý���


/* Exported function prototypes -----------------------------------------------*/
void Graphic_Interface_Task(void * pvParameters);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);
void debug_mode_main(void);
void debug_mode_parameter(void);
void mdelay(volatile uint32_t timer);


#endif
