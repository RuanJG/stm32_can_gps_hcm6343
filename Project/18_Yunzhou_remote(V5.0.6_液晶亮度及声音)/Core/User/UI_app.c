/*-------------------------------------------------------------------------
工程名称：遥控器界面应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "UI_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Graphic_Interface_Task_Handle;

uint8_t interface_index = DEBUG_MAIN_INTERFACE_INDEX;			//界面索引：1为主界面
																													//					2为参数配置界面
																													//					3为采样配置界面
uint8_t modify_index = 0;					//参数修改索引
uint8_t EngineTimerCount = 5;			//禁止连续打火计时器

GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg1a_memdev, bg0a_memdev;								//显示存储变量																	


/* Private variables ---------------------------------------------------------*/
char show_message[460];			//显示接收的消息

uint8_t debug_mode_parameter_init = 1;			//界面步骤索引
uint8_t blink_times = 0;										//闪烁次数
uint8_t blink_t = 0;												//光标闪烁

/**
  * @brief  Graphic user interface task
  * @param  pvParameters not used
  * @retval None
  */
void Graphic_Interface_Task(void * pvParameters)
{   
	uint8_t pointi = 0, tempi = 0;
	uint16_t G_div = 0;						//显示分频因子
	uint16_t pointi2 = 0;
	char tempChar[20];
	uint16_t wait_send = 0;
	uint16_t one_second_div = 0;			//1秒分频计数
	
	/* Initialize the BSP layer */
  LowLevel_Init();
	
	//读出保存的参数
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
		G_div++;												//屏幕显示计时器
		RF_transmit_timer++;						//无线传输计时器
		
		//debug模式下的界面显示
		local_para_conf[4] = 1;
		
		if(local_para_conf[4] == 1)
		{
			//索引号为1为主界面--------------------------------------------------
			if(interface_index == DEBUG_MAIN_INTERFACE_INDEX)
			{
				//100毫秒响应一次显示
				if(G_div >= DEBUG_MAIN_DIVISION)
				{
					G_div = 0;
					debug_mode_main();
//					USART_SendData(USART1, 0x88);
				}
				
				if(MENU_Button == 1)				//在主界面下按下menu键进入参数设置界面
				{
					interface_index = DEBUG_PARAM_INTERFACE_INDEX;
					debug_mode_parameter_init = 1;				//进入参数配置界面时的初始化标志
					LCD_clear();						//清屏幕
				}
				
				else if((SAMPLE_Button == 1) && (MODE_Button != CONTROL_MODE_AUTO))			//在主界面下拨动 SAMPLE 采样 钮子开关进入采样配置界面（具有采水功能的船只）
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
							load_sample_config();					//读取采样参数
							LCD_clear();						//清屏幕							
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
					//停止启动发动机
					StopEngineTrigger();
				}
				
				
				//无线数传命令发送----------------------------
				if(RF_transmit_timer >= (RF_TRANSMIT_TIMES - RF_TRANSMIT_TIMES_TOLER))					
				{
					RF_transmit_timer = 0;
					GenerateCommandBase64(TRUE);					//在主界面下发送数传控制命令
					
//					NeedSendAlertLED = 1;
//					NeedSendSpeaker = 1;
//					GenerateAlarmCommandBase64(TRUE);
				}			
			}
			
			//索引号为2为参数配置界面--------------------------------------------------
			else if(interface_index == DEBUG_PARAM_INTERFACE_INDEX)
			{				
				if(debug_mode_parameter_init == 1)
					debug_mode_parameter();
				
				//500毫秒响应一次显示
				if(G_div >= DEBUG_PARAMETER_DIVISION)
				{
					G_div = 0;
					debug_mode_parameter();
				}
				
				//确认保存-------------------------------------------------
				if(OK_Button == 1)				//在参数配置界面下按下OK键后，保存当前参数并返回主调试界面
				{
					save_parameter();						//保存当前参数于STM32的FLASH中
					save_RF_parameter();				//保存数传模块参数	
					LCD_PWM_Config();								//液晶亮度修改					
					
					if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
					{
						save_Valve_parameter();			//保存阀门参数
					}
									
					vTaskDelay(500);
					
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断
					
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//返回调试主界面	
					LCD_clear();						//清屏幕
				}
				
				
				//取消返回-------------------------------------------------
				else if(CANCEL_Button == 1)				//在参数配置界面下按下CANCEL键后，不保存当前参数并返回主调试界面
				{
					not_save_RF_parameter();				//取消保存
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断
					
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//返回调试主界面	
					LCD_clear();						//清屏幕
				}
				
				//若读取参数完毕后按MENU键，则进入LCD测试模式
				else if((debug_mode_parameter_init == 5) && (MENU_Button == 1))
				{
					interface_index = DEBUG_LCD_TEST_INDEX;
					G_div = 0;
				}			
			}

			//索引号为3为采水配置测试界面-------------------------------------------------
			else if(interface_index == DEBUG_SAMPLE_INTERFACE_INDEX)
			{
				//第一次快速切入界面
				if(debug_mode_parameter_init == 1)
				{	
					debug_mode_parameter_init = 0;
					sample_mode_config();
				}

				//500毫秒响应一次显示
				if(G_div >= DEBUG_PARAMETER_DIVISION)
				{
					G_div = 0;
					sample_mode_config();
				}

				if(OK_Button == 1)						//在参数配置界面下按下OK键后，保存当前参数并返回主调试界面
				{
					save_sample_config();				//保存当前采样参数配置
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;					//返回调试主界面
					NeedSendSampleCount = 10;		//发送10次采水命令
					GlobalVariable.IsManualSampling = TRUE;								//正在采样标志
					LCD_clear();								//清屏幕					
				}		

				else if(CANCEL_Button == 1)				//在参数配置界面下按下CANCEL键后，不保存当前参数并返回主调试界面
				{
					interface_index = DEBUG_MAIN_INTERFACE_INDEX;
					LCD_clear();						//清屏幕
				}				
			}
			
			//索引号为4为液晶测试界面-------------------------------------------------
			else if(interface_index == DEBUG_LCD_TEST_INDEX)
			{													
				G_div = LCD_Test(G_div);				
			}
		}


