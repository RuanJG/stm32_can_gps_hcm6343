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

#define US_LEFT_SET		GPIOA->BSRR = GPIO_Pin_9
#define US_LEFT_RESET	GPIOA->BRR  = GPIO_Pin_9
#define US_RIGHT_SET	GPIOB->BSRR = GPIO_Pin_9
#define US_RIGHT_RESET	GPIOB->BRR  = GPIO_Pin_9

#define US_K	30


// function declaration
void PutUART(char ch);
void PutString (char *buffer);	 
void TxBufFill(unsigned char ucData);
void TxBufRelease(void);
extern void ExecuteCommand(u8 *commamnd, int len);
extern void CommandPaserOld(u8 c);
extern void OnReceivedCommandByte(u8 c);

// variables declaration
char TxBuf[400];
int TxBufTop=0,TxBufBottom=0;

char TxBuf2[400];
int TxBufTop2=0,TxBufBottom2=0;

char TxBuf3[400];
int TxBufTop3=0,TxBufBottom3=0;


//char sdata1[4];
//char sdata2[4];

void PutUART2(char ch);	
void PutString2(char *buffer); 
void TxBufFill2(unsigned char ucData);
void TxBufRelease2(void);

void PutUART3(char ch);	 
void PutString3(char *buffer); 
void TxBufFill3(unsigned char ucData);
void TxBufRelease3(void);


//static u8 index1 = 0;
//static u8 index2 = 0;    
//static u8 index = 0; 
#define UART_BUFFER_SIZE 30
static u8 uart_buffer2[UART_BUFFER_SIZE];
static int uart_buffer2_count = 0;
static u8 uart_buffer3[UART_BUFFER_SIZE];
static int uart_buffer3_count = 0;
static u8 uart_buffer2_readytoget = 0;
static u8 uart_buffer3_readytoget = 0;

void UART_Configuration (void)
{		  
	USART_InitTypeDef USART_InitStructure;
	
	USART_StructInit (&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
	USART_Init(USART1, &USART_InitStructure); 
	
	//USART_StructInit (&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 9600;//超声波
	//USART_InitStructure.USART_BaudRate = 115200;//调试
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;  
	USART_Init(USART2, &USART_InitStructure); 

	USART_StructInit (&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx| USART_Mode_Tx;  
	USART_Init(USART3, &USART_InitStructure); 

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
	USART_Cmd(USART1, ENABLE);	  
	USART_Cmd(USART2, ENABLE);
	USART_Cmd(USART3, ENABLE);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	}													  
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
	{
	}
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
	{
	}
}

void USART1_IRQHandler(void)
{
	u8 c;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//Rec Interrupt
	{
		c=USART_ReceiveData(USART1);
		
		//STM32解BASE64码时严重延迟,所以ARM9发送给STM32采用明文传输
		//CommandPaserOld(c);
		
		switch(usv_protocol_version)
		{
			case USV_PROTOCOL_VERSION_1:
				CommandPaserOld(c);
				break;
			case USV_PROTOCOL_VERSION_2:
				OnReceivedCommandByte(c);
				break;
			default:
				break;
		}
		
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)	//发送中断
	{   
		TxBufRelease();     
	}
} 

void USART2_IRQHandler(void)
{
	char c;
	u16 data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //Rec Interrupt
	{	
		//readyToSend91 = FALSE;
		c = USART_ReceiveData(USART2);
		
		//PutUART2(c);
		if (c == 0x52)
		{
			uart_buffer2_readytoget = 1;
			//index1 = 0;
			uart_buffer2_count = 0;
		}
		else if(uart_buffer2_readytoget == 1)
		{
			if(uart_buffer2_count >= UART_BUFFER_SIZE)
			{
				uart_buffer2_readytoget = 0;
				//index1 = 0;
				uart_buffer2_count = 0;
			}
			
			if (c == 13)
			{
				uart_buffer2_readytoget = 0;
				//index1 = 0;
				uart_buffer2_count = 0;
			}
			else
			{
				//sdata1[index1] = c-48;
				uart_buffer2[uart_buffer2_count++] = c - 48;
				//if(index1 == 3)
				if(uart_buffer2_count == 4)
				{
					//US_LEFT_RESET;
					
					//data = sdata1[0]*1000 + sdata1[1]*100 + sdata1[2]*10 + sdata1[3];
					data = uart_buffer2[0]*1000 + uart_buffer2[1]*100 + uart_buffer2[2]*10 + uart_buffer2[3];
//					GlobalVariable.PreviousPreviousUltraSonic[0] = GlobalVariable.PreviousUltraSonic[0];					
//					GlobalVariable.PreviousUltraSonic[0] = GlobalVariable.UltraSonic[0];
					
					GlobalVariable.UltraSonic[0] = data;
					
					//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
					//USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
					
					//US_RIGHT_SET;
													
				}
				//index1++;												
			}
		}		
	}
	
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)	 //发送中断
	{   
		TxBufRelease2();     
	}
} 

