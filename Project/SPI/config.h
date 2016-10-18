#ifndef _CONFIG_H
#define _CONFIG_H

#include <stdio.h>
#include "stm32f4xx.h"
//#define Open_SPI1
#define Open_SPI4
/**
 * @brief Definition for connected to I2C1
 */
/* SPIx Communication boards Interface */
// SPI1 MISO (PA6)	MOSI(PA7)  CLK(PA5)  NSS(PA4)
#ifdef Open_SPI1

#define Open_RCC_APB2Periph_SPIx   	       RCC_APB2Periph_SPI1

#define Open_SPIx                           SPI1
#define Open_SPIx_CLK                       RCC_APB2Periph_SPI1
#define Open_SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define Open_SPIx_IRQn                      SPI1_IRQn
#define Open_SPIx_IRQHANDLER                SPI1_IRQHandler

#define Open_SPIx_SCK_PIN                   GPIO_Pin_5
#define Open_SPIx_SCK_GPIO_PORT             GPIOA
#define Open_SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define Open_SPIx_SCK_SOURCE                GPIO_PinSource5
#define Open_SPIx_SCK_AF                    GPIO_AF_SPI1

#define Open_SPIx_MISO_PIN                  GPIO_Pin_6
#define Open_SPIx_MISO_GPIO_PORT            GPIOA
#define Open_SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define Open_SPIx_MISO_SOURCE               GPIO_PinSource6
#define Open_SPIx_MISO_AF                   GPIO_AF_SPI1

#define Open_SPIx_MOSI_PIN                  GPIO_Pin_7
#define Open_SPIx_MOSI_GPIO_PORT            GPIOA
#define Open_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define Open_SPIx_MOSI_SOURCE               GPIO_PinSource7
#define Open_SPIx_MOSI_AF                   GPIO_AF_SPI1

#define RCC_GPIO_CS					RCC_AHB1Periph_GPIOA
#define RCC_AHBxPeriphClockCmd		RCC_AHB1PeriphClockCmd
#define GPIO_PIN_CS					GPIO_Pin_4
#define GPIO_CS_PORT				GPIOA

#else 

#define Open_RCC_APB2Periph_SPIx   	       RCC_APB2Periph_SPI4

#define Open_SPIx                           SPI4
#define Open_SPIx_CLK                       RCC_APB2Periph_SPI4
#define Open_SPIx_CLK_INIT                  RCC_APB1PeriphClockCmd
#define Open_SPIx_IRQn                      SPI4_IRQn
#define Open_SPIx_IRQHANDLER                SPI4_IRQHandler

#define Open_SPIx_SCK_PIN                   GPIO_Pin_12
#define Open_SPIx_SCK_GPIO_PORT             GPIOE
#define Open_SPIx_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOE
#define Open_SPIx_SCK_SOURCE                GPIO_PinSource12
#define Open_SPIx_SCK_AF                    GPIO_AF_SPI4

#define Open_SPIx_MISO_PIN                  GPIO_Pin_13
#define Open_SPIx_MISO_GPIO_PORT            GPIOE
#define Open_SPIx_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOE
#define Open_SPIx_MISO_SOURCE               GPIO_PinSource13
#define Open_SPIx_MISO_AF                   GPIO_AF_SPI4

#define Open_SPIx_MOSI_PIN                  GPIO_Pin_14
#define Open_SPIx_MOSI_GPIO_PORT            GPIOE
#define Open_SPIx_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOE
#define Open_SPIx_MOSI_SOURCE               GPIO_PinSource14
#define Open_SPIx_MOSI_AF                   GPIO_AF_SPI4

#define RCC_GPIO_CS					RCC_AHB1Periph_GPIOE
#define RCC_AHBxPeriphClockCmd		RCC_AHB1PeriphClockCmd
#define GPIO_PIN_CS					GPIO_Pin_11
#define GPIO_CS_PORT				GPIOE

#endif

#endif