//		//触摸屏坐标更新
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
		
		//秒分频
		if(one_second_div >= 100)
		{
			one_second_div = 0;
			
			//连续打火时间不能超过5秒---------------------------
			
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
			//连续打火时间不能超过5秒===========================
			
			
			//船数据超时计数
			BoatDataTimeoutCount++;
			
			if(BoatDataTimeoutCount > 2 * BOATDATATIMEOUTVALUE)
			{
				BoatDataTimeoutCount = BOATDATATIMEOUTVALUE;
			}	

			//连接超时警报
			if((BoatDataTimeoutCount > 5) && (interface_index == DEBUG_MAIN_INTERFACE_INDEX))
			{
				if(BoatDataTimeoutCount % 2)
				{
					local_para_conf_buf[5] = 0;				//关闭声音
					Buzzer_PWM_Config();
				}
				else
				{
					local_para_conf_buf[5] = local_para_conf[5];				//开启声音
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
	GUI_SetBkColor(GUI_TRANSPARENT);
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


	//显示前清屏幕-------------------------------------------------
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
	
	//显示电路温度或深度
	switch(local_para_conf[2])
	{
		case USV_Boat_Type_ME70:
		case USV_Boat_Type_ME120:
		case USV_Boat_Type_ME300:
			//显示深度
			GUI_DispStringAt("Water depth:", 3, 8 + 36);
			float_to_string(BoatData.Depth * 100, tempChar, 3, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 36);			
			break;
		
		default:
			//显示电路温度
			GUI_DispStringAt("Internal temperature:", 3, 8 + 36);
			float_to_string(BoatData.InternalTemp*10, tempChar, 3, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 36);		
			break;
	}
	
	//采样状态显示
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
			//是否正在采样
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
  * @brief  debug_mode_parameter 调试模式参数配置界面
  * @param  
  * @retval 
  */
void debug_mode_parameter(void)
{
	char tempChar[20];
	uint16_t temp_i;
	
	//Step1：显示背景参数--------------------------------------------------------------
	if(debug_mode_parameter_init == 1)
	{	
		//显示前清屏幕
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		
		GUI_SetFont(&GUI_Font10_1);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetColor(GUI_WHITE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		//数传模块相关参数----------------------------------------------------------------------
		//显示数传模块相关参数
		GUI_DispStringAt("RF Hopping Channel:", 5, 10);										//通信信道
		GUI_DispStringAt("RF VID:", 5, 10 + 12);													//通信ID号
		GUI_DispStringAt("RF Source Address:", 5, 10 + 24);								//本地地址
		GUI_DispStringAt("RF Destination Address:", 5, 10 + 36);					//目的地址
		GUI_DispStringAt("RF Address Mask:", 5, 10 + 48);									//地址掩码
		GUI_DispStringAt("RF TX Power Level:", 5, 10 + 60);								//发射功率
		GUI_DispStringAt("RF API Enable:", 5, 10 + 72);										//API模式切换
		GUI_DispStringAt("RF Temperature:", 5, 10 + 84);									//数传模块温度								

		//读取数传模块相关参数
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(Xtend_900_para[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 130, 10 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0);		
		//数传模块相关参数======================================================================

		
		//本地存储参数--------------------------------------------------------------------------
		//显示背光灯亮度
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("LCD Backlight:", 5, 8);

		//显示船体操作相关参数
		GUI_DispStringAt("Control Mode:", 5, 8 + 12);								//控制模式
		GUI_DispStringAt("USV Series:", 5, 8 + 24);									//船型系列
		GUI_DispStringAt("Rudder Mid-point:", 5, 8 + 36);						//舵机中位
		
		//其他参数
		GUI_DispStringAt("Debug Mode:", 5, 8 + 48);									//调试模式
		GUI_DispStringAt("Sound Modify:", 5, 8 + 60);								//声音调节
		GUI_DispStringAt("Version:", 5, 8 + 72);										//版本号
		GUI_DispStringAt(version_number, 110, 8 + 72);		
		
		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(local_para_conf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0 + 120);
		//本地存储参数==========================================================================	

		
		//阀门参数------------------------------------------------------------------------------
		if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
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
		//阀门参数==============================================================================	

		
		//进入初始化第二步
		debug_mode_parameter_init = 2;	
		blink_times = ATCOMMAND_DELAY;								
	}
	
	//Step2：等待AT “+++”指令返回的2秒“OK”--------------------------------------------------------------
	else if(debug_mode_parameter_init == 2)
	{
		if(blink_times > 0)
		{
			if(blink_times == 4)
				enter_AT_Command();					//进入AT命令模式，发送+++  
			
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

			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 200, 10 + 0 + 200);			
			blink_times --;
		}
		
		else
		{
			debug_mode_parameter_init = 3;			//进入读取参数初始化第3步
		}			
	}
	
	//Step3：接收缓冲区及缓冲指针归零后发送查询指令--------------------------------------------------------------
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
	
	//Step4：等待500毫秒缓冲命令返回--------------------------------------------------------------
	else if(debug_mode_parameter_init == 4)	
	{
		if(blink_times > 0)
		{
			blink_times--;
		}
		else
		{
			handle_parameter_message();					//处理从数传模块回读的参数
			
			//将数传模块读出的参数赋值至显示缓冲
			for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
			{
				Xtend_900_para_buf[temp_i] = Xtend_900_para[temp_i];
			}
			
			//读出当前遥控其他参数并赋值显示缓冲
			for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
			{
				local_para_conf_buf[temp_i] = local_para_conf[temp_i];
			}			
			
			debug_mode_parameter_init = 5;			//处理各个变量数值
			
			USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);					//在参数配置时禁止中断
		}
	}
	
	//Step5：参数配置修改状态
	else if(debug_mode_parameter_init == 5)	
	{
		//显示前景参数----------------------------------------------------------------------------------
		//数传模块相关参数----------------------------------------------------------------------		
		//显示前清屏幕
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
		GUI_DispStringAt("RF Hopping Channel:", 5, 10);										//通信信道
		GUI_DispStringAt("RF VID:", 5, 10 + 12);													//通信ID号
		GUI_DispStringAt("RF Source Address:", 5, 10 + 24);								//本地地址
		GUI_DispStringAt("RF Destination Address:", 5, 10 + 36);					//目的地址
		GUI_DispStringAt("RF Address Mask:", 5, 10 + 48);									//地址掩码
		GUI_DispStringAt("RF TX Power Level:", 5, 10 + 60);								//发射功率
		GUI_DispStringAt("RF API Enable:", 5, 10 + 72);										//API模式切换
		GUI_DispStringAt("RF Temperature:", 5, 10 + 84);									//数传模块温度		

		//读取数传模块相关参数
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(Xtend_900_para_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 130, 10 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0);	
		//数传模块相关参数======================================================================
		

		//本地存储参数--------------------------------------------------------------------------		
		//显示背光灯亮度
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("LCD Backlight:", 5, 8);

		//显示船体操作相关参数
		GUI_DispStringAt("Control Mode:", 5, 8 + 12);								//控制模式
		GUI_DispStringAt("USV Series:", 5, 8 + 24);									//船型系列
		GUI_DispStringAt("Rudder Mid-point:", 5, 8 + 36);						//舵机中位
		
		//其他参数
		GUI_DispStringAt("Debug Mode:", 5, 8 + 48);									//调试模式
		GUI_DispStringAt("Sound Modify:", 5, 8 + 60);								//声音调节
		GUI_DispStringAt("Version:", 5, 8 + 72);										//版本号
		GUI_DispStringAt(version_number, 110, 8 + 72);		

		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			float_to_string(local_para_conf_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 110, 8 + 12 * temp_i);
		}		
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0 + 120);
		//本地存储参数==========================================================================			

		
		//阀门参数------------------------------------------------------------------------------
		if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
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
		//阀门参数==============================================================================			
		//显示前景参数==================================================================================
		              
		
//		printf("%x, %x, %x, %x, %x, %x, %x, %x\n", Xtend_900_para_buf[0], Xtend_900_para_buf[1], Xtend_900_para_buf[2], Xtend_900_para_buf[3], \
//						Xtend_900_para_buf[4], Xtend_900_para_buf[5], Xtend_900_para_buf[6], Xtend_900_para_buf[7]);
		
		
		//显示当前修改背景块位置------------------------------------------------------------------------------------------------
		//显示前先清除块内容
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);			
		
		if(blink_t == 0)
		{
			blink_t = 1;
			
			//RF相关参数
			if(modify_index < PARAMETER_QUANTITY)				
			{
				GUI_DrawGradientRoundedH(0, 4 + modify_index * 12, 180, 15 + modify_index * 12, 2, GUI_ORANGE, GUI_RED);
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0);
			}
			
			//遥控器其他参数
			else if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY)
			{
				GUI_DrawGradientRoundedH(0, 2 + (modify_index - PARAMETER_QUANTITY) * 12, 180, 13 + (modify_index - PARAMETER_QUANTITY) * 12, 2, GUI_ORANGE, GUI_RED);
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 10, 10 + 0 + 120);			
			}

			//阀门参数
			else if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
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
			
			if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
			{
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 265, 10);	
			}			
		}		
		//显示当前修改背景块位置================================================================================================
	}
		
	return;
}


