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

//uint8_t RT_buffer[50];								//����������ģʽ�ض����ݻ�����
//uint8_t RT_pointer = 0;								//����������ģʽ����ָ��

char Xtend_900_para_buf[PARAMETER_QUANTITY][4];						//Xtend 900 �����洢�ַ���ʽ
char Xtend_900_para_buf_tmp[PARAMETER_QUANTITY][4];				//Xtend 900 �����洢�ַ���ʽ����ʾ����
uint32_t Hopping_Channel = 0;					//����ģ���ŵ� (HP) 			ȡֵ��ΧΪ 0 ~ 9
uint32_t Modem_VID = 0;								//����ģ��VID	(ID)				ȡֵ��ΧΪ 0x11 ~ 0x7fff
uint32_t Source_Address = 0;					//���ص�ַ		(MY)				ȡֵ��ΧΪ 0 ~ 0xffff
uint32_t Destination_Address = 0;			//Ŀ�ĵ�ַ		(DT)				ȡֵ��ΧΪ 0 ~ 0xffff   0xffffΪ�㲥��ַ
uint32_t Address_Mask = 0;						//�����ַ		(MK)				ȡֵ��ΧΪ 0 ~ 0xffff
uint32_t TX_Power_Level = 0;					//���书��		(PL)				ȡֵ��ΧΪ 0 ~ 4    0Ϊ1mW��1Ϊ10mW��2Ϊ100mW��3Ϊ500mW��4Ϊ1W
uint32_t API_Enable = 0;							//APIģʽ			(AP)				ȡֵ��ΧΪ 0 ~ 2		0Ϊ��ֹAPI��1Ϊ����escaped��APIģʽ��2Ϊ��escaped��APIģʽ
uint32_t Board_Temperature = 0;				//����ģ�鵱ǰ�¶�				ȡֵ��ΧΪ 0 ~ 0x7f	


/* Private const ---------------------------------------------------------*/
const uint8_t ATRT[15] = {0x41, 0x54, 0x52, 0x54, 0x31, 0x2c, 0x57, 0x52, 0x2c, 0x43, 0x4e, 0x0d};				//ATRT1,WR,CN<Enter>		�������������ģʽ
const uint8_t RTTP[3] = {0x38, 0x00, 0x00};						//ATTP�Ķ����������ʽ
const uint8_t RTRT[4] = {0x16, 0x00, 0x00, 0x08};						//����AT����ģʽ

//���Ͳ�����ѯ���� ATHP,ID,MY,DT,MK,PL,AP,TP<CR>
//��ȡ�ŵ���VID�����ص�ַ��Ŀ�ĵ�ַ�������ַ�����书�ʣ�APIģʽ�������¶ȵ���Ϣ
const uint8_t inquiry_para[26] = {0x41, 0x54, 0x48, 0x50, 0x2c, 0x49, 0x44, 0x2c, 0x4d, \
																	0x59, 0x2c, 0x44, 0x54, 0x2c, 0x4d, 0x4b, 0x2c, 0x50, \
																	0x4c, 0x2c, 0x41, 0x50, 0x2c, 0x54, 0x50, 0x0d};																									

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
				
//				//ATRT1,WR,CN<Enter>
//				for(xt900_i = 0; xt900_i < 15; xt900_i++)
//				{
//					USART_SendData(USART1, ATRT[xt900_i]);

//					/* Loop until the end of transmission */
//					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);					
//				}
			}
			
//			//����Binary Commandsģʽ���ȡ����ģ��״̬��Ϣ----------------------------
//			GPIO_SetBits(GPIOI, GPIO_Pin_7);			//����Xtend 900 ��CMD
//			RT_pointer = 0;
//			
////			//��ȡ����ģ�鵱ǰ�¶�
////			for(xt900_i = 0; xt900_i < 3; xt900_i++)
////			{
////				USART_SendData(USART1, RTTP[xt900_i]);

////				/* Loop until the end of transmission */
////				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
////				{
////					vTaskDelay(1);
////				}					
////			}

//			//��ȡ����ģ�鵱ǰ�¶�
//			for(xt900_i = 0; xt900_i < 4; xt900_i++)
//			{
//				USART_SendData(USART1, RTRT[xt900_i]);

//				/* Loop until the end of transmission */
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//				{
//					vTaskDelay(1);
//				}					
//			}				
//			
//			GPIO_ResetBits(GPIOI, GPIO_Pin_7);			//�ر�Xtend 900 ��CMD	
//			//����Binary Commandsģʽ���ȡ����ģ��״̬��Ϣ----------------------------



		}
		
		vTaskDelay(100);							//����ʱ����Ϊ1�룬����ᵼ�����ݶ���ʧ��
	}		
}


/**
  * @brief  enter_AT_Command
  * @param  None
  * @retval None
  */
void enter_AT_Command(void)
{
	uint16_t xt900_i = 0;
	
	//����ATģʽ�� +++   
	for(xt900_i = 0; xt900_i < 3; xt900_i++)
	{
		USART_SendData(USART1, '+');
		
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}		
}


/**
  * @brief  send_AT_inquiry
  * @param  None
  * @retval None
  */
void send_AT_inquiry(void)
{
	uint16_t xt900_i = 0;
	
	//�������� ATHP,ID,MY,DT,MK,PL,AP,TP<CR>  
	for(xt900_i = 0; xt900_i < 26; xt900_i++)
	{
		USART_SendData(USART1, inquiry_para[xt900_i]);
		
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}		
}


/**
  * @brief  handle_parameter_message
  * @param  None
  * @retval None
  */
void handle_parameter_message(void)
{
	uint16_t temp_i;
	uint16_t temp_len;
	
	//�����ڻ����������ݷֱ𱣴浽�������У��ָ���Ϊ<CR> ��16����Ϊ0x0d
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		
	}
}

