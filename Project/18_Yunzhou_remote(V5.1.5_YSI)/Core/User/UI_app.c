/*-------------------------------------------------------------------------
�������ƣ�ң��������Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "UI_app.h"
#include "WindowDLG.c"
#include "songti16.c"
#include "songti12.c"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Graphic_Interface_Task_Handle;

uint8_t interface_index = MAIN_GRAPHIC_INDEX;			//����������1Ϊ������
																													//					2Ϊ�������ý���
																													//					3Ϊ�������ý���
uint8_t modify_index = 0;					//�����޸�����
uint8_t EngineTimerCount = 5;			//��ֹ��������ʱ��

GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg0n_memdev, bg1n_memdev;								//��ʾ�洢����		


/* Private variables ---------------------------------------------------------*/
char show_message[460];			//��ʾ���յ���Ϣ

uint8_t debug_mode_parameter_init = 1;			//���沽������
uint8_t blink_times = 0;										//��˸����
uint8_t blink_t = 0;												//�����˸

uint8_t debug_change_flag = 1;							//1��ʾ���Խ������������л���0��ʾû�л�

static double lat;//=-2.12345;							//γ�Ȼ���
static double lng;//=-10.12345;							//���Ȼ���

uint16_t show_depth_div = 0;								//���ˢ�·�Ƶ���ӣ���λΪ�ٺ���
uint16_t waveform_xaxis = 0;								//����X��

uint8_t RF_transmit_timer_toler = 0;				//��ʾ���Ͳ���

uint8_t para_set_flag = 0;									//�������ñ�־��0��ʾû�����������ã����յ��������ݣ���Ϊ͸�����ݣ�1��ʾ�ض�����������

uint16_t key_protect_timer = 0;							//������ճ������������

uint16_t wave_last_temp, wave_temp, wave_test_timer = 0;					//���λ����ݴ�

uint32_t RSSI_Timer;

/**
  * @brief  Graphic user interface task
  * @param  pvParameters not used
  * @retval None
  */
void Graphic_Interface_Task(void * pvParameters)
{   
	uint8_t pointi = 0, tempi = 0;
	uint16_t G_div = 0;						//��ʾ��Ƶ����
	uint16_t pointi2 = 0;
	char tempChar[20];
	uint16_t wait_send = 0;
	uint16_t one_second_div = 0;			//1���Ƶ����
	
	/* Initialize the BSP layer */
  LowLevel_Init();
	
	//��������Ĳ���
	load_parameter();
	
  /* Init the STemWin GUI Library */
  GUI_Init(); 
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
//  GUI_SelectLayer(0);

//	GUI_BMP_Draw(_acImage_0, 358, 226);				//Yunzhou logo
	
	GUI_SelectLayer(1);	
	bg1_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);
	bg1w_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);
	bg1n_memdev = GUI_MEMDEV_Create(0, 0, BG1N_XAXIS, BG1N_YAXIS);								//С���Դ���
	
	GUI_SelectLayer(0);	
	bg0_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	bg0s_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG0S_XAXIS, DEBUG_BG0S_YAXIS);
//	bg0n_memdev = GUI_MEMDEV_Create(0, 0, BG0N_XAXIS, BG0N_YAXIS);								//С���Դ���
	
	interface_index = MAIN_GRAPHIC_INDEX;
//	interface_index = MAIN_PARAM1_INDEX;
	local_para_conf[4] = 0;							//������ʾģʽ
//	interface_index = DEBUG_MAIN_INTERFACE_INDEX;	
//	local_para_conf[4] = 1;							//debugģʽ�µĽ�����ʾ
	
	
  /* Run the Graphic_Interface task */
  while (1)
  {			
		G_div++;												//��Ļ��ʾ��ʱ��
		RF_transmit_timer++;						//���ߴ����ʱ��
			
		
		//����ģʽ��ʾ------------------------------------------------------------------------------------------------------	
		if(local_para_conf[4] == 1)
		{			
			//������Ϊ1Ϊ������--------------------------------------------------
			if(interface_index == DEBUG_MAIN_INTERFACE_INDEX)
			{
				//100������Ӧһ����ʾ
				if(G_div >= DEBUG_MAIN_DIVISION)
				{
					G_div = 0;
					
					//����ģʽ��ʾˢ��
					debug_mode_main();
//					USART_SendData(USART1, 0x88);
				}
				
				if(MENU_Button == 1)				//���������°���menu������������ý���
				{
					interface_index = DEBUG_PARAM_INTERFACE_INDEX;
					debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
					LCD_clear();						//����Ļ
				}

				//���������²��� SAMPLE ���� ť�ӿ��ؽ���������ý��棨���в�ˮ���ܵĴ�ֻ��
				else if((SAMPLE_Button == 1) && (MODE_Button != CONTROL_MODE_AUTO))			
				{
//					USART_SendData(USART2, local_para_conf[2]);
					switch(local_para_conf[2])
					{						
						case USV_Boat_Type_SS30:
						case USV_Boat_Type_ES30:
						case USV_Boat_Type_ESM30:
						case USV_Boat_Type_MS70:
						case USV_Boat_Type_MM70:
						case USV_Boat_Type_SS20:
						case USV_Boat_Type_ESM20:
						case USV_Boat_Type_MS120:
						case USV_Boat_Type_MM120:
							//if(GlobalVariable.IsManualSampling != TRUE)
			
							interface_index = DEBUG_SAMPLE_INTERFACE_INDEX;
							debug_mode_parameter_init = 1;
							load_sample_config();					//��ȡ��������
							LCD_clear();						//����Ļ							
							break;
						
						case USV_Boat_Type_ME300:
							if(ALARM_Button != FALSE && EngineTimerCount >= 5)
							{
//								EngineTimerCount = 0;
								//LCD_EngineStart_Window();
								if(EngineStarted != TRUE)
								{
									EngineTimerCount = 0;
									EngineStarted = TRUE;
									NeedSendAlertLED = MAX_ALERT_SEND_COUNT;
//									USART_SendData(USART2, 0x68);
								}
							}						
							else if(ALARM_Button == FALSE)
							{
								EngineTimerCount = 0;
								StopEngineTrigger();
							}
							
							break;
						default:
							break;
					}																
				}
				
				else 
				{
					//ֹͣ����������
					StopEngineTrigger();
				}
				
				
				//�������������----------------------------
				if(RF_transmit_timer >= (RF_TRANSMIT_TIMES - RF_TRANSMIT_TIMES_TOLER))					
				{
					RF_transmit_timer = 0;
					GenerateCommandBase64(TRUE);					//���������·���������������
					
//					NeedSendAlertLED = 1;
//					NeedSendSpeaker = 1;
//					GenerateAlarmCommandBase64(TRUE);
				}			
			}
			
			//������Ϊ2Ϊ�������ý���--------------------------------------------------
			else if(interface_index == DEBUG_PARAM_INTERFACE_INDEX)
			{				
				if(debug_mode_parameter_init == 1)
					debug_mode_parameter();
				
				//500������Ӧһ����ʾ
				if(G_div >= DEBUG_PARAMETER_DIVISION)
				{
					G_div = 0;
					debug_mode_parameter();
				}
				
				//ȷ�ϱ���-------------------------------------------------
				if(OK_Button == 1)				//�ڲ������ý����°���OK���󣬱��浱ǰ���������������Խ���
				{
					save_parameter();						//���浱ǰ������STM32��FLASH��
					save_RF_parameter();				//��������ģ�����	
					LCD_PWM_Config(ENABLE);								//Һ�������޸�					
					
					if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
					{
						save_Valve_parameter();			//���淧�Ų���
					}									

					LCD_clear();						//����Ļ
					
					//����ģʽ�ж�---------------------------------------------------------------
					if(local_para_conf[4] == 1)				//���ֵ���ģʽ
						interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//���ص���������	
					
					else
					{
						interface_index = MAIN_GRAPHIC_INDEX;										//��������������	
						debug_change_flag	= 1;					
					}					
					//����ģʽ�ж�===============================================================

					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�
				}
				
				
				//ȡ������-------------------------------------------------
				else if(CANCEL_Button == 1)				//�ڲ������ý����°���CANCEL���󣬲����浱ǰ���������������Խ���
				{
					not_save_RF_parameter();				//ȡ������
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�
					
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//���ص���������	
					LCD_clear();						//����Ļ
				}
				
				//����ȡ������Ϻ�MENU���������LCD����ģʽ
				else if((debug_mode_parameter_init == 5) && (MENU_Button == 1))
				{
					interface_index = DEBUG_LCD_TEST_INDEX;
					G_div = 0;
				}			
			}

			//������Ϊ3Ϊ��ˮ���ò��Խ���-------------------------------------------------
			else if(interface_index == DEBUG_SAMPLE_INTERFACE_INDEX)
			{
				//��һ�ο����������
				if(debug_mode_parameter_init == 1)
				{	
					debug_mode_parameter_init = 0;
					sample_mode_config();
				}

				//500������Ӧһ����ʾ
				if(G_div >= DEBUG_PARAMETER_DIVISION)
				{
					G_div = 0;
					sample_mode_config();
				}

				if(OK_Button == 1)						//�ڲ������ý����°���OK���󣬱��浱ǰ���������������Խ���
				{
					save_sample_config();				//���浱ǰ������������
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;					//���ص���������
					NeedSendSampleCount = 10;		//����10�β�ˮ����
					GlobalVariable.IsManualSampling = TRUE;								//���ڲ�����־
					LCD_clear();								//����Ļ					
				}		

				else if(CANCEL_Button == 1)				//�ڲ������ý����°���CANCEL���󣬲����浱ǰ���������������Խ���
				{
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;
					LCD_clear();						//����Ļ
				}				
			}
			
			//������Ϊ4ΪҺ�����Խ���-------------------------------------------------
			else if(interface_index == DEBUG_LCD_TEST_INDEX)
			{													
				G_div = LCD_Test(G_div);				
			}
		}
		//����ģʽ��ʾ======================================================================================================	
		
		
		//������ʾģʽ------------------------------------------------------------------------------------------------------
		else
		{
			//��������ʾ-------------------------------------------------------
			if(interface_index == MAIN_GRAPHIC_INDEX)
			{								
				if(MENU_Button == 1)				//���������°���menu������������ý���
				{
					interface_index = MAIN_PARAM1_INDEX;
					debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
					debug_change_flag	= 1; 								//��ʼ����־��1
					key_protect_timer = 0;								//ճ��������������
					LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
					LCD_clear();						//����Ļ
				}				
				
				//���������²��� SAMPLE ���� ť�ӿ��ؽ���������ý��棨���в�ˮ���ܵĴ�ֻ��
				else if((SAMPLE_Button == 1) && (MODE_Button != CONTROL_MODE_AUTO))			
				{
//					USART_SendData(USART2, local_para_conf[2]);
					switch(local_para_conf[2])
					{						
						case USV_Boat_Type_SS30:
						case USV_Boat_Type_ES30:
						case USV_Boat_Type_ESM30:
						case USV_Boat_Type_MS70:
						case USV_Boat_Type_MM70:
						case USV_Boat_Type_SS20:
						case USV_Boat_Type_ESM20:
						case USV_Boat_Type_MS120:
						case USV_Boat_Type_MM120:
							//if(GlobalVariable.IsManualSampling != TRUE)
			
							interface_index = MAIN_SAMPLE_INDEX;
							debug_mode_parameter_init = 1;
							load_sample_config();					//��ȡ��������
							LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
							LCD_clear();						//����Ļ							
							break;
						
						case USV_Boat_Type_ME300:
							if(ALARM_Button != FALSE && EngineTimerCount >= 5)
							{
//								EngineTimerCount = 0;
								//LCD_EngineStart_Window();
								if(EngineStarted != TRUE)
								{
									EngineTimerCount = 0;
									EngineStarted = TRUE;
									NeedSendAlertLED = MAX_ALERT_SEND_COUNT;
//									USART_SendData(USART2, 0x68);
								}
							}						
							else if(ALARM_Button == FALSE)
							{
								EngineTimerCount = 0;
								StopEngineTrigger();
							}
							
							break;
						default:
							break;
					}																
				}
				
				else 
				{
					//ֹͣ����������
					StopEngineTrigger();
				}
				
				
				//������������ͣ�������������� 200ms һ����----------------------------
				if(RF_transmit_timer >= (RF_TRANSMIT_TIMES - RF_TRANSMIT_TIMES_TOLER - RF_transmit_timer_toler))					
				{
					RF_transmit_timer = 0;
					GenerateCommandBase64(TRUE);					//���������·���������������
				}	
				
				
				//100������Ӧһ����ʾ
				if(G_div >= MAIN_GRAPHIC_DIVISION)
				{
					G_div = 0;
					main_graphic_show();
					
					RF_transmit_timer_toler = 3;					//Ϊ������ʾ���µ����������ӳ٣����´��������͵�ʱ������
				}	
			}
			//��������ʾ=======================================================

			
			//�����̨�������ý���---------------------------------------------
			else if(interface_index == MAIN_PARAM1_INDEX)
			{
				//100������Ӧһ����ʾ
				if(G_div >= MAIN_PARAM1_DIVISION)
				{
					G_div = 0;
					main_param_show();
				}	

				
				//menu��ճ������
				key_protect_timer++;
				
				if(key_protect_timer < KEY_PROTECT_TIME)						//�ڱ���ʱ���ڲ���Ӧ��Ӧ����
				{
					MENU_Button = 0;
				}
				else
				{
					key_protect_timer = KEY_PROTECT_TIME;
					
					//ȷ�ϱ���-------------------------------------------------
					if(OK_Button == 1)						//�ڲ������ý����°���OK���󣬱��浱ǰ���������������Խ���
					{
						show_save_wait();						//��ʾ�洢������Ϣ
						save_parameter();						//���浱ǰ������STM32��FLASH��
						save_RF_parameter();				//��������ģ�����	
//						LCD_PWM_Config(ENABLE);								//Һ�������޸�					
											
						
						//����ģʽ�ж�---------------------------------------------------------------
						if(local_para_conf[4] == 1)				//���ֵ���ģʽ
							interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//���ص���������	
						
						else
						{
							interface_index = MAIN_GRAPHIC_INDEX;										//��������������	
							debug_change_flag	= 1;					
						}					
						//����ģʽ�ж�===============================================================
						
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();						//����Ļ

						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�
						
						para_set_flag = 1;						//�������ú���
					}
					
					
					//ȡ������-------------------------------------------------
					else if(CANCEL_Button == 1)				//�ڲ������ý����°���CANCEL���󣬲����浱ǰ����������������
					{
//					not_save_RF_parameter();				//ȡ������

						interface_index = MAIN_GRAPHIC_INDEX;						//����������	
						debug_change_flag	= 1;													//�����ʼ�����
						para_set_flag = 1;															//�������ú���
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();																		//����Ļ
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�
					}

					//�˵�ѡ��------------------------------------------------
					else if(MENU_Button == 1)							//�ڲ��������°�menu�������������2�������ϵ��Ϣ����
					{
						//ME300�������ɿ��Ʒ���
						if(local_para_conf[2] == USV_Boat_Type_ME300)						
						{
							interface_index = MAIN_PARAM2_INDEX;
						}
						
						//�����������ϵ��Ϣ����
						else
						{
							interface_index = MAIN_CONTACT_INDEX;
						}
						
						debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
						debug_change_flag	= 1; 								//��ʼ����־��1
						
						key_protect_timer = 0;
						
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();						//����Ļ
					}							
				}						
			}			
			//�����̨�������ý���=============================================			
			
			
			//����ME300�������ý���--------------------------------------------
			else if(interface_index == MAIN_PARAM2_INDEX)
			{
				//100������Ӧһ����ʾ--------------------------------------
				if(G_div >= MAIN_PARAM1_DIVISION)
				{
					G_div = 0;
					main_valve_show();
				}		


				//menu��ճ������
				key_protect_timer++;

				if(key_protect_timer < KEY_PROTECT_TIME)						//�ڱ���ʱ���ڲ���Ӧ��Ӧ����
				{
					MENU_Button = 0;
				}
				else
				{
					key_protect_timer = KEY_PROTECT_TIME;
					
					//ȷ�ϱ���-------------------------------------------------
					if(OK_Button == 1)						//�ڲ������ý����°���OK���󣬱��浱ǰ���������������Խ���
					{
						if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
						{
							save_Valve_parameter();			//���淧�Ų���
						}	

						interface_index = MAIN_GRAPHIC_INDEX;						//����������		
						debug_change_flag	= 1;													//�����ʼ�����	
						para_set_flag = 1;															//�������ú���
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();																		//����Ļ
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�															
					}
					
					//ȡ������-------------------------------------------------
					else if(CANCEL_Button == 1)				//�ڲ������ý����°���CANCEL���󣬲����浱ǰ����������������
					{
						interface_index = MAIN_GRAPHIC_INDEX;						//����������		
						debug_change_flag	= 1;													//�����ʼ�����	
						para_set_flag = 1;															//�������ú���
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();																		//����Ļ
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�												
					}
		
					//�˵�ѡ��------------------------------------------------
					else if(MENU_Button == 1)							//��menu��������ϵ��ʽ����
					{		
						interface_index = MAIN_CONTACT_INDEX;
						
						debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
						debug_change_flag	= 1; 								//��ʼ����־��1
						
						key_protect_timer = 0;
						
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();						//����Ļ
					}							
				}			
			}			
			//����ME300�������ý���============================================	


			//������ϵ��ʽ����-------------------------------------------------
			else if(interface_index == MAIN_CONTACT_INDEX)	
			{
				//100������Ӧһ����ʾ------------------------------
				if(G_div >= MAIN_GRAPHIC_DIVISION)
				{
					G_div = 0;
					main_contact_show();
				}
				
				
				//menu��ճ������
				key_protect_timer++;
				
				if(key_protect_timer < KEY_PROTECT_TIME)						//�ڱ���ʱ���ڲ���Ӧ��Ӧ����
				{
					MENU_Button = 0;
				}
				else
				{
					key_protect_timer = KEY_PROTECT_TIME;
					
					
					//����ϵ��ʽ�����°���OK �� CANCEL��������������ֱ�ӷ���������
					if((OK_Button == 1) || (CANCEL_Button == 1))
					{
						interface_index = MAIN_GRAPHIC_INDEX;						//����������		
						debug_change_flag	= 1;													//�����ʼ�����	
						para_set_flag = 1;															//�������ú���
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();																		//����Ļ
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�							
					}
					
					//�ٴΰ��²˵���-------------------------------------
					else if(MENU_Button == 1) 
					{
						interface_index = MAIN_PARAM1_INDEX;
						debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
						debug_change_flag	= 1; 								//��ʼ����־��1
						
						key_protect_timer = 0;
						
						LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
						LCD_clear();						//����Ļ				
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�							
					}					
				}
			}							
			//������ϵ��ʽ����=================================================
			
			
			//�����ˮ�������ý���---------------------------------------------
			else if(interface_index == MAIN_SAMPLE_INDEX)
			{
				//100������Ӧһ����ʾ
				if(G_div >= MAIN_GRAPHIC_DIVISION)
				{
					G_div = 0;
					main_sample_show();
				}

				if(OK_Button == 1)							//�ڲ������ý����°���OK���󣬱��浱ǰ���������������Խ���
				{
					save_sample_config();																//���浱ǰ������������
					interface_index = MAIN_GRAPHIC_INDEX;								//����������
					
					NeedSendSampleCount = 10;		//����10�β�ˮ����
					GlobalVariable.IsManualSampling = TRUE;								//���ڲ�����־
					SampleInfo.IsBottleTaking[water_sample_conf[0] - 1] = TRUE;						//���ò������
					SampleInfo.VolumeTaken[water_sample_conf[0] - 1] = water_sample_conf[3];			//���ò�������
					
					debug_change_flag	= 1;	
					LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
					LCD_clear();								//����Ļ					
				}		

				else if(CANCEL_Button == 1)				//�ڲ������ý����°���CANCEL���󣬲����浱ǰ���������������Խ���
				{
					interface_index = MAIN_GRAPHIC_INDEX;
					debug_change_flag	= 1;	
					LCD_PWM_Config(DISABLE);							//ˢ��Ļǰ�ر����
					LCD_clear();						//����Ļ
				}						
			}	
			//�����ˮ�������ý���=============================================			
		}		
		//������ʾģʽ======================================================================================================
		

//		//�������������
//		ExecTouch();
		
		wait_send++;
		
		if(wait_send > 100)
		{
			wait_send = 0;
//			printf("aaaaa\n");
//			USART_SendData(USART2, 0x86);
//			printf("L_UD: %d, L_LR: %d, R_UD: %d, R_LR: %d\n", left_joystick_ud, left_joystick_lr, right_joystick_ud, right_joystick_lr);
//			printf("%x\n", *(__IO uint32_t*)(0x080e0000 - 4));
		}	
		
		one_second_div++;
		
		//���Ƶ
		if(one_second_div >= 100)
		{
			one_second_div = 0;
			
			//�������ʱ�䲻�ܳ���5��---------------------------
			
			EngineTimerCount++;
			
			if(EngineTimerCount > 10)
			{
				EngineTimerCount = 10;
			}
			if((EngineTimerCount >= 5) && (EngineStarted != FALSE))
			{
				EngineTimerCount = 0;
				StopEngineTrigger();
			}
			//�������ʱ�䲻�ܳ���5��===========================
			
			
			//�����ݳ�ʱ����
			BoatDataTimeoutCount++;
			
			//�������ӳ�ʱ
			if(BoatDataTimeoutCount > LINKTIMEOUT)
			{
				connect_ok = 0;
			}
			
			if(BoatDataTimeoutCount > 2 * BOATDATATIMEOUTVALUE)
			{
				BoatDataTimeoutCount = BOATDATATIMEOUTVALUE;
			}	

			//���ӳ�ʱ����			
//			if((BoatDataTimeoutCount > 5) && (interface_index == DEBUG_MAIN_INTERFACE_INDEX))
			if((BoatDataTimeoutCount > 5) && (interface_index == MAIN_GRAPHIC_INDEX))
			{
				if(BoatDataTimeoutCount % 2)
				{
					local_para_conf_buf[5] = 0;				//�ر�����
					Buzzer_PWM_Config();
				}
				else
				{
					local_para_conf_buf[5] = local_para_conf[5];				//��������
					Buzzer_PWM_Config();
				}
			}					
		}
		
//		GUI_Exec();
//		WM_Exec();		
//		USART_SendData(USART1, 0x86);
		
    vTaskDelay(10);
  }
}


