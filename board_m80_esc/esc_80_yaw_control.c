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
// need to sure two thing:
// 1,�Ƕ�adc�仯ʱ����Ӧ���ؽǶ�[1000,2000]�Ĺ�ϵ  YAW_ANGLE_ADD_ANGLE_ADC_ADD
// 2,Ҫ�ýǶ�adc�仯���Ƹ˵Ŀ��Ʒ���    YAW_FORWARD_ANGLE_ADC_ADD

#define YAW_FORWARD_ANGLE_ADC_ADD 0
#define YAW_ANGLE_ADD_ANGLE_ADC_ADD 1 //angle is from [1000,2000] left->right

#define _YAW_ANGLE_DIFF_RANGE 100 // ��ǰ�Ƕ�ADC�������ĽǶȵ�ADC ֮��Ĳ�ֵ����
#define _YAW_TIMER_CHECK_MS 20 // each 20ms check angle task
#define _YAW_ADC_UPDATE_CHECK_MIN_HZ 5 // ÿ�� ִ������� angle check task ʱ��adc ���ݵ����ٸ��´���, ������ADC�����￴����Ƶ��



// �Ƹ����Ƕ�ADCֵ��Χ  TODO config
#define YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE 2910 //2800 
#define YAW_DEFAULT_ANGLE_MAX_ADC_VALUE 3300
#define YAW_DEFAULT_ANGLE_MIN_ADC_VALUE 2520 //2300
#define YAW_DEFAULT_ANGLE_FAILSAFE_MAX_ADC_VALUE 3450 //3610 //2869
#define YAW_DEFAULT_ANGLE_FAILSAFE_MIN_ADC_VALUE 2150 //1990 //1241
u16 yawAngleLowPartAdcValueR = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE - YAW_DEFAULT_ANGLE_MIN_ADC_VALUE;
u16 yawAngleHightPartAdcValueR = YAW_DEFAULT_ANGLE_MAX_ADC_VALUE - YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE ;

// ��ǰ�ĽǶ�adc ֵ
volatile u16 yawAngleAdcValue = 0;


// ���ط�������,Ҫ��� �Ƕ�

float yawExpectAngle = YAW_DEFAULE_MIDDLE_ANGLE;
u16 yawExpectAngleAdcValue = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE;



// ����������� 16A 0-16A
// test: I = (x-350)/147
// �Ƹ˵���ADCֵ���Χ
#define YAW_CURRENT_MAX_ADC_VALUE 2209 //2702 
// ��ǰ����adc ֵ 
volatile u16 yawCurrentAdcValue = 0;
volatile u16 yawCurrentMaxFailSafeAdcValue = 0;

#define OIL_MASS_MIN_ADC_VALUE 1147
#define OIL_MASS_MAX_ADC_VALUE 2869

// ��ʼ��ʱ���Լ�
int yawControlInited = 0; 
systick_time_t _yaw_t;


// ���� failsafe ʱ��ʱ��
uint32_t _failsafe_time_ms = 0;




#define YAW_USE_PWM 1

//(prescale+1) =72000000/YAW_PWM_RATE/YAW_PWM_MAX_VALUE
#define YAW_PWM_MAX_VALUE 2000
#define YAW_PWM_MIN_VALUE 0
#define YAW_PWM_LIMT_MAX_VALUE 2000
#define YAW_PWM_DEFAULT_VALUE 1800
#define YAW_PWM_RATE 18000  //18k

