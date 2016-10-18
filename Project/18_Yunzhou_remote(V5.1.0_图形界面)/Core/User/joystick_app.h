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
#include "local_handle.h"
#include "keyboard_app.h"
#include <math.h>


/* Exported define ------------------------------------------------------------*/
#define Joystick_Task_PRIO    ( tskIDLE_PRIORITY  + 14 )								//ҡ�˴�������
#define Joystick_Task_STACK   ( 256 )

#define MIN_KNOB_VALUE 100//20140523:1500,ԭ��1000,����1700
#define MAX_KNOB_VALUE 4000

#define FORWARD_NUM		2200			//ǰ����ֵ
#define BACKWARD_NUM	1800			//������ֵ
#define	MIDDLE_NUM		2000			//��ֵλ��

#define DEFAULT_MIDDLE_VALUE 2028					//ҡ����λ

#define CONTROLLER_TYPE_1		0X01 	//ֻ�е�ҡ��
#define CONTROLLER_TYPE_2		0X02	//˫ҡ��+��ť


/* Exported variables ---------------------------------------------------------*/
extern 	xTaskHandle                   Joystick_Task_Handle;

extern u16 RightRudderCommand;				//�Ҷ�����ӳ��
extern u16 RightSpeedCommand;				//���ƽ��ٶ�ӳ��
extern u16 LeftRudderCommand;				//�������ӳ��
extern u16 LeftSpeedCommand;					//���ƽ��ٶ�ӳ��

//��ť��ֵӳ��
extern u16 KnobValue;									//��ť������Ĵ���ֵ	---  ��ˮ
extern u16 SpeedFactor;								//��ť������Ĵ���ֵ	---	 �ٶ����ӻ�����

extern u8 ControllerType;							//ҡ��ģʽ���Ƿ������ť�ж�


/* Exported function prototypes -----------------------------------------------*/
void Joystick_Task(void * pvParameters);
uint16_t LowPassFilter(uint16_t buffer[ADC_BUFFER_SIZE][6], uint16_t length, uint8_t subpara);
uint16_t FindMax(uint16_t * buffer, uint16_t length);
void CountKnobValue(void);
void count_angle_speed_right(void);
void count_angle_speed_left(void);


#endif