/*-------------------------------------------------------------------------
	��������main_valve_show
	��  �ܣ�����������ʾ����
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void main_valve_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;
	
	//ִ�з������ý����ʼ��
	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
		
		//����״̬��-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//�滭 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//���Ʊ���
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_DispStringAt(_famenshezhi[0], 54, 18);								//��������
		}	
		else
		{
			GUI_DispStringAt("Valve Configuration", 54, 18);					
		}
		
		//���ƹ��ɴ���
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//�滭����һ��״̬��
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);		


		//��ȡ��ǰ���Ų���
		load_Valve_parameter();	

		LCD_PWM_Config(ENABLE);							//��ʼ����ϣ��򿪱����
	}
	
	
	//��ʾ��ˮ������ز���----------------------------------------
	//��ʾǰ����Ļ
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//��ʾ����������ز���
	if(local_para_conf[6] == 1)						//����
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_yihaofamen[0], 48, 17);										//1�ŷ���
		GUI_DispStringAt(":", 111, 17);		
		GUI_DispStringAt(_erhaofamen[0], 48, 42);										//2�ŷ���
		GUI_DispStringAt(":", 111, 42);		
		GUI_DispStringAt(_sanhaofamen[0], 48, 67);									//3�ŷ���
		GUI_DispStringAt(":", 111, 67);		
		GUI_DispStringAt(_sihaofamen[0], 48, 92);										//4�ŷ���
		GUI_DispStringAt(":", 111, 92);		
	}	
	else
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("Valve 1:", 60, 17);	
		GUI_DispStringAt("Valve 2:", 60, 42);	
		GUI_DispStringAt("Valve 3:", 60, 67);	
		GUI_DispStringAt("Valve 4:", 60, 92);	
	}	
	
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font16_1);	
	
	//��ȡ����������ز���	
	for(temp_i = 0; temp_i < VALVE_QUANTITY - 1; temp_i++)
	{
		if(ValveStatus[temp_i])
		{
			GUI_DispStringAt("ON", 128, 17 + 25 * temp_i);
		}
		else
		{
			GUI_DispStringAt("OFF", 128, 17 + 25 * temp_i);
		}
	}
	
	//�滭���λ��
	if(modify_index < 4)
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + modify_index* 25);			
	}
		
	//ˢ��LCD����
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);	


	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//��ʾ����������ز���
	if(local_para_conf[6] == 1)						//����
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_bengshuifamen[0], 48, 17);										//��ˮ����
		GUI_DispStringAt(":", 111, 17);		
	}	
	else
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("Pump:", 60, 17);	
	}	

	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font16_1);	
	
	//��ȡ����������ز���	
	if(ValveStatus[VALVE_QUANTITY - 1])
	{
		GUI_DispStringAt("ON", 128, 17);
	}
	else
	{
		GUI_DispStringAt("OFF", 128, 17);
	}
	
	//�滭���λ��
	if((modify_index >= 4) && (modify_index < 8))
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 4) * 25);			
	}
		
	//ˢ��LCD����
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);			
}


/*-------------------------------------------------------------------------
	��������main_contact_show
	��  �ܣ���ϵ��ʽ��Ļ��ʾ����
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void main_contact_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;
	
	//ִ�з������ý����ʼ��
	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
		
		//����״̬��-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//�滭 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//���Ʊ���
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_DispStringAt(_lianxifangshi[0], 54, 18);								//��ϵ��ʽ
		}	
		else
		{
			GUI_DispStringAt("Contact Us", 54, 18);					
		}
		
		//���ƹ��ɴ���
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//�滭����һ��״̬��
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);

		
		//��ʾ��ͼ��Ϣ
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		GUI_BMP_Draw(_acImage_20, 0, 0);											//��˾��ַ��ͼ

		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS + 20, 60);
		
		
		//��ʾ��ά�뼰��˾logo
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
		GUI_SetBkColor(GUI_TRANSPARENT);

		GUI_BMP_Draw(_acImage_21, 10, 20);											//��ҳ��ά��
		GUI_BMP_Draw(_acImage_22, 118, 20);											//��˾logo

		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 146);
		
		LCD_PWM_Config(ENABLE);							//��ʼ����ϣ��򿪱����
	}
	
	
	//��ʾ��ϵ��ʽ----------------------------------------
	//��ʾǰ����Ļ
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	
	//��ʾ��ϵ��Ϣ
	if(local_para_conf[6] == 1)						//����
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt(_lianxidizhi[0], 8, 17);												//��ϵ��ַ
		GUI_DispStringAt(":", 73, 17);
		
		GUI_SetColor(GUI_RED);		
		GUI_DispStringAt(_zhongguoguangdongshengzhuhaishi[0], 36, 42);										//�й��㶫ʡ�麣�и�����
		GUI_DispStringAt(_nanfangruanjianyuan[0], 8, 63);																	//�Ϸ����԰D2-214
		
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt(_youzhengbianma[0], 8, 92);										//��������
		GUI_DispStringAt(":", 73, 92);
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("519080", 86, 92);
	}	
	else
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font16_1);	
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt("Address:", 8, 17);
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("D2-214, No.1 Software Road, Zhuhai", 36, 42);
		GUI_DispStringAt("Hi-tech Zone, Zhuhai, Guangdong, China.", 8, 63);

		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt("Zip Code:", 8, 92);	
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("519080", 73, 92);			
	}	
			
	//ˢ��LCD����
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);

	
	//��һ��������
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	GUI_SetBkColor(GUI_TRANSPARENT);

	//��ʾ��ϵ��Ϣ
	if(local_para_conf[6] == 1)						//����
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_SetColor(GUI_BLUE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_dianhuahaoma[0], 8, 17);											//�绰����
		GUI_DispStringAt(":", 73, 17);			
		GUI_SetColor(GUI_RED);	
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("+86(756)3626636", 86, 17);
		
		GUI_SetColor(GUI_BLUE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_chuanzhenhaoma[0], 8, 42);										//�������
		GUI_DispStringAt(":", 73, 42);			
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("+86(756)3626619", 86, 42);		
		
		GUI_SetColor(GUI_BLUE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_dianziyoujian[0], 8, 67);											//�����ʼ�
		GUI_DispStringAt(":", 73, 67);			
		GUI_SetColor(GUI_RED);	
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("info@yunzhou-tech.com", 86, 67);	

		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt(_guanfangwangzhi[0], 8, 92);										//�ٷ���ַ
		GUI_DispStringAt(":", 73, 92);			
		GUI_SetColor(GUI_RED);	
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("www.yunzhou-tech.com", 86, 92);	
	}	
	else
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font16_1);	
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt("Tel Num:", 8, 17);
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("+86(756)3626636", 73, 17);

		GUI_SetColor(GUI_BLUE);		
		GUI_DispStringAt("Fax Num:", 8, 42);	
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("+86(756)3626619", 73, 42);	

		GUI_SetColor(GUI_BLUE);				
		GUI_DispStringAt("Email:", 8, 67);
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("info@yunzhou-tech.com", 73, 67);

		GUI_SetColor(GUI_BLUE);					
		GUI_DispStringAt("Website:", 8, 92);	
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("www.yunzhou-tech.com", 73, 92);			
	}	
			
	//ˢ��LCD����
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);	
}


/*-------------------------------------------------------------------------
	��������main_sample_show
	��  �ܣ���ˮ������Ļ��ʾ����
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void main_sample_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;
	
	//ִ�в�ˮ�����ʼ��
	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
		
		//����״̬��-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//�滭 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//���Ʊ���
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_DispStringAt(_caiyangshezhi[0], 54, 18);								//��������
		}	
		else
		{
			GUI_DispStringAt("Sampling Configuration", 50, 18);					
		}
		
		//���ƹ��ɴ���
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//�滭����һ��״̬��
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);		


		//����������ƿ��
		if(local_para_conf[2] <= 6)							//С�ʹ����4��ƿ
		{
			water_sample_conf_max[0] = 4;
		}
		else if(local_para_conf[2] <= 11)				//���ʹ����8��ƿ
		{
			water_sample_conf_max[0] = 8;
		}		
		else 																		//���ʹ����12��ƿ
		{
			water_sample_conf_max[0] = 12;
		}			

		LCD_PWM_Config(ENABLE);							//��ʼ����ϣ��򿪱����
	}
	
	//��ʾ��ˮ������ز���----------------------------------------
	//��ʾǰ����Ļ
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//��ʾ����������ز���
	if(local_para_conf[6] == 1)						//����
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_caiyangpinghao[0], 48, 17);										//����ƿ��
		GUI_DispStringAt(":", 111, 17);		
		GUI_DispStringAt(_caiyangshendu[0], 48, 42);										//�������
		GUI_DispStringAt(":", 111, 42);		
		GUI_DispStringAt(_caiyangmoshi[0], 48, 67);											//����ģʽ
		GUI_DispStringAt(":", 111, 67);		
		GUI_DispStringAt(_caiyangrongliang[0], 48, 92);									//��������
		GUI_DispStringAt(":", 111, 92);		
	}	
	else
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("Bottle Number:", 48, 17);	
		GUI_DispStringAt("Sample Depth:", 48, 42);	
		GUI_DispStringAt("Sample Mode:", 48, 67);	
		GUI_DispStringAt("Water Volume:", 48, 92);	
	}	
	
	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font16_1);	
	
	//��ȡ����������ز���
	for(temp_i = 0; temp_i < 4; temp_i++)
	{
		float_to_string(water_sample_conf_buf[temp_i], tempChar, 0, 0, 0, 0);
		GUI_DispStringAt(tempChar, 140, 17 + 25 * temp_i);
	}	
	
	//�滭���λ��
	if(modify_index < 4)
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + modify_index* 25);			
	}
		
	//ˢ��LCD����
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);	


	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//��ʾ����������ز���
	if(local_para_conf[6] == 1)						//����
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_qingximoshi[0], 48, 17);										//��ϴģʽ
		GUI_DispStringAt(":", 111, 17);		
	}	
	else
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("Clean Mode:", 48, 17);	
	}	

	GUI_SetColor(GUI_RED);
	GUI_SetFont(&GUI_Font16_1);	
	
	//��ȡ����������ز���
	float_to_string(water_sample_conf_buf[4], tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 140, 17);
	
	//�滭���λ��
	if((modify_index >= 4) && (modify_index < 8))
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 4) * 25);			
	}
		
	//ˢ��LCD����
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);		
}
	


/*-------------------------------------------------------------------------
	��������main_param_show
	��  �ܣ�����������Ļ��ʾ����
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void main_param_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;

	//Step1: �������������л���һ�γ�ʼ��  -----------------------------------------------------------------------
	if(debug_mode_parameter_init == 1)
	{
//		debug_change_flag = 0;

//		local_para_conf[6] = 0;
		
		LCD_clear();
		
		//����״̬��-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//�滭 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//���Ʊ���
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_DispStringAt(_canshushezhi[0], 54, 18);								//��������
		}	
		else
		{
			GUI_DispStringAt("USV Configuration", 54, 18);					
		}
		
		//���ƹ��ɴ���
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//�滭����һ��״̬��
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt(version_number, 180 - 60, 18);						//��ʾ�汾��
		GUI_DispStringAt(release_date, 180 - 8, 18);									//��ʾ��������
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);				
	
	
		//��ʾ����ģ����ز���--------------------------------------
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tongxinpindao[0], 48, 17);								//ͨ��Ƶ��
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_tongxinbianhao[0], 48, 42);								//ͨ�ű��
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_bendidizhi[0], 48, 67);										//���ص�ַ
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_mudidizhi[0], 48, 92);										//Ŀ�ĵ�ַ
			GUI_DispStringAt(":", 111, 92);			
		}	
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			
			GUI_DispStringAt("Channel:", 60, 17);
			GUI_DispStringAt("Comm ID:", 60, 42);
			GUI_DispStringAt("My Addr:", 60, 67);
			GUI_DispStringAt("Dt Addr:", 60, 92);		
		}
		
		//��ȡ����ģ����ز���
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		
		for(temp_i = 0; temp_i < 4; temp_i++)
		{
			float_to_string(Xtend_900_para[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 128, 17 + 25 * temp_i);
		}		

		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);

		
		//�°벿�ִ洢�豸����
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_dizhiyanma[0], 48, 17);										//��ַ����
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_fashegonglv[0], 48, 42);									//���书��
			GUI_DispStringAt(":", 111, 42);		
			GUI_DispStringAt(_diantaimoshi[0], 48, 67);									//��̨ģʽ
			GUI_DispStringAt(":", 111, 67);		
			GUI_DispStringAt(_diantaiwendu[0], 48, 92);									//��̨�¶�
			GUI_DispStringAt(":", 111, 92);		
		}	
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			GUI_DispStringAt("Mask ID:", 60, 17);	
			GUI_DispStringAt("TX Power:", 60, 42);	
			GUI_DispStringAt("RF Mode:", 60, 67);	
			GUI_DispStringAt("RF Temp:", 60, 92);	
		}

		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		
		//��ȡ����ģ����ز���		
		float_to_string(Xtend_900_para[4], tempChar, 0, 0, 0, 0);				//��ַ����
		GUI_DispStringAt(tempChar, 128, 17);
		GUI_DispStringAt(get_TX_Power(Xtend_900_para[5]), 128, 42);			//��̨���书��
		float_to_string(Xtend_900_para[6], tempChar, 0, 0, 0, 0);				//APIģʽ
		GUI_DispStringAt(tempChar, 128, 67);	
		
		GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
		float_to_string(Xtend_900_para[7], tempChar, 0, 0, 0, 0);				//��̨�¶�
		GUI_DispStringAt(tempChar, 156, 92);
		
		GUI_SetFont(&GUI_Fontsongti12);			
		GUI_DispStringAt(_sheshidu[0], 159, 94);													//���϶� ��
			
		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);	


		//����ң�ؼ�USV����------------------------------------------
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_pingmuliangdu[0], 48, 17);								//��Ļ����
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_tuijinmoshi[0], 48, 42);									//�ƽ�ģʽ
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_chuanxingxuanze[0], 48, 67);							//����ѡ��
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_duojizhongwei[0], 48, 92);								//�����λ
			GUI_DispStringAt(":", 111, 92);				
		}
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			
			GUI_DispStringAt("Dimming:", 60, 17);		
			GUI_DispStringAt("Propellor:", 60, 42);	
			GUI_DispStringAt("USV Type:", 60, 67);
			GUI_DispStringAt("Mid-point:", 60, 92);				
		}

		//��ȡң�ؼ����Ͳ���
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);	
		
		float_to_string(local_para_conf[0], tempChar, 0, 0, 0, 0);						//��Ļ���� 			
		GUI_DispStringAt(tempChar, 128, 17);
		float_to_string(local_para_conf[1] + 1, tempChar, 0, 0, 0, 0);				//�ƽ�ģʽ 			
		GUI_DispStringAt(tempChar, 128, 42);								
		GUI_DispStringAt(GetBoatTypeText(local_para_conf[2]), 128, 67);				//����ѡ��
		float_to_string(local_para_conf[3], tempChar, 0, 0, 0, 0);						//�����λ			
		GUI_DispStringAt(tempChar, 128, 92);		
	
		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 46);
		
		
		//�°벿����
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tiaoshimoshi[0], 48, 17);										//����ģʽ
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_shengyinshezhi[0], 48, 42);									//��������
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_yuyanshezhi[0], 48, 67);										//��������
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_tongxinxieyi[0], 48, 92);										//ͨ��Э��
			GUI_DispStringAt(":", 111, 92);				
		}
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			
			GUI_DispStringAt("Debugging:", 60, 17);		
			GUI_DispStringAt("Sound Set:", 60, 42);	
			GUI_DispStringAt("Language:", 60, 67);
			GUI_DispStringAt("Protocol:", 60, 92);									
		}	

		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);	
		
		GUI_DispStringAt(Get_On_Off(local_para_conf[4]), 128, 17);				//����ģʽ
		GUI_DispStringAt(Get_On_Off(local_para_conf[5]), 128, 42);				//��������

		if(local_para_conf[6] == 1)												//����
		{
			GUI_SetFont(&GUI_Fontsongti16);
			GUI_DispStringAt(_zhongwen[0], 128, 67);				//����
		}			
		else
		{
			GUI_SetFont(&GUI_Font16_1);
			GUI_DispStringAt("English", 128, 67);
		}
		
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("V2", 128, 92);									//ͨ��Э��
				
		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 146);			
		
		//�����ʼ���ڶ���
		debug_mode_parameter_init = 2;
		blink_times = ATCOMMAND_DELAY / 2;

		
		//���ƶ�ȡ������ʾ
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);	
		GUI_SetColor(GUI_TRANSPARENT);		
		GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	

		GUI_BMP_Draw(_acImage_14, 80, 0);											//��̾��ͼ��

		GUI_SetColor(GUI_YELLOW);
		GUI_SetBkColor(GUI_TRANSPARENT);	
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
			GUI_SetFont(&GUI_Fontsongti16);	
			GUI_DispStringAt(_canshuduquzhong[0], 100, 60);				//������ȡ��
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(_qingshaohou[0], 100, 85);						//���Ժ�
		}
		else
		{
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
			GUI_SetFont(&GUI_Font16_1);					
			GUI_DispStringAt("Loading", 100, 60);	
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt("Please wait...", 100, 85);					
		}																				

		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 130, 48);
		
		
		LCD_PWM_Config(ENABLE);							//��ʼ����ϣ��򿪱����
		
		//����ATָ��ļ��
		vTaskDelay(1200);
	}
	//Step1: �������������л���һ�γ�ʼ��  =======================================================================	
	
	
	//Step2: �ȴ�AT "+++"	ָ��ص�2��	"OK"  --------------------------------------------------------------------
	else if(debug_mode_parameter_init == 2)
	{
		while(blink_times > 0)
		{
			if(blink_times == ATCOMMAND_DELAY / 2)
				enter_AT_Command();					//����AT����ģʽ������ +++
			
//			//��˸��ʾ����
//			GUI_SelectLayer(1);
//			GUI_MEMDEV_Select(bg1_memdev);	
//			GUI_MEMDEV_Clear(bg1_memdev);	
//			GUI_SetColor(GUI_TRANSPARENT);		
//			GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
//			
//			//��˸Ч��
//			if(blink_times % 2 == 0)								//��2����
//			{	
//				GUI_BMP_Draw(_acImage_14, 80, 0);											//��̾��ͼ��
//		
//				GUI_SetColor(GUI_YELLOW);
//				GUI_SetBkColor(GUI_TRANSPARENT);	
//				
//				if(local_para_conf[6] == 1)						//����
//				{
//					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
//					GUI_SetFont(&GUI_Fontsongti16);	
//					GUI_DispStringAt(_canshuduquzhong[0], 100, 60);				//������ȡ��
//					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
//					GUI_DispStringAt(_qingshaohou[0], 100, 85);						//���Ժ�
//				}
//				else
//				{
//					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
//					GUI_SetFont(&GUI_Font16_1);					
//					GUI_DispStringAt("Loading", 100, 60);	
//					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
//					GUI_DispStringAt("Please wait...", 100, 85);					
//				}																				
//			}

//			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 130, 48);

			//����750����
			vTaskDelay(750);
			
			blink_times --;
		}
		
		debug_mode_parameter_init = 3;			//�����ȡ������ʼ����3��
	}
	//Step2: �ȴ�AT "+++"	ָ��ص�2��	"OK"  ====================================================================
	

	//Step3: ���ջ�����������ָ�������Ͳ�ѯָ��  -------------------------------------------------------------
	else if(debug_mode_parameter_init == 3)
	{
		//��ս��ջ�����
		for(temp_i = 0; temp_i < USART1_RX_BUFFER_SIZE; temp_i++)
		{
			USART1_RX_Buffer[temp_i] = 0;
		}
		
		receive_pointer = 0;								//����ָ�����		
		
		//���Ͳ�ѯ����
		send_AT_inquiry();			
		
		//�ȴ�500�����������ݷ���
		vTaskDelay(500);
		
		//����Ժ�ͼ��
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);	
		GUI_SetColor(GUI_TRANSPARENT);		
		GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 130, 48);
		
		debug_mode_parameter_init = 4;			//�����ȡ������ʼ����4��
	}	
	//Step3: ���ջ�����������ָ�������Ͳ�ѯָ��  =============================================================

	
	//Step4: ��������ģ�鷵�ص�����  -----------------------------------------------------------------------------
	else if(debug_mode_parameter_init == 4)
	{
		handle_parameter_message();					//���������ģ��ض��Ĳ���
		
		//������ģ������Ĳ�����ֵ����ʾ����
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			Xtend_900_para_buf[temp_i] = Xtend_900_para[temp_i];
		}
		
		//������ǰң��������������ֵ��ʾ����
		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			local_para_conf_buf[temp_i] = local_para_conf[temp_i];
		}			
		
		debug_mode_parameter_init = 5;			//�������������ֵ
		
		not_save_RF_parameter();				//����ģ���˳�����ģʽ
		
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);					//�ڲ�������ʱ��ֹ�ж�		
	}
	//Step4: ��������ģ�鷵�ص�����  =============================================================================
	

	//Step5: ���������޸�״̬  -----------------------------------------------------------------------------------
	else if(debug_mode_parameter_init == 5)		
	{
		//��ʾ����ģ����ز���--------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tongxinpindao[0], 48, 17);								//ͨ��Ƶ��
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_tongxinbianhao[0], 48, 42);								//ͨ�ű��
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_bendidizhi[0], 48, 67);										//���ص�ַ
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_mudidizhi[0], 48, 92);										//Ŀ�ĵ�ַ
			GUI_DispStringAt(":", 111, 92);			
		}	
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			
			GUI_DispStringAt("Channel:", 60, 17);
			GUI_DispStringAt("Comm ID:", 60, 42);
			GUI_DispStringAt("My Addr:", 60, 67);
			GUI_DispStringAt("Dt Addr:", 60, 92);		
		}
		
		//��ȡ����ģ����ز���
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		
		for(temp_i = 0; temp_i < 4; temp_i++)
		{
			float_to_string(Xtend_900_para_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 128, 17 + 25 * temp_i);
		}		

		//�滭���λ��
		if(modify_index < 4)
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + modify_index * 25);			
		}		
		
		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);

		
		//�°벿�ִ洢�豸����
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_dizhiyanma[0], 48, 17);										//��ַ����
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_fashegonglv[0], 48, 42);									//���书��
			GUI_DispStringAt(":", 111, 42);		
			GUI_DispStringAt(_diantaimoshi[0], 48, 67);									//��̨ģʽ
			GUI_DispStringAt(":", 111, 67);		
			GUI_DispStringAt(_diantaiwendu[0], 48, 92);									//��̨�¶�
			GUI_DispStringAt(":", 111, 92);		
		}	
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			GUI_DispStringAt("Mask ID:", 60, 17);	
			GUI_DispStringAt("TX Power:", 60, 42);	
			GUI_DispStringAt("RF Mode:", 60, 67);	
			GUI_DispStringAt("RF Temp:", 60, 92);	
		}

		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		
		//��ȡ����ģ����ز���		
		float_to_string(Xtend_900_para_buf[4], tempChar, 0, 0, 0, 0);				//��ַ����
		GUI_DispStringAt(tempChar, 128, 17);
		GUI_DispStringAt(get_TX_Power(Xtend_900_para_buf[5]), 128, 42);			//��̨���书��
		float_to_string(Xtend_900_para_buf[6], tempChar, 0, 0, 0, 0);				//APIģʽ
		GUI_DispStringAt(tempChar, 128, 67);	
		
		GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
		float_to_string(Xtend_900_para_buf[7], tempChar, 0, 0, 0, 0);				//��̨�¶�
		GUI_DispStringAt(tempChar, 156, 92);
		
		GUI_SetFont(&GUI_Fontsongti12);			
		GUI_DispStringAt(_sheshidu[0], 159, 94);													//���϶� ��

		//�滭���λ��
		if((modify_index >= 4) && (modify_index < 8))
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 4) * 25);			
		}
		
		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);	


		//����ң�ؼ�USV����------------------------------------------
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_pingmuliangdu[0], 48, 17);								//��Ļ����
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_tuijinmoshi[0], 48, 42);									//�ƽ�ģʽ
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_chuanxingxuanze[0], 48, 67);							//����ѡ��
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_duojizhongwei[0], 48, 92);								//�����λ
			GUI_DispStringAt(":", 111, 92);				
		}
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			
			GUI_DispStringAt("Dimming:", 60, 17);		
			GUI_DispStringAt("Propellor:", 60, 42);	
			GUI_DispStringAt("USV Type:", 60, 67);
			GUI_DispStringAt("Mid-point:", 60, 92);				
		}

		//��ȡң�ؼ����Ͳ���
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);	
		
		float_to_string(local_para_conf_buf[0], tempChar, 0, 0, 0, 0);						//��Ļ���� 			
		GUI_DispStringAt(tempChar, 128, 17);
		float_to_string(local_para_conf_buf[1] + 1, tempChar, 0, 0, 0, 0);				//�ƽ�ģʽ 			
		GUI_DispStringAt(tempChar, 128, 42);								
		GUI_DispStringAt(GetBoatTypeText(local_para_conf_buf[2]), 128, 67);				//����ѡ��
		float_to_string(local_para_conf_buf[3], tempChar, 0, 0, 0, 0);						//�����λ			
		GUI_DispStringAt(tempChar, 128, 92);		

		//�滭���λ��
		if((modify_index >= 8) && (modify_index < 12))
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 8) * 25);			
		}
		
		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 46);
		
		
		//�°벿����
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//����
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tiaoshimoshi[0], 48, 17);										//����ģʽ
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_shengyinshezhi[0], 48, 42);									//��������
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_yuyanshezhi[0], 48, 67);										//��������
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_tongxinxieyi[0], 48, 92);										//ͨ��Э��
			GUI_DispStringAt(":", 111, 92);				
		}
		else
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Font16_1);	
			
			GUI_DispStringAt("Debugging:", 60, 17);		
			GUI_DispStringAt("Sound Set:", 60, 42);	
			GUI_DispStringAt("Language:", 60, 67);
			GUI_DispStringAt("Protocol:", 60, 92);									 
		}	

		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);	
		
		GUI_DispStringAt(Get_On_Off(local_para_conf_buf[4]), 128, 17);				//����ģʽ
		GUI_DispStringAt(Get_On_Off(local_para_conf_buf[5]), 128, 42);				//��������

		if(local_para_conf_buf[6] == 1)												//����
		{
			GUI_SetFont(&GUI_Fontsongti16);
			GUI_DispStringAt(_zhongwen[0], 128, 67);				//����
		}			
		else
		{
			GUI_SetFont(&GUI_Font16_1);
			GUI_DispStringAt("English", 128, 67);
		}
						
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("V2", 128, 92);							//ͨ��Э��
		
		//�滭���λ��
		if(modify_index >= 12)
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 12) * 25);			
		}
				
		//ˢ��LCD����
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 146);			
	}
	//Step5: ���������޸�״̬  ===================================================================================		
}


/*-------------------------------------------------------------------------
	��������main_graphic_show
	��  �ܣ���������Ļ��ʾ����
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void main_graphic_show(void)
{
	char tempChar[30];	
	uint16_t temp_num = 0, last_temp_num = 0;
	int16_t rdbm = 0;

	
	//�������������л���һ�γ�ʼ��
	if(debug_change_flag == 1)
	{
		debug_change_flag = 0;	
		waveform_xaxis = 0;
		
		//����״̬��-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);

		GUI_MEMDEV_Clear(bg0_memdev);	
		
		//���ͺ�����
		GUI_SetFont(&GUI_Font10_1);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);	
		GUI_DispStringAt(GetBoatTypeText(local_para_conf[2]), 65, 18);	
//		GUI_SetColor(GUI_DARKRED);
		GUI_DrawRoundedRect(39, 7, 90, 28, 2);
		GUI_DrawRoundedRect(36, 4, 93, 31, 3);
		
		GUI_BMP_Draw(_acImage_0, 3, 3);				//Yunzhou logo
		
		//����ͼƬ��ʾ
		if(local_para_conf[2] <= 2)				//��ʾ20��ͼ��
		{
			GUI_BMP_Draw(_acImage_10, 97, 3);		
		}
		else if(local_para_conf[2] <= 6)				//��ʾ30��ͼ��
		{
			GUI_BMP_Draw(_acImage_1, 97, 3);		
		}
		else if(local_para_conf[2] <= 11)				//��ʾ70��ͼ��
		{
			GUI_BMP_Draw(_acImage_2, 97, 3);		
		}
		else if(local_para_conf[2] <= 15)				//��ʾ120��ͼ��
		{
			GUI_BMP_Draw(_acImage_3, 97, 3);		
		}
		else if(local_para_conf[2] <= 18)				//��ʾ300��ͼ��
		{
			GUI_BMP_Draw(_acImage_4, 95, 3);		
		}		

		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 176, 0);

		
		//��ʾ����ģʽ
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);	
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		
		if(local_para_conf[6] == 1)		//����
		{
			if(local_para_conf[1] == 0)				//����
				GUI_DispStringAt(_dantui[0], 0, 8);

			else
				GUI_DispStringAt(_shuangtui[0], 0, 8);	//˫��
			
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 15, 10);
		}
		else
		{
			if(local_para_conf[1] == 0)				//����
				GUI_DispStringAt("Single", 0, 8);

			else
				GUI_DispStringAt("Double", 0, 8);				//˫��

			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 2, 10);			
		}	

		
		//�������½Ƕ�̬��ʾ��
		switch (local_para_conf[2])
		{
			case USV_Boat_Type_SF20:																				
			case USV_Boat_Type_SS20:
			case USV_Boat_Type_ESM20:
			case USV_Boat_Type_SS30:
			case USV_Boat_Type_ESM30:
			case USV_Boat_Type_ES30:
			case USV_Boat_Type_ESMH30:
			case USV_Boat_Type_MC70:
			case USV_Boat_Type_MM70:
			case USV_Boat_Type_MS70:
			case USV_Boat_Type_ME70:
			case USV_Boat_Type_MMH70:
			case USV_Boat_Type_MC120:
			case USV_Boat_Type_ME120:
			case USV_Boat_Type_MS120:	
			case USV_Boat_Type_MM120:	
			case USV_Boat_Type_ME300:	
			case USV_Boat_Type_LE2000:	
			case USV_Boat_Type_LE3000:					
			{
				//���������ʾ����
				GUI_SelectLayer(0);
				GUI_MEMDEV_Select(bg0_memdev);	
				GUI_MEMDEV_Clear(bg0_memdev);
				GUI_SetColor(GUI_TRANSPARENT);
				GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
				
				GUI_BMP_Draw(_acImage_13, 30, 0);													//���󱳾�
							
				
				//�滭��ȿ̶�
				GUI_SetColor(GUI_BLUE);
				GUI_SetBkColor(GUI_TRANSPARENT);
				GUI_SetFont(&GUI_Font8_1);
				
				for(temp_num = 0; temp_num < 8; temp_num++)
				{
					GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
					float_to_string(temp_num * 10, tempChar, 1, 0, 0, 0);
					GUI_DispStringAt(tempChar, 28, 18 + temp_num * 10);
					
					GUI_SetLineStyle(GUI_LS_DOT);																				//���õ�����
					GUI_DrawLine(30, 18 + temp_num * 10, 218, 18 + temp_num * 10);					
				}
								
				GUI_SetLineStyle(GUI_LS_SOLID);					//�л���ʵ��
								
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 160);			
				break;
			}
				
			default:
			{	
				break;
			}
		}

		LCD_PWM_Config(ENABLE);							//��ʼ����ϣ��򿪱����
	}

	
	//��ʾ״̬�ؼ�-------------------------------------------------------------------------
	//��ʾǰ����Ļ
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1w_memdev);	
	GUI_MEMDEV_Clear(bg1w_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);	
	
	//ҡ�˿���ģʽѡ����ʾ-----------------------
	if(MODE_Button == CONTROL_MODE_MANUAL)			//��ҡ��ģʽ
		GUI_BMP_Draw(_acImage_5, 9, 5);	
	
	else if(MODE_Button == CONTROL_MODE_MANUAL2)
	{
		GUI_BMP_Draw(_acImage_5, 0, 5);	
		GUI_BMP_Draw(_acImage_5, 18, 5);
	}
	else 
	{
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(GUI_WHITE);
		
//		local_para_conf[6] = 1;
		
		if(local_para_conf[6] == 1)		//����
			GUI_DispStringAt(_zidong[0], 3, 15);	
		
		else													//Ӣ��
			GUI_DispStringAt("Auto", 3, 15);
	}	
	
//	//��������״̬��ʾ------------------------
//	if((BoatDataTimeoutCount <= 5) && (connect_ok == 1))						
//	{
//		GUI_BMP_Draw(_acImage_6, 43, 3);				//���ӳɹ���ʾ
//	}
//	else
//	{
//		GUI_BMP_Draw(_acImage_7, 43, 3);				//����ʧ����ʾ
//	}
	
	//������״̬-------------------------------
	if(ALARM_Button == 1)
	{
		GUI_BMP_Draw(_acImage_9, 85, 0);				//������ͼ��
	}
	else
	{
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(85, 0, 29 + 85, 30);	
	}

	
	//�����ŵ�ָʾ--------------------------------
	GUI_SetFont(&GUI_Font10_1);	
	GUI_SetColor(GUI_RED);	
	GUI_SetBkColor(GUI_TRANSPARENT);	
	float_to_string(Xtend_900_para[0], tempChar, 0, 0, 0, 0);
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(tempChar, 45, 22);	
	
	
	//�ź�ǿ���ж�
//	if((RSSI_Interrupt_flag == 0) && (para_set_flag == 0) && (RSSI_Timer > 3))							//���յ����ݵ�û���ź�ǿ���жϣ���ʾ�ź�����Ϊ-40dbm				
//	{
//		rdbm = -40;
//		RSSI_Timer = 0;
//	}
//	else if((RSSI_Interrupt_flag == 0) && (para_set_flag == 1))					//��ʾ���յ�����ģ������ģʽ�µĻض�����
//	{
//		rdbm = -113;
//	}
//	else																																//��ʾ���յ�����͸�����ݣ��źŲ�����
//	{
		rdbm = DutyCycle * 2 / 3 - 113;	
//	}
		
	float_to_string(rdbm, tempChar, 0, 0, 0, 0);		
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
//	GUI_DispStringAt("The RSSI is:", 30, 91);
	GUI_DispStringAt(tempChar, 76, 22);
	
	GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 56, 3);
	
	
	//��������״̬��ʾ------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0s_memdev);	
	GUI_MEMDEV_Clear(bg0s_memdev);
	
	if((BoatDataTimeoutCount <= 5) && (connect_ok == 1))		
	{		
		//����RSSI��ʾ�ź�ǿ��		����-60dbm��ʾ�źźã�-80 ~ -60dbm��ʾ�ź�һ�㣬 -80���±�ʾ�źŲ�
		if(rdbm >= -65)
		{
			GUI_BMP_Draw(_acImage_16, 3, 0);				//�źź�
		}
		else if((rdbm >= -85) && (rdbm < -65))
		{
			GUI_BMP_Draw(_acImage_17, 3, 0);				//�ź�һ��		
		}
		else
		{
			GUI_BMP_Draw(_acImage_18, 3, 0);				//�źŲ�			
		}
	}
	else
	{
		GUI_BMP_Draw(_acImage_19, 3, 0);				//����ʧ����ʾ
	}
	GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 96, 3);
	
	
	//��ˮ����ʾ---------------------------------	
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0s_memdev);	
	GUI_MEMDEV_Clear(bg0s_memdev);	

	if(GlobalVariable.IsManualSampling == TRUE)
	{
		GUI_BMP_Draw(_acImage_12, 0, 0);				//��ʾ��ˮͼ��
	}
	else
	{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 0, 18, 30);			
	}
	GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 480 - 128, 3);
	
	
	//ң������ص�����ʾ---------------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0s_memdev);	
	GUI_MEMDEV_Clear(bg0s_memdev);	
	GUI_BMP_Draw(_acImage_8, 0, 1);				//��ر���ͼ��

	//��ص�����ʾӳ�䷶ΧΪ5 ~ 32
	if(Battery_Power_Percent <= 20)				//����20%����
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * Battery_Power_Percent / 100, 15, GUI_BLACK, GUI_RED);
		GUI_DrawGradientV(5, 16, 5 + 27 * Battery_Power_Percent / 100, 26, GUI_RED, GUI_BLACK);
	}	
	else if(Battery_Power_Percent <= 40)				//����40%����
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * Battery_Power_Percent / 100, 15, GUI_BLACK, GUI_YELLOW);
		GUI_DrawGradientV(5, 16, 5 + 27 * Battery_Power_Percent / 100, 26, GUI_YELLOW, GUI_BLACK);
	}
	else                                     
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * Battery_Power_Percent / 100, 15, GUI_BLACK, GUI_GREEN);
		GUI_DrawGradientV(5, 16, 5 + 27 * Battery_Power_Percent / 100, 26, GUI_GREEN, GUI_BLACK);
	}		

//	printf("%d\n", Battery_Power_Percent);
	
	GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 480 - 50, 3);
	
	
	//USV����ӳ��----------------------------------------------------
	GUI_MEMDEV_Clear(bg0s_memdev);
	GUI_BMP_Draw(_acImage_8, 0, 1);				//��ر���ͼ��
	
	if(BoatData.BatLife >= 100)
	{
		BoatData.BatLife = 100;
	}
	
	if(BoatData.BatLife <= 20)				//����20%����
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * BoatData.BatLife / 100, 15, GUI_BLACK, GUI_RED);
		GUI_DrawGradientV(5, 16, 5 + 27 * BoatData.BatLife / 100, 26, GUI_RED, GUI_BLACK);
	}	
	else if(BoatData.BatLife <= 40)				//����40%����
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * BoatData.BatLife / 100, 15, GUI_BLACK, GUI_YELLOW);
		GUI_DrawGradientV(5, 16, 5 + 27 * BoatData.BatLife / 100, 26, GUI_YELLOW, GUI_BLACK);
	}
	else                                     
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * BoatData.BatLife / 100, 15, GUI_BLACK, GUI_GREEN);
		GUI_DrawGradientV(5, 16, 5 + 27 * BoatData.BatLife / 100, 26, GUI_GREEN, GUI_BLACK);
	}			

	GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 480 - 100, 3);
	
	
	//��ʾң�ص�����������
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1n_memdev);	
	GUI_MEMDEV_Clear(bg1n_memdev);	
	GUI_SetFont(&GUI_Font10_1);	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, BG1N_XAXIS, BG1N_YAXIS);		
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	float_to_string(Battery_Power_Percent, tempChar, 0, 1, 0, 0);
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
	GUI_DispStringAt("REM", 19, 10);	
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(tempChar, 19, 22);
	GUI_MEMDEV_CopyToLCDAt(bg1n_memdev, 480 - 50, 3);	
	
	
	//��ʾUSV������������
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, BG1N_XAXIS, BG1N_YAXIS);		
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	float_to_string(BoatData.BatLife, tempChar, 0, 1, 0, 0);
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
	GUI_DispStringAt("USV", 20, 10);	
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(tempChar, 20, 22);
	GUI_MEMDEV_CopyToLCDAt(bg1n_memdev, 480 - 100, 3);	
	//��ʾ״̬�ؼ�==============================================================================
	
	
	//��ʾ����������----------------------------------------------------------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	
	//�����
	GUI_SetColor(GUI_WHITE);
	GUI_DrawRoundedRect(8, 16, 250 - 8, 100 - 5, 3);
	
	//�Զ�ģʽ��ʾ--------------------------------------------------------------------
	if(MODE_Button == CONTROL_MODE_AUTO)
	{
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetColor(GUI_BLUE);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(GUI_BLACK);
		
//		local_para_conf[6] = 0;
		
		//��������
		if(local_para_conf[6] == 1)
		{
			GUI_DispStringAt(_zhinengrenwu[0], 125, 16);									//��������
		}
		else
		{
			GUI_DispStringAt("AI Mission", 125, 16);
		}

		GUI_SetColor(GUI_RED);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);

//		BoatData.Status = 238 + 49152;
//		local_para_conf[6] = 0;
//		BoatData.Bottle = 2;
		
		switch (BoatData.Status)
		{
			case 0:																											//δ���õ�������
			{
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_weishezhidanghangrenwu[0], 125, 53);		   
				}
				else
				{
					GUI_DispStringAt("No Mission", 125, 53);
				}
				break;
			}
			case 65535:	 																								//���������ѽ���
			{
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_daohangrenwuyijieshu[0], 125, 53);							
				}
				else
				{
					GUI_DispStringAt("Mission Teminated", 125, 53);
				}
				break;
			}
			case 65534:										//������
			{
				GlobalVariable.IsManualSampling = TRUE;
				
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_zhengzaicaiyangzhi[0], 85, 53);							//���ڲ�����
					float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//ƿ��
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 138, 53);	
					GUI_DispStringAt(_haoping[0], 150, 53);												//��ƿ
				}
				else 
				{
					GUI_DispStringAt("Sampling bottle", 100, 53);								
					float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//ƿ��
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 180, 53);						
				}
				break;
			}
////			case 65533://���߼����
////			{
////				LCD_ClearArea(30, 79, 82, 120);
////				LCD_WR_L_HanZi_String(30, 79 , zheng zai jian ce);	//
////				//LCD_WR_L_HanZi_String(30, 120 , zhi ); 	//
////				break;
////			}
			case 65532:																								//�����ѱ�ȡ��
			{
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_daohangquxiao[0], 125, 53);							//����ȡ��
				}	
				else 
				{
					GUI_DispStringAt("Mission cancelled", 125, 53);							
				}
				
				break;
			}
			case 65531:																								//�����ѱ���ͣ
			{
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_daohangzanting[0], 125, 53);							//������ͣ				
				}
				else
				{
					GUI_DispStringAt("Mission paused", 125, 53);		
				}
				
				break;		  
			}
			default:
			{
				if(local_para_conf[6] == 1)
				{
					if (BoatData.Status >= 1 && BoatData.Status <= 255)
					{
						GUI_DispStringAt(_zhengzaidaohangzhi[0], 85, 53);							//���ڵ�����							
						float_to_string(BoatData.Status, tempChar, 0, 0, 0, 0);				//������
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 140, 53);	
						GUI_DispStringAt(_haodian[0], 156, 53);												//�ŵ�					
					}
					else if (BoatData.Status >= 32768 && BoatData.Status <= 32768 + 255)
					{
						GUI_DispStringAt(_zhengzaijiancezhi[0], 85, 53);							//���ڼ����							
						float_to_string(BoatData.Status - 32768, tempChar, 0, 0, 0, 0);				//������
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 140, 53);	
						GUI_DispStringAt(_haodian[0], 156, 53);												//�ŵ�							
					}
					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //�������
					{
						GUI_DispStringAt(_caiyangjiance[0], 85, 46);							//�������						
						float_to_string(BoatData.Status - 49152, tempChar, 0, 0, 0, 0);				//������
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 132, 46);	
						GUI_DispStringAt(_haodian[0], 148, 46);												//�ŵ�		

						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_caiyangzhi[0], 85, 70);											//������		
						float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//����ƿ
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 118, 70);	
						GUI_DispStringAt(_haoping[0], 128, 70);												//��ƿ								
					}
				}
				else
				{
					if (BoatData.Status >= 1 && BoatData.Status <= 255)
					{
						GUI_DispStringAt("Moving to", 100, 53);												//���ڵ���
						float_to_string(BoatData.Status, tempChar, 0, 0, 0, 0);				//������
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 150, 53);								
					}
					else if (BoatData.Status >= 32768 && BoatData.Status <= 32768 + 255)
					{
						GUI_DispStringAt("Monitoring", 100, 53);												//���ڼ��
						float_to_string(BoatData.Status - 32768, tempChar, 0, 0, 0, 0);				//������
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 152, 53);
					}
					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //�������
					{
						GUI_DispStringAt("Sampling monitoring", 110, 46);												//���ڼ��
						float_to_string(BoatData.Status - 49152, tempChar, 0, 0, 0, 0);				//������
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 203, 46);						
						
						GUI_DispStringAt("Sampling", 68, 70);																//������
						float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//������
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 148, 70);								
					}
				}
				break;
			}
		}
	}
	//�Զ�ģʽ��ʾ====================================================================

	
	//��ҡ��ģʽ��ʾ------------------------------------------------------------------
	else if(MODE_Button == CONTROL_MODE_MANUAL)
	{
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetColor(GUI_BLUE);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(GUI_BLACK);
		
//		local_para_conf[6] = 0;
//		local_para_conf[2] = USV_Boat_Type_MC120;
//		local_para_conf[2] = USV_Boat_Type_ME300;
		
		//��������
		if(local_para_conf[6] == 1)
		{
			GUI_DispStringAt(_youshoukongzhi[0], 125, 16);									//���ֿ���
		}
		else
		{
			GUI_DispStringAt("Right Hand Control", 125, 16);
		}

		
		GUI_SetColor(GUI_RED);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		
		switch (local_para_conf[2])
		{
			case USV_Boat_Type_LE2000:																				//LE2000��������
			case USV_Boat_Type_LE3000:																				//LE3000��������
			{
				break;
			}
				
			default:
			{
				if(local_para_conf[6] == 1)																		//������ʾ
				{
					if(local_para_conf[2] == USV_Boat_Type_MC120)
					{
						//��ʾˮ�÷�������
						GUI_DispStringAt(_shuibeng[0], 36, 46);												//ˮ��							
					}
					else if(local_para_conf[2] == USV_Boat_Type_ME300)
					{
						//��ʾ���ŷ�������
						GUI_DispStringAt(_youmen[0], 36, 46);												//����
					}
					else
					{
						//��ʾ��λ��������
						GUI_DispStringAt(_dangwei[0], 36, 46);												//����						
					}

					GUI_DispStringAt(":", 54, 46);

					//��ťADCӳ�䷶Χ
					temp_num = Knob_Avg;
					
					if(temp_num >= MAX_KNOB_VALUE)
					{
						temp_num = MAX_KNOB_VALUE;
					}
					
					float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//��ť�ٷֱ�
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 68, 46);	
					
					
					//��ʾҡ�˿��Ʒ�������--------------------------------------
					//�ƽ��ٷֱ�--------------------------
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt(_tuijin[0], 36, 70);												//�ƽ�
					GUI_DispStringAt(":", 54, 70);					

					temp_num = RightSpeedCommand;
					
					if(temp_num >= FORWARD_NUM)
					{
						temp_num = temp_num - MIDDLE_NUM;
						GUI_DispStringAt(_qianjin[0], 105, 70);												//ǰ��					
					}
					else if(temp_num <= BACKWARD_NUM)
					{
						temp_num = MIDDLE_NUM - temp_num;
						GUI_DispStringAt(_houtui[0], 105, 70);												//����					
					}
					else
					{
						temp_num = 0;
					}
					
					float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 68, 70);						
					
					
					//��ǰٷֱ�-----------------------------		
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt(_duojiao[0], 145, 70);													//���
					GUI_DispStringAt(":", 163, 70);									

					temp_num = RightRudderCommand;

					//ME300���
					if(local_para_conf[2] == USV_Boat_Type_ME300)
					{
						if(temp_num >= 2100)
						{
							temp_num = 2100;
						}
						else if(temp_num <= 1110)
						{
							temp_num = 1110;
						}
						
						if((temp_num >= 1570) && (temp_num <= 2100))
						{
							temp_num = temp_num - 1560;
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��	
							
							float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
						}
						else if((temp_num <= 1540) && (temp_num >= 1110))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��			
							
							float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}						
						
					}
					
					//�������
					else
					{
						if(temp_num >= 2000)
						{
							temp_num = 2000;
						}
						else if(temp_num <= 1010)
						{
							temp_num = 1010;
						}
						
						if((temp_num >= 1570) && (temp_num <= 2000))
						{
							temp_num = temp_num - 1560;
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��		
							
							float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
						}
						else if((temp_num <= 1540) && (temp_num >= 1010))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��			
							
							float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}						
					}

					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 177, 70);						
					
//					//��ʾҡ�˿��Ʒ�������======================================					
//					printf("RRS: %d, %d,  LRS: %d, %d\n", RightRudderCommand, RightSpeedCommand, LeftRudderCommand, LeftSpeedCommand);
				}
				
				else																															//Ӣ��
				{
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					
					if(local_para_conf[2] == USV_Boat_Type_MC120)
					{
						//��ʾˮ�÷�������
						GUI_DispStringAt("Water  pump:", 20, 46);											//ˮ��							
					}
					else if(local_para_conf[2] == USV_Boat_Type_ME300)
					{
						//��ʾ���ŷ�������
						GUI_DispStringAt("Accelerator:", 20, 46);											//����
					}
					else
					{
						//��ʾ��λ��������
						GUI_DispStringAt("Speed limit:", 20, 46);											//��λ					
					}

					//��ťADCӳ�䷶Χ
					temp_num = Knob_Avg;
					
					if(temp_num >= MAX_KNOB_VALUE)
					{
						temp_num = MAX_KNOB_VALUE;
					}
					
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//��ť�ٷֱ�
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 136, 46);	
					
					
					//��ʾҡ�˿��Ʒ�������--------------------------------------
					//�ƽ��ٷֱ�--------------------------
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt("Spe", 36, 70);												//�ƽ�
					GUI_DispStringAt(":", 54, 70);					

					temp_num = RightSpeedCommand;
					
					if(temp_num >= FORWARD_NUM)
					{
						temp_num = temp_num - MIDDLE_NUM;
						GUI_DispStringAt(_qianjin[0], 105, 70);												//ǰ��					
					}
					else if(temp_num <= BACKWARD_NUM)
					{
						temp_num = MIDDLE_NUM - temp_num;
						GUI_DispStringAt(_houtui[0], 105, 70);												//����					
					}
					else
					{
						temp_num = 0;
					}
					
					float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 68, 70);						
					
					
					//��ǰٷֱ�-----------------------------		
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt("Rud", 145, 70);													//���
					GUI_DispStringAt(":", 163, 70);									

					temp_num = RightRudderCommand;
					
					//ME300���
					if(local_para_conf[2] == USV_Boat_Type_ME300)
					{
						if(temp_num >= 2100)
						{
							temp_num = 2100;
						}
						else if(temp_num <= 1110)
						{
							temp_num = 1110;
						}
						
						if((temp_num >= 1570) && (temp_num <= 2100))
						{
							temp_num = temp_num - 1560;
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��	
							
							float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
						}
						else if((temp_num <= 1540) && (temp_num >= 1110))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��		
							
							float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}						
						
					}
					
					//�������
					else
					{
						if(temp_num >= 2000)
						{
							temp_num = 2000;
						}
						else if(temp_num <= 1010)
						{
							temp_num = 1010;
						}
						
						if((temp_num >= 1570) && (temp_num <= 2000))
						{
							temp_num = temp_num - 1560;
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��		
							
							float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
						}
						else if((temp_num <= 1540) && (temp_num >= 1010))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��			
							
							float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
						}						
					}

					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 177, 70);					
				}
				
				break;
			}														
		}					
	}
	//��ҡ��ģʽ��ʾ==================================================================
	
	
	//˫ҡ��ģʽ��ʾ------------------------------------------------------------------
	else if(MODE_Button == CONTROL_MODE_MANUAL2)
	{
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetColor(GUI_BLUE);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(GUI_BLACK);
		
//		local_para_conf[6] = 0;
//		local_para_conf[1] = 0;
//		local_para_conf[2] = USV_Boat_Type_MC120;
//		local_para_conf[2] = USV_Boat_Type_ME300;
		
		//��������
		if(local_para_conf[6] == 1)				//����
		{
			if(local_para_conf[1] == 0)			//����
			{
				GUI_DispStringAt(_zuosuyouduo[0], 125, 16);									//�����Ҷ�
			}
			else														//˫��
			{
				GUI_DispStringAt(_zuoyouchasu[0], 125, 16);									//���Ҳ���
			}						
		}
		else                              //Ӣ��
		{
			if(local_para_conf[1] == 0)			//����
			{
				GUI_DispStringAt("LH-Speed  RH-Rudder", 125, 16);									//�����Ҷ�
			}
			else														//˫��
			{
				GUI_DispStringAt("Differential Control", 125, 16);									//���Ҳ���
			}			
		}

		
		GUI_SetColor(GUI_RED);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		
		switch (local_para_conf[2])
		{
			case USV_Boat_Type_LE2000:																				//LE2000��������
			case USV_Boat_Type_LE3000:																				//LE3000��������
			{
//				break;
			}
				
			default:
			{
				if(local_para_conf[6] == 1)																		//������ʾ
				{
					//����  �����Ҷ�---------------------------------------------					
					if(local_para_conf[1] == 0)			
					{
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//��ʾˮ�÷�������
							GUI_DispStringAt(_shuibeng[0], 36, 46);												//ˮ��							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//��ʾ���ŷ�������
							GUI_DispStringAt(_youmen[0], 36, 46);												//����
						}
						else
						{
							//��ʾ��λ��������
							GUI_DispStringAt(_dangwei[0], 36, 46);												//����						
						}

						GUI_DispStringAt(":", 54, 46);

						//��ťADCӳ�䷶Χ
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//��ť�ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 46);	
						
						
						//��ʾҡ�˿��Ʒ�������--------------------------------------
						//�ƽ��ٷֱ�--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_tuijin[0], 36, 70);												//�ƽ�
						GUI_DispStringAt(":", 54, 70);					

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 70);												//ǰ��					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 70);												//����					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 70);						
						
						
						//��ǰٷֱ�-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_duojiao[0], 145, 70);													//���
						GUI_DispStringAt(":", 163, 70);									

						temp_num = RightRudderCommand;
						
						//ME300���
						if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							if(temp_num >= 2100)
							{
								temp_num = 2100;
							}
							else if(temp_num <= 1110)
							{
								temp_num = 1110;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2100))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��		
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}									
						}
						
						//�������
						else
						{
							if(temp_num >= 2000)
							{
								temp_num = 2000;
							}
							else if(temp_num <= 1010)
							{
								temp_num = 1010;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2000))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}							
						}

						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 70);					
					}
					//����  �����Ҷ�=============================================
					
					
					//˫��  ���Ҳ���---------------------------------------------
					else														
					{
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//��ʾˮ�÷�������
							GUI_DispStringAt(_shuibeng[0], 36, 36);												//ˮ��							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//��ʾ���ŷ�������
							GUI_DispStringAt(_youmen[0], 36, 36);												//����
						}
						else
						{
							//��ʾ��λ��������
							GUI_DispStringAt(_dangwei[0], 36, 36);												//����						
						}

						GUI_DispStringAt(":", 54, 36);

						//��ťADCӳ�䷶Χ
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//��ť�ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 36);	
						
						
						//��ʾҡ�˿��Ʒ�������--------------------------------------
						//���ƽ��ٷֱ�--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_zuojin[0], 36, 56);												//���
						GUI_DispStringAt(":", 54, 56);							

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 56);												//ǰ��					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 56);												//����					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 56);						
						
						
						//���ǰٷֱ�-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_zuoduo[0], 36, 76);												//���
						GUI_DispStringAt(":", 54, 76);									

						temp_num = LeftRudderCommand;

						//ME300���
						if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							if(temp_num >= 2100)
							{
								temp_num = 2100;
							}
							else if(temp_num <= 1110)
							{
								temp_num = 1110;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2100))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//ת��	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}															
						}
						
						//���������ӳ��
						else
						{
							if(temp_num >= 2000)
							{
								temp_num = 2000;
							}
							else if(temp_num <= 1010)
							{
								temp_num = 1010;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2000))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//ת��		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}							
						}
						
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 76);

						
						//���ƽ��ٷֱ�--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_youjin[0], 145, 56);												//�ҽ�
						GUI_DispStringAt(":", 163, 56);							

						temp_num = RightSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 214, 56);												//ǰ��					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 214, 56);												//����					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 56);		
						
						
						//�Ҷ�ǰٷֱ�-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_youduo[0], 145, 76);													//�Ҷ�
						GUI_DispStringAt(":", 163, 76);									

						temp_num = RightRudderCommand;
						
						//ME300���
						if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							if(temp_num >= 2100)
							{
								temp_num = 2100;
							}
							else if(temp_num <= 1110)
							{
								temp_num = 1110;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2100))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//ת��		
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}							
						}
						
						//���������
						else
						{
							if(temp_num >= 2000)
							{
								temp_num = 2000;
							}
							else if(temp_num <= 1010)
							{
								temp_num = 1010;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2000))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//ת��		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}						
						}
						
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 76);							
					}
					//˫��  ���Ҳ���=============================================			
						
					
				}
				
				else																															//Ӣ��
				{
					//����  �����Ҷ�---------------------------------------------					
					if(local_para_conf[1] == 0)			
					{
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//��ʾˮ�÷�������
							GUI_DispStringAt("Water  pump:", 20, 46);											//ˮ��							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//��ʾ���ŷ�������
							GUI_DispStringAt("Accelerator:", 20, 46);											//����
						}
						else
						{
							//��ʾ��λ��������
							GUI_DispStringAt("Speed limit:", 20, 46);											//��λ					
						}

						//��ťADCӳ�䷶Χ
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//��ť�ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 136, 46);	
						
						
						//��ʾҡ�˿��Ʒ�������--------------------------------------
						//�ƽ��ٷֱ�--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("Spe", 36, 70);												//�ƽ�
						GUI_DispStringAt(":", 54, 70);					

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 70);												//ǰ��					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 70);												//����					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 70);						
						
						
						//��ǰٷֱ�-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("Rud", 145, 70);													//���
						GUI_DispStringAt(":", 163, 70);									

						temp_num = RightRudderCommand;
						
						//ME300���
						if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							if(temp_num >= 2100)
							{
								temp_num = 2100;
							}
							else if(temp_num <= 1110)
							{
								temp_num = 1110;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2100))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��		
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}									
						}
						
						//�������
						else
						{
							if(temp_num >= 2000)
							{
								temp_num = 2000;
							}
							else if(temp_num <= 1010)
							{
								temp_num = 1010;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2000))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//ת��		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}							
						}

						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 70);					
					}
					//����  �����Ҷ�=============================================
					
					
					//˫��  ���Ҳ���---------------------------------------------
					else														
					{
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//��ʾˮ�÷�������
							GUI_DispStringAt("Water  Pump:", 20, 36);												//ˮ��							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//��ʾ���ŷ�������
							GUI_DispStringAt("Accelerator:", 20, 36);												//����
						}
						else
						{
							//��ʾ��λ��������
							GUI_DispStringAt("Speed limit:", 20, 36);												//��λ						
						}


						//��ťADCӳ�䷶Χ
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//��ť�ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 136, 36);	
						
						
						//��ʾҡ�˿��Ʒ�������--------------------------------------
						//���ƽ��ٷֱ�--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("LSp", 36, 56);												//���
						GUI_DispStringAt(":", 54, 56);							

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 56);												//ǰ��					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 56);												//����					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 56);						
						
						
						//���ǰٷֱ�-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("LRu", 36, 76);												//���
						GUI_DispStringAt(":", 54, 76);									

						temp_num = LeftRudderCommand;

						//ME300���
						if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							if(temp_num >= 2100)
							{
								temp_num = 2100;
							}
							else if(temp_num <= 1110)
							{
								temp_num = 1110;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2100))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//ת��	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}															
						}
						
						//���������ӳ��
						else
						{
							if(temp_num >= 2000)
							{
								temp_num = 2000;
							}
							else if(temp_num <= 1010)
							{
								temp_num = 1010;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2000))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//ת��		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}							
						}
						
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 76);

						
						//���ƽ��ٷֱ�--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("RSp", 145, 56);												//�ҽ�
						GUI_DispStringAt(":", 163, 56);							

						temp_num = RightSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 214, 56);												//ǰ��					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 214, 56);												//����					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//�ƽ��ٷֱ�
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 56);		
						
						
						//�Ҷ�ǰٷֱ�-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("RRu", 145, 76);													//�Ҷ�
						GUI_DispStringAt(":", 163, 76);									

						temp_num = RightRudderCommand;
						
						//ME300���
						if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							if(temp_num >= 2100)
							{
								temp_num = 2100;
							}
							else if(temp_num <= 1110)
							{
								temp_num = 1110;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2100))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//ת��		
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}							
						}
						
						//���������
						else
						{
							if(temp_num >= 2000)
							{
								temp_num = 2000;
							}
							else if(temp_num <= 1010)
							{
								temp_num = 1010;
							}
							
							if((temp_num >= 1570) && (temp_num <= 2000))
							{
								temp_num = temp_num - 1560;
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//ת��		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//ת��			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//��ǰٷֱ�	
							}						
						}
						
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 76);							
					}										
				}
				
//				printf("RRS: %d, %d,  LRS: %d, %d\n", RightRudderCommand, RightSpeedCommand, LeftRudderCommand, LeftSpeedCommand);
				
				break;
			}														
		}					
	}
	//˫ҡ��ģʽ��ʾ==================================================================
	
	//ˢ�²���������
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 43);	
	//��ʾ����������============================================================================

	
	//��ʾ״̬�����----------------------------------------------------------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

//	local_para_conf[6] = 0;
	
	//�����
	GUI_SetColor(GUI_WHITE);
	GUI_DrawRoundedRect(8, 16, 250 - 8, 100 - 2, 3);	

	//��������
	GUI_SetColor(GUI_BLUE);
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);

	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_zhuangtaijiance[0], 125, 16);									//��������
	}
	else																//Ӣ��
	{
		GUI_DispStringAt("USV Status", 125, 16);
	}

	//��ʾ���Ƕ�λ��Ϣ--------------------------------------------
	GUI_SetFont(&GUI_Fontsongti12);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_UC_SetEncodeUTF8();
	GUI_SetColor(GUI_RED);
	
	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_weizhi[0], 16, 36);									//λ��	
		GUI_DispStringAt(":", 41, 36);									
	}
	else
	{
		GUI_DispStringAt("GPS:", 16, 36);		
	}

	//���澭����γ��
	lat = BoatData.DoubleLatitude;
	lng = BoatData.DoubleLongitude;

//	lat = 88.55 / 1000;
//	lng = 120.66 / 1000;

	//γ��------------------------------------
	//��γ
	if (lat > 0)
	{	//-90,90
		GUI_DispStringAt("N", 53, 36);			//N		
	}
	//��γ
	else 
	{
		lat = -lat;		
		GUI_DispStringAt("S", 53, 36);			//S	
	}
	
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
//	float_to_string(lat * 1000, tempChar, 4, 0, 0, 0);
	float_to_string(lat, tempChar, 4, 0, 0, 0);	
	GUI_DispStringAt(tempChar, 108, 36);	
	GUI_DispStringAt(_du[0], 110, 36);									//�� ��		

	//����-------------------------------------
	//����
	if (lng > 0)
	{//-180,180
		GUI_DispStringAt("E", 136, 36);			//E	
	}
	//����
	else 
	{
		lng = -lng;		
		GUI_DispStringAt("W", 136, 36);			//W	
	}	
	
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
//	float_to_string(lng * 1000, tempChar, 4, 0, 0, 0);
	float_to_string(lng, tempChar, 4, 0, 0, 0);
	GUI_DispStringAt(tempChar, 196, 36);		
	GUI_DispStringAt(_du[0], 198, 36);									//�� ��				

//	BoatData.Speed = 233;
	
	//�ٶ�--------------------------------------
	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_sudu[0], 16, 52);									//�ٶ�
		GUI_DispStringAt(":", 41, 52);									
	}
	else
	{
		GUI_DispStringAt("SPE:", 16, 52);		
	}	

	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
	float_to_string(((double) BoatData.Speed) / 10, tempChar, 2, 0, 0, 0);
//	float_to_string(((double) BoatData.Speed), tempChar, 2, 0, 0, 0);
	GUI_DispStringAt(tempChar, 82, 52);
	GUI_DispStringAt("m/s", 86, 52);
	//��ʾ���Ƕ�λ��Ϣ============================================

//	local_para_conf[6] = 0;
	
	//��ʾ����״̬------------------------------------------------
	//��ҡ  Yaw----------------------------------
	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_shouyao[0], 129, 52);									//��ҡ 
		GUI_DispStringAt(":", 25 + 129, 52);									
	}
	else
	{
		GUI_DispStringAt("Yaw:", 129, 52);		
	}	
	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	float_to_string(((int16_t) BoatData.Yaw) / 10, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 168, 52);	
	
	
	//��ҡ  Yaw----------------------------------	
	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_hengyao[0], 16, 68);									//��ҡ
		GUI_DispStringAt(":", 41, 68);									
	}
	else
	{
		GUI_DispStringAt("Roll:", 16, 68);		
	}	
	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	float_to_string(((int16_t) BoatData.Roll) / 10, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 55, 68);
	
	
	//��ҡ  Yaw----------------------------------
	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_zongyao[0], 129, 68);									//��ҡ
		GUI_DispStringAt(":", 25 + 129, 68);									
	}
	else
	{
		GUI_DispStringAt("Pitch:", 129, 68);		
	}	

//	BoatData.Pitch = 12048;
	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	float_to_string(((int16_t) BoatData.Pitch) / 10, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 168, 68);		
	//��ʾ����״̬================================================

	
	//��ʾ�¶ȼ��------------------------------------------------
	//ˮ��  WaterTemp----------------------------------	
	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_shuiwen[0], 16, 84);									//ˮ��
		GUI_DispStringAt(":", 41, 84);									
	}
	else
	{
		GUI_DispStringAt("W T:", 16, 84);		
	}	
	
//	BoatData.WaterTemp = -32.5 / 10;
	
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
	float_to_string(BoatData.WaterTemp, tempChar, 1, 0, 0, 0);
	GUI_DispStringAt(tempChar, 86, 84);
	GUI_DispStringAt(_sheshidu[0], 88, 84);										//��  ���϶�


	//����  InternalTemp----------------------------------	
	if(local_para_conf[6] == 1)					//����
	{
		GUI_DispStringAt(_chuanwen[0], 129, 84);									//����
		GUI_DispStringAt(":", 25 + 129, 84);									
	}
	else
	{
		GUI_DispStringAt("I T:", 129, 84);											//�����¶�
	}	
	
//	BoatData.InternalTemp = 132.5 / 10;
	
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
	float_to_string(BoatData.InternalTemp, tempChar, 1, 0, 0, 0);
	GUI_DispStringAt(tempChar, 199, 84);
	GUI_DispStringAt(_sheshidu[0], 201, 84);										//��  ���϶�	
	//��ʾ�¶ȼ��================================================
	
	//ˢ��״̬�����
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 150);	
	//��ʾ״̬�����============================================================================
	
	
	//��ʾ�������������Ϣ----------------------------------------------------------------------
//	uint8_t number_min = MinBottleNumber;
	uint8_t number_max;
	int color_temp;
	
	//��ʾ�����������
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1w_memdev);	
	GUI_MEMDEV_Clear(bg1w_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);	

	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_DARKBLUE);
	
	if(local_para_conf[6] == 1)					//����
	{
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_caiyangyujiance[0], 16, 12);										//���� & ���		
	}	
	else
	{
		GUI_SetFont(&GUI_Fontsongti12);
		GUI_DispStringAt("RT Monitoring", 16, 12);												//ʵʱ���								
	}

	GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 250, 44);


	//��ʾ������Ϣ-------------------------------------------
	//��ʾ������ⱳ��
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	
	GUI_DrawGradientRoundedH(8, 2, 116, 22, 3, GUI_RED, GUI_ORANGE);				//���� & ��� ������

	
	//���Ʋ���ƿ����
	switch (local_para_conf[2])
	{
		//���� 1 ~ 4 ��ƿ
		case USV_Boat_Type_SF20:																			
		case USV_Boat_Type_SS20:
		case USV_Boat_Type_ESM20:
		case USV_Boat_Type_SS30:
		case USV_Boat_Type_ESM30:
		case USV_Boat_Type_ES30:
		case USV_Boat_Type_ESMH30:			
		{
			GUI_BMP_Draw(_acImage_11, 8 + 10, 28);				//����ƿ����  1 ~ 4			
			Draw_Empty_Bottle(8 + 10);
			
			number_max = 4;
			
			break;
		}


		//���� 5 ~ 8 ��ƿ
		case USV_Boat_Type_MC70:
		case USV_Boat_Type_MM70:
		case USV_Boat_Type_MS70:
		case USV_Boat_Type_ME70:
		case USV_Boat_Type_MMH70:
		{
			GUI_BMP_Draw(_acImage_11, 8 + 10, 28);					//����ƿ����  1 ~ 4
			GUI_BMP_Draw(_acImage_11, 78 + 10, 28);				//����ƿ����  5 ~ 8
			Draw_Empty_Bottle(8 + 10);
			Draw_Empty_Bottle(78 + 10);	
			
			number_max = 8;

			break;
		}

		//���� 9 ~ 12 ��ƿ		
		case USV_Boat_Type_MC120:	
		case USV_Boat_Type_ME120:	
		case USV_Boat_Type_MS120:	
		case USV_Boat_Type_MM120:	
		case USV_Boat_Type_ME300:
		{
			GUI_BMP_Draw(_acImage_11, 8 + 10, 28);					//����ƿ����  1 ~ 4
			GUI_BMP_Draw(_acImage_11, 78 + 10, 28);				//����ƿ����  5 ~ 8
			GUI_BMP_Draw(_acImage_11, 148 + 10, 28);				//����ƿ����   9 ~ 12
			Draw_Empty_Bottle(8 + 10);
			Draw_Empty_Bottle(78 + 10);			
			Draw_Empty_Bottle(148 + 10);	
			
			number_max = 12;

			break;
		}		
		
		default:
		{
			break;
		}
	}
	
//	SampleInfo.VolumeTaken[1] = 18;
//	SampleInfo.VolumeTaken[8] = 10;
//	SampleInfo.VolumeTaken[11] = 8;
//	SampleInfo.IsBottleTaking[11] = TRUE;
	
	GUI_SetFont(&GUI_Font8_1);
	GUI_SetColor(GUI_WHITE);

	//��ʾ��ˮ����
	for(temp_num = MinBottleNumber; temp_num <= number_max; temp_num++)
	{
		//��ʾ�Ѿ������õ�����
		if(SampleInfo.IsBottleTaking[temp_num - 1] == FALSE)
		{
			color_temp = GUI_BLUE;
		}
		
		//��ʾ���ڲ���������
		else
		{
			color_temp = GUI_GREEN;
		}
		
		//3�����ƿ
		if(temp_num <= 4)
		{
			GUI_DrawGradientH(8 + 2 + 15 * (temp_num - 1) + 10, 59 - SampleInfo.VolumeTaken[temp_num - 1], 8 + 7 + 15 * (temp_num - 1) + 10, 59, GUI_GRAY, color_temp);									
			GUI_DrawGradientH(8 + 7 + 15 * (temp_num - 1) + 10, 59 - SampleInfo.VolumeTaken[temp_num - 1], 8 + 11 + 15 * (temp_num - 1) + 10, 59, color_temp, GUI_GRAY);
			float_to_string(temp_num, tempChar, 0, 0, 0, 0);
			GUI_SetColor(GUI_WHITE);
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(tempChar, 8 + 7 + 15 * (temp_num - 1) + 10, 70);
			
			float_to_string(SampleInfo.VolumeTaken[temp_num - 1], tempChar, 0, 0, 0, 0);
			GUI_SetColor(GUI_BLUE);
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(tempChar, 8 + 7 + 15 * (temp_num - 1) + 10, 80);			
		}
		else if(temp_num <= 8)
		{
			GUI_DrawGradientH(78 + 2 + 15 * (temp_num - 5) + 10, 59 - SampleInfo.VolumeTaken[temp_num - 1], 78 + 7 + 15 * (temp_num - 5) + 10, 59, GUI_GRAY, color_temp);									
			GUI_DrawGradientH(78 + 7 + 15 * (temp_num - 5) + 10, 59 - SampleInfo.VolumeTaken[temp_num - 1], 78 + 11 + 15 * (temp_num - 5) + 10, 59,color_temp, GUI_GRAY);	
			float_to_string(temp_num, tempChar, 0, 0, 0, 0);
			GUI_SetColor(GUI_WHITE);
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(tempChar, 78 + 7 + 15 * (temp_num - 5) + 10, 70);
			
			float_to_string(SampleInfo.VolumeTaken[temp_num - 1], tempChar, 0, 0, 0, 0);
			GUI_SetColor(GUI_BLUE);
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(tempChar, 78 + 7 + 15 * (temp_num - 5) + 10, 80);			
		}
		else if(temp_num <= 12)
		{
			GUI_DrawGradientH(148 + 2 + 15 * (temp_num - 9) + 10, 59 - SampleInfo.VolumeTaken[temp_num - 1], 148 + 7 + 15 * (temp_num - 9) + 10, 59, GUI_GRAY, color_temp);									
			GUI_DrawGradientH(148 + 7 + 15 * (temp_num - 9) + 10, 59 - SampleInfo.VolumeTaken[temp_num - 1], 148 + 11 + 15 * (temp_num - 9) + 10, 59, color_temp, GUI_GRAY);
			float_to_string(temp_num, tempChar, 0, 0, 0, 0);
			GUI_SetColor(GUI_WHITE);
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(tempChar, 148 + 7 + 15 * (temp_num - 9) + 10, 70);
			
			float_to_string(SampleInfo.VolumeTaken[temp_num - 1], tempChar, 0, 0, 0, 0);
			GUI_SetColor(GUI_BLUE);
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(tempChar, 148 + 7 + 15 * (temp_num - 9) + 10, 80);				
		}		
	}		
	//��ʾ������Ϣ===========================================
		
	//ˢ�²������������
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 44);	
	//��ʾ�������������Ϣ======================================================================
	
	
	//��ʾ��ȼ��������Ϣ----------------------------------------------------------------------
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1w_memdev);	
	GUI_MEMDEV_Clear(bg1w_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);		

	GUI_SetColor(GUI_RED);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);	
	GUI_SetFont(&GUI_Fontsongti12);
	
	if(local_para_conf[6] == 1)									//����
	{
		GUI_DispStringAt(_shendu[0], 8, 8);										//���
		GUI_DispStringAt(":", 33, 8);		
	}
	else
	{
		GUI_DispStringAt("Depth:", 2, 8);	
	}									

	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);	
//	float_to_string(BoatData.Depth * 100, tempChar, 1, 2, 0, 0);
	float_to_string(BoatData.Depth, tempChar, 1, 2, 0, 0);
	GUI_DispStringAt(tempChar, 41, 8);	
	
	GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 260, 148);	

//BoatData.Depth = 30 - 30 * sin(wave_test_timer * 2 * 3.14 / 50) + wave_test_timer % 70;
//	wave_test_timer++;
//	BoatData.Depth = waveform_xaxis % 70;
	
	//�滭��Ȳ���-------------------------------------
	show_depth_div++;
	
	if(show_depth_div >= SHOW_DEPTH_TIME)					//0.5��ˢ��һ��
	{
		show_depth_div = 0;
		
		//ˢ��X��λ��
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);	
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(30 + waveform_xaxis + 1, 0, 30 + waveform_xaxis + 4, DEBUG_BG1_YAXIS);		
	
		//��ȡ��ǰ��Ⱥ�ת��Ϊ��ʾ���
//		temp_num = (uint16_t) (BoatData.Depth * 100);
		wave_temp = (uint16_t) (BoatData.Depth);
		
		if(wave_temp >= SHOW_MAX_DEPTH)
		{
			wave_temp = SHOW_MAX_DEPTH; 
		}
		
		//�滭��ǰλ�����ֱ��
		GUI_SetColor(GUI_RED);
		GUI_DrawLine(30 + waveform_xaxis, wave_last_temp, 30 + waveform_xaxis + 3, wave_temp);
		
		waveform_xaxis = waveform_xaxis + 3;
		
		wave_last_temp = wave_temp;
		
		if(waveform_xaxis >= 186)
		{
			waveform_xaxis = 0;
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(30, 0, 32, DEBUG_BG1_YAXIS);
		}
	}	
	
	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 178);		
	//��ʾ��ȼ��������Ϣ======================================================================
	
}


/**
  * @brief  debug_mode_main ����ģʽ������
  * @param  
  * @retval 
  */
