#ifndef _MAIN_CONFIG_H
#define _MAIN_CONFIG_H






#include "stm32f10x.h"
#include "bsp.h"



#define NAVIGATION_BOARD_TYPE 1
#define MAIN_CONTROLLER_BOARD_TYPE 0






#define IAP_BOARD_TYPE		NAVIGATION_BOARD_TYPE



#if (IAP_BOARD_TYPE == NAVIGATION_BOARD_TYPE)

#define UARTDEV		 	USART1
#define UART_TX_GPIO 	GPIOB
#define UART_TX_PIN 	GPIO_Pin_6
#define UART_RX_GPIO 	GPIOB
#define UART_RX_PIN 	GPIO_Pin_7
#define CAN1_ID			0x11

#else

#define UARTDEV		 	USART1
#define UART_TX_GPIO 	GPIOB
#define UART_TX_PIN 	GPIO_Pin_6
#define UART_RX_GPIO 	GPIOB
#define UART_RX_PIN 	GPIO_Pin_7
#define CAN1_ID			0x11


#endif









#endif