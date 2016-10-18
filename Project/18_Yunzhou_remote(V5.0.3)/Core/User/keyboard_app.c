/*-------------------------------------------------------------------------
�������ƣ���������Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "keyboard_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Keyboard_Task_Handle;

//������ť�ӿ������ȫ�ֱ���
uint8_t MENU_Button = 0;				//MENU �˵�����0��ʾû�а��£�1��ʾ����
uint8_t OK_Button = 0;					//OK ȷ�ϼ���0��ʾû�а��£�1��ʾ����
uint8_t CANCEL_Button = 0;			//CANCEL ȡ������0��ʾû�а��£�1��ʾ����
uint8_t ALARM_Button = 0;				//ALARM ��������ť�ӿ��أ�0��ʾû�а��£�1��ʾ����
uint8_t SAMPLE_Button = 0;			//SAMPLE �������أ�0��ʾû�а��£�1��ʾ����
uint8_t MODE_Button = 0;				//MODE ģʽ���أ�0��ʾģʽ0��1��ʾģʽ1��2��ʾģʽ2

//ҡ�˷������
uint8_t left_joystick_ud = 0;				//���ҡ�����·���ָʾ��0������λ��1���������ˣ�2����������
uint8_t left_joystick_lr = 0;				//���ҡ�����ҷ���ָʾ��0������λ��1���������ˣ�2����������
uint8_t right_joystick_ud = 0;			//�Ҳ�ҡ�����·���ָʾ��0������λ��1���������ˣ�2����������
uint8_t right_joystick_lr = 0;			//�Ҳ�ҡ�����ҷ���ָʾ��0������λ��1���������ˣ�2����������


/**
  * @brief  Keyboard task
  * @param  pvParameters not used
  * @retval None
  */
