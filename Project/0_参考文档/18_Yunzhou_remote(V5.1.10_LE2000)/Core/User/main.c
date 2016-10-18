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
#include "global_includes.h"

#include "UI_app.h"
#include "keyboard_app.h"
#include "joystick_app.h"
//#include "message_app.h"
#include "Xtend_900.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Background_Task_PRIO    ( tskIDLE_PRIORITY  + 18 )								//背景任务
#define Background_Task_STACK   ( 128 )


/* Private macro -------------------------------------------------------------*/
/* Private const -------------------------------------------------------------*/
/* Exported variables ---------------------------------------------------------*/
char version_number[7] = "V5.1.10";						//版本号
char release_date[9] = "(151125)";								//发布日期

xTaskHandle                   Background_Task_Handle;


/* Private function prototypes -----------------------------------------------*/
//static void vTimerCallback( xTimerHandle pxTimer );
void Background_Task(void * pvParameters);


/* Private functions ---------------------------------------------------------*/


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
							
	/* Create joystick sample task */
  xTaskCreate(Background_Task,
              (signed char const*)"Background_T",
              Background_Task_STACK,
              NULL,
              Background_Task_PRIO,
              &Background_Task_Handle);
	

  /* Start the FreeRTOS scheduler */
  vTaskStartScheduler();
}


/**
  * @brief  Background task
  * @param  pvParameters not used
  * @retval None
  */
void Background_Task(void * pvParameters)
{
	//等待300毫秒板载驱动及液晶屏初始化
	vTaskDelay(300);
	LCD_PWM_Config(ENABLE);						//液晶屏幕PWM调节
	
	
	/* Run the Keyboard task */
  while (1)
	{
		vTaskDelay(100);
	}
}	


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
