/*-------------------------------------------------------------------------
�������ƣ���1.5��ң�����������
����˵������������ҪΪ����ϵͳ��ʼ�������������
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150526 ������    5.0.0		��������ң�������Գ���
																		

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h "
#include <stddef.h>
#include "TouchPanel/TouchPanel.h"
#include "stm32f4xx.h"
#include "remoter_sender_RF.h"
#include "remoter_sender_UI.h"
#include "remoter_sender_jostick.h"

int main(void)
{ 
	
	//�����ж�����4�У����������ж����ȼ����=0��adc����=1���������ź�ǿ�ȵ�timer�ж�=2
	// ���Դ���uart2�ж�Ϊ6
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* Setup SysTick Timer for 1 msec interrupts.*/
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
	 /* Capture error */ 
	 while (1);
	}  
	
	
	bsp_Init();
	

  xTaskCreate(remoter_sender_RF_Task,
              (signed char const*)"RF_T",
              1024,
              NULL,
              tskIDLE_PRIORITY+14,
              &remoter_sender_RF_Task_Handle);	
							
  xTaskCreate(remoter_sender_Joystick_Task,
              (signed char const*)"Joystick_T",
              256, //stack size
              NULL,
              tskIDLE_PRIORITY+12,
              &remoter_sender_Joystick_Task_Handle);					
							

  xTaskCreate(remoter_sender_UI_Task,
              (signed char const*)"GUI_Interface",
              2000,//stack size
              NULL,
              tskIDLE_PRIORITY+10,
              &remoter_sender_UI_Task_Handle);							

							
							
  /* Start the FreeRTOS scheduler */
  vTaskStartScheduler();
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
