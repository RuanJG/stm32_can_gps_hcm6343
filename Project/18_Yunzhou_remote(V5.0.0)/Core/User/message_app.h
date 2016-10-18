/*-------------------------------------------------------------------------
工程名称：消息接收发送处理程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MESSAGE_APP_H
#define __MESSAGE_APP_H


/* Includes ------------------------------------------------------------------*/
#include "SetUART.h"
#include "global_includes.h"


/* Exported define ------------------------------------------------------------*/
#define Message_Task_PRIO    ( tskIDLE_PRIORITY  + 12 )								//消息接收发送处理任务
#define Message_Task_STACK   ( 128 )


/* Exported variables ---------------------------------------------------------*/
extern xTaskHandle                   Message_Task_Handle;

extern char show_message[52];			//显示接收的消息


/* Exported function prototypes -----------------------------------------------*/
void Message_Task(void * pvParameters);

#endif
