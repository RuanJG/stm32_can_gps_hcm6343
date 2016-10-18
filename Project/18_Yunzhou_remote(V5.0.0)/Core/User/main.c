/*-------------------------------------------------------------------------
工程名称：第1.5代遥控器控制软件
描述说明：本程序主要为操作系统初始化及多任务调度
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150526 赵铭章    5.0.0		初步建立遥控器调试程序
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "timers.h "
#include <stddef.h>
#include "TouchPanel/TouchPanel.h"
#include "stm32f4xx.h"

#include "UI_app.h"
#include "keyboard_app.h"
#include "joystick_app.h"
#include "message_app.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define ATcommand_Task_PRIO    ( tskIDLE_PRIORITY  + 16 )								//AT命令发送任务
#define ATcommand_Task_STACK   ( 64 )


/* Private macro -------------------------------------------------------------*/
/* Private const -------------------------------------------------------------*/
/* Exported variables ---------------------------------------------------------*/
uint8_t debug_Mode = 1;				//调试模式： 0为正常模式，1为进入调试模式


/* Private function prototypes -----------------------------------------------*/
//static void vTimerCallback( xTimerHandle pxTimer );


/* Private functions ---------------------------------------------------------*/
void mdelay(uint32_t timer)
{
	while(timer--);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */ 
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	 
	/* Setup SysTick Timer for 1 msec interrupts.*/
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
	 /* Capture error */ 
	 while (1);
	}  
  
  /* Create Graphic_Interface task */
  xTaskCreate(Graphic_Interface_Task,
              (signed char const*)"GUI_Interface",
              Graphic_Interface_Task_STACK,
              NULL,
              Graphic_Interface_Task_PRIO,
              &Graphic_Interface_Task_Handle);

	/* Create keyboard sample task */
  xTaskCreate(Keyboard_Task,
              (signed char const*)"Keyboard_T",
              Keyboard_Task_STACK,
              NULL,
              Keyboard_Task_PRIO,
              &Keyboard_Task_Handle);
							
	/* Create joystick sample task */
  xTaskCreate(Joystick_Task,
              (signed char const*)"Joystick_T",
              Joystick_Task_STACK,
              NULL,
              Joystick_Task_PRIO,
              &Joystick_Task_Handle);
							
	/* Create message handling task */
  xTaskCreate(Message_Task,
              (signed char const*)"Message_T",
              Message_Task_STACK,
              NULL,
              Message_Task_PRIO,
              &Message_Task_Handle);
	

  /* Start the FreeRTOS scheduler */
  vTaskStartScheduler();
}


/**
  * @brief  ATcommand task
  * @param  pvParameters not used
  * @retval None
  */
//static void ATcommand_Task(void * pvParameters)
//{
//  /* Run the ATcommand task */
//  while (1)
//  {
//		if()
//	}
//}


/**
  * @brief  Error callback function
  * @param  None
  * @retval None
  */
void vApplicationMallocFailedHook( void )
{
  while (1)
  {}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
