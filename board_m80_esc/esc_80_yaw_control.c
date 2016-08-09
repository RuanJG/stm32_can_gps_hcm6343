#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "esc_box.h"
#include "stm32f10x_tim.h"	


//ǰ���ǣ�adc �� �Ƕ� �Ĺ�ϵ�����Ե�
// arm9 -> angle -> adc_value -> compare and change yaw 
// ���������Ƕȵ���ʵ��Χ��С
// Ϊ�˷�ֹӲ���Ķ������������ұ任Ϊadc value �Ĵ�С�仯 

// �Ƹ����Ƕ�ADCֵ��Χ  TODO config
#define YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE 2048
#define YAW_DEFAULT_ANGLE_MAX_ADC_VALUE 1024
#define YAW_DEFAULT_ANGLE_MIN_ADC_VALUE 3072

// ��ǰ�ĽǶ�adc ֵ
volatile u16 yawAngleAdcValue = 0;

// ���ط�������,Ҫ��� �Ƕ�
#define YAW_DEFAULE_ANGLE  1500 // 1000 - 1500 - 2000
float yawExpectAngle = YAW_DEFAULE_ANGLE;
u16 yawExpectAngleAdcValue = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE;




// �Ƹ˵���ADCֵ���Χ
#define YAW_CURRENT_MAX_ADC_VALUE 3072 //4096 
// ��ǰ����adc ֵ
volatile u16 yawCurrentAdcValue = 0;




#define _yaw_control_shutdown() 	GPIO_ResetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN)
#define _yaw_control_poweron()  GPIO_SetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN);
void _yaw_control_forward()
{
	_yaw_control_shutdown();
	GPIO_SetBits(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN);
	GPIO_ResetBits(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN);
	_yaw_control_poweron();
}
void _yaw_control_back()
{
	_yaw_control_shutdown();
	GPIO_ResetBits(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN);
	GPIO_SetBits(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN);
	_yaw_control_poweron();
}

#define  _yaw_control_reduce_angle() _yaw_control_back()
#define  _yaw_control_add_angle() _yaw_control_forward()


//failsafe ��������Ƕȳ�����Χʱ��Ҫ������
volatile unsigned char failsafe_AngleOverFlow = 0;
volatile unsigned char failsafe_CurrentOverFlow = 0;
void yaw_control_failsafe()
{
	//_yaw_control_shutdown();
}

//call by adc dma irq or loop in main for listen the current and angle 
void _esc_yaw_adc_update_event()
{
	yawCurrentAdcValue = Get_ISA_Adc_value();
	if( yawCurrentAdcValue > YAW_CURRENT_MAX_ADC_VALUE )
	{
		failsafe_CurrentOverFlow = 1; 
		yaw_control_failsafe();
	}
	yawAngleAdcValue = Get_PUMP_ANGLE_Adc_value();
	if( yawAngleAdcValue >= YAW_DEFAULT_ANGLE_MAX_ADC_VALUE  || yawAngleAdcValue <= YAW_DEFAULT_ANGLE_MIN_ADC_VALUE)
	{
		failsafe_AngleOverFlow = 1;
		yaw_control_failsafe();
	}
}





void Esc_Yaw_Control_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_CTRL3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_CTRL3_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_CTRL4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_CTRL4_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_PWMB_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_PWMB_GPIO_BANK, &GPIO_InitStructure);
	
	Esc_ADC_Configuration (_esc_yaw_adc_update_event);
}



void Esc_Yaw_Control_Event()
{
	//init 
	
	//check expect angle ~= angle 
}