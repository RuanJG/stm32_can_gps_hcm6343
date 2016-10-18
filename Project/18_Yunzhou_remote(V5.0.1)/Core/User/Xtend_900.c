/*-------------------------------------------------------------------------
�������ƣ�Xtend900 ����ģ�������������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150608 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


#include "Xtend_900.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle		Xtend_900_Task_Handle;

uint8_t board_temperature = 0;				//����ģ�鵱ǰ�¶�


/* Private const ---------------------------------------------------------*/
const uint8_t ATRT[15] = {0x41, 0x54, 0x52, 0x54, 0x31, 0x2c, 0x57, 0x52, 0x2c, 0x43, 0x4e, 0x0d};				//ATRT1,WR,CN<Enter>		�������������ģʽ
const uint8_t RTTP[3] = {0x38, 0x00, 0x00};						//ATTP�Ķ����������ʽ

/**
  * @brief  Xtend_900 handling task
  * @param  pvParameters not used
  * @retval None
  */
void Xtend_900_Task(void * pvParameters)
{
	uint16_t xt900_i = 0;
	
	/* Run the Xtend_900 handling task */
  while (1)
  {
		//�ж��Ƿ�Ϊ����ģʽ
		if(debug_Mode == 1)
		{		
			//�ж��Ƿ����Binary Commandsģʽ
			if(CANCEL_Button && OK_Button)
			{
				//����ATģʽ�� +++   
				for(xt900_i = 0; xt900_i < 3; xt900_i++)
				{
					USART_SendData(USART1, '+');
					
					/* Loop until the end of transmission */
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				}	

				vTaskDelay(2000);
				
				//ATRT1,WR,CN<Enter>
				for(xt900_i = 0; xt900_i < 15; xt900_i++)
				{
					USART_SendData(USART1, ATRT[xt900_i]);

					/* Loop until the end of transmission */
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);					
				}
			}
			
//			GPIO_SetBits(GPIOI, GPIO_Pin_7);			//����Xtend 900 ��CMD
//			//��ȡ����ģ�鵱ǰ�¶�
//			for(xt900_i = 0; xt900_i < 3; xt900_i++)
//			{
//				USART_SendData(USART1, RTTP[xt900_i]);

//				/* Loop until the end of transmission */
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//				{
//					vTaskDelay(1);
//				}					
//			}		
//			GPIO_ResetBits(GPIOI, GPIO_Pin_7);			//�ر�Xtend 900 ��CMD
//			vTaskDelay(10);		
		}
		
		vTaskDelay(1000);							//����ʱ����Ϊ1�룬����ᵼ�����ݶ���ʧ��
	}		
}





