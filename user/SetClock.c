/******************************************************************
STM32 Clock Configuration
2010.1.12:creat time
******************************************************************/
#include <stm32f10x.h>
#define HSE 1

ErrorStatus HSEStartUpStatus;

void SetClock(void)
{
    if(HSE)
	{
		RCC_DeInit();//Rest
		RCC_HSEConfig(RCC_HSE_ON);//HSE ON


		/* Wait till HSE is ready */
		HSEStartUpStatus = RCC_WaitForHSEStartUp();
		if(HSEStartUpStatus == SUCCESS)
		  {
	    	/* HCLK(AHB) = SYSCLK /x*/
	    	RCC_HCLKConfig(RCC_SYSCLK_Div1);//x=1,2,4,8,16,64,128,256,512   
		    /* PCLK2(APB2) = HCLK/x */
		    RCC_PCLK2Config(RCC_HCLK_Div1);//x=1,2,4,8,16 
		    /* PCLK1(APB1) = HCLK/x */
		    RCC_PCLK1Config(RCC_HCLK_Div2);//x=1,2,4,8,16
			/* Configure ADCCLK such as ADCCLK = PCLK2/6 */ 
		 	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
		    /* 设置代码延时x周期*/
		    FLASH_SetLatency(FLASH_Latency_2);//此值与系统时钟值有关
			/* Enable Prefetch Buffer */
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

			/* PLLCLK = 8MHz * 9 = 72 MHz */
		    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_12);//72M
			/* Enable PLL */ 
 	  		RCC_PLLCmd(ENABLE);
			/* Wait till PLL is ready */
		    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		    {    }
			/* Select PLL as system clock source */
		    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		    /* Wait till PLL is used as system clock source */
		    while(RCC_GetSYSCLKSource() != 0x08)
		    {    }
		  }
		 else
		 {
		  	RCC_HSEConfig(RCC_HSE_OFF);//HSE OFF
			RCC_HSICmd(ENABLE);

		 	/* HCLK(AHB) = SYSCLK /x*/
		 	RCC_HCLKConfig(RCC_SYSCLK_Div1);//x=1,2,4,8,16,64,128,256,512   
		 	/* PCLK2(APB2) = HCLK/x */
		 	RCC_PCLK2Config(RCC_HCLK_Div1);//x=1,2,4,8,16 
		 	/* PCLK1(APB1) = HCLK/x */
		 	RCC_PCLK1Config(RCC_HCLK_Div2);//x=1,2,4,8,16
			/* Configure ADCCLK such as ADCCLK = PCLK2/4 */ 
		 	RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
		 	/* 设置代码延时x周期*/
		 	FLASH_SetLatency(FLASH_Latency_2);//此值与系统时钟值有关
		 	/* Enable Prefetch Buffer */
		 	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);


		 	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);//64M
		 	RCC_PLLCmd(ENABLE);
		 	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		 	{    }
		 	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		 	while(RCC_GetSYSCLKSource() != 0x08)
		 	{    }
		 }
      }
	else
	{
		 RCC_DeInit();
		 RCC_HSEConfig(RCC_HSE_OFF);
		 RCC_HSICmd(ENABLE);

		 /* HCLK(AHB) = SYSCLK /x*/
		 RCC_HCLKConfig(RCC_SYSCLK_Div1);//x=1,2,4,8,16,64,128,256,512   
		 /* PCLK2(APB2) = HCLK/x */
		 RCC_PCLK2Config(RCC_HCLK_Div1);//x=1,2,4,8,16 
		 /* PCLK1(APB1) = HCLK/x */
		 RCC_PCLK1Config(RCC_HCLK_Div2);//x=1,2,4,8,16
		 /* Configure ADCCLK such as ADCCLK = PCLK2/4 */ 
		 RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
		 /* 设置代码延时x周期*/
		 FLASH_SetLatency(FLASH_Latency_2);//此值与系统时钟值有关
		 /* Enable Prefetch Buffer */
		 FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);


		 RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_14);//56M
		 RCC_PLLCmd(ENABLE);
		 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		 {    }
		 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		 while(RCC_GetSYSCLKSource() != 0x08)
		 {    }
	}

}
