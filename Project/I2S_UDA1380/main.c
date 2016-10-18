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
#include "waveDate.h"
#include "usart.h"
#include "UDA1380.h"
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */
	
#define AUIDO_START_ADDRESS     58 /* Offset relative to audio file header size */
#define UDA1380_WRITE_ADDRESS     0x30
uint32_t WaveDataLength=0;
uint8_t XferCplt=0;

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file.
     */     
  RCC_ClocksTypeDef SYS_Clocks;
  /* USART configuration */
  USART_Config();
       
  /* Output a message on Hyperterminal using printf function */
	
	RCC_GetClocksFreq(&SYS_Clocks);
	printf("\r\nSYSCLK:%dM\r\n",SYS_Clocks.SYSCLK_Frequency/1000000);
	printf("HCLK:%dM\r\n",SYS_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1:%dM\r\n",SYS_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2:%dM\r\n",SYS_Clocks.PCLK2_Frequency/1000000);	
	printf(" Welcome to use UDA1380 I2S test:\r\n"); 
	WaveDataLength = sizeof(WaveData) - AUIDO_START_ADDRESS;
	printf("WaveDataLength:%d\r\n",WaveDataLength);
	CODEC_I2S_Configuration();
	printf(" Started to Transmission data.\r\n");
	AudioFlashPlay((uint16_t*)(WaveData + AUIDO_START_ADDRESS),WaveDataLength,AUIDO_START_ADDRESS);
	UDA1380_Configuration();
  while (1)
  {
		while(!XferCplt)
		{
			
		}
		XferCplt=0;
		printf(" Data transmission to complete.\r\n");
			
		WaveDataLength = sizeof(WaveData) - AUIDO_START_ADDRESS;
		printf(" Started to Transmission data.\r\n");
		AudioFlashPlay((uint16_t*)(WaveData + AUIDO_START_ADDRESS),WaveDataLength,AUIDO_START_ADDRESS);

  }
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
