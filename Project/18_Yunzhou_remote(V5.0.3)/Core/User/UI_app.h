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
#include "m_flash.h"
#include "local_handle.h"


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
extern uint8_t modify_index;					//�����޸�����

extern GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg1a_memdev, bg0a_memdev;								//��ʾ�洢����	


/* Exported function prototypes -----------------------------------------------*/
void Graphic_Interface_Task(void * pvParameters);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);
void debug_mode_main(void);
void debug_mode_parameter(void);
void raw_message_handling(void);
uint16_t LCD_Test(uint16_t timer);
void LCD_clear(void);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);
void mdelay(volatile uint32_t timer);


#endif
