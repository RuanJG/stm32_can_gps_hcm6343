/******************************************************************************
 * @file:    SetUART.c
 * @purpose: functions related to UART
 * @version: V1.00
 * @date:    11. Jul 2011
 *----------------------------------------------------------------------------
 ******************************************************************************/

#include "stm32f10x.h"
#include "global.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	

Uart_t *uart1_p ;
Uart_t *uart2_p ;
Uart_t *uart3_p ;

void Uart_Configuration (Uart_t *uart, USART_TypeDef *uartDev, uint32_t USART_BaudRate, uint16_t USART_WordLength, uint16_t USART_StopBits, uint16_t USART_Parity)
{		  
	USART_InitTypeDef USART_InitStructure;
	
	USART_ITConfig(uartDev, USART_IT_RXNE, DISABLE);
	USART_Cmd(uartDev, DISABLE);

	USART_StructInit (&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = USART_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength;
	USART_InitStructure.USART_StopBits = USART_StopBits;
	USART_InitStructure.USART_Parity = USART_Parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
	USART_Init(uartDev, &USART_InitStructure); 

	//uart->read_cb = rcb;
	uart->uartDev = uartDev;
	if( uartDev == USART1 ){
		uart1_p = uart;
	}else if( uartDev == USART2 ){
		uart2_p = uart;
	}else if( uartDev == USART3 ){
		uart3_p = uart;
	}
	fifo_init(&uart->txfifo, uart->txbuff, sizeof(uart->txbuff));
	fifo_init(&uart->rxfifo, uart->rxbuff, sizeof(uart->rxbuff));

	USART_ITConfig(uartDev, USART_IT_RXNE, ENABLE);
	USART_Cmd(uartDev, ENABLE);	  
	/*
	@TODO check this loop is need ?
	while (USART_GetFlagStatus(uartDev, USART_FLAG_TC) == RESET)
	{
	}	
	*/
}

void _uart_irq_function(Uart_t *uart)
{
	char c,tc;
	USART_TypeDef *uartDev = uart->uartDev;
	
	if(USART_GetITStatus(uartDev, USART_IT_RXNE) != RESET)  	//Rec Interrupt
	{
		c=USART_ReceiveData(uartDev);
		//if( uart->read_cb != NULL ) uart->read_cb(c);
		fifo_put(&uart->rxfifo,c);
	}

	if(USART_GetITStatus(uartDev, USART_IT_TXE) != RESET)	//·¢ËÍÖÐ¶Ï
	{   
		if( 1==fifo_enabled(&uart->txfifo) && 0 < fifo_avail(&uart->txfifo) ){
			if(USART_GetFlagStatus(uartDev, USART_FLAG_TC) != RESET)
			{
				if( fifo_get(&uart->txfifo,&tc) ){
					USART_SendData(uartDev, tc);
				}
			}
		}else{
			//no data ,close tx
			USART_ITConfig(uartDev, USART_IT_TXE, DISABLE);
		}
	}
}


void USART1_IRQHandler(void)
{
	_uart_irq_function(uart1_p);
} 

void USART2_IRQHandler(void)
{
	_uart_irq_function(uart2_p);
} 

void USART3_IRQHandler(void)
{
	_uart_irq_function(uart3_p);
}




void Uart_PutChar(Uart_t *uart,char ch)
{
	USART_TypeDef *uartDev = uart->uartDev;
	if (USART_GetFlagStatus(uartDev, USART_FLAG_TC) != RESET)
	{
		USART_SendData(uartDev, ch);
	}
	else
	{
		if( 0 == fifo_put(&uart->txfifo,ch) ){
			;//TODO add error check;
		}
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
}

void Uart_PutString (Uart_t *uart,char *buffer)
{
	while (*buffer != '\0')
	{
		Uart_PutChar(uart,*buffer);
		buffer ++;
	}
}
	
void Uart_PutBytes (Uart_t *uart,const char *buffer, int len)
{
	int i;
	for( i=0;i<len;i++){
		Uart_PutChar(uart,buffer[i]);
	}
}

int Uart_GetChar(Uart_t *uart, char *c){
	// return 1 new data in c;  0: no data 
	return fifo_get( &uart->rxfifo,c);
}

__asm void wait()
{
	BX LR
}

 

void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
       wait();
}
//End of File
