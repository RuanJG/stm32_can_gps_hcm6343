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

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void ADC_Configuration(void);
#define ADC1_DR_Address    ((u32)0x4001204C)

float AD_value_1,AD_value_2,AD_value_3,AD_value_4,AD_value_5,AD_value_6;
vu16 ADC_ConvertedValue[6];
void ADC1_CH6_DMA_Config(void);
void PWM_Config(void);
void GPIO_Config(void);

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
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	int ccr, frq;
	uint8_t maichong = 0;

  /* USART configuration */
  USART_Config();
	
	PWM_Config();
	GPIO_Config();
  
  RCC_GetClocksFreq(&SYS_Clocks);
	printf("\r\nSYSCLK:%dM\r\n",SYS_Clocks.SYSCLK_Frequency/1000000);
	printf("HCLK:%dM\r\n",SYS_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1:%dM\r\n",SYS_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2:%dM\r\n",SYS_Clocks.PCLK2_Frequency/1000000);	
	printf("\n\r ADC Example \r\n");
	
	ADC1_CH6_DMA_Config();
	ADC_SoftwareStartConv(ADC1);
  while (1)
  {
//		/* Printf message with AD value to serial port every 1 second */
//		printf("\r\n****************************************************************\r\n");
//		AD_value_1 = ADC_ConvertedValue[0];
//		AD_value_1 = (float)(AD_value_1/65535)*3.3;
//		printf("The current AD1 value = %4.2fV \r\n", AD_value_1);
//		
//		AD_value_2 = ADC_ConvertedValue[1];
//		AD_value_2 = (float)(AD_value_2/65535)*3.3;
//		printf("The current AD2 value = %4.2fV \r\n", AD_value_2);
//		
//		AD_value_3 = ADC_ConvertedValue[2];
//		AD_value_3 = (float)(AD_value_3/65535)*3.3;
//		printf("The current AD3 value = %4.2fV \r\n", AD_value_3);

//		AD_value_4 = ADC_ConvertedValue[3];
//		AD_value_4 = (float)(AD_value_4/65535)*3.3;
//		printf("The current AD4 value = %4.2fV \r\n", AD_value_4);

//		AD_value_5 = ADC_ConvertedValue[4];
//		AD_value_5 = (float)(AD_value_5/65535)*3.3;
//		printf("The current AD5 value = %4.2fV \r\n", AD_value_5);

		AD_value_6 = ADC_ConvertedValue[5];
//		ccr = AD_value_6;
		AD_value_6 = (float)(AD_value_6/4095)*3.3;
		printf("The current AD6 value = %4.2fV \r\n", AD_value_6);
////		Delay(0x1ffffff);
//		
//		USART_SendData(USART1, 0x85);

//		USART1_RX_Buffer[0] = (uint16_t)(USART1->DR & (uint16_t)0x01FF);

//		printf("%c%c%c%c\r\n", USART1_RX_Buffer[0], USART1_RX_Buffer[1], USART1_RX_Buffer[2], USART1_RX_Buffer[3]);

//		ccr = ((ADC_ConvertedValue[5] / 128) * 9000  / 4095) * 128;
		ccr = USART1_RX_Buffer[0] * 256 + USART1_RX_Buffer[1];
		frq = USART1_RX_Buffer[2] * 256 + USART1_RX_Buffer[3];
		printf("The current ccr1 value = %d \r\n", ccr);
		printf("The current frq value = %d \r\n", frq);
		
//		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
//		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
//		TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
//		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
//		TIM_TimeBaseInitStructure.TIM_Period = frq - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
//		TIM_TimeBaseInit(TIM13, &TIM_TimeBaseInitStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		
		if(maichong == 0)
		{
			TIM_OCInitStructure.TIM_Pulse = ccr;
			maichong = 1;
		}
		
		else
		{
			maichong = 0;
			TIM_OCInitStructure.TIM_Pulse = 0;
		}
		
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

		TIM_OC1Init(TIM13, &TIM_OCInitStructure);
		
		ccr = USART1_RX_Buffer[0] * 256 + USART1_RX_Buffer[1];
		printf("The current ccr2 value = %d \r\n", ccr);
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_Pulse = ccr;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

		TIM_OC1Init(TIM10, &TIM_OCInitStructure);
		
		GPIO_ToggleBits(GPIOF, GPIO_Pin_7);
		
		Delay(10000000);
  }
	
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);	
}

void PWM_Config(void)
{
	//PWM GPIO配置
	//PF8初始化
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	
	//PF6初始化
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_TIM10);	
	
	//定时器13作PWM输出
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	int TimerPeriod =  (SystemCoreClock / 600 ) - 1;
  int ccr1 = TimerPeriod / 2;  	//50%
  int ccr2 = TimerPeriod / 3;  	//33%
  int ccr3 = TimerPeriod / 4;  	//25%
  int ccr4 = TimerPeriod / 5;  	//20%
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
  //时基初始化
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
  TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInitStructure.TIM_Period = TimerPeriod - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
  TIM_TimeBaseInit(TIM13, &TIM_TimeBaseInitStructure);

  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ccr1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  
  TIM_OC1Init(TIM13, &TIM_OCInitStructure);

//  TIM_OCInitStructure.TIM_Pulse = ccr2;
//  TIM_OC2Init(TIM1,&TIM_OCInitStructure);
//  
//  TIM_OCInitStructure.TIM_Pulse = ccr3;
//  TIM_OC3Init(TIM1,&TIM_OCInitStructure);
//  
//  TIM_OCInitStructure.TIM_Pulse = ccr4;
//  TIM_OC4Init(TIM1,&TIM_OCInitStructure);
  
  TIM_Cmd(TIM13, ENABLE);
  TIM_CtrlPWMOutputs(TIM13, ENABLE);
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
  //时基初始化
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
  TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInitStructure.TIM_Period = TimerPeriod - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseInitStructure);

  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ccr1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);

//  TIM_OCInitStructure.TIM_Pulse = ccr2;
//  TIM_OC2Init(TIM1,&TIM_OCInitStructure);
//  
//  TIM_OCInitStructure.TIM_Pulse = ccr3;
//  TIM_OC3Init(TIM1,&TIM_OCInitStructure);
//  
//  TIM_OCInitStructure.TIM_Pulse = ccr4;
//  TIM_OC4Init(TIM1,&TIM_OCInitStructure);
  
  TIM_Cmd(TIM10, ENABLE);
  TIM_CtrlPWMOutputs(TIM10, ENABLE);

}

void ADC1_CH6_DMA_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

//  DMA_DeInit(DMA2_Stream0);
  /* DMA2 Stream0 channe0 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 6;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* Configure ADC1 Channel6 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 6;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channe6 configuration *************************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_3Cycles);
 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

}


/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {

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
