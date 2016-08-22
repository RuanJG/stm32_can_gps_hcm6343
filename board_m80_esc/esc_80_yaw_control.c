#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "esc_box.h"
#include "stm32f10x_tim.h"	


//前提是，adc 与 角度 的关系是线性的
// arm9 -> angle -> adc_value -> compare and change yaw 
// 电流决定角度的真实范围大小
// 为了防止硬件改动，将向左向右变换为adc value 的大小变化 
// need to sure two thing:
// 1,角度adc变化时，对应主控角度[1000,2000]的关系  YAW_ANGLE_ADD_ANGLE_ADC_ADD
// 2,要让角度adc变化，推杆的控制方向    YAW_FORWARD_ANGLE_ADC_ADD

#define YAW_FORWARD_ANGLE_ADC_ADD 0
#define YAW_ANGLE_ADD_ANGLE_ADC_ADD 1 //angle is from [1000,2000] left->right

#define _YAW_ANGLE_DIFF_RANGE 100 // 当前角度ADC与期望的角度的ADC 之间的差值区间
#define _YAW_TIMER_CHECK_MS 20 // each 20ms check angle task
#define _YAW_ADC_UPDATE_CHECK_MIN_HZ 5 // 每次 执行上面的 angle check task 时，adc 数据的最少更新次数, 可以在ADC配置里看更新频率



// 推杆最大角度ADC值范围  TODO config
#define YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE 2910 //2800 
#define YAW_DEFAULT_ANGLE_MAX_ADC_VALUE 3300
#define YAW_DEFAULT_ANGLE_MIN_ADC_VALUE 2520 //2300
#define YAW_DEFAULT_ANGLE_FAILSAFE_MAX_ADC_VALUE 3450 //3610 //2869
#define YAW_DEFAULT_ANGLE_FAILSAFE_MIN_ADC_VALUE 2150 //1990 //1241
u16 yawAngleLowPartAdcValueR = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE - YAW_DEFAULT_ANGLE_MIN_ADC_VALUE;
u16 yawAngleHightPartAdcValueR = YAW_DEFAULT_ANGLE_MAX_ADC_VALUE - YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE ;

// 当前的角度adc 值
volatile u16 yawAngleAdcValue = 0;


// 主控发过来的,要求的 角度

float yawExpectAngle = YAW_DEFAULE_MIDDLE_ANGLE;
u16 yawExpectAngleAdcValue = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE;



// 测量电流最大到 16A 0-16A
// test: I = (x-350)/147
// 推杆电流ADC值最大范围
#define YAW_CURRENT_MAX_ADC_VALUE 2209 //2702 
// 当前电流adc 值 
volatile u16 yawCurrentAdcValue = 0;
volatile u16 yawCurrentMaxFailSafeAdcValue = 0;

#define OIL_MASS_MIN_ADC_VALUE 1147
#define OIL_MASS_MAX_ADC_VALUE 2869

// 初始化时，自检
int yawControlInited = 0; 
systick_time_t _yaw_t;


// 出现 failsafe 时的时间
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
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//初始化TIM3的时间基数单位
    TIM_OCInitTypeDef  TIM_OCInitStructure;//初始化TIM3的外设
	 GPIO_InitTypeDef GPIO_InitStructure;
	 
	 //clk
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	 	//pb1 pb0 timer3 pwm // //PWMA 控制倒车斗 即喷泵的pitch方向, 主要设置三个值，即前，中，后 ;  pwmB no use
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_PWMB_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_PWMB_GPIO_BANK, &GPIO_InitStructure);	
	
	 
   //TIM1的时间基数单位设置     
   TIM_TimeBaseStructure.TIM_Period = YAW_PWM_MAX_VALUE-1; //       
   TIM_TimeBaseStructure.TIM_Prescaler = systemClk/YAW_PWM_RATE/YAW_PWM_MAX_VALUE-1; // (prescale+1) =72000000/10k/MAX_PWM_VALUE (10khz)            
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
   TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	 
	 
   //TIM1的OC外设的设置
   TIM_OCStructInit(&TIM_OCInitStructure);
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM脉冲宽度调制模式1    
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable ;//TIM_OutputState_Enable;//这个暂时不知道，stm32固件库里没有搜到。应该是定时器输出声明使能的意思    
	 TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;//这个暂时不知道，stm32固件库里没有搜到。应该是定时器输出声明使能的意思 	 
   TIM_OCInitStructure.TIM_Pulse = YAW_PWM_MIN_VALUE;//设置了待装入捕获比较寄存器的脉冲值          
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //TIM输出比较极性高
   TIM_OC2Init(TIM1, &TIM_OCInitStructure);
   TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR1上的预装载寄存器
		


   TIM_CtrlPWMOutputs(TIM1,ENABLE);
   TIM_ARRPreloadConfig(TIM1, ENABLE);        //使能TIM3重载寄存器ARR                
   TIM_Cmd(TIM1, ENABLE);//使能TIM3              
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
	
	//先关闭，等ADC稳定后再开始自检
	_yaw_control_shutdown();

	//Esc_ADC_Configuration (_esc_yaw_adc_update_event);
	
	//systick_time_start(&_yaw_t,_YAW_TIMER_CHECK_MS); //每10ms运行一次检查
}