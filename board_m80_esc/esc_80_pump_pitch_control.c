#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "esc_box.h"
#include "stm32f10x_tim.h"	



volatile uint16_t pwmA_ccr = MIDDLE_PUMP_PITCH_ANGLE_PWM;//(MAX_PWM_VALUE-MIN_PWM_VALUE)/2 + MIN_PWM_VALUE;
volatile uint16_t pwmB_ccr = MIDDLE_PUMP_PITCH_ANGLE_PWM;//(MAX_PWM_VALUE-MIN_PWM_VALUE)/2 + MIN_PWM_VALUE;


void TIM3_Mode_Config(void)
 {
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʼ��TIM3��ʱ�������λ
    TIM_OCInitTypeDef  TIM_OCInitStructure;//��ʼ��TIM3������
	 GPIO_InitTypeDef GPIO_InitStructure;
	 
	 //clk
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

	 	//pb1 pb0 timer3 pwm // //PWMA ���Ƶ����� ����õ�pitch����, ��Ҫ��������ֵ����ǰ���У��� ;  pwmB no use
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PWMB_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PWMB_GPIO_BANK, &GPIO_InitStructure);	
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = PWMA_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PWMA_GPIO_BANK, &GPIO_InitStructure);
	
	 
   //TIM3��ʱ�������λ����         10khz
   TIM_TimeBaseStructure.TIM_Period = MAX_PUMP_PITCH_PWM_VALUE-1; //       
   TIM_TimeBaseStructure.TIM_Prescaler = systemClk/10000/MAX_PUMP_PITCH_PWM_VALUE-1; // (prescale+1) =72000000/10k/MAX_PWM_VALUE (10khz)            
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	 
   //TIM3��OC���������
   TIM_OCStructInit(&TIM_OCInitStructure);
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM�����ȵ���ģʽ1    
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�����ʱ��֪����stm32�̼�����û���ѵ���Ӧ���Ƕ�ʱ���������ʹ�ܵ���˼        
   TIM_OCInitStructure.TIM_Pulse = pwmA_ccr;//�����˴�װ�벶��ȽϼĴ���������ֵ          
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM����Ƚϼ��Ը�
   TIM_OC3Init(TIM3, &TIM_OCInitStructure);
   TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���

   TIM_OCStructInit(&TIM_OCInitStructure);
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM�����ȵ���ģʽ1  
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = pwmB_ccr;  
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM����Ƚϼ��Ը�	 
   TIM_OC4Init(TIM3, &TIM_OCInitStructure);        
   TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);


   TIM_CtrlPWMOutputs(TIM3,ENABLE);
   TIM_ARRPreloadConfig(TIM3, ENABLE);        //ʹ��TIM3���ؼĴ���ARR                
   TIM_Cmd(TIM3, ENABLE);//ʹ��TIM3              
 }

void Esc_Pump_Pitch_Config()
 {
	 TIM3_Mode_Config();
 }
int esc_set_pwmA(uint16_t pwm)
{
	 if( pwm >= MIN_PUMP_PITCH_PWM_VALUE && pwm <= MAX_PUMP_PITCH_PWM_VALUE ){
		 TIM3->CCR3 = pwm;
		 pwmA_ccr = pwm;
		 return 1;
	 }else{
		 return 0;
	 }
}
int esc_set_pwmB(unsigned short pwm)
{
	 if( pwm >= MIN_PUMP_PITCH_PWM_VALUE && pwm <= MAX_PUMP_PITCH_PWM_VALUE ){
		 TIM3->CCR4 = pwm;
		 pwmB_ccr = pwm;
		 return 1;
	 }else{
		 return 0;
	 }
}

int esc_set_pump_pitch_pwm(uint16_t pwm)
{
	esc_set_pwmA(pwm);
}

void Esc_Pump_Pitch_Back()
{
	esc_set_pump_pitch_pwm(BACK_PUMP_PITCH_ANGLE_PWM);
}
void Esc_Pump_Pitch_Middle()
{
	esc_set_pump_pitch_pwm(MIDDLE_PUMP_PITCH_ANGLE_PWM);
}
void Esc_Pump_Pitch_Forward()
{
	esc_set_pump_pitch_pwm(FORWARD_PUMP_PITCH_ANGLE_PWM);
}