void debug_mode_main(void)
{
	char tempChar[20];	
	
//	USART_SendData(USART2, 0x86);
	
	//��ʾǰ����Ļ-------------------------------------------------
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 120, 90);
	
	GUI_SetFont(&GUI_Font10_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//��ʾң��������
	GUI_DispStringAt("RC_Power:", 5, 22);	
	float_to_string(Battery_Power_Avg, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22);		
	
	//��ʾ��λ����ť����
	GUI_DispStringAt("KNOB:", 5, 22 + 12);	
	float_to_string(Knob_Avg, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 12);		
	
	//��ʾ���ҡ�� X �������ֵ
	GUI_DispStringAt("LJX:", 5, 22 + 24);	
	float_to_string(Left_Joystick_X_axis_Avg, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 24);	
	
	//��ʾ���ҡ�� Y �������ֵ
	GUI_DispStringAt("LJY:", 5, 22 + 36);	
	float_to_string(Left_Joystick_Y_axis_Avg, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 36);	
	
	//��ʾ�Ҳ�ҡ�� X �������ֵ
	GUI_DispStringAt("RJX:", 5, 22 + 48);	
	float_to_string(Right_Joystick_X_axis_Avg, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 48);	
	
	//��ʾ�Ҳ�ҡ�� Y �������ֵ
	GUI_DispStringAt("RJY:", 5, 22 + 60);	
	float_to_string(Right_Joystick_Y_axis_Avg, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 60);					

	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10);
	
	//��ʾǰ����Ļ-------------------------------------------------
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 120, 90);	
	
	GUI_SetFont(&GUI_Font10_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);

	//��ʾMENU �˵�����ֵ
	GUI_DispStringAt("MENU:", 5, 22);	
	float_to_string(MENU_Button, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22);	
	
	//��ʾOK ȷ�ϼ���ֵ
	GUI_DispStringAt("OK:", 5, 22 + 12);	
	float_to_string(OK_Button, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 12);	

	//��ʾCANCEL ȡ������ֵ
	GUI_DispStringAt("CANCEL:", 5, 22 + 24);	
	float_to_string(CANCEL_Button, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 24);	

	//��ʾS_Alarm �������Ƽ���ֵ
	GUI_DispStringAt("Alarm:", 5, 22 + 36);	
	float_to_string(ALARM_Button, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 36);
	
	//��ʾS_Sample ��������ֵ
	GUI_DispStringAt("Sample:", 5, 22 + 48);	
	float_to_string(SAMPLE_Button, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 48);
	
	//��ʾS_MODE ģʽ�л�����ֵ
	GUI_DispStringAt("Mode:", 5, 22 + 60);	
	float_to_string(MODE_Button, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 66, 22 + 60);

	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 100);
	
	LCD_PWM_Config(ENABLE);
								
	
	//��ʾǰ����Ļ-------------------------------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 90);
	
	GUI_SetFont(&GUI_Font8_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);	
	
