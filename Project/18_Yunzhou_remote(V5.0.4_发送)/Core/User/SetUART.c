/*-------------------------------------------------------------------------
�������ƣ�ң����ƽ̨�������������ĵ�
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150526 ������    5.0.0		�½���
																		USART1 ��Ϊ915ģ��ͨ�ſ�
																		USART2 ��Ϊ���Կ�
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "SetUART.h"

/* Private variables ---------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
uint8_t USART1_RX_Buffer[USART1_RX_BUFFER_SIZE];						//USART1 ���ݽ��ջ�����
uint16_t receive_pointer = 0;																//���ջ�����ָ��
uint32_t receive_counter = 0;																//�����ֽ�����
uint32_t receive_good_count = 0;														//�ڲ���ģʽ��ͳ�ƽ�����ȷ���ֽ���

uint8_t USART1_TX_Buffer[USART1_TX_BUFFER_SIZE];						//USART1 ���ݷ��ͻ�����
uint16_t transmit_pointer = 0;															//���ͻ�����ָ��

int rfcommandReceivedBufCount = 0;				//�������ݻ�����
u8 rfcommandReceivedBuf[100];							//32					


/*-------------------------------------------------------------------------
	��������mCOMInit
	��  �ܣ���ʼ������ͨ�Ŵ����Լ������ô���
					USART1 ��Ϊ900MHz����ͨ��
					USART2 ��ΪDebug��
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void mCOMInit(void)
{
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef 	GPIO_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//��ʼ��USART1 ---------------------------------------------------------
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
	
//	//����DMA2ʱ��
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
//	
//	//��ʼ��DMA2 Stream5 Channel4��ΪUSART1����ͨ��
//	DMA_DeInit(DMA2_Stream5);
//	/* DMA2 Stream5 channel4 configuration **************************************/
//  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  					// Stream5 Channel4��USART1 RX��DMAͨ��
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

	//�ж�Ƕ�׳�ʼ��
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);     	//��ռ���ȼ�2�������ȼ�2λ
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;    		//�������ж�1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   	//�ƶ���ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    				//ָ�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
	//�жϽ������ݣ���ѯ��������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	 
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	 
	
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);	
	//--------------------------------------------------------- ��ʼ��USART1
	
	//��ʼ��USART2 ---------------------------------------------------------
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
	��������transmit_data
	��  �ܣ�ͨ��USART1 ����ѯ��ʽ����һ������
	��  ����send_buf	���ͻ�������
					send_len	�������鳤��

	����ֵ��
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
	��������PutUART
	��  �ܣ�ͨ��USART1 ����һ���ֽ�����  
	��  ����ch ���͵��ַ�

	����ֵ��
-------------------------------------------------------------------------*/
void PutUART(char ch)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		
	USART_SendData(USART1, ch);
}


/*-------------------------------------------------------------------------
	��������USART1_IRQHandler
	��  �ܣ�USART1�жϷ������
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void USART1_IRQHandler(void)
{
	u8 c;				//char c;
	
	//�����ж�---------------------------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{		
		c = USART_ReceiveData(USART1);
		
		//�����յ����ַ��ݴ��ڻ�����
		USART1_RX_Buffer[receive_pointer] = c;
		
		receive_pointer++;					//����ָ��
		receive_counter++;					//�����ַ������ۼ�
		
		if(receive_pointer >= USART1_RX_BUFFER_SIZE)
		{
			receive_pointer = 0;
		}
		
		//����ģʽ��ͳ�Ƴɹ����յ��ֽڸ���
		if((debug_Mode == 1) && (c == GOOD_REF))
		{
			receive_good_count++;
		}		

//		OnCommandDataReceived(c);					//��Ϣ���մ�����
	}	
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//�жϱ�־��λ
	
	//�����ж�---------------------------------------------------------
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{}		
	USART_ClearITPendingBit(USART1, USART_IT_TXE);			//�жϱ�־��λ
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
