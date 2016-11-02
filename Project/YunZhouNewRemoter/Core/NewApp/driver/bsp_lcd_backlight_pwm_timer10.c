#include "bsp_lcd_backlight_pwm_timer10.h"
#include "stm32f4xx.h"
#include "stdio.h"


static int _TimerPeriod;

void bsp_lcd_backlight_pwm_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	int pwm_value = 0;//default close bakclight;
	_TimerPeriod =  SystemCoreClock / 30000  ;
	
	//PF6初始化
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_TIM10);	
	

	
	//定时器10作PF6的PWM输出-------------------------------
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
  //时基初始化
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
  TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInitStructure.TIM_Period = _TimerPeriod - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseInitStructure);

  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = pwm_value;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);
  
  TIM_Cmd(TIM10, ENABLE);
  TIM_CtrlPWMOutputs(TIM10, ENABLE);
}

void bsp_lcd_backlight_pwm_set_level(unsigned int level)
{
	unsigned int value ;
	
	if( level > 100 ) return;
	
	//总量程*30% * level%; 100%的总量程会有电流过大的问题
	value  = (_TimerPeriod*3/10) * level/100; 
	TIM_SetCompare1(TIM10,value);
	//printf("baclight pwm=%d\r\n",value);
}


