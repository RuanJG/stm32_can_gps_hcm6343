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
uint8_t ALARM_Button_Previous = 0;				//ǰһ�εľ������ư���״̬
uint8_t SAMPLE_Button = 0;			//SAMPLE �������أ�0��ʾû�а��£�1��ʾ����
uint8_t MODE_Button = 0;				//MODE ģʽ���أ�0��ʾģʽ0��1��ʾģʽ1��2��ʾģʽ2
uint8_t MODE_Button_Previous = 0;				//ǰһ�εĿ���ģʽ�������ж�ĳЩָ���Ƿ���

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
		if(K_div > 1000)
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
					if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
					{
						modify_index = PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY + VALVE_QUANTITY - 1;
					}
					else
					{
						modify_index = PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY - 1;
					}
				}
			}

			//�����Ӳ���������ҡ�����²�
			else if((interface_index == 2) && (left_joystick_ud == 2))
			{
				//�ж������Ƿ񵽵ף������׼�һ����
				//ME300�������ɿ��Ʒ���----------------------------------------------
				if(local_para_conf[2] == USV_Boat_Type_ME300)						
				{
					if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY + VALVE_QUANTITY - 1)
					{
						modify_index++;
					}
					
					//����ָ����������
					else
					{
						modify_index = 0;
					}		
				}					
		
				//��ME300û�з�����ʾ����---------------------------------------------
				else
				{
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
			}
			//��������ʱ�޸������޸�=================================================================================
			
			
			//��������ʱ�޸������޸�---------------------------------------------------------------------------------			
			//����������������ҡ�����ϲ�
			if((interface_index == 3) && (left_joystick_ud == 1))
			{
				//�ж������Ƿ񵽶�����������һ����
				if(modify_index > 0)
				{
					modify_index--;
				}
				
				//����ָ�������ײ�
				else
				{
					modify_index = WATER_SAMPLE_PARAMETER - 1;
				}
			}

			//�����Ӳ���������ҡ�����²�
			else if((interface_index == 3) && (left_joystick_ud == 2))
			{
				//�ж������Ƿ񵽵ף������׼�һ����
				if(modify_index < WATER_SAMPLE_PARAMETER - 1)
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

			
			//��ǰ�����µĲ��������޸ģ��������ã�-------------------------------------------------------------------------------
			//�����Ӳ���������ҡ�����ϲ�
			if((interface_index == 2) && (right_joystick_ud == 1))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 ����ʱ
				//����ģ���������-----------------------------------------------------------------------
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

				
				//���ز�������-----------------------------------------------------------------------
				else if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY)
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

				
				//���Ų�������-----------------------------------------------------------------------
				else
				{
					//�жϵ�ǰ�����Ƿ񵽶������������¸�ֵ���ײ�
					if(ValveStatus[modify_index - PARAMETER_QUANTITY - LOCAL_PARAMETER_QUANTITY] >= TRUE)
					{
						ValveStatus[modify_index - PARAMETER_QUANTITY - LOCAL_PARAMETER_QUANTITY] = FALSE;
					}
					
					else
					{
						ValveStatus[modify_index - PARAMETER_QUANTITY - LOCAL_PARAMETER_QUANTITY] = TRUE;
					}					
				}
			}


			//����������������ҡ�����²�
			else if((interface_index == 2) && (right_joystick_ud == 2))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 ����ʱ
				//����ģ���������-----------------------------------------------------------------------
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

				
				//���ز�������-----------------------------------------------------------------------
				else if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY)
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
			//��ǰ�����µĲ��������޸ģ��������ã�===============================================================================
			
			
			//��ǰ�����µĲ��������޸ģ��������ã�-------------------------------------------------------------------------------
			//�����Ӳ���������ҡ�����ϲ�
			if((interface_index == 3) && (right_joystick_ud == 1))
			{
				//�жϵ�ǰ�����Ƿ񵽶������������¸�ֵ���ײ�
				if(water_sample_conf_buf[modify_index] >= water_sample_conf_max[modify_index])
				{
					water_sample_conf_buf[modify_index] = water_sample_conf_min[modify_index];
				}
				
				else
				{
					water_sample_conf_buf[modify_index]++;
				}								
			}


			//����������������ҡ�����²�
			else if((interface_index == 3) && (right_joystick_ud == 2))
			{
				//�жϵ�ǰ�����Ƿ񵽵ף����׺����¸�ֵ������
				if(water_sample_conf_buf[modify_index] <= water_sample_conf_min[modify_index])
				{
					water_sample_conf_buf[modify_index] = water_sample_conf_max[modify_index];
				}
				
				else
				{
					water_sample_conf_buf[modify_index]--;
				}
			}											
			//��ǰ�����µĲ��������޸ģ��������ã�===============================================================================
		}
		
		
		//2������Ӧһ��
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

		
		//ģʽ�л�ʱ��־λ����----------------------------------------------------------------------
		if(MODE_Button_Previous != MODE_Button)
		{
			//�����˵��Զ���ˮ״̬���������ɴ��Զ�ָ��
			if(MODE_Button == CONTROL_MODE_AUTO)
			{
				NeedSendAutoModeCommandCount = 2;
//				USART_SendData(USART2, 0x98);
			}			
		}
		
		MODE_Button_Previous = MODE_Button;
		//ģʽ�л�ʱ��־λ����======================================================================
				

		//�������ƿ����л�ʱ��־λ����--------------------------------------------------------------
		if(ALARM_Button_Previous != ALARM_Button)
		{
			NeedSendSpeaker = MAX_SPEAKER_SEND_COUNT;
			NeedSendAlertLED = MAX_ALERT_SEND_COUNT;
		}	

		ALARM_Button_Previous = ALARM_Button;
		//�������ƿ����л�ʱ��־λ����==============================================================


		vTaskDelay(1);
	}
}

