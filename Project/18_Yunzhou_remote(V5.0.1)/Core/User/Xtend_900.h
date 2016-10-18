/*-------------------------------------------------------------------------
工程名称：Xtend900 数传模块相关驱动程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150608 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __XTEND_900_H
#define __XTEND_900_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "keyboard_app.h"


/* Exported define ------------------------------------------------------------*/
#define Xtend_900_Task_PRIO    ( tskIDLE_PRIORITY  + 20 )				//数值越小优先级越高
#define Xtend_900_Task_STACK   ( 256 )


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Xtend_900_Task_Handle;

extern uint8_t board_temperature;					//数传模块当前温度


/* Exported function prototypes -----------------------------------------------*/
void Xtend_900_Task(void * pvParameters);


#endif