void Keyboard_Task(void * pvParameters)
{
  uint8_t menu_temp = 0;
	uint8_t ok_temp = 0;
	uint8_t cancel_temp = 0;
	uint8_t alarm_temp = 0;
	uint8_t sample_temp = 0;
	uint8_t modeup_temp = 0;
	uint8_t modedown_temp = 0;
	
	uint16_t K_div;						//������Ƶϵ��������
	uint16_t KQ_div;					//							����
	
	
	/* Run the Keyboard task */
  while (1)
  {	
		/*������ť�ӿ��ز���-----------------------------------------------------------------------------------*/
		//B_MENU �˵�������------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == 0)
		{
			menu_temp++;
			
			//����������
			if(menu_temp >= 2)
			{
				menu_temp = 2;				
				MENU_Button = 1;
			}
		}
		else
		{
			menu_temp = 0;
			MENU_Button = 0;
		}
		
		//B_OK ȷ�ϼ�����------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13) == 0)
		{
			ok_temp++;
			
			//����������
			if(ok_temp >= 2)
			{
				ok_temp = 2;				
				OK_Button = 1;
			}
		}
		else
		{
			ok_temp = 0;
			OK_Button = 0;
		}	

		//B_CANCEL ȡ��������------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12) == 0)
		{
			cancel_temp++;
			
			//����������
			if(cancel_temp >= 2)
			{
				cancel_temp = 2;				
				CANCEL_Button = 1;
			}
		}
		else
		{
			cancel_temp = 0;
			CANCEL_Button = 0;
		}				
		
		//S_Alarm �������Ƽ�����----------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0)
		{
			alarm_temp++;
			
			//����������
			if(alarm_temp >= 2)
			{
				alarm_temp = 2;				
				ALARM_Button = 1;
			}
		}
		else
		{
			alarm_temp = 0;
			ALARM_Button = 0;
		}	

		//S_Sample ����������----------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
		{
			sample_temp++;
			
			//����������
			if(sample_temp >= 2)
			{
				sample_temp = 2;				
				SAMPLE_Button = 1;
			}
		}
		else
		{
			sample_temp = 0;
			SAMPLE_Button = 0;
		}	

		//S_MODE2 �ֶ�����ģʽ������----------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		{
			modeup_temp++;
			
			//����������
			if(modeup_temp >= 2)
			{
				modeup_temp = 2;				
				MODE_Button = 0;
			}
		}
		//S_MODE1 �Զ�ģʽ������----------------------------------------------
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
		{
			modedown_temp++;
			
			//����������
			if(modedown_temp >= 2)
			{
				modedown_temp = 2;				
				MODE_Button = 2;
			}			
		}
		else 
		{
			modeup_temp = 0;
			modedown_temp = 0;
			MODE_Button = 1;
		}			
		/*������ť�ӿ��ز���===================================================================================*/

		
		/*ҡ�˰���ӳ��-----------------------------------------------------------------------------------------*/
		//���ҡ������ӳ��-----------------------------------------------------
		if(Left_Joystick_Y_axis_Avg > UP_LEFT_THRESHOLD)
			left_joystick_ud = 1;
		
		else if(Left_Joystick_Y_axis_Avg < DOWN_RIGHT_THRESHOLD)
			left_joystick_ud = 2;
		
		else
			left_joystick_ud = 0;


		//���ҡ������ӳ��-----------------------------------------------------
		if(Left_Joystick_X_axis_Avg > UP_LEFT_THRESHOLD)		
			left_joystick_lr = 1;
		
		else if(Left_Joystick_X_axis_Avg < DOWN_RIGHT_THRESHOLD)
			left_joystick_lr = 2;
		
		else
			left_joystick_lr = 0;		


		//�Ҳ�ҡ������ӳ��-----------------------------------------------------
		if(Right_Joystick_Y_axis_Avg > UP_LEFT_THRESHOLD)
			right_joystick_ud = 1;
		
		else if(Right_Joystick_Y_axis_Avg < DOWN_RIGHT_THRESHOLD)
			right_joystick_ud = 2;
		
		else
			right_joystick_ud = 0;


		//�Ҳ�ҡ������ӳ��-----------------------------------------------------
		if(Right_Joystick_X_axis_Avg > UP_LEFT_THRESHOLD)
			right_joystick_lr = 1;
		
		else if(Right_Joystick_X_axis_Avg < DOWN_RIGHT_THRESHOLD)
			right_joystick_lr = 2;
		
		else
			right_joystick_lr = 0;		
		/*ҡ�˰���ӳ��=========================================================================================*/
		
//		USART_SendData(USART2, 0x89);

		
		K_div++;
		
		//1000������Ӧһ��
		if(K_div > 100)
		{
			K_div = 0;
			
			//��������ʱ�޸������޸�---------------------------------------------------------------------------------			
			//����������������ҡ�����ϲ�
			if((interface_index == 2) && (left_joystick_ud == 1))
			{
				//�ж������Ƿ񵽶�����������һ����
				if(modify_index > 0)
				{
					modify_index--;
				}
				
				//����ָ�������ײ�
				else
				{
					modify_index = PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY - 1;
				}
			}

			//����������������ҡ�����²�
			else if((interface_index == 2) && (left_joystick_ud == 2))
			{
				//�ж������Ƿ񵽵ף������׼�һ����
				if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY - 1)
				{
					modify_index++;
				}
				
				//����ָ����������
				else
				{
					modify_index = 0;
				}				
			}
			//��������ʱ�޸������޸�=================================================================================

			
			//��ǰ�����µĲ��������޸�-------------------------------------------------------------------------------
			//�����Ӳ���������ҡ�����ϲ�
			if((interface_index == 2) && (right_joystick_ud == 1))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 ����ʱ
				if(modify_index < PARAMETER_QUANTITY)
				{
					//�жϵ�ǰ�����Ƿ񵽶������������¸�ֵ���ײ�
					if(Xtend_900_para_buf[modify_index] >= Xtend_900_para_max[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_min[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]++;
					}
				}	

				
				//��������ʱ
				else
				{
					//�жϵ�ǰ�����Ƿ񵽶������������¸�ֵ���ײ�
					if(local_para_conf_buf[modify_index - PARAMETER_QUANTITY] >= local_para_conf_max[modify_index - PARAMETER_QUANTITY])
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY] = local_para_conf_min[modify_index - PARAMETER_QUANTITY];
					}
					
					else
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY]++;
					}					
				}							
			}


			//����������������ҡ�����²�
			else if((interface_index == 2) && (right_joystick_ud == 2))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 ����ʱ
				if(modify_index < PARAMETER_QUANTITY)
				{
					//�жϵ�ǰ�����Ƿ񵽵ף����׺����¸�ֵ������
					if(Xtend_900_para_buf[modify_index] <= Xtend_900_para_min[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_max[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]--;
					}
				}	

				
				//��������ʱ
				else
				{
					//�жϵ�ǰ�����Ƿ񵽵ף����׺����¸�ֵ������
					if(local_para_conf_buf[modify_index - PARAMETER_QUANTITY] <= local_para_conf_min[modify_index - PARAMETER_QUANTITY])
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY] = local_para_conf_max[modify_index - PARAMETER_QUANTITY];
					}
					
					else
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY]--;
					}					
				}							
			}			
			//��ǰ�����µĲ��������޸�===============================================================================
		}
		
		
		//20������Ӧһ��
		KQ_div++;
		
		if(KQ_div >= 2)
		{
			KQ_div = 0;
			
			//��ǰ�����µĲ��������޸�-------------------------------------------------------------------------------
			//�����Ӳ���������ҡ������
			if((interface_index == 2) && (right_joystick_lr == 1))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 ����ʱ
				if(modify_index < PARAMETER_QUANTITY)
				{
					//�жϵ�ǰ�����Ƿ񵽶������������¸�ֵ���ײ�
					if(Xtend_900_para_buf[modify_index] >= Xtend_900_para_max[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_min[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]++;
					}
				}	

				
				//��������ʱ
				else
				{
					//�жϵ�ǰ�����Ƿ񵽶������������¸�ֵ���ײ�
					if(local_para_conf_buf[modify_index - PARAMETER_QUANTITY] >= local_para_conf_max[modify_index - PARAMETER_QUANTITY])
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY] = local_para_conf_min[modify_index - PARAMETER_QUANTITY];
					}
					
					else
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY]++;
					}					
				}							
			}


			//����������������ҡ�����Ҳ�
			else if((interface_index == 2) && (right_joystick_lr == 2))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 ����ʱ
				if(modify_index < PARAMETER_QUANTITY)
				{
					//�жϵ�ǰ�����Ƿ񵽵ף����׺����¸�ֵ������
					if(Xtend_900_para_buf[modify_index] <= Xtend_900_para_min[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_max[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]--;
					}
				}	

				
				//��������ʱ
				else
				{
					//�жϵ�ǰ�����Ƿ񵽵ף����׺����¸�ֵ������
					if(local_para_conf_buf[modify_index - PARAMETER_QUANTITY] <= local_para_conf_min[modify_index - PARAMETER_QUANTITY])
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY] = local_para_conf_max[modify_index - PARAMETER_QUANTITY];
					}
					
					else
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY]--;
					}					
				}							
			}			
			//��ǰ�����µĲ��������޸�===============================================================================			
		}


		
		

		vTaskDelay(10);
	}
}
