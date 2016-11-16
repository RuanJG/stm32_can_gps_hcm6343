
#include "bsp_xtend900_uart.h"
#include "stm32f4xx.h"
#include "stdio.h"


void bsp_xtend900_uart_init(void)
{
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef 	GPIO_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//初始化USART1 ---------------------------------------------------------
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
	
	
	/* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Configure USART Rx as alternate function  */
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  //GPIO_Init(GPIOA, &GPIO_InitStructure);

  
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;    		//开串口中断1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   	//制定抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    				//指定从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
	//中断接收数据，轮询发送数据
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	 
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	 
	
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);

}

void USART1_IRQHandler(void)
{
	u8 c;				//char c;
	
	//接收中断---------------------------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{		
		c = USART_ReceiveData(USART1);
		xtend900_parase(c);
		bps_log_uart1_to_uart2( c);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//中断标志软复位
	}
	//发送中断---------------------------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		//USART_ClearITPendingBit(USART1, USART_IT_TXE);
		//采用block 方式发送， 所以这里不做处理
	}	
		
	//USART_ClearITPendingBit(USART1, USART_IT_TXE|USART_IT_RXNE);			//中断标志软复位
} 
