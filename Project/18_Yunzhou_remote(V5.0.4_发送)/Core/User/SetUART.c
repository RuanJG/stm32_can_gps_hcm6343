/*-------------------------------------------------------------------------
工程名称：遥控器平台串口驱动程序文档
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150526 赵铭章    5.0.0		新建立
																		USART1 作为915模块通信口
																		USART2 作为调试口
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "SetUART.h"

/* Private variables ---------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
uint8_t USART1_RX_Buffer[USART1_RX_BUFFER_SIZE];						//USART1 数据接收缓冲区
uint16_t receive_pointer = 0;																//接收缓冲区指针
uint32_t receive_counter = 0;																//接收字节总数
uint32_t receive_good_count = 0;														//于测试模式下统计接收正确的字节数

uint8_t USART1_TX_Buffer[USART1_TX_BUFFER_SIZE];						//USART1 数据发送缓冲区
uint16_t transmit_pointer = 0;															//发送缓冲区指针

int rfcommandReceivedBufCount = 0;				//接收数据缓冲区
u8 rfcommandReceivedBuf[100];							//32					


/*-------------------------------------------------------------------------
	函数名：mCOMInit
	功  能：初始化数传通信串口以及调试用串口
					USART1 作为900MHz数传通信
					USART2 作为Debug用
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void mCOMInit(void)
{
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef 	GPIO_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//初始化USART1 ---------------------------------------------------------
  /* USARTx configured as follows:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable UART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(USART1, &USART_InitStructure);
	
//	//开启DMA2时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
//	
//	//初始化DMA2 Stream5 Channel4作为USART1接收通道
//	DMA_DeInit(DMA2_Stream5);
//	/* DMA2 Stream5 channel4 configuration **************************************/
//  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  					// Stream5 Channel4是USART1 RX的DMA通道
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_Address;					
//  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&USART1_RX_Buffer;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//  DMA_InitStructure.DMA_BufferSize = USART1_RX_BUFFER_SIZE;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//  DMA_Init(DMA2_Stream5, &DMA_InitStructure);
//  DMA_Cmd(DMA2_Stream5, ENABLE);
//	
//	/* Enable USART1 DMA */
//	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

	//中断嵌套初始化
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);     	//先占优先级2，从优先级2位
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
	//--------------------------------------------------------- 初始化USART1
	
	//初始化USART2 ---------------------------------------------------------
  /* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	
	/* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
  /* Enable UART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
  /* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  /* USART configuration */
	USART_Init(USART2, &USART_InitStructure);
    
  /* Enable USART */	
	USART_Cmd(USART2, ENABLE);
}


/*-------------------------------------------------------------------------
	函数名：transmit_data
	功  能：通过USART1 以轮询形式发送一组数据
	参  数：send_buf	发送缓存数组
					send_len	发送数组长度

	返回值：
-------------------------------------------------------------------------*/
void transmit_data(uint8_t *send_buf, uint16_t send_len)
{
	uint16_t i;
	
  for(i = 0; i < send_len; i++)
	{
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

		USART_SendData(USART1, send_buf[i]);
	}
}


/*-------------------------------------------------------------------------
	函数名：PutUART
	功  能：通过USART1 发送一个字节数据  
	参  数：ch 发送的字符

	返回值：
-------------------------------------------------------------------------*/
void PutUART(char ch)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		
	USART_SendData(USART1, ch);
}


/*-------------------------------------------------------------------------
	函数名：USART1_IRQHandler
	功  能：USART1中断服务程序
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
	u8 c;				//char c;
	
	//接收中断---------------------------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{		
		c = USART_ReceiveData(USART1);
		
		//将接收到的字符暂存在缓冲区
		USART1_RX_Buffer[receive_pointer] = c;
		
		receive_pointer++;					//缓存指针
		receive_counter++;					//接收字符总数累加
		
		if(receive_pointer >= USART1_RX_BUFFER_SIZE)
		{
			receive_pointer = 0;
		}
		
		//调试模式下统计成功接收的字节个数
		if((debug_Mode == 1) && (c == GOOD_REF))
		{
			receive_good_count++;
		}		

//		OnCommandDataReceived(c);					//信息接收处理函数
	}	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//中断标志软复位
	
	//发送中断---------------------------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{}		
	USART_ClearITPendingBit(USART1, USART_IT_TXE);			//中断标志软复位
} 


/**3

  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
  {}

  return ch;
}
