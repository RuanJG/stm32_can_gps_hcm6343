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
********************************　　Board config and clock config 
*
*/	
	
#define MAIN_CONTROLLER_BOARD 0
#define NAVIGATION_BOX 0
#define M80_ESC_BOX 0
#define STM32F103V_BOARD 0
#define NAVIGATION_TEST_BOX 1
#define NAVIGATION_CURRENT_BOX 0


// if make iap firmware ， stm32f103 : start addr =0x8000000-0x8010000 ; size 0x10000 
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
 



//########################################  配置 中断




/*
优先级： 数值越小越高级
抢占优先级：即可打断中断的优先级；如NVIC_PriorityGroup_1，PREPRIORITY有两个，0与1组，每个组8个子优先级， 0组里面的8个子优先级可以抢占1组里的中断
子优先级：不能打断同组的中断；
PREPRIORITY与SSUBPRIORITY相同时，按中断号排先后

按NVIC_PriorityGroup_0来分组：抢占优先级为1个，子优先级为16个；
按NVIC_PriorityGroup_1来分组：抢占优先级为2个，子优先级为8个；
按NVIC_PriorityGroup_2来分组：抢占优先级为4个，子优先级为4个；
按NVIC_PriorityGroup_3来分组：抢占优先级为8个，子优先级为2个；
按NVIC_PriorityGroup_4来分组：抢占优先级为16个，子优先级为1个；

方案1：
{
默认配置是 NVIC_PriorityGroup_2， 有4个可抢先的中断组；
串口与can1都在第3个组里；
systick 在第2个组里；可以为系统提供实时的时间
第1个组给用户的重要实时中断的用途
第4个组给用户一般中断的用途
}

*/
 
#define CUSTOM_IRQ_GROUP 				NVIC_PriorityGroup_2
//uart
#define CUSTOM_UART1_IRQ_PREPRIORITY 	2
#define CUSTOM_UART1_IRQ_SUBPRIORITY	3
#define CUSTOM_UART2_IRQ_PREPRIORITY 	2
#define CUSTOM_UART2_IRQ_SUBPRIORITY	3
#define CUSTOM_UART3_IRQ_PREPRIORITY 	2
#define CUSTOM_UART3_IRQ_SUBPRIORITY	3
//can
#define CUSTOM_CAN1_IRQ_PREPRIORITY 	2
#define CUSTOM_CAN1_IRQ_SUBPRIORITY		2
//systick, cm3 中断,配置有点不同
#define CUSTOM_SYSTICK_IRQ_PREPRIORITY 	1
#define CUSTOM_SYSTICK_IRQ_SUBPRIORITY	0
//NVIC_SetPriority函数指定中断优先级的寄存器位的数据（STM32只用4位来表示优先级），例如中断优先级组设置为了2，即高2位用于指定抢占式优先级，低2位用于指定响应优先级，0x00~0x03高2位为0，所以抢占优先级为0；0x04~0x07高2位为1，所以抢占优先级为1，以此类推
#define CUSTOM_SYSTICK_IRQ_PRIORITY ((CUSTOM_SYSTICK_IRQ_PREPRIORITY<<2) | CUSTOM_SYSTICK_IRQ_SUBPRIORITY )

//user normal irq
#define CUSTOM_USER_NORMAL_IRQ_PREPRIORITY 3
//user importent irq
#define CUSTOM_USER_IMPORTENT_IRQ_PREPRIORITY 2
//user realtime irq
#define CUSTOM_USER_REALTIME_IRQ_PREPRIORITY 0
 
 
 
#endif /* __STM32F10x_CONF_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
