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

uint8_t interface_index = 1;			//界面索引：1为主界面
																	//					2为参数配置界面
																	//					3为采样配置界面
																	

/* Private variables ---------------------------------------------------------*/
GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev;								//显示存储变量

uint8_t debug_mode_parameter_init = 1;			//参数模式初始化标志，1表示没初始化，0表示已初始化
uint8_t blink_times = 0;										//闪烁次数


/**
  * @brief  Graphic user interface task
  * @param  pvParameters not used
  * @retval None
  */
void Graphic_Interface_Task(void * pvParameters)
{   
	uint8_t pointi = 0, tempi = 0;
	uint8_t G_div = 0;						//显示分频因子
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
	bg1_memdev = GUI_MEMDEV_Create(0, 0, 250, 100);
	bg1w_memdev = GUI_MEMDEV_Create(0, 0, 100, 50);
//	bg1w_memdev = GUI_MEMDEV_CreateFixed(0, 0, 35, 43, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
	GUI_SelectLayer(0);	
	bg0_memdev = GUI_MEMDEV_Create(0, 0, 250, 100);
	bg0s_memdev = GUI_MEMDEV_Create(0, 0, 100, 50);
//	bg1_memdev = GUI_MEMDEV_CreateFixed(0, 0, 90, 90, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);

	interface_index = 1;
	
	//虚拟页面初始化
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
			//索引号为1为主界面--------------------------------------------------
			if(interface_index == 1)
			{
				//切换至第1页面
				GUI_SetOrg(0, 0); 				/* Set origin to screen 1 */	

				//100毫秒响应一次显示
				if(G_div >= DEBUG_MAIN_DIVISION)
				{
					G_div = 0;
					debug_mode_main();
				}
				
				if(MENU_Button == 1)				//在主界面下按下menu键进入参数设置界面
				{
					interface_index = 2;
					debug_mode_parameter_init = 1;				//进入参数配置界面时的初始化标志
				}
				
				else if(SAMPLE_Button == 1)			//在主界面下拨动 SAMPLE 采样 钮子开关进入采样配置界面
				{
					interface_index = 3;
				}
			}
			
			//索引号为2为参数配置界面--------------------------------------------------
			else if(interface_index == 2)
			{
				//切换至第2页面
				GUI_SetOrg(0, 272); 				/* Set origin to screen 2 */								
				
				if(debug_mode_parameter_init == 1)
					debug_mode_parameter();
				
				//500毫秒响应一次显示
				if(G_div >= DEBUG_PARAMETER_DIVISION)
				{
					G_div = 0;
					debug_mode_parameter();
				}
														
				if(OK_Button == 1)				//在参数配置界面下按下OK键后，保存当前参数并返回主调试界面
				{
					interface_index = 1;
				}
				
				else if(CANCEL_Button == 1)				//在参数配置界面下按下CANCEL键后，不保存当前参数并返回主调试界面
				{
					interface_index = 1;
				}
			}
			

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
  * @brief  debug_mode_main 调试模式主界面
  * @param  
  * @retval 
  */
void debug_mode_main(void)
{
	char tempChar[20];	
	
//	USART_SendData(USART2, 0x86);
	
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
  * @brief  debug_mode_parameter 调试模式参数配置界面
  * @param  
  * @retval 
  */
void debug_mode_parameter(void)
{
	char tempChar[20];
	uint16_t temp_i;
	
	//进入参数配置界面时初始化一次
	if(debug_mode_parameter_init == 1)
	{	
		//显示前清屏幕-------------------------------------------------
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		
		GUI_SetFont(&GUI_Font10_1);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetColor(GUI_WHITE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		//显示数传模块相关参数
		GUI_DispStringAt("RF Hopping Channel:", 5, 22);										//通信信道
		GUI_DispStringAt("RF VID:", 5, 22 + 12);													//通信ID号
		GUI_DispStringAt("RF Source Address:", 5, 22 + 24);								//本地地址
		GUI_DispStringAt("RF Destination Address:", 5, 22 + 36);					//目的地址
		GUI_DispStringAt("RF Address Mask:", 5, 22 + 48);									//地址掩码
		GUI_DispStringAt("RF TX Power Level:", 5, 22 + 60);								//发射功率
		GUI_DispStringAt("RF API Enable:", 5, 22 + 72);										//API模式切换
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 272);
		
		//显示背光灯亮度
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("LCD Backlight:", 5, 22);

		//显示船体操作相关参数
		GUI_DispStringAt("Control Mode:", 5, 22 + 22);										//控制模式
		GUI_DispStringAt("USV Series:", 5, 22 + 22 + 12);									//船型系列
		GUI_DispStringAt("Rudder Mid-point:", 5, 22 + 22 + 24);						//舵机中位
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 272 + 100);
		
		//其他参数
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		
		GUI_DispStringAt("Debug Mode:", 5, 8);												//调试模式
		if(debug_Mode == 1)
			GUI_DispStringAt("ON", 72, 8);	
		else
			GUI_DispStringAt("OFF", 72, 8);
		
		GUI_DispStringAt("Version:", 5, 8 + 12);											//版本号
		GUI_DispStringAt(version_number, 50, 8 + 12);									
		
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 272 + 200);

		
		//进入AT命令模式，发送+++      
		enter_AT_Command();	
		
		//进入初始化第二步
		debug_mode_parameter_init = 2;	
		blink_times = ATCOMMAND_DELAY;								
	}
	
	//第二步：等待AT “+++”指令返回的2秒“OK”
	else if(debug_mode_parameter_init == 2)
	{
		if(blink_times > 0)
		{
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1_memdev);	
			GUI_MEMDEV_Clear(bg1_memdev);					
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 250, 100);			
			
			if(blink_times % 2 == 0)								//对2求余，为0则显示，1显示消隐
			{	
				GUI_SetFont(&GUI_Font10_1);
				GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
				GUI_SetColor(GUI_WHITE);
				GUI_SetBkColor(GUI_TRANSPARENT);	
				
				GUI_DispStringAt("Reading RF parameter, please wait...", 5, 22);										//显示等待信息								
			}

			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 150, 10 + 272 + 100);			
			blink_times --;
		}
		
		else
		{
			debug_mode_parameter_init = 3;			//进入读取参数初始化第3步
		}			
	}
	
	//第三步：接收缓冲区及缓冲指针归零后发送查询指令
	else if(debug_mode_parameter_init == 3)	
	{
		//清接收缓冲区
		for(temp_i = 0; temp_i < USART1_RX_BUFFER_SIZE; temp_i++)
		{
			USART1_RX_Buffer[temp_i] = 0;
		}
		
		receive_pointer = 0;				//缓冲指针归零
		
		//发送查询命令
		send_AT_inquiry();	
		
		blink_times = 1;						//等待500毫秒命令返回
		debug_mode_parameter_init = 4;				//等待500毫秒
	}		
	
	//第四步：等待500毫秒缓冲命令返回
	else if(debug_mode_parameter_init == 4)	
	{
		if(blink_times > 0)
		{
			blink_times--;
		}
		else
		{
			debug_mode_parameter_init = 5;			//处理各个变量数值
		}
	}
	
	return;
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


/**
  * @brief  mdelay 独占延时函数
  * @param  timer：独占延时递减变量
  * @retval none
  */
void mdelay(volatile uint32_t timer)
{
	while(timer--);
}
