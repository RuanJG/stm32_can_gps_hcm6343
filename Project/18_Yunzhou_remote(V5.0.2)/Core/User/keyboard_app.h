/*-------------------------------------------------------------------------
工程名称：按键采样应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KEYBOARD_APP_H
#define __KEYBOARD_APP_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"


/* Exported define ------------------------------------------------------------*/
#define Keyboard_Task_PRIO    ( tskIDLE_PRIORITY  + 14 )								//按键及钮子开关采样任务
#define Keyboard_Task_STACK   ( 128 )


/* Exported variables ---------------------------------------------------------*/
extern xTaskHandle                   Keyboard_Task_Handle;

//按键及钮子开关相关全局变量
extern uint8_t MENU_Button;				//MENU 菜单键，0表示没有按下，1表示按下
extern uint8_t OK_Button;					//OK 确认键，0表示没有按下，1表示按下
extern uint8_t CANCEL_Button;			//CANCEL 取消键，0表示没有按下，1表示按下
extern uint8_t ALARM_Button;				//ALARM 警报警灯钮子开关，0表示没有按下，1表示按下
extern uint8_t SAMPLE_Button;			//SAMPLE 采样开关，0表示没有按下，1表示按下
extern uint8_t MODE_Button;				//MODE 模式开关，0表示模式0，1表示模式1，2表示模式2


/* Exported function prototypes -----------------------------------------------*/
void Keyboard_Task(void * pvParameters);


#endif
