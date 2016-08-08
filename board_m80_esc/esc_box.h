#ifndef _ESC_BOX_H
#define _ESC_BOX_H

#include <stm32f10x.h>	
#include "stdio.h"	
#include "system.h"



//gpio confiuare
void Esc_GPIO_Configuration (void);




//pump pitch push_rod
#define MIN_PUMP_PITCH_PWM_VALUE 0
#define MAX_PUMP_PITCH_PWM_VALUE 400 //200 400 800
#define FORWARD_PUMP_PITCH_ANGLE_PWM 0
#define MIDDLE_PUMP_PITCH_ANGLE_PWM 200
#define BACK_PUMP_PITCH_ANGLE_PWM 400
void Esc_Pump_Pitch_Config();
void Esc_Pump_Pitch_Back();
void Esc_Pump_Pitch_Middle();
void Esc_Pump_Pitch_Forward();
//int esc_set_pump_pitch_pwm(uint16_t pwm);



//adc 
void Esc_ADC_Configuration (void);



//led
void Esc_Led_set_toggle(int id, int _10ms);
void Esc_Led_Event();
void Esc_Led_on(int id);
void Esc_Led_off(int id);
void Esc_Led_Configuration();
#define LED_RED_ID 0
#define LED_GREEN_ID 1
#define LED_YELLOW_ID 2



// H Bridge


//接近开关，检测是否方向推动杆推动泵的roll方向yaw 是否到达最左，最右，中间，的位置
// 采用中断来做处理
#define LIMIT_L_GPIO_BANK GPIOA
#define LIMIT_L_GPIO_PIN GPIO_Pin_0
#define LIMIT_M_GPIO_BANK GPIOA
#define LIMIT_M_GPIO_PIN GPIO_Pin_1
#define LIMIT_R_GPIO_BANK GPIOA
#define LIMIT_R_GPIO_PIN GPIO_Pin_7

//PWMA 控制倒车斗 即喷泵的pitch方向, 主要设置三个值，即前，中，后 ;  pwmB no use
#define PWMA_GPIO_BANK GPIOB
#define PWMA_GPIO_PIN GPIO_Pin_0
#define PWMB_GPIO_BANK GPIOB
#define PWMB_GPIO_PIN GPIO_Pin_1

// rudderen_a rudderen_b no use
#define RUDDEREN_A_GPIO_BANK GPIOB
#define RUDDEREN_A_GPIO_PIN GPIO_Pin_14
#define RUDDEREN_B_GPIO_BANK GPIOB
#define RUDDEREN_B_GPIO_PIN GPIO_Pin_15

//SET1 SET2 no use
#define SET1_GPIO_BANK GPIOB
#define SET1_GPIO_PIN GPIO_Pin_12
#define SET2_GPIO_BANK GPIOB
#define SET2_GPIO_PIN GPIO_Pin_13

//i2c
#define I2C1_CLK_GPIO_BANK GPIOB
#define I2C1_CLK_GPIO_PIN GPIO_Pin_8
#define I2C1_CDA_GPIO_BANK GPIOB
#define I2C1_CDA_GPIO_PIN GPIO_Pin_9

// ADC 油量
#define RDR_FB_GPIO_BANK GPIOA
#define RDR_FB_GPIO_PIN GPIO_Pin_6
// ADC 方向推动杆的推动下，泵的转向角, yaw
#define RDR_FBA_GPIO_BANK GPIOA
#define RDR_FBA_GPIO_PIN GPIO_Pin_5


//**********  H Bridge
	//oc_a oc_b  电流检测 no use
#define OCA_GPIO_BANK GPIOB
#define OCA_GPIO_PIN GPIO_Pin_2
#define OCB_GPIO_BANK GPIOB
#define OCB_GPIO_PIN GPIO_Pin_3
	// IS_A：ADC 转向推杆电流 is_B is no use
#define IS_A_GPIO_BANK GPIOA
#define IS_A_GPIO_PIN GPIO_Pin_4
	// H_BRIDGE Ctrl1 ctrl2 ctrl3 ctrl4 控制方向推动杆，泵喷水方向左右转,yaw
#define H_BRIDGE_A_CTRL3_BANK GPIOB
#define H_BRIDGE_A_CTRL3_PIN GPIO_Pin_4
#define H_BRIDGE_A_CTRL4_BANK GPIOB
#define H_BRIDGE_A_CTRL4_PIN GPIO_Pin_5
#define H_BRIDGE_B_CTRL1_BANK GPIOB
#define H_BRIDGE_B_CTRL1_PIN GPIO_Pin_6
#define H_BRIDGE_B_CTRL2_BANK GPIOB
#define H_BRIDGE_B_CTRL2_PIN GPIO_Pin_7
	//H_BRIDGE  pwmb pwmj  控制方向推动杆 速度
	//now use H bridge A , pwmb is use for uart , so  RUDDEREN_A_GPIO is connect to pwmb 
//#define H_BRIDGE_A_PWMB_GPIO_BANK  GPIOB
//#define H_BRIDGE_A_PWMB_GPIO_PIN  GPIO_Pin_10
#define H_BRIDGE_A_PWMB_GPIO_BANK RUDDEREN_A_GPIO_BANK //GPIOB
#define H_BRIDGE_A_PWMB_GPIO_PIN RUDDEREN_A_GPIO_PIN //GPIO_Pin_10
#define H_BRIDGE_B_PWMJ_GPIO_BANK GPIOA
#define H_BRIDGE_B_PWMJ_GPIO_PIN GPIO_Pin_15
/*
pwmj=1
	       ctrl1 ctrl2 forward back shutdown
          1      1     0      0      1
          0       1    1       0      1
          1      0     0      1      1
          0      0     1       1      0 

ctrl1 = 0 ctrl2 = 1 : forward
ctrl1 = 1 ctrl2 = 0 : back


pwmj=0
	       ctrl1 ctrl2 forward back shutdown
          1      1     1      1      0
          0       1    1       1      0
          1      0     1      1      0
          0      0     1       1      0 

pwmj = 0   shutdown

*/

#endif