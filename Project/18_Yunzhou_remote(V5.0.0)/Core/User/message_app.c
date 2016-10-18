/*-------------------------------------------------------------------------
�������ƣ���Ϣ���շ��ʹ������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

#include "message_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Message_Task_Handle;

char show_message[52];			//��ʾ���յ���Ϣ


/**
  * @brief  message handling task
  * @param  pvParameters not used
  * @retval None
  */
void Message_Task(void * pvParameters)
{
	uint16_t message_i = 0;
	uint16_t rec_mes_len = 0;					//�����ַ�������
	DMA_InitTypeDef   DMA_InitStructure;
	
	/* Run the message handling task */
  while (1)
  {
		//�ж��Ƿ�Ϊ����ģʽ
		if(debug_Mode == 1)
		{			
			if(USART1_RX_Buffer[0] != 0)
			{
				for(message_i = 0; message_i < 50; message_i++)
				{
					show_message[message_i] = USART1_RX_Buffer[message_i];
				}
				
				show_message[message_i] = '\0';				
				
				//UART DMA����������				
				for(message_i = 0; message_i < USART1_RX_BUFFER_SIZE; message_i++)
				{
					USART1_RX_Buffer[message_i] = 0;
				}
				
				//��ʼ��DMA2 Stream5 Channel4��ΪUSART1����ͨ��
				DMA_DeInit(DMA2_Stream5);
				/* DMA2 Stream5 channel4 configuration **************************************/
				DMA_InitStructure.DMA_Channel = DMA_Channel_4;  					// Stream5 Channel4��USART1 RX��DMAͨ��
				DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_Address;					
				DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&USART1_RX_Buffer;
				DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
				DMA_InitStructure.DMA_BufferSize = USART1_RX_BUFFER_SIZE;
				DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
				DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
				DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
				DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
				DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
				DMA_InitStructure.DMA_Priority = DMA_Priority_High;
				DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
				DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
				DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
				DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
				DMA_Init(DMA2_Stream5, &DMA_InitStructure);
				DMA_Cmd(DMA2_Stream5, ENABLE);
		
			}
		}
		
		vTaskDelay(100);
	}		
}






