/*-------------------------------------------------------------------------
�������ƣ�ң��������Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

#include "UI_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Graphic_Interface_Task_Handle;

uint8_t interface_index = 1;			//����������1Ϊ������
																	//					2Ϊ�������ý���
																	//					3Ϊ�������ý���
																	

/* Private variables ---------------------------------------------------------*/
GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev;								//��ʾ�洢����

uint8_t debug_mode_parameter_init = 1;			//����ģʽ��ʼ����־��1��ʾû��ʼ����0��ʾ�ѳ�ʼ��
uint8_t blink_times = 0;										//��˸����


/**
  * @brief  Graphic user interface task
  * @param  pvParameters not used
  * @retval None
  */
void Graphic_Interface_Task(void * pvParameters)
{   
	uint8_t pointi = 0, tempi = 0;
	uint8_t G_div = 0;						//��ʾ��Ƶ����
	uint16_t pointi2 = 0;
	char tempChar[20];
	uint32_t colorTemp;				//�ݴ���ɫ
	uint16_t wait_send = 0;
	
	/* Initialize the BSP layer */
  LowLevel_Init();
	
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
//	bg1w_memdev = GUI_MEMDEV_CreateFixed(0, 0, 35, 43, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
	GUI_SelectLayer(0);	
	bg0_memdev = GUI_MEMDEV_Create(0, 0, 250, 100);
	bg0s_memdev = GUI_MEMDEV_Create(0, 0, 100, 50);
//	bg1_memdev = GUI_MEMDEV_CreateFixed(0, 0, 90, 90, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);

	interface_index = 1;
	
	//����ҳ���ʼ��
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 120, 90);
	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 272);
	

  /* Run the Graphic_Interface task */
  while (1)
  {			
		G_div++;	
		
		if(debug_Mode == 1)
		{
			//������Ϊ1Ϊ������--------------------------------------------------
			if(interface_index == 1)
			{
				//�л�����1ҳ��
				GUI_SetOrg(0, 0); 				/* Set origin to screen 1 */	

				//100������Ӧһ����ʾ
				if(G_div >= DEBUG_MAIN_DIVISION)
				{
					G_div = 0;
					debug_mode_main();
				}
				
				if(MENU_Button == 1)				//���������°���menu������������ý���
				{
					interface_index = 2;
					debug_mode_parameter_init = 1;				//����������ý���ʱ�ĳ�ʼ����־
				}
				
				else if(SAMPLE_Button == 1)			//���������²��� SAMPLE ���� ť�ӿ��ؽ���������ý���
				{
					interface_index = 3;
				}
			}
			
			//������Ϊ2Ϊ�������ý���--------------------------------------------------
			else if(interface_index == 2)
			{
				//�л�����2ҳ��
				GUI_SetOrg(0, 272); 				/* Set origin to screen 2 */								
				
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
					interface_index = 1;
				}
				
				else if(CANCEL_Button == 1)				//�ڲ������ý����°���CANCEL���󣬲����浱ǰ���������������Խ���
				{
					interface_index = 1;
				}
			}
			

			//����LCD����ģʽ
			if(MENU_Button && OK_Button)
			{
				GUI_SelectLayer(1);
				GUI_Clear();
				GUI_SetBkColor(GUI_BLACK); 		
				GUI_SelectLayer(0);
				GUI_Clear();					
				GUI_SetColor(GUI_BLACK);
				GUI_FillRect(0, 0, 479, 271);	
				vTaskDelay(2000);
				
				GUI_SetColor(GUI_RED);
				GUI_FillRect(0, 0, 479, 271);
				vTaskDelay(2000);

				GUI_SetColor(GUI_GREEN);
				GUI_FillRect(0, 0, 479, 271);
				vTaskDelay(2000);

				GUI_SetColor(GUI_BLUE);
				GUI_FillRect(0, 0, 479, 271);
				vTaskDelay(2000);

				GUI_SetColor(GUI_WHITE);
				GUI_FillRect(0, 0, 479, 271);
				vTaskDelay(2000);		

				GUI_SetBkColor(GUI_TRANSPARENT);
				GUI_SelectLayer(1);
				GUI_Clear();
				GUI_SetBkColor(GUI_TRANSPARENT); 
				GUI_SelectLayer(0);	
				GUI_Clear();
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
	
	GUI_DispStringAt("For binary command, push OK and CANCEL", 3, 22);										//915����ģ���������������ģʽ��ʾ
	GUI_DispStringAt("For LCD testing, push MENU and OK at the same time", 3, 22 + 12);				//Һ����������Գ���
	
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
	
	//����������ý���ʱ��ʼ��һ��
	if(debug_mode_parameter_init == 1)
	{	
		//��ʾǰ����Ļ-------------------------------------------------
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
		GUI_DispStringAt("RF Hopping Channel:", 5, 22);										//ͨ���ŵ�
		GUI_DispStringAt("RF VID:", 5, 22 + 12);													//ͨ��ID��
		GUI_DispStringAt("RF Source Address:", 5, 22 + 24);								//���ص�ַ
		GUI_DispStringAt("RF Destination Address:", 5, 22 + 36);					//Ŀ�ĵ�ַ
		GUI_DispStringAt("RF Address Mask:", 5, 22 + 48);									//��ַ����
		GUI_DispStringAt("RF TX Power Level:", 5, 22 + 60);								//���书��
		GUI_DispStringAt("RF API Enable:", 5, 22 + 72);										//APIģʽ�л�
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 272);
		
		//��ʾ���������
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("LCD Backlight:", 5, 22);

		//��ʾ���������ز���
		GUI_DispStringAt("Control Mode:", 5, 22 + 22);										//����ģʽ
		GUI_DispStringAt("USV Series:", 5, 22 + 22 + 12);									//����ϵ��
		GUI_DispStringAt("Rudder Mid-point:", 5, 22 + 22 + 24);						//�����λ
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 272 + 100);
		
		//��������
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		
		GUI_DispStringAt("Debug Mode:", 5, 8);												//����ģʽ
		if(debug_Mode == 1)
			GUI_DispStringAt("ON", 72, 8);	
		else
			GUI_DispStringAt("OFF", 72, 8);
		
		GUI_DispStringAt("Version:", 5, 8 + 12);											//�汾��
		GUI_DispStringAt(version_number, 50, 8 + 12);									
		
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 272 + 200);

		
		//����AT����ģʽ������+++      
		enter_AT_Command();	
		
		//�����ʼ���ڶ���
		debug_mode_parameter_init = 2;	
		blink_times = ATCOMMAND_DELAY;								
	}
	
	//�ڶ������ȴ�AT ��+++��ָ��ص�2�롰OK��
	else if(debug_mode_parameter_init == 2)
	{
		if(blink_times > 0)
		{
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

			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 150, 10 + 272 + 100);			
			blink_times --;
		}
		
		else
		{
			debug_mode_parameter_init = 3;			//�����ȡ������ʼ����3��
		}			
	}
	
	//�����������ջ�����������ָ�������Ͳ�ѯָ��
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
	
	//���Ĳ����ȴ�500���뻺�������
	else if(debug_mode_parameter_init == 4)	
	{
		if(blink_times > 0)
		{
			blink_times--;
		}
		else
		{
			debug_mode_parameter_init = 5;			//�������������ֵ
		}
	}
	
	return;
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

	if(unit == 1)
	{
		str[i++] = '%';
	}
	else if(unit == 2)
	{
		str[i++] = 'm';
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
