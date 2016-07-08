/******************************************************************************
 * @file:    SetUART.c
 * @purpose: functions related to UART
 * @version: V1.00
 * @date:    11. Jul 2011
 *----------------------------------------------------------------------------
 ******************************************************************************/

#include "stm32f10x.h"
#include "system.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	

Uart_t *uart1_p ;
Uart_t *uart2_p ;
Uart_t *uart3_p ;

void Uart_Configuration (Uart_t *uart, USART_TypeDef *uartDev, uint32_t USART_BaudRate, uint16_t USART_WordLength, uint16_t USART_StopBits, uint16_t USART_Parity)
{		  
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//softwoare config
	fifo_init(&uart->txfifo, uart->txbuff, sizeof(uart->txbuff));
	fifo_init(&uart->rxfifo, uart->rxbuff, sizeof(uart->rxbuff));
	
	// pll clock
	if( uartDev == USART1 ){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	}else if( uartDev == USART2 ){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	
	}else if( uartDev == USART3 ){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	}
	
	// uart base config
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

	// irq
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//uart->read_cb = rcb;
	uart->uartDev = uartDev;
	if( uartDev == USART1 ){
		uart1_p = uart;
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	}else if( uartDev == USART2 ){
		uart2_p = uart;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	}else if( uartDev == USART3 ){
		uart3_p = uart;
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	}
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	


	// start .... 
	USART_ITConfig(uartDev, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(uartDev, USART_IT_TXE, ENABLE);
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
	
	if(USART_GetITStatus(uartDev, USART_IT_RXNE) != RESET)  	//Rec Interrupt enabled
	{
		if( USART_GetFlagStatus(uartDev, USART_FLAG_RXNE) != RESET) // data recived
		{
			c=USART_ReceiveData(uartDev);
			if( fifo_free(&uart->rxfifo) > 0 ){
				fifo_put(&uart->rxfifo,c);
			}else{
				system_Error_Callback(ERROR_UART_RX_FIFO_OVERFLOW_TYPE, (int)uartDev);
				/*recovery old data */
				#if 0
				fifo_get(&uart->rxfifo,&tc);
				fifo_put(&uart->rxfifo,c);
				#else
				fifo_recovery_put(&uart->rxfifo,c);
				#endif
			}
		}
	}

	if(USART_GetITStatus(uartDev, USART_IT_TXE) != RESET)	//∑¢ÀÕ÷–∂œenabled
	{   
		//if(USART_GetFlagStatus(uartDev, USART_FLAG_TC) != RESET)
		if( USART_GetFlagStatus(uartDev, USART_FLAG_TXE) != RESET)
		{
			// empty , can to send data
			if( 0 < fifo_avail(&uart->txfifo) ){
				fifo_get(&uart->txfifo,&tc);
				USART_SendData(uartDev, tc);
			}else{
				USART_ITConfig(uartDev, USART_IT_TXE, DISABLE);
			}
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



int Uart_GetChar(Uart_t *uart, char *c){
	// return 1 new data in c;  0: no data 
	int res = 0;
	
	USART_ITConfig(uart->uartDev, USART_IT_RXNE, DISABLE);
	res = fifo_get(&uart->rxfifo,c);
	USART_ITConfig(uart->uartDev, USART_IT_RXNE, ENABLE);

	return res;
}
void Uart_PutChar(Uart_t *uart,char ch)
{
	char c;
	USART_TypeDef *uartDev = uart->uartDev;
	
	USART_ITConfig(uartDev, USART_IT_TXE, DISABLE);
	if( 0 >= fifo_free(&uart->txfifo) ){
		//TODO add error check;
		system_Error_Callback(ERROR_UART_TX_FIFO_OVERFLOW_TYPE, (int)uart);
		#if 1
		USART_ITConfig(uartDev, USART_IT_TXE, ENABLE);
		while( 0 >= fifo_free(&uart->txfifo) ); //wait for empty buff
		USART_ITConfig(uartDev, USART_IT_TXE, DISABLE);
		#else
		//recovery
		fifo_get(&uart->txfifo,&c);
		#endif
	}
	fifo_put(&uart->txfifo,ch);
	USART_ITConfig(uartDev, USART_IT_TXE, ENABLE);
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



//End of File