//	GUI_BMP_Draw(_acImage_0, 140, 10 + 32);				//Yunzhou logo
	
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 210, 190);	

	//��ʾǰ����Ļ-------------------------------------------------
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 90);
	
	GUI_SetFont(&GUI_Font8_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);	

	raw_message_handling();
	GUI_DispStringAt("Receive message:", 3, 8);
	GUI_DispStringAt(show_message, 18, 8 + 12);
	
	GUI_DispStringAt("Receive count:", 3, 8 + 24);
	float_to_string(receive_counter, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8 + 24);	

	GUI_DispStringAt("Receive good:", 3, 8 + 36);
	float_to_string(receive_good_count, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8 + 36);	
	
	GUI_DispStringAt("GPS latitude:", 3, 8 + 48);
	float_to_string(BoatData.DoubleLatitude, tempChar, 5, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8 + 48);	

	GUI_DispStringAt("GPS longitude:", 3, 8 + 60);
	float_to_string(BoatData.DoubleLongitude, tempChar, 5, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8 + 60);	
	
	GUI_DispStringAt("GPS speed:", 3, 8 + 72);
	float_to_string(BoatData.Speed, tempChar, 3, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8 + 72);	
	
	GUI_DispStringAt("Boat battery:", 3, 8 + 84);
	float_to_string(BoatData.BatLife, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8 + 84);

	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 130, 20);


	//��ʾǰ����Ļ-------------------------------------------------
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 100);
	
	GUI_SetFont(&GUI_Font8_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);	
	
	GUI_DispStringAt("Boat status:", 3, 8);
	float_to_string(BoatData.Status, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8);	
	
	GUI_DispStringAt("Timeout Count:", 3, 8 + 12);
	float_to_string(BoatDataTimeoutCount, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 8 + 12);	
	
	GUI_DispStringAt("Water temperature:", 3, 8 + 24);
	float_to_string(BoatData.WaterTemp * 10, tempChar, 3, 0, 0, 0);
	GUI_DispStringAt(tempChar, 110, 8 + 24);	
	
	//��ʾ��·�¶Ȼ����
	switch(local_para_conf[2])
	{
		case USV_Boat_Type_ME70:
		case USV_Boat_Type_ME120:
		case USV_Boat_Type_ME300:
			//��ʾ���
			GUI_DispStringAt("Water depth:", 3, 8 + 36);
			float_to_string(BoatData.Depth * 100, tempChar, 3, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 36);			
			break;
		
		default:
			//��ʾ��·�¶�
			GUI_DispStringAt("Internal temperature:", 3, 8 + 36);
			float_to_string(BoatData.InternalTemp*10, tempChar, 3, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 36);		
			break;
	}
	
	//����״̬��ʾ
	switch(local_para_conf[2])
	{
		case USV_Boat_Type_SS30:
		case USV_Boat_Type_ES30:
		case USV_Boat_Type_ESM30:
		case USV_Boat_Type_MS70:
		case USV_Boat_Type_MM70:
		case USV_Boat_Type_SS20:
		case USV_Boat_Type_ESM20:
		case USV_Boat_Type_MS120:
		case USV_Boat_Type_MM120:
			//�Ƿ����ڲ���
			GUI_DispStringAt("Is sampling:", 3, 8 + 48);
			float_to_string(GlobalVariable.IsManualSampling, tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 88, 8 + 48);				
			break;
		
		default:
			break;
	}
	
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 130, 120);

	return;
}	


