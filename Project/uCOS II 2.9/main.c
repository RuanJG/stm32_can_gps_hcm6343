/**
  ******************************************************************************
  * @file    USART/USART_Printf/main.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    13-November-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "KEY&LED.h"
#include "usart.h"

#include "ucos_ii.h"
#include <includes.h>


//宏定义
#define OS_MINI_STACK_SIZE 128

#define TASK_Start_PRIO 4
#define TASK_Key_PRIO   5
#define TASK_LED_PRIO   6

//任务堆栈
static  OS_STK TaskStartStk[OS_MINI_STACK_SIZE];
static  OS_STK TaskLEDStk[OS_MINI_STACK_SIZE];



static void TaskLED()
{
	for(;;)
	{ 
		Led_Toggle(Read_JOYState());
		
	}
}

static INT8U TaskStart()
{   
  INT8U  os_err;
	os_err = OSTaskCreate((void (*) (void *)) TaskLED,
                        (void *) 0,
                        (OS_STK *) &TaskLEDStk[OS_MINI_STACK_SIZE - 1],
                        (INT8U) TASK_LED_PRIO);
	OSTaskSuspend(TASK_Start_PRIO);
	return (os_err);
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
INT32S main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file.
     */     
  RCC_ClocksTypeDef SYS_Clocks;
  INT8U  os_err;
  /* USART configuration */
  USART_Config();
 //  USART_SendData(USART2,0x55);    
  /* Output a message on Hyperterminal using printf function */
  
  RCC_GetClocksFreq(&SYS_Clocks);
	printf("\r\nSYSCLK:%dM\r\n",SYS_Clocks.SYSCLK_Frequency/1000000);
	printf("HCLK:%dM\r\n",SYS_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1:%dM\r\n",SYS_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2:%dM\r\n",SYS_Clocks.PCLK2_Frequency/1000000);	
	printf("\n\r KEY Example \r\n");
	JOYState_LED_GPIO_Init();
	
	OS_CPU_SysTickInit();
	
  OSInit(); //Initialize "uC/OS-II, The Real-Time Kernel". 
	
	os_err = OSTaskCreate((void (*) (void *)) TaskStart,
                        (void *) 0,
                        (OS_STK *) &TaskStartStk[OS_MINI_STACK_SIZE - 1],
                        (INT8U) TASK_Start_PRIO);
	
	OSTimeSet(0);
	OSStart();  //Start multitasking (i.e. give control to uC/OS-II).  
  return (os_err);
												
	
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
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
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
