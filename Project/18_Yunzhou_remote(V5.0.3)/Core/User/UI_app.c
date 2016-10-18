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

uint8_t interface_index = 1;			//����������1Ϊ������
																	//					2Ϊ�������ý���
																	//					3Ϊ�������ý���
uint8_t modify_index = 0;					//�����޸�����

GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg1a_memdev, bg0a_memdev;								//��ʾ�洢����																	


/* Private variables ---------------------------------------------------------*/
char show_message[60];			//��ʾ���յ���Ϣ

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
		G_div++;	
		
		//debugģʽ�µĽ�����ʾ
		local_para_conf[4] = 1;
		
		if(local_para_conf[4] == 1)
		{
			//������Ϊ1Ϊ������--------------------------------------------------
			if(interface_index == 1)
			{
				//100������Ӧһ����ʾ
				if(G_div >= DEBUG_MAIN_DIVISION)
				{
					G_div = 0;
					debug_mode_main();
					USART_SendData(USART1, 0x88);
				}
				
				if(MENU_Button == 1)				//���������°���menu������������ý���
				{
					interface_index = 2;
					debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
					modify_index = 0;
					LCD_clear();						//����Ļ
				}
				
				else if(SAMPLE_Button == 1)			//���������²��� SAMPLE ���� ť�ӿ��ؽ���������ý���
				{
//					interface_index = 3;
				}
			}
			
			//������Ϊ2Ϊ�������ý���--------------------------------------------------
			else if(interface_index == 2)
			{				
				if(debug_mode_parameter_init == 1)
					debug_mode_parameter();
				
				//500������Ӧһ����ʾ
				if(G_div >= DEBUG_PARAMETER_DIVISION)
				{
					G_div = 0;
					debug_mode_parameter();
				}
														
				if(OK_Button == 1)				//�ڲ������ý����°���OK���󣬱��浱ǰ���������������Խ���
				{
					save_parameter();				//���浱ǰ������STM32��FLASH��
					save_RF_parameter();		//��������ģ�����				
					interface_index = 1;					
					LCD_clear();						//����Ļ
				}
				
				else if(CANCEL_Button == 1)				//�ڲ������ý����°���CANCEL���󣬲����浱ǰ���������������Խ���
				{
					interface_index = 1;
					LCD_clear();						//����Ļ
				}
				
				//����ȡ������Ϻ�MENU���������LCD����ģʽ
				else if((debug_mode_parameter_init == 5) && (MENU_Button == 1))
				{
					interface_index = 4;
					G_div = 0;
				}			
			}
			
			//������Ϊ4ΪҺ�����Խ���-------------------------------------------------
			else if(interface_index == 4)
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
//			USART_SendData(USART1, 0x86);
//			printf("L_UD: %d, L_LR: %d, R_UD: %d, R_LR: %d\n", left_joystick_ud, left_joystick_lr, right_joystick_ud, right_joystick_lr);
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
	GUI_DispStringAt("Receive message:", 3, 12);
	GUI_DispStringAt(show_message, 18, 12 + 12);
	
	GUI_DispStringAt("Receive count:", 3, 12 + 24);
	float_to_string(receive_counter, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 12 + 24);	

	GUI_DispStringAt("Receive good:", 3, 12 + 36);
	float_to_string(receive_good_count, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 88, 12 + 36);	
	
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 130, 20);
	
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
		GUI_DispStringAt("Version:", 5, 8 + 60);										//�汾��
		GUI_DispStringAt(version_number, 110, 8 + 60);		
		
		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(local_para_conf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0 + 120);
		    
		
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

			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 200, 10 + 0 + 100);			
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
		}
	}
	
	//Step5�����������޸�״̬
	else if(debug_mode_parameter_init == 5)	
	{
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
		GUI_DispStringAt("Version:", 5, 8 + 60);										//�汾��
		GUI_DispStringAt(version_number, 110, 8 + 60);		

		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(local_para_conf_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 12 * temp_i);
		}		
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0 + 120);
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
			if(modify_index <= 7)				
			{
				GUI_DrawGradientRoundedH(0, 4 + modify_index * 12, 180, 15 + modify_index * 12, 2, GUI_ORANGE, GUI_RED);
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0);
			}
			
			//ң������������
			else
			{
				GUI_DrawGradientRoundedH(0, 2 + (modify_index - 8) * 12, 180, 13 + (modify_index - 8) * 12, 2, GUI_ORANGE, GUI_RED);
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0 + 120);			
			}	
		}
		
		else
		{
			blink_t = 0;
			
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0);	
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0 + 120);
		}		
		//��ʾ��ǰ�޸ı�����λ��================================================================================================
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
		}
		
		show_message[message_i] = '\0';
		
		printf("%s", show_message);
						
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