/**
  * @brief  debug_mode_parameter ����ģʽ�������ý���
  * @param  
  * @retval 
  */
void debug_mode_parameter(void)
{
	char tempChar[20];
	uint16_t temp_i;
	
	//Step1����ʾ��������--------------------------------------------------------------
	if(debug_mode_parameter_init == 1)
	{	
		//��ʾǰ����Ļ
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		
		GUI_SetFont(&GUI_Font10_1);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetColor(GUI_WHITE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		//����ģ����ز���----------------------------------------------------------------------
		//��ʾ����ģ����ز���
		GUI_DispStringAt("RF Hopping Channel:", 5, 10);										//ͨ���ŵ�
		GUI_DispStringAt("RF VID:", 5, 10 + 12);													//ͨ��ID��
		GUI_DispStringAt("RF Source Address:", 5, 10 + 24);								//���ص�ַ
		GUI_DispStringAt("RF Destination Address:", 5, 10 + 36);					//Ŀ�ĵ�ַ
		GUI_DispStringAt("RF Address Mask:", 5, 10 + 48);									//��ַ����
		GUI_DispStringAt("RF TX Power Level:", 5, 10 + 60);								//���书��
		GUI_DispStringAt("RF API Enable:", 5, 10 + 72);										//APIģʽ�л�
		GUI_DispStringAt("RF Temperature:", 5, 10 + 84);									//����ģ���¶�								

		//��ȡ����ģ����ز���
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(Xtend_900_para[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 130, 10 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0);		
		//����ģ����ز���======================================================================

		
		//���ش洢����--------------------------------------------------------------------------
		//��ʾ���������
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("LCD Backlight:", 5, 8);

		//��ʾ���������ز���
		GUI_DispStringAt("Control Mode:", 5, 8 + 12);								//����ģʽ
		GUI_DispStringAt("USV Series:", 5, 8 + 24);									//����ϵ��
		GUI_DispStringAt("Rudder Mid-point:", 5, 8 + 36);						//�����λ
		
		//��������
		GUI_DispStringAt("Debug Mode:", 5, 8 + 48);									//����ģʽ
		GUI_DispStringAt("Sound Modify:", 5, 8 + 60);								//��������
		GUI_DispStringAt("language:", 5, 8 + 72);								//��������
		GUI_DispStringAt("Version:", 5, 8 + 84);										//�汾��
		GUI_DispStringAt(version_number, 110, 8 + 84);		
		
		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(local_para_conf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0 + 120);
		//���ش洢����==========================================================================	

		
		//���Ų���------------------------------------------------------------------------------
		if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
		{
			GUI_MEMDEV_Clear(bg1_memdev);					
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 250, 100);
			GUI_SetColor(GUI_WHITE);		
			
			GUI_DispStringAt("Valve 1:", 5, 8);
			GUI_DispStringAt("Valve 2:", 5, 8 + 12);
			GUI_DispStringAt("Valve 3:", 5, 8 + 24);
			GUI_DispStringAt("Valve 4:", 5, 8 + 36);
			GUI_DispStringAt("Valve 5:", 5, 8 + 48);

			for(temp_i = 0; temp_i < VALVE_QUANTITY; temp_i++)
			{
				float_to_string(ValveStatusUsed[temp_i], tempChar, 0, 0, 0, 0);
				GUI_DispStringAt(tempChar, 50, 8 + 12 * temp_i);
			}
			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 270, 10);	
		}						
		//���Ų���==============================================================================	

		
		//�����ʼ���ڶ���
		debug_mode_parameter_init = 2;	
		blink_times = ATCOMMAND_DELAY;								
	}
	
	//Step2���ȴ�AT ��+++��ָ��ص�2�롰OK��--------------------------------------------------------------
	else if(debug_mode_parameter_init == 2)
	{
		if(blink_times > 0)
		{
			if(blink_times == 4)
				enter_AT_Command();					//����AT����ģʽ������+++  
			
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1_memdev);	
			GUI_MEMDEV_Clear(bg1_memdev);					
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 250, 100);			
			
			if(blink_times % 2 == 0)								//��2���࣬Ϊ0����ʾ��1��ʾ����
			{	
				GUI_SetFont(&GUI_Font10_1);
				GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
				GUI_SetColor(GUI_WHITE);
				GUI_SetBkColor(GUI_TRANSPARENT);	
				
				GUI_DispStringAt("Reading RF parameter, please wait...", 5, 22);										//��ʾ�ȴ���Ϣ								
			}

			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 200, 10 + 0 + 200);			
			blink_times --;
		}
		
		else
		{
			debug_mode_parameter_init = 3;			//�����ȡ������ʼ����3��
		}			
	}
	
	//Step3�����ջ�����������ָ�������Ͳ�ѯָ��--------------------------------------------------------------
	else if(debug_mode_parameter_init == 3)	
	{
		//����ջ�����
		for(temp_i = 0; temp_i < USART1_RX_BUFFER_SIZE; temp_i++)
		{
			USART1_RX_Buffer[temp_i] = 0;
		}
		
		receive_pointer = 0;				//����ָ�����
		
		//���Ͳ�ѯ����
		send_AT_inquiry();	
		
		blink_times = 1;						//�ȴ�500���������
		debug_mode_parameter_init = 4;				//�ȴ�500����
	}		
	
	//Step4���ȴ�500���뻺�������--------------------------------------------------------------
	else if(debug_mode_parameter_init == 4)	
	{
		if(blink_times > 0)
		{
			blink_times--;
		}
		else
		{
			handle_parameter_message();					//���������ģ��ض��Ĳ���
			
			//������ģ������Ĳ�����ֵ����ʾ����
			for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
			{
				Xtend_900_para_buf[temp_i] = Xtend_900_para[temp_i];
			}
			
			//������ǰң��������������ֵ��ʾ����
			for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
			{
				local_para_conf_buf[temp_i] = local_para_conf[temp_i];
			}			
			
			debug_mode_parameter_init = 5;			//�������������ֵ
			
			USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);					//�ڲ�������ʱ��ֹ�ж�
		}
	}
	
	//Step5�����������޸�״̬
	else if(debug_mode_parameter_init == 5)	
	{
		//��ʾǰ������----------------------------------------------------------------------------------
		//����ģ����ز���----------------------------------------------------------------------		
		//��ʾǰ����Ļ
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		
		GUI_SetFont(&GUI_Font10_1);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetColor(GUI_WHITE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		//��ʾ����ģ����ز���
		GUI_DispStringAt("RF Hopping Channel:", 5, 10);										//ͨ���ŵ�
		GUI_DispStringAt("RF VID:", 5, 10 + 12);													//ͨ��ID��
		GUI_DispStringAt("RF Source Address:", 5, 10 + 24);								//���ص�ַ
		GUI_DispStringAt("RF Destination Address:", 5, 10 + 36);					//Ŀ�ĵ�ַ
		GUI_DispStringAt("RF Address Mask:", 5, 10 + 48);									//��ַ����
		GUI_DispStringAt("RF TX Power Level:", 5, 10 + 60);								//���书��
		GUI_DispStringAt("RF API Enable:", 5, 10 + 72);										//APIģʽ�л�
		GUI_DispStringAt("RF Temperature:", 5, 10 + 84);									//����ģ���¶�		

		//��ȡ����ģ����ز���
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(Xtend_900_para_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 130, 10 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0);	
		//����ģ����ز���======================================================================
		

		//���ش洢����--------------------------------------------------------------------------		
		//��ʾ���������
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("LCD Backlight:", 5, 8);

		//��ʾ���������ز���
		GUI_DispStringAt("Control Mode:", 5, 8 + 12);								//����ģʽ
		GUI_DispStringAt("USV Series:", 5, 8 + 24);									//����ϵ��
		GUI_DispStringAt("Rudder Mid-point:", 5, 8 + 36);						//�����λ
		
		//��������
		GUI_DispStringAt("Debug Mode:", 5, 8 + 48);									//����ģʽ
		GUI_DispStringAt("Sound Modify:", 5, 8 + 60);								//��������
		GUI_DispStringAt("language:", 5, 8 + 72);								//��������
		GUI_DispStringAt("Version:", 5, 8 + 84);										//�汾��
		GUI_DispStringAt(version_number, 110, 8 + 84);		

		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(local_para_conf_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 12 * temp_i);
		}		
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0 + 120);
		//���ش洢����==========================================================================			

		
		//���Ų���------------------------------------------------------------------------------
		if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
		{
			GUI_MEMDEV_Clear(bg1_memdev);					
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 250, 100);
			GUI_SetColor(GUI_WHITE);		
			
			GUI_DispStringAt("Valve 1:", 5, 8);
			GUI_DispStringAt("Valve 2:", 5, 8 + 12);
			GUI_DispStringAt("Valve 3:", 5, 8 + 24);
			GUI_DispStringAt("Valve 4:", 5, 8 + 36);
			GUI_DispStringAt("Valve 5:", 5, 8 + 48);

			for(temp_i = 0; temp_i < VALVE_QUANTITY; temp_i++)
			{
				float_to_string(ValveStatus[temp_i], tempChar, 0, 0, 0, 0);
				GUI_DispStringAt(tempChar, 50, 8 + 12 * temp_i);
			}
			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 270, 10);				
		}
		//���Ų���==============================================================================			
		//��ʾǰ������==================================================================================
		              
		
