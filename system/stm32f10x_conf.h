/**
  ******************************************************************************
  * @file    ADC/3ADCs_DMA/stm32f10x_conf.h 
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Library configuration file.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
#include "stm32f10x_adc.h" 
/* #include "stm32f10x_bkp.h" */
#include "stm32f10x_can.h"
/* #include "stm32f10x_crc.h" */
/* #include "stm32f10x_dac.h" */
/* #include "stm32f10x_dbgmcu.h" */
#include "stm32f10x_dma.h" 
//#include "stm32f10x_exti.h"
#include "stm32f10x_flash.h" 
/* #include "stm32f10x_fsmc.h" */
#include "stm32f10x_gpio.h" 
/* #include "stm32f10x_i2c.h" */
/* #include "stm32f10x_iwdg.h" */
/* #include "stm32f10x_pwr.h" */
#include "stm32f10x_rcc.h" 
/* #include "stm32f10x_rtc.h" */
/* #include "stm32f10x_sdio.h" */
/* #include "stm32f10x_spi.h" */
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
/* #include "stm32f10x_wwdg.h" */
#include "misc.h"   /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *   which reports the name of the source file and the source
  *   line number of the call that failed. 
  *   If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

	
	
	
	

	
	
	
	
/*
********************************����Board config and clock config 
*
*/	
	
#define MAIN_CONTROLLER_BOARD 0
#define NAVIGATION_BOX 0
#define M80_ESC_BOX 0
#define STM32F103V_BOARD 0
#define NAVIGATION_TEST_BOX 0
#define NAVIGATION_CURRENT_BOX 1


// if make iap firmware �� stm32f103 : start addr =0x8000000-0x8010000 ; size 0x10000 
// iap firmware : addr=0x8000000   size=0x4000   ; 0x4000/1024 = 16 = 16kB     
// iap tag : addr = 0x8004000 size=1024 (0x400);
// app firmware : addr= 0x8004400   size=0xBC00  = (0x10000 - 0x4400) = 47k
//#define IAP_FIRMWARE 0
//#define IAP_UART_BAUDRATE 9600


/*
************************  clock config
*           this define use in system_stm32f10x.c SetSysClockTo72() for clock config
*           it will ignore the HSE_VALUE define in stm32f10x.h
*						now we support 8000000 6000000
*/
#undef HSE_VALUE

#if STM32F103V_BOARD
#define HSE_VALUE    ((uint32_t) 8000000)
#elif NAVIGATION_BOX | NAVIGATION_TEST_BOX | NAVIGATION_CURRENT_BOX
#define HSE_VALUE    ((uint32_t) 6000000)
#else
#error "define HSE_VALUE for you board in stm32f10x_conf.h !"
#endif
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
#endif /* __STM32F10x_CONF_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
