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


/* Exported define ------------------------------------------------------------*/
#define Joystick_Task_PRIO    ( tskIDLE_PRIORITY  + 14 )								//摇杆处理任务
#define Joystick_Task_STACK   ( 256 )


/* Exported variables ---------------------------------------------------------*/
extern 	xTaskHandle                   Joystick_Task_Handle;


/* Exported function prototypes -----------------------------------------------*/
void Joystick_Task(void * pvParameters);


#endif