//		printf("%x, %x, %x, %x, %x, %x, %x, %x\n", Xtend_900_para_buf[0], Xtend_900_para_buf[1], Xtend_900_para_buf[2], Xtend_900_para_buf[3], \
//						Xtend_900_para_buf[4], Xtend_900_para_buf[5], Xtend_900_para_buf[6], Xtend_900_para_buf[7]);
		
		
		//��ʾ��ǰ�޸ı�����λ��------------------------------------------------------------------------------------------------
		//��ʾǰ�����������
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);			
		
		if(blink_t == 0)
		{
			blink_t = 1;
			
			//RF��ز���
			if(modify_index < PARAMETER_QUANTITY)				
			{
				GUI_DrawGradientRoundedH(0, 4 + modify_index * 12, 180, 15 + modify_index * 12, 2, GUI_ORANGE, GUI_RED);
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0);
			}
			
			//ң������������
			else if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY)
			{
				GUI_DrawGradientRoundedH(0, 2 + (modify_index - PARAMETER_QUANTITY) * 12, 180, 13 + (modify_index - PARAMETER_QUANTITY) * 12, 2, GUI_ORANGE, GUI_RED);
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0 + 120);			
			}

			//���Ų���
			else if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
			{
				GUI_DrawGradientRoundedH(0, 2 + (modify_index - PARAMETER_QUANTITY - LOCAL_PARAMETER_QUANTITY) * 12, 180, 13 + (modify_index - PARAMETER_QUANTITY - LOCAL_PARAMETER_QUANTITY) * 12, 2, GUI_ORANGE, GUI_RED);
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 265, 10);							
			}
		}
		
		else
		{
			blink_t = 0;
			
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0);	
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0 + 120);
			
			if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
			{
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 265, 10);	
			}			
		}		
		//��ʾ��ǰ�޸ı�����λ��================================================================================================
	}
		
	return;
}


