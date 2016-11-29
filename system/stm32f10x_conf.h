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
#define NAVIGATION_TEST_BOX 1
#define NAVIGATION_CURRENT_BOX 0


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
 



//########################################  ���� �ж�




/*
���ȼ��� ��ֵԽСԽ�߼�
��ռ���ȼ������ɴ���жϵ����ȼ�����NVIC_PriorityGroup_1��PREPRIORITY��������0��1�飬ÿ����8�������ȼ��� 0�������8�������ȼ�������ռ1������ж�
�����ȼ������ܴ��ͬ����жϣ�
PREPRIORITY��SSUBPRIORITY��ͬʱ�����жϺ����Ⱥ�

��NVIC_PriorityGroup_0�����飺��ռ���ȼ�Ϊ1���������ȼ�Ϊ16����
��NVIC_PriorityGroup_1�����飺��ռ���ȼ�Ϊ2���������ȼ�Ϊ8����
��NVIC_PriorityGroup_2�����飺��ռ���ȼ�Ϊ4���������ȼ�Ϊ4����
��NVIC_PriorityGroup_3�����飺��ռ���ȼ�Ϊ8���������ȼ�Ϊ2����
��NVIC_PriorityGroup_4�����飺��ռ���ȼ�Ϊ16���������ȼ�Ϊ1����

����1��
{
Ĭ�������� NVIC_PriorityGroup_2�� ��4�������ȵ��ж��飻
������can1���ڵ�3�����
systick �ڵ�2���������Ϊϵͳ�ṩʵʱ��ʱ��
��1������û�����Ҫʵʱ�жϵ���;
��4������û�һ���жϵ���;
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
//systick, cm3 �ж�,�����е㲻ͬ
#define CUSTOM_SYSTICK_IRQ_PREPRIORITY 	1
#define CUSTOM_SYSTICK_IRQ_SUBPRIORITY	0
//NVIC_SetPriority����ָ���ж����ȼ��ļĴ���λ�����ݣ�STM32ֻ��4λ����ʾ���ȼ����������ж����ȼ�������Ϊ��2������2λ����ָ����ռʽ���ȼ�����2λ����ָ����Ӧ���ȼ���0x00~0x03��2λΪ0��������ռ���ȼ�Ϊ0��0x04~0x07��2λΪ1��������ռ���ȼ�Ϊ1���Դ�����
#define CUSTOM_SYSTICK_IRQ_PRIORITY ((CUSTOM_SYSTICK_IRQ_PREPRIORITY<<2) | CUSTOM_SYSTICK_IRQ_SUBPRIORITY )

//user normal irq
#define CUSTOM_USER_NORMAL_IRQ_PREPRIORITY 3
//user importent irq
#define CUSTOM_USER_IMPORTENT_IRQ_PREPRIORITY 2
//user realtime irq
#define CUSTOM_USER_REALTIME_IRQ_PREPRIORITY 0
 
 
 
#endif /* __STM32F10x_CONF_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