/**
  * @brief  sample_mode_config 采样参数设置
  * @param  
  * @retval 
  */
void sample_mode_config(void)
{
		uint16_t temp_i = 0;
		char tempChar[20];
	
		//显示前景参数----------------------------------------------------------------------------------
		//显示前清屏幕
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, 250, 100);
		
		GUI_SetFont(&GUI_Font10_1);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetColor(GUI_WHITE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		//显示采样配置相关参数
		GUI_DispStringAt("Bottle Number:", 5, 10);												//采样瓶号
		GUI_DispStringAt("Sample Depth:", 5, 10 + 12);										//采样深度
		GUI_DispStringAt("Sample Mode:", 5, 10 + 24);											//采样模式
		GUI_DispStringAt("Sample_Volume:", 5, 10 + 36);										//采样容量
		GUI_DispStringAt("Wash Mode", 5, 10 + 48);												//清洗模式

		//读取采样配置相关参数
		for(temp_i = 0; temp_i < WATER_SAMPLE_PARAMETER; temp_i++)
		{
			float_to_string(water_sample_conf_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 130, 10 + 12 * temp_i);
		}
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 10, 10 + 0);	
		//显示前景参数==================================================================================


		//显示当前修改背景块位置------------------------------------------------------------------------------------------------
		//显示前先清除块内容
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
		//显示当前修改背景块位置================================================================================================			
		
		return;
}


