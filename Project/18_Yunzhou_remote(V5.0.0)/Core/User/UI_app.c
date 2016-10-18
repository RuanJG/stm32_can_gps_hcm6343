/*-------------------------------------------------------------------------
工程名称：遥控器界面应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

#include "UI_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Graphic_Interface_Task_Handle;


/**
  * @brief  Graphic user interface task
  * @param  pvParameters not used
  * @retval None
  */
void Graphic_Interface_Task(void * pvParameters)
{   
  GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev;
	uint8_t pointi = 0, tempi = 0, difSp_active = 0;
	uint16_t pointi2 = 0;
	char tempChar[20];
	uint32_t colorTemp;				//暂存颜色
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
	bg1_memdev = GUI_MEMDEV_Create(0, 0, 120, 90);
	bg1w_memdev = GUI_MEMDEV_Create(0, 0, 39, 43);
//	bg1w_memdev = GUI_MEMDEV_CreateFixed(0, 0, 35, 43, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
	GUI_SelectLayer(0);	
	bg0_memdev = GUI_MEMDEV_Create(0, 0, 250, 100);
	bg0s_memdev = GUI_MEMDEV_Create(0, 0, 58, 14);
//	bg1_memdev = GUI_MEMDEV_CreateFixed(0, 0, 90, 90, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
	
  /* Run the Graphic_Interface task */
  while (1)
  {			
		pointi++;
		
		if(pointi >= 10)
		{
			pointi = 0;
			
			if(debug_Mode == 1)
			{
				//进入LCD测试模式
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
				
				else
				{
					//显示前清屏幕-------------------------------------------------
					GUI_SelectLayer(1);
					GUI_MEMDEV_Select(bg1_memdev);	
					GUI_MEMDEV_Clear(bg1_memdev);					
					GUI_SetColor(GUI_TRANSPARENT);
					GUI_FillRect(0, 0, 120, 90);
					
					GUI_SetFont(&GUI_Font10_1);
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_SetColor(GUI_WHITE);
					GUI_SetBkColor(GUI_TRANSPARENT);
					
					//显示遥控器电量
					GUI_DispStringAt("RC_Power:", 5, 22);	
					float_to_string(Battery_Power_Avg, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22);		
					
					//显示电位器旋钮读数
					GUI_DispStringAt("KNOB:", 5, 22 + 12);	
					float_to_string(Knob_Avg, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 12);		
					
					//显示左侧摇杆 X 轴采样均值
					GUI_DispStringAt("LJX:", 5, 22 + 24);	
					float_to_string(Left_Joystick_X_axis_Avg, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 24);	
					
					//显示左侧摇杆 Y 轴采样均值
					GUI_DispStringAt("LJY:", 5, 22 + 36);	
					float_to_string(Left_Joystick_Y_axis_Avg, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 36);	
					
					//显示右侧摇杆 X 轴采样均值
					GUI_DispStringAt("RJX:", 5, 22 + 48);	
					float_to_string(Right_Joystick_X_axis_Avg, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 48);	
					
					//显示右侧摇杆 Y 轴采样均值
					GUI_DispStringAt("RJY:", 5, 22 + 60);	
					float_to_string(Right_Joystick_Y_axis_Avg, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 60);					

					GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10);
					
					//显示前清屏幕-------------------------------------------------
					GUI_SelectLayer(1);
					GUI_MEMDEV_Select(bg1_memdev);	
					GUI_MEMDEV_Clear(bg1_memdev);					
					GUI_SetColor(GUI_TRANSPARENT);
					GUI_FillRect(0, 0, 120, 90);	
					
					GUI_SetFont(&GUI_Font10_1);
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_SetColor(GUI_WHITE);
					GUI_SetBkColor(GUI_TRANSPARENT);

					//显示MENU 菜单键键值
					GUI_DispStringAt("MENU:", 5, 22);	
					float_to_string(MENU_Button, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22);	
					
					//显示OK 确认键键值
					GUI_DispStringAt("OK:", 5, 22 + 12);	
					float_to_string(OK_Button, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 12);	

					//显示CANCEL 取消键键值
					GUI_DispStringAt("CANCEL:", 5, 22 + 24);	
					float_to_string(CANCEL_Button, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 24);	

					//显示S_Alarm 警报警灯键键值
					GUI_DispStringAt("Alarm:", 5, 22 + 36);	
					float_to_string(ALARM_Button, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 36);
					
					//显示S_Sample 采样键键值
					GUI_DispStringAt("Sample:", 5, 22 + 48);	
					float_to_string(SAMPLE_Button, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 48);
					
					//显示S_MODE 模式切换键键值
					GUI_DispStringAt("Mode:", 5, 22 + 60);	
					float_to_string(MODE_Button, tempChar, 0, 0, 0, 0);
					GUI_DispStringAt(tempChar, 66, 22 + 60);

					GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 100);
												
					
					//显示前清屏幕-------------------------------------------------
					GUI_SelectLayer(0);
					GUI_MEMDEV_Select(bg0_memdev);	
					GUI_MEMDEV_Clear(bg0_memdev);					
					GUI_SetColor(GUI_TRANSPARENT);
					GUI_FillRect(0, 0, 250, 90);
					
					GUI_SetFont(&GUI_Font8_1);
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_SetColor(GUI_WHITE);
					GUI_SetBkColor(GUI_TRANSPARENT);	
					
					GUI_DispStringAt("For binary command, push OK and CANCEL", 3, 22);										//915数传模块允许二进制命令模式提示
					GUI_DispStringAt("For LCD testing, push MENU and OK at the same time", 3, 22 + 12);				//液晶屏坏点测试程序
					
					GUI_BMP_Draw(_acImage_0, 140, 10 + 32);				//Yunzhou logo
					
					GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 210, 190);	

					//显示前清屏幕-------------------------------------------------
					GUI_MEMDEV_Clear(bg0_memdev);					
					GUI_SetColor(GUI_TRANSPARENT);
					GUI_FillRect(0, 0, 250, 90);
					
					GUI_SetFont(&GUI_Font8_1);
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_SetColor(GUI_WHITE);
					GUI_SetBkColor(GUI_TRANSPARENT);	
					
					GUI_DispStringAt("Receive message:", 3, 12);
					GUI_DispStringAt(show_message, 18, 12 + 12);
					GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 130, 20);	
				}				
			}
		}

//		//触摸屏坐标更新
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
  * @brief  float to string 数值转字符串函数aq

  * @param  data：输入数据
						str：为输出字符数组
						length：保留小数点后位数
						unit：输出单位 0为没有单位，1为“%”，2为“m“
						prechar：前缀字符数组
						prelength：前缀数组长度

  * @retval 返回转换后的字符数组长度i
  */
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength)
{
  int i = 0, j, k;
  long temp, tempoten;
  u8 intpart[20], dotpart[20]; 

	//处理前缀部分
	for(k = 0; k < prelength; k++)
	{
		str[i++] = prechar[k];
	}

	//处理正负数
  if(data < 0) 
	{
		str[i++] = '-';
		data = -data;
	}
//  else str[0]='+';

	//处理整数部分
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

	//处理小数部分
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
