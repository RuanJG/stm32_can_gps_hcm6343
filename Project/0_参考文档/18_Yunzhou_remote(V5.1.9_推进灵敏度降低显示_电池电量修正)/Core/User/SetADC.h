/*-------------------------------------------------------------------------
�������ƣ�ң����ƽ̨ADC���������ĵ�
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150526 ������    5.0.0		�½���
																		PC0 CH10	Bat_ADC 	��ص�ѹ����
																		PC1 CH11	RCKLY_ADC 	��ҡ��X��
																		PC2 CH12	Wheel_ADC		��λ����ť
																		PC3 CH13	RCKLX_ADC		��ҡ��Y��
																		PC4	CH14	RCKRY_ADC		��ҡ��X��
																		PC5	CH15	RCKRX_ADC		��ҡ��Y��
					
					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SETADC_H
#define __SETADC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//#define ADC1_DR_Address    ((u32)0x4001204C)
#define ADC_BUFFER_SIZE 50	//300, 200, 100


/* Public variables ---------------------------------------------------------*/
extern uint16_t ADCValue[ADC_BUFFER_SIZE][6];
extern uint16_t Battery_Power_Avg;									//��ص�ѹ��ֵ
extern uint16_t Left_Joystick_X_axis_Avg;						//���ҡ�� X �������ֵ
extern uint16_t Left_Joystick_Y_axis_Avg;						//���ҡ�� Y �������ֵ
extern uint16_t Right_Joystick_X_axis_Avg;					//�Ҳ�ҡ�� X �������ֵ
extern uint16_t Right_Joystick_Y_axis_Avg;					//�Ҳ�ҡ�� Y �������ֵ
extern uint16_t Knob_Avg;														//��λ����ť��ֵ
extern int8_t Battery_Power_Percent;							//��ص�������ٷֱ���ʽ


/*-------------------------------------------------------------------------
	��������ADC1_CH6_DMA_Config
	��  �ܣ���ʼ����ADC��
						PC0 CH10	Bat_ADC 	��ص�ѹ����
						PC1 CH11	RCKLY_ADC 	��ҡ��X��
						PC2 CH12	Wheel_ADC		��λ����ť
						PC3 CH13	RCKLX_ADC		��ҡ��Y��
						PC4	CH14	RCKRY_ADC		��ҡ��X��
						PC5	CH15	RCKRX_ADC		��ҡ��Y��
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void ADC1_CH6_DMA_Config(void);


#endif