volatile uint16_t yaw_pwm = YAW_PWM_DEFAULT_VALUE;
void TIM1_Pwm_Config(void)
 {
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʼ��TIM3��ʱ�������λ
    TIM_OCInitTypeDef  TIM_OCInitStructure;//��ʼ��TIM3������
	 GPIO_InitTypeDef GPIO_InitStructure;
	 
	 //clk
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	 	//pb1 pb0 timer3 pwm // //PWMA ���Ƶ����� ����õ�pitch����, ��Ҫ��������ֵ����ǰ���У��� ;  pwmB no use
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_PWMB_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_PWMB_GPIO_BANK, &GPIO_InitStructure);	
	
	 
   //TIM1��ʱ�������λ����     
   TIM_TimeBaseStructure.TIM_Period = YAW_PWM_MAX_VALUE-1; //       
   TIM_TimeBaseStructure.TIM_Prescaler = systemClk/YAW_PWM_RATE/YAW_PWM_MAX_VALUE-1; // (prescale+1) =72000000/10k/MAX_PWM_VALUE (10khz)            
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
   TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	 
	 
   //TIM1��OC���������
   TIM_OCStructInit(&TIM_OCInitStructure);
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM�����ȵ���ģʽ1    
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable ;//TIM_OutputState_Enable;//�����ʱ��֪����stm32�̼�����û���ѵ���Ӧ���Ƕ�ʱ���������ʹ�ܵ���˼    
	 TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//�����ʱ��֪����stm32�̼�����û���ѵ���Ӧ���Ƕ�ʱ���������ʹ�ܵ���˼ 	 
   TIM_OCInitStructure.TIM_Pulse = YAW_PWM_MIN_VALUE;//�����˴�װ�벶��ȽϼĴ���������ֵ          
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //TIM����Ƚϼ��Ը�
   TIM_OC2Init(TIM1, &TIM_OCInitStructure);
   TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
		


   TIM_CtrlPWMOutputs(TIM1,ENABLE);
   TIM_ARRPreloadConfig(TIM1, ENABLE);        //ʹ��TIM3���ؼĴ���ARR                
   TIM_Cmd(TIM1, ENABLE);//ʹ��TIM3              
 }

void _yaw_change_pwm(uint16_t pwm)
{
	if( pwm > YAW_PWM_LIMT_MAX_VALUE )
		TIM1->CCR2 = YAW_PWM_LIMT_MAX_VALUE;
	else
		TIM1->CCR2 = pwm;
}
void yaw_set_yaw_pwm(uint16_t pwm)
{
	if( pwm > YAW_PWM_LIMT_MAX_VALUE )
		yaw_pwm = YAW_PWM_LIMT_MAX_VALUE;
	else
		yaw_pwm = pwm;
	
	_yaw_change_pwm(yaw_pwm);
}


#define _YAW_ANGLE_ADC_ADD_DIRECTION 1
#define _YAW_ANGLE_ADC_REDUCE_DIRECTION 2
#define _YAW_ANGLE_ADC_STOP_DIRECTION 3 //may be stop and gpio not init
volatile char _yaw_angle_direction = _YAW_ANGLE_ADC_STOP_DIRECTION;

void _yaw_control_shutdown() 
{
	_yaw_change_pwm(YAW_PWM_MIN_VALUE);
	GPIOB->BSRR = H_BRIDGE_A_CTRL3_PIN | H_BRIDGE_A_CTRL4_PIN;
	_yaw_angle_direction = _YAW_ANGLE_ADC_STOP_DIRECTION;
}
void _yaw_control_poweron()
{
	_yaw_change_pwm(yaw_pwm);
}
void _yaw_control_forward()
{
	/*
	GPIOB->BSRR = H_BRIDGE_A_CTRL3_PIN;
	GPIOB->BRR = H_BRIDGE_A_CTRL4_PIN;
	*/
	uint32_t data;
	data = GPIOB->ODR ;
	data = (data & (~H_BRIDGE_A_CTRL3_PIN) );
	data = (data | H_BRIDGE_A_CTRL4_PIN);
	GPIOB->ODR = data;
	
	_yaw_control_poweron();
}
void _yaw_control_back()
{
	uint32_t data;
	data = GPIOB->ODR ;
	data = (data & (~H_BRIDGE_A_CTRL4_PIN) );
	data = (data | H_BRIDGE_A_CTRL3_PIN);
	GPIOB->ODR = data;
	
	_yaw_control_poweron();
}




void Esc_Yaw_Control_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
#if YAW_USE_PWM
	TIM1_Pwm_Config();
#else
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_PWMB_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_PWMB_GPIO_BANK, &GPIO_InitStructure);
	GPIO_SetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN);
#endif
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
	
	//�ȹرգ���ADC�ȶ����ٿ�ʼ�Լ�
	_yaw_control_shutdown();

	//Esc_ADC_Configuration (_esc_yaw_adc_update_event);
	
	//systick_time_start(&_yaw_t,_YAW_TIMER_CHECK_MS); //ÿ10ms����һ�μ��
}