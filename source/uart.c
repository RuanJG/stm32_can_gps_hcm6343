#include "stm32f10x_conf.h"
#include "main_config.h"



void Uart_Configuration (void)
{		  
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	USART_StructInit (&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
	USART_Init(UARTDEV, &USART_InitStructure); 
	

	USART_ITConfig(UARTDEV, USART_IT_RXNE, ENABLE);	 
	USART_Cmd(UARTDEV, ENABLE);	  
	
	
		// irq
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	if( UARTDEV == USART1 ){
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	}else if( UARTDEV == USART2 ){
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	}else if( UARTDEV == USART3 ){
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	}
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	

	while (USART_GetFlagStatus(UARTDEV, USART_FLAG_TC) == RESET)
	{
	}													  

}

void UART_Receive_Byte( unsigned char c)
{
	
	
}

void Uart_send(unsigned *data, int len)
{
	int i;
	for( i=0 ; i< len; i++)
	{
		USART_SendData(UARTDEV, data[i]);
		while(USART_GetFlagStatus(UARTDEV, USART_FLAG_TXE) == RESET){}
	}
	
}

void USART1_IRQHandler(void)
{
	u8 c;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//Rec Interrupt
	{
		c=USART_ReceiveData(USART1);
		UART_Receive_Byte(c);
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)	//发送中断
	{    
		c++;
	}
} 
void USART2_IRQHandler(void)
{
	u8 c;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  	//Rec Interrupt
	{
		c=USART_ReceiveData(USART2);
		UART_Receive_Byte(c);
	}

	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)	//发送中断
	{    
		c++;
	}
} 
void USART3_IRQHandler(void)
{
	u8 c;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  	//Rec Interrupt
	{
		c=USART_ReceiveData(USART3);
		UART_Receive_Byte(c);
	}

	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)	//发送中断
	{    
		c++;
	}
} 