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
#include "usart.h"
#include "dcmi_OV2640.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
static __IO uint32_t TimingDelay;
uint32_t JpegDataCnt=0;
extern uint8_t JpegBuffer[1024*33];
uint8_t jpg_flag=0;
uint8_t key_flag=0;
#define JpegBufferLen (sizeof(JpegBuffer)/sizeof(char)) //计算JpegBuffer元素总个数

#define GPIO_WAKEUP_CLK    RCC_AHB1Periph_GPIOA
#define GPIO_WAKEUP_PORT   GPIOA
#define GPIO_WAKEUP_PIN    GPIO_Pin_0

/* Private functions ---------------------------------------------------------*/

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
	GPIO_InitTypeDef GPIO_InitStructure;
	OV2640_IDTypeDef OV2640ID;
	RCC_ClocksTypeDef SYS_Clocks;
	uint32_t i=0;
		if (SysTick_Config(SystemCoreClock / 1000))
  	{ 
	    /* Capture error */ 
	    while (1);
  	}
  /* USART configuration */
  USART_Config();
       
  /* Output a message on Hyperterminal using printf function */
  
  RCC_GetClocksFreq(&SYS_Clocks);
	printf("\r\nSYSCLK:%dM\r\n",SYS_Clocks.SYSCLK_Frequency/1000000);
	printf("HCLK:%dM\r\n",SYS_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1:%dM\r\n",SYS_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2:%dM\r\n",SYS_Clocks.PCLK2_Frequency/1000000);	
	printf("\n\r DCMI Example\n\r");
	OV2640_Init();
	Delay(1);		
	if(DCMI_OV2640_ReadID(&OV2640ID)==0)
	{	
		if(OV2640ID.Manufacturer_ID1==0x7f && OV2640ID.Manufacturer_ID2==0xa2 
			&& OV2640ID.Version==0x26 && OV2640ID.PID==0x42){
 			printf("OV2640 ID:0x%x 0x%x 0x%x 0x%x\r\n",
 				OV2640ID.Manufacturer_ID1, OV2640ID.Manufacturer_ID2, OV2640ID.PID, OV2640ID.Version);
		}
		else{
 			printf("OV2640 ID is Error!\r\n");
		}			
	}	
	
		OV2640_JPEGConfig(JPEG_320x240);
		OV2640_BrightnessConfig(0x20);	
		OV2640_AutoExposure(2);	
		Delay(10);

			
	  Delay(10);		
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_WAKEUP_PIN;
	GPIO_Init(GPIO_WAKEUP_PORT, &GPIO_InitStructure);
  while (1)
  {
	if(GPIO_ReadInputDataBit(GPIO_WAKEUP_PORT,GPIO_WAKEUP_PIN))
		{
		 key_flag = 1;
	    DMA_Cmd(DMA2_Stream1, ENABLE);		
			DCMI_Cmd(ENABLE);
			DCMI_CaptureCmd(ENABLE);
			//printf("key_flag =1 \r\n");
			while(GPIO_ReadInputDataBit(GPIO_WAKEUP_PORT,GPIO_WAKEUP_PIN));
		}
		
		
		
		if(jpg_flag)//&&key_flag
		{
			DCMI_Cmd(DISABLE); 
			DCMI_CaptureCmd(DISABLE);
			DMA_Cmd(DMA2_Stream1, DISABLE);
			if( (JpegBuffer[0]==0xFF)&&(JpegBuffer[1]==0xD8) )
			{
					while ( !( (JpegBuffer[JpegBufferLen - JpegDataCnt-2]==0xFF) && (JpegBuffer[JpegBufferLen-JpegDataCnt-1]==0xD9) ) ) //从数据包的尾开始检索  
				{		
					JpegDataCnt++;
				}				
				 for(i = 0; i < (JpegBufferLen - JpegDataCnt); i++)	//sizeof(JpegBuffer)
				{
					USART_Transmit(JpegBuffer[i]);
				} 
			}
			JpegDataCnt = 0;
			jpg_flag = 0;
			key_flag = 0;	
	
		}
  }
}

void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0)
  {}
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
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