/**
  * @brief  sample_mode_config ������������
  * @param  
  * @retval 
  */
void sample_mode_config(void)
{
	uint16_t temp_i = 0;
	char tempChar[20];

	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
	}
	
	
	//��ʾǰ������----------------------------------------------------------------------------------
	//��ʾǰ����Ļ
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 100);
	
	GUI_SetFont(&GUI_Font10_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//��ʾ����������ز���
	GUI_DispStringAt("Bottle Number:", 5, 10);												//����ƿ��
	GUI_DispStringAt("Sample Depth:", 5, 10 + 12);										//�������
	GUI_DispStringAt("Sample Mode:", 5, 10 + 24);											//����ģʽ
	GUI_DispStringAt("Sample_Volume:", 5, 10 + 36);										//��������
	GUI_DispStringAt("Wash Mode", 5, 10 + 48);												//��ϴģʽ

	//��ȡ����������ز���
	for(temp_i = 0; temp_i < WATER_SAMPLE_PARAMETER; temp_i++)
	{
		float_to_string(water_sample_conf_buf[temp_i], tempChar, 0, 0, 0, 0);
		GUI_DispStringAt(tempChar, 130, 10 + 12 * temp_i);
	}
	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0);	
	//��ʾǰ������==================================================================================


	//��ʾ��ǰ�޸ı�����λ��------------------------------------------------------------------------------------------------
	//��ʾǰ�����������
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 100);			
	
	if(blink_t == 0)
	{
		blink_t = 1;
		
		GUI_DrawGradientRoundedH(0, 4 + modify_index * 12, 180, 15 + modify_index * 12, 2, GUI_ORANGE, GUI_RED);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0);
	}
	
	else
	{
		blink_t = 0;
		
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0);	
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0 + 120);
	}		
	//��ʾ��ǰ�޸ı�����λ��================================================================================================			
	
	return;
}


