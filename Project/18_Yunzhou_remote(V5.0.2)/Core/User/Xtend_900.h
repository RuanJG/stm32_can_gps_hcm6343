/*-------------------------------------------------------------------------
�������ƣ�Xtend900 ����ģ�������������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150608 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __XTEND_900_H
#define __XTEND_900_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "keyboard_app.h"


/* Exported define ------------------------------------------------------------*/
#define Xtend_900_Task_PRIO    ( tskIDLE_PRIORITY  + 20 )				//��ֵԽС���ȼ�Խ��
#define Xtend_900_Task_STACK   ( 256 )

#define PARAMETER_QUANTITY	8						//����ȡ8��������ATHP,ID,MY,DT,MK,PL,AP,TP


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Xtend_900_Task_Handle;

//extern uint8_t RT_buffer[50];								//����������ģʽ�ض����ݻ�����
//extern uint8_t RT_pointer;										//����������ģʽ����ָ��
extern uint32_t Board_Temperature;					//����ģ�鵱ǰ�¶�


/* Exported function prototypes -----------------------------------------------*/
void Xtend_900_Task(void * pvParameters);
void enter_AT_Command(void);
void send_AT_inquiry(void);


#endif
