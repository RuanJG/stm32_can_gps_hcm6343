/******************************************************************************
 * @file:    SetTimer.c
 * @purpose: functions related to timer
 * @version: V1.00
 * @date:    11. Jul 2011
 *----------------------------------------------------------------------------
 ******************************************************************************/

#include "stm32f10x.h"
#include "system.h"	
#include "stm32f10x_tim.h"	

#define	DS_SKIP_ROM	0xCC
#define DS_CONVERT	0x44
#define DS_READ_PAD	0xBE
#define WRITE	1
#define READ	0					 

// declaration of variables

void Timer_Configuration (void)
{	   
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure; 
NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		

	 										 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	
	TIM_BaseInitStructure.TIM_Period = 0;
	TIM_BaseInitStructure.TIM_Prescaler = 7200-1; //10KHz 计数， 0.1ms基准延时
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_BaseInitStructure);
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);	
	TIM_Cmd(TIM1, DISABLE);

	TIM_BaseInitStructure.TIM_Period = 400-1;
	TIM_BaseInitStructure.TIM_Prescaler = 35999-1; //2KHz 计数， 0.5ms基准延时
	
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	
	TIM_Cmd(TIM2, DISABLE);
}

void TIM1_UP_IRQHandler(void){
	if(TIM_GetITStatus(TIM1, TIM_IT_Update)==SET){
		TIM_Cmd(TIM1, DISABLE);
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);
		TIM_Cmd(TIM1, ENABLE);		  	
	}	
}


void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)==SET){
		TIM_Cmd(TIM2, DISABLE);	
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
		TIM_Cmd(TIM2, ENABLE);	
	}
}



/*		 			
TIM_OCInitTypeDef TIM_OCInitStructure;  
//PWM
TIM_BaseInitStructure.TIM_Period = 14200-1;
TIM_BaseInitStructure.TIM_Prescaler = 72-1;
TIM_BaseInitStructure.TIM_ClockDivision = 0;
TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);

TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable; //？？？保留位CCER
//500Y 1000   1500   2000 2500Z
TIM_OCInitStructure.TIM_Pulse =0 ;//占空比           
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    //OC1低电平有效　CCER
TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;    //？？？保留位CCER
TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;   //???保留位CR2 
TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset; //CR2

TIM_OC3Init(TIM2, &TIM_OCInitStructure);	
TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); 
TIM_OC4Init(TIM2, &TIM_OCInitStructure);	
TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 								
TIM_CtrlPWMOutputs(TIM2, ENABLE);
TIM_Cmd(TIM2, ENABLE); 	 			 								  
*/

//End of File