void USART3_IRQHandler(void)
{
	char c;
	u16 data;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //Rec Interrupt
	{	
		c = USART_ReceiveData(USART3);
		//PutUART2(c);
		
		if (c == 0x52)
		{
			uart_buffer3_readytoget = 1;
			//index2 = 0;
			uart_buffer3_count = 0;
		}
		else if(uart_buffer3_readytoget == 1)
		{
			if(uart_buffer3_count >= UART_BUFFER_SIZE)
			{
				uart_buffer3_readytoget = 0;
				//index2 = 0;
				uart_buffer3_count = 0;
			}
			
			if (c == 13)
			{
				uart_buffer3_readytoget = 0;
				//index2 = 0;
				uart_buffer3_count = 0;
			}
			else
			{
				//sdata2[index2] = c-48;
				uart_buffer3[uart_buffer3_count++] = c - 48;
				//if(index1 == 3)
				if(uart_buffer3_count == 4)
				{
					//US_RIGHT_RESET;
					
					//data = sdata2[0]*1000 + sdata2[1]*100 + sdata2[2]*10 + sdata2[3];
					data = uart_buffer3[0]*1000 + uart_buffer3[1]*100 + uart_buffer3[2]*10 + uart_buffer3[3];
//					GlobalVariable.PreviousPreviousUltraSonic[1] = GlobalVariable.PreviousUltraSonic[1];					
//					GlobalVariable.PreviousUltraSonic[1] = GlobalVariable.UltraSonic[1];
					
					GlobalVariable.UltraSonic[1] = data;
					
					//USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
					//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);					
					
					//US_LEFT_SET;
//					GlobalVariable.US_After_Process[0] = (GlobalVariable.PreviousPreviousUltraSonic[0] + 
//						GlobalVariable.PreviousUltraSonic[0] + GlobalVariable.UltraSonic[0] ) / 3;
//					GlobalVariable.US_After_Process[1] = (GlobalVariable.PreviousPreviousUltraSonic[1] +
//						GlobalVariable.PreviousUltraSonic[1] + GlobalVariable.UltraSonic[1] ) / 3;									
				}
				//index2++;												
			}
		}
	}
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)	 //发送中断
	{   
		TxBufRelease3();     
	}
}



// definition of functions

/**
 * @brief  send a char via UART
 * @param  char
 * @return None
 */ 
void PutUART(char ch)
{
	//Utext[Uindex++] = ch;
	//Uindex++;
	if (USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
	{
		USART_SendData(USART1, ch);
	}	
	else
	{
		TxBufFill(ch);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
}

/**
 * @brief  send a string via UART
 * @param  array
 * @return None
 */
void PutString (char *buffer)
{
	while (*buffer != '\0')
	{
		PutUART (*buffer);
		buffer ++;
	}
}
	
void PutBytes (const char *buffer, int len)
{
	while (len-->=0)
	{
		PutUART (*buffer);
		buffer ++;
		if((len%8)==0)
		{
			delay_us(1000);
		}
	}
}

/**
 * @brief  send a string via UART
 * @param  array
 * @return None
 */
void PutString2 (char *buffer)
{
	while (*buffer != '\0')
	{
		PutUART2 (*buffer);
		buffer ++;
	}
}

/**
 * @brief  send a string via UART
 * @param  array
 * @return None
 */
void PutString3 (char *buffer)
{
	while (*buffer != '\0')
	{
		PutUART3 (*buffer);
		buffer ++;
	}
}

/**
 * @brief  
 * @param  1 char
 * @return None
 */
void TxBufFill(unsigned char ucData)
{	
	TxBuf[TxBufTop]=ucData;
	if(++TxBufTop==400){TxBufTop=0;}    
	if(TxBufTop==TxBufBottom){if(--TxBufTop==-1)TxBufTop=399;}
}

/**
 * @brief  软FIFO释放函数
 * @param  None
 * @return None
 */
void TxBufRelease(void)
{
	if(TxBufBottom!=TxBufTop)
	{
		if(USART_GetFlagStatus(USART1, USART_FLAG_TC) != RESET)
		{    
		  USART_SendData(USART1, TxBuf[TxBufBottom]);
		  if(++TxBufBottom==400)TxBufBottom=0;
		}
		else 
		return;
	}
	else
	{
	 USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	}
}

void PutUART2(char ch)
{
	if (USART_GetFlagStatus(USART2, USART_FLAG_TC) != RESET)
	{
		USART_SendData(USART2, ch);
	}	
	else
	{
		TxBufFill2(ch);
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
}

void PutUART3(char ch)
{
	if (USART_GetFlagStatus(USART3, USART_FLAG_TC) != RESET)
	{
		USART_SendData(USART3, ch);
	}	
	else
	{
		TxBufFill3(ch);
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
}

void TxBufFill2(unsigned char ucData)
{	
	TxBuf2[TxBufTop2]=ucData;
	if(++TxBufTop2==400){TxBufTop2=0;}    
	if(TxBufTop2==TxBufBottom2){if(--TxBufTop2==-1)TxBufTop2=399;}
}

void TxBufFill3(unsigned char ucData)
{	
	TxBuf3[TxBufTop3]=ucData;
	if(++TxBufTop3==400){TxBufTop3=0;}    
	if(TxBufTop3==TxBufBottom3){if(--TxBufTop3==-1)TxBufTop3=399;}
}

void TxBufRelease2(void)
{
	if(TxBufBottom2!=TxBufTop2)
	{
		if(USART_GetFlagStatus(USART2, USART_FLAG_TC) != RESET)
		{    
		  USART_SendData(USART2, TxBuf2[TxBufBottom2]);
		  if(++TxBufBottom2==400)TxBufBottom2=0;
		}
		else 
		return;
	}
	else
	{
	 USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
}

void TxBufRelease3(void)
{
	if(TxBufBottom3!=TxBufTop3)
	{
		if(USART_GetFlagStatus(USART3, USART_FLAG_TC) != RESET)
		{    
		  USART_SendData(USART3, TxBuf3[TxBufBottom3]);
		  if(++TxBufBottom3==400)TxBufBottom3=0;
		}
		else 
		return;
	}
	else
	{
	 USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	}
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
