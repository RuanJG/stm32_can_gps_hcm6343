#include "bsp_buzzer_pwm.h"
#include "stm32f4xx.h"
#include "stdio.h"

static int buzzer_TimerPeriod;




void bsp_buzzer_pwm_init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	int level = 0;  //defalut close buzzer
	buzzer_TimerPeriod = (SystemCoreClock / 916 );
	
	
	//PWM GPIO配置
	//PF8初始化
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	

		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	
	//时基初始化
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInitStructure.TIM_Period = buzzer_TimerPeriod - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = level;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);		

	TIM_Cmd(TIM13, DISABLE);
	TIM_CtrlPWMOutputs(TIM13, DISABLE);
	
}


void bsp_buzzer_pwm_set_level(unsigned int level)
{
	int pwm;
	
	if( level > 100 ) return;
	
	if( level >0 ){
		pwm = buzzer_TimerPeriod*level/100;
		TIM_SetCompare1(TIM13,pwm);
		
		TIM_Cmd(TIM13, ENABLE);
		TIM_CtrlPWMOutputs(TIM13, ENABLE);
		//printf("set buzzer pwm=%d,level=%d\r\n",pwm,level);
	}else{
		TIM_SetCompare1(TIM13,0);
		TIM_Cmd(TIM13, DISABLE);
		TIM_CtrlPWMOutputs(TIM13, DISABLE);
		//printf("set buzzer pwm=0,level=%d\r\n",level);
	}
}

 
void bsp_buzzer_pwm_set_voice(unsigned int freq, unsigned int level)
{
	int pwm;
	
	if( level > 100 ) return;
	
	buzzer_TimerPeriod = 180000000 / freq;
	
	if( level >0 ){
		TIM_Cmd(TIM13, DISABLE);
		TIM_CtrlPWMOutputs(TIM13, DISABLE);
		
		pwm = buzzer_TimerPeriod*level/100;//buzzer_TimerPeriod*level/100;
		TIM_SetAutoreload(TIM13,buzzer_TimerPeriod);
		TIM_SetCompare1(TIM13,pwm);
		
		TIM_Cmd(TIM13, ENABLE);
		TIM_CtrlPWMOutputs(TIM13, ENABLE);
		//printf("set buzzer period=%d,pwm=%d,level=%d\r\n",period,pwm,level);
	}else{
		TIM_SetCompare1(TIM13,0);
		TIM_SetAutoreload(TIM13,0);
		TIM_Cmd(TIM13, DISABLE);
		TIM_CtrlPWMOutputs(TIM13, DISABLE);
		//printf("set buzzer pwm=0,level=%d\r\n",level);
	}
}