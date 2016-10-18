/*-------------------------------------------------------------------------
工程名称：摇杆、旋钮及电池处理应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
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
#define Joystick_Task_PRIO    ( tskIDLE_PRIORITY  + 14 )								//摇杆处理任务
#define Joystick_Task_STACK   ( 256 )

#define MIN_KNOB_VALUE 100//20140523:1500,原来1000,苏州1700
#define MAX_KNOB_VALUE 4000

#define FORWARD_NUM		2200			//前进限值
#define BACKWARD_NUM	1800			//后退限值
#define	MIDDLE_NUM		2000			//中值位置

#define DEFAULT_MIDDLE_VALUE 2028					//摇杆中位

#define CONTROLLER_TYPE_1		0X01 	//只有单摇杆
#define CONTROLLER_TYPE_2		0X02	//双摇杆+旋钮


/* Exported variables ---------------------------------------------------------*/
extern 	xTaskHandle                   Joystick_Task_Handle;

extern u16 RightRudderCommand;				//右舵机舵角映射
extern u16 RightSpeedCommand;				//右推进速度映射
extern u16 LeftRudderCommand;				//左舵机舵角映射
extern u16 LeftSpeedCommand;					//左推进速度映射

//旋钮键值映射
extern u16 KnobValue;									//旋钮采样后的处理值	---  喷水
extern u16 SpeedFactor;								//旋钮采样后的处理值	---	 速度因子或油门

extern u8 ControllerType;							//摇杆模式，是否叠加旋钮判定


/* Exported function prototypes -----------------------------------------------*/
void Joystick_Task(void * pvParameters);
uint16_t LowPassFilter(uint16_t buffer[ADC_BUFFER_SIZE][6], uint16_t length, uint8_t subpara);
uint16_t FindMax(uint16_t * buffer, uint16_t length);
void CountKnobValue(void);
void count_angle_speed_right(void);
void count_angle_speed_left(void);


#endif