/**
  * @brief  save_sample_config ������������
  * @param  
  * @retval 
  */
void save_sample_config(void)
{
	uint16_t i;
	
	for(i = 0; i < WATER_SAMPLE_PARAMETER; i++)
	{
		water_sample_conf[i] = water_sample_conf_buf[i];
	}
	
	return;
}


/**
  * @brief  load_sample_config ����������ȡ
  * @param  
  * @retval 
  */
void load_sample_config(void)
{
	uint16_t i;
	
	for(i = 0; i < WATER_SAMPLE_PARAMETER; i++)
	{
		water_sample_conf_buf[i] = water_sample_conf[i];
	}
	
	return;
}


/**
  * @brief  raw_message_handling ֱ����ʾ���յ��Ļ�������
  * @param  
  * @retval 
  */
void raw_message_handling(void)
{
	uint16_t message_i = 0;

	//��������֡����---------------------------------------
	if(receive_pointer > 0)			//�����յ��ַ��������֡����
	{
		for(message_i = 0; message_i < receive_pointer; message_i++)
		{
			show_message[message_i] = USART1_RX_Buffer[message_i];
			
			if(message_i >= 400)
			{
				message_i = 0;
			}
		}
		
		show_message[message_i] = '\0';
		
//		printf("%s", show_message);
						
		receive_pointer = 0;
	}	
}


/**
  * @brief  Draw_Empty_Bottle ������ƿ����
  * @param  start_pix�� ��ʼ���ص�
  * @retval 
  */
void Draw_Empty_Bottle(uint8_t start_pix)
{
	uint8_t i = 0;
	
	for(i = 0; i < 4; i++)
	{
		GUI_DrawGradientRoundedH(start_pix + 2 + 15 * i, 39, start_pix + 7 + 15 * i, 59, 1, GUI_GRAY, GUI_WHITE);									
		GUI_DrawGradientRoundedH(start_pix + 7 + 15 * i, 39, start_pix + 11 + 15 * i, 59, 1, GUI_WHITE, GUI_GRAY);
	}
}




/**
  * @brief  LCD_Test LCD����
  * @param  timer: ��¼��ǰˢ��״̬
  * @retval ����G_div
  */
uint16_t LCD_Test(uint16_t timer)
{
	uint16_t G_div = timer;
	
	//2������ʾ����
	if(G_div == 200)
	{
		printf("G_div: %d\n", G_div);
		
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);
		GUI_SetColor(GUI_RED);
		GUI_FillRect(0, 0, 250, 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0);	
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 200);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 200);				
	}
	
	//2����ʾ����
	else if(G_div == 400)
	{
		printf("G_div: %d\n", G_div);

		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);
		GUI_SetColor(GUI_GREEN);
		GUI_FillRect(0, 0, 250, 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0);	
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 200);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 200);					
	}
	
	//2����ʾ����
	else if(G_div == 600)
	{
		printf("G_div: %d\n", G_div);
		
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);
		GUI_SetColor(GUI_BLUE);
		GUI_FillRect(0, 0, 250, 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0);	
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 200);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 200);					
	}
	
	//2����ʾ����
	else if(G_div == 800)
	{
		printf("G_div: %d\n", G_div);
		
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 0, 250, 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0);	
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 200);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 200);					
	}
	
	else if(G_div > 1000)
	{
		printf("G_div: %d\n", G_div);
		
		G_div = 0;
		
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0);	
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 100);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 200);
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 200);	
		
		interface_index = 2;
		debug_mode_parameter_init = 5;
	}

	return G_div;
}


/**
  * @brief  LCD_clear ����Ļ
  * @param  
  * @retval 
  */
void LCD_clear(void)
{
	//����ʾ����
//	GUI_SelectLayer(1);
//	GUI_MEMDEV_Select(bg1_memdev);	
//	GUI_MEMDEV_Clear(bg1_memdev);					
//	GUI_SetColor(GUI_TRANSPARENT);
//	GUI_SetBkColor(GUI_TRANSPARENT);
//	GUI_ClearRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);					
//	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0);
//	GUI_ClearRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
//	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0);
//	GUI_ClearRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
//	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 100);
//	GUI_ClearRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
//	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 100);
//	GUI_ClearRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
//	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 0 + 200);
//	GUI_ClearRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
//	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 250, 0 + 200);

//	
//	GUI_SelectLayer(0);
//	GUI_MEMDEV_Select(bg0_memdev);	
//	GUI_MEMDEV_Clear(bg0_memdev);					
//	GUI_SetColor(GUI_TRANSPARENT);
//	GUI_SetBkColor(GUI_TRANSPARENT);
//	GUI_ClearRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
//	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);	
//	GUI_ClearRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
//	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 0);
//	GUI_ClearRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
//	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0 + 100);	
//	GUI_ClearRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
//	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 0 + 100);		
//	GUI_ClearRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
//	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0 + 200);		
//	GUI_ClearRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
//	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 0 + 200);	

//	GUI_Clear();
	GUI_SelectLayer(1);
	GUI_Clear();	
	GUI_SelectLayer(0);
	GUI_Clear();	

	GUI_MEMDEV_Clear(bg1_memdev);
	GUI_MEMDEV_Clear(bg0_memdev);	
	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	modify_index = 0;
}


/*-------------------------------------------------------------------------
	��������show_save_wait
	��  �ܣ��������ñ���ȴ�����
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void show_save_wait(void)
{
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);		
	GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
			
	GUI_BMP_Draw(_acImage_14, 80, 0);											//��̾��ͼ��
		
	GUI_SetColor(GUI_YELLOW);
	GUI_SetBkColor(GUI_TRANSPARENT);	
				
	if(local_para_conf[6] == 1)						//����
	{
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
		GUI_SetFont(&GUI_Fontsongti16);	
		GUI_DispStringAt(_canshucunchuzhong[0], 100, 60);				//�����洢��
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_DispStringAt(_qingwuduandian[0], 100, 85);						//����ϵ�
	}
	else
	{
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
		GUI_SetFont(&GUI_Font16_1);					
		GUI_DispStringAt("Saving", 100, 60);	
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_DispStringAt("Don't power down!!!", 100, 85);					
	}																				
		
	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 130, 48);
}


/**
  * @brief  float to string ��ֵת�ַ�������aq

  * @param  data����������
						str��Ϊ����ַ�����
						length������С�����λ��
						unit�������λ 0Ϊû�е�λ��1Ϊ��%����2Ϊ��m��
						prechar��ǰ׺�ַ�����
						prelength��ǰ׺���鳤��

  * @retval ����ת������ַ����鳤��i
  */
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength)
{
  int i = 0, j, k;
  long temp, tempoten;
  u8 intpart[20], dotpart[20]; 

	//����ǰ׺����
	for(k = 0; k < prelength; k++)
	{
		str[i++] = prechar[k];
	}

	//����������
  if(data < 0) 
	{
		str[i++] = '-';
		data = -data;
	}
//  else str[0]='+';

	//������������
  temp = (long) data;

	j = 0;
  tempoten = temp / 10;
  while(tempoten != 0)
  {
    intpart[j] = temp - 10 * tempoten + 48; 	//to ascii code
    temp = tempoten;
    tempoten = temp / 10;
    j++;
  }
  intpart[j++] = temp + 48;

	//����С������
	if(length >= 1)
	{
		data = data - (long) data;  
		for(k = 0; k < length; k++)
		{
			dotpart[k] = (int)(data*10) + 48;
			data = data * 10.0;
			data = data - (long)data;
		}
	}

	for(; j > 0; j--)
	{
		str[i++] = intpart[j - 1];
	}
	
	if(length >= 1)
	{
		str[i++] = '.';
		
		for(k = 0; k < length; k++)
		{
			str[i++] = dotpart[k];
		}	
	}		

	//���ֵ�λ
	if(unit == 1)
	{
		str[i++] = '%';							//�ٷ�����λ	
	}
	else if(unit == 2)
	{
		str[i++] = 'm';							//�׵�λ
	}
	else if(unit == 3)
	{
		str[i++] = '#';							//�׵�λ
	}
	
	str[i++] = '\0';
	
	return i;
}


/**
  * @brief  mdelay ��ռ��ʱ����
  * @param  timer����ռ��ʱ�ݼ�����
  * @retval none
  */
void mdelay(volatile uint32_t timer)
{
	while(timer--);
}
