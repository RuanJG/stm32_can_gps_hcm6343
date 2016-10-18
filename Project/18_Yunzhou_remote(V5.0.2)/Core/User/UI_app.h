/*-------------------------------------------------------------------------
工程名称：遥控器界面应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UI_APP_H
#define __UI_APP_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "bsp.h"
#include "UI_element.h"
#include "keyboard_app.h"
#include "message_app.h"


/* Exported define ------------------------------------------------------------*/
#define Graphic_Interface_Task_PRIO    ( tskIDLE_PRIORITY  + 10 )				//数值越小优先级越高
#define Graphic_Interface_Task_STACK   ( 2000 )

#define DEBUG_MAIN_DIVISION			10						//调试主界面显示分频         10毫秒 * 50 = 500毫秒	
#define DEBUG_PARAMETER_DIVISION	50					//调试参数显示分频         10毫秒 * 50 = 500毫秒
#define ATCOMMAND_DELAY		6										//进入AT命令模式等待时间  6 * 500毫秒 = 3秒


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Graphic_Interface_Task_Handle;

extern uint8_t interface_index;			//界面索引：1为主界面
																		//					2为参数配置界面
																		//					3为采样配置界面


/* Exported function prototypes -----------------------------------------------*/
void Graphic_Interface_Task(void * pvParameters);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);
void debug_mode_main(void);
void debug_mode_parameter(void);
void mdelay(volatile uint32_t timer);


#endif