/**
  * @brief  save_sample_config 采样参数保存
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
  * @brief  load_sample_config 采样参数读取
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
  * @brief  raw_message_handling 直接显示接收到的缓冲数据
  * @param  
  * @retval 
  */
void raw_message_handling(void)
{
	uint16_t message_i = 0;

	//接收数据帧处理---------------------------------------
	if(receive_pointer > 0)			//若接收到字符，则进行帧处理
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
  * @brief  LCD_Test LCD测试
  * @param  timer: 记录当前刷新状态
  * @retval 返回G_div
  */
uint16_t LCD_Test(uint16_t timer)
{
	uint16_t G_div = timer;
	
	//2秒钟显示红屏
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
	
	//2秒显示绿屏
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
	
	//2秒显示蓝屏
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
	
	//2秒显示白屏
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
  * @brief  LCD_clear 清屏幕
  * @param  
  * @retval 
  */
void LCD_clear(void)
{
	//清显示缓冲
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

	//各种单位
	if(unit == 1)
	{
		str[i++] = '%';							//百分数单位	
	}
	else if(unit == 2)
	{
		str[i++] = 'm';							//米单位
	}
//	else if(unit == 3)
//	{
//		str[i++] = 0xe6;							//℃单位
//		str[i++] = 0xa1;	
//	}
	
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
