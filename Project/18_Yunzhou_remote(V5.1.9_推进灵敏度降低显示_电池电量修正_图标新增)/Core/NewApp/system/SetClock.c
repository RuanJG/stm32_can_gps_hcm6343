/******************************************************************
STM32 Clock Configuration
2010.1.12:creat time
******************************************************************/
#include <stm32f10x.h>
#include "system.h"

uint32_t systemClk = 72000000;

void configHisClock()
{
		 RCC_DeInit();
		 RCC_HSEConfig(RCC_HSE_OFF);
	
		 RCC_HSICmd(ENABLE);
		 while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET){}
	
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
		 systemClk = 56000000;
		
		 RCC_PLLCmd(ENABLE);
		 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		 {    }
		 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		 while(RCC_GetSYSCLKSource() != 0x08)
		 {    }

}
/*
* SetupPllClock: setup base system clock  
* hse_mhz: how much HSE clock is , HSE_CLOCK_6MHZ  HSE_CLOCK_8MHZ, if HSE_CLOCK_NO_USE , use HSI
*/
int SetupPllClock( unsigned char hse_mhz) 
{
	
	ErrorStatus HSEStartUpStatus;
	
	if( hse_mhz != HSE_CLOCK_NO_USE )
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
		    /* 设置代码延时x周期*/
		    FLASH_SetLatency(FLASH_Latency_2);//此值与系统时钟值有关	
				/* Enable Prefetch Buffer */
				FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

				if( hse_mhz == HSE_CLOCK_6MHZ ){
					/* PLLCLK = 6MHz /1  * 12 = 72 MHz */
					RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_12);//72M
					/* Configure ADCCLK such as ADCCLK = PCLK2/6 12Mhz can not > 14M*/ 
					RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
					systemClk = 72000000;
				}else if( hse_mhz == HSE_CLOCK_8MHZ ){
					/* PLLCLK = 8MHz /1  * 9 = 72 MHz */
					RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);//72M
					/* Configure ADCCLK such as ADCCLK = PCLK2/6 12Mhz can not > 14M*/ 
					RCC_ADCCLKConfig(RCC_PCLK2_Div6);
					systemClk = 72000000;
				}else{
					system_Error_Callback(ERROR_HSE_SETUP_TYPE,1);
					//configHisClock();
					while(1);
				}
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
		 }else{
				system_Error_Callback(ERROR_HSE_SETUP_TYPE,1);
				//configHisClock();
			  
			 //关中断
				__set_PRIMASK(1);
				__set_FAULTMASK(1);
				// vectreset reset cm3 but other extern hardword
				//*((uint32_t*)0xE000ED0C) = 0x05FA0001;
				// sysresetReq reset all ic hardword system
				*((uint32_t*)0xE000ED0C) = 0x05FA0004;
			 while(1);
		 }
  }else{
		 configHisClock();
	}

}
