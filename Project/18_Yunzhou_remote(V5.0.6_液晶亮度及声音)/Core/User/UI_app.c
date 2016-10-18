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


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Graphic_Interface_Task_Handle;

uint8_t interface_index = DEBUG_MAIN_INTERFACE_INDEX;			//����������1Ϊ������
																													//					2Ϊ�������ý���
																													//					3Ϊ�������ý���
uint8_t modify_index = 0;					//�����޸�����
uint8_t EngineTimerCount = 5;			//��ֹ��������ʱ��

GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg1a_memdev, bg0a_memdev;								//��ʾ�洢����																	


/* Private variables ---------------------------------------------------------*/
char show_message[460];			//��ʾ���յ���Ϣ

uint8_t debug_mode_parameter_init = 1;			//���沽������
uint8_t blink_times = 0;										//��˸����
uint8_t blink_t = 0;												//�����˸

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
  GUI_SelectLayer(0);

//	GUI_BMP_Draw(_acImage_0, 358, 226);				//Yunzhou logo
	
	GUI_SelectLayer(1);	
	bg1_memdev = GUI_MEMDEV_Create(0, 0, 250, 100);
	bg1w_memdev = GUI_MEMDEV_Create(0, 0, 100, 50);
	GUI_SelectLayer(0);	
	bg0_memdev = GUI_MEMDEV_Create(0, 0, 250, 100);
	bg0s_memdev = GUI_MEMDEV_Create(0, 0, 100, 50);
	
	interface_index = 1;	

  /* Run the Graphic_Interface task */
  while (1)
  {			
		G_div++;												//��Ļ��ʾ��ʱ��
		RF_transmit_timer++;						//���ߴ����ʱ��
		
		//debugģʽ�µĽ�����ʾ
		local_para_conf[4] = 1;
		
		if(local_para_conf[4] == 1)
		{
			//������Ϊ1Ϊ������--------------------------------------------------
			if(interface_index == DEBUG_MAIN_INTERFACE_INDEX)
			{
				//100������Ӧһ����ʾ
				if(G_div >= DEBUG_MAIN_DIVISION)
				{
					G_div = 0;
					debug_mode_main();
//					USART_SendData(USART1, 0x88);
				}
				
				if(MENU_Button == 1)				//���������°���menu������������ý���
				{
					interface_index = DEBUG_PARAM_INTERFACE_INDEX;
					debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
					LCD_clear();						//����Ļ
				}
				
				else if((SAMPLE_Button == 1) && (MODE_Button != CONTROL_MODE_AUTO))			//���������²��� SAMPLE ���� ť�ӿ��ؽ���������ý��棨���в�ˮ���ܵĴ�ֻ��
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
					LCD_PWM_Config();								//Һ�������޸�					
					
					if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300�������ɿ��Ʒ���
					{
						save_Valve_parameter();			//���淧�Ų���
					}
									
					vTaskDelay(500);
					
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//�������ý����������ж�
					
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//���ص���������	
					LCD_clear();						//����Ļ
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


//		//�������������
//		ExecTouch();
		
		wait_send++;
		
		if(wait_send > 10)
		{
			wait_send = 0;
//			printf("aaaaa\n");
//			USART_SendData(USART2, 0x86);
//			printf("L_UD: %d, L_LR: %d, R_UD: %d, R_LR: %d\n", left_joystick_ud, left_joystick_lr, right_joystick_ud, right_joystick_lr);
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
			
			if(BoatDataTimeoutCount > 2 * BOATDATATIMEOUTVALUE)
			{
				BoatDataTimeoutCount = BOATDATATIMEOUTVALUE;
			}	

			//���ӳ�ʱ����
			if((BoatDataTimeoutCount > 5) && (interface_index == DEBUG_MAIN_INTERFACE_INDEX))
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
	
	GUI_BMP_Draw(_acImage_0, 140, 10 + 32);				//Yunzhou logo
	
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
		GUI_DispStringAt("Version:", 5, 8 + 72);										//�汾��
		GUI_DispStringAt(version_number, 110, 8 + 72);		
		
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
		GUI_DispStringAt("Version:", 5, 8 + 72);										//�汾��
		GUI_DispStringAt(version_number, 110, 8 + 72);		

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
	
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 100);					
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);	
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 0);
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0 + 100);
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 0 + 100);
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0 + 200);
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 0 + 200);	
	
	modify_index = 0;
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
//	else if(unit == 3)
//	{
//		str[i++] = 0xe6;							//�浥λ
//		str[i++] = 0xa1;	
//	}
	
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
