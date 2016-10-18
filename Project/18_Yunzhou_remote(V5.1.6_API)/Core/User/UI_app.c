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
#include "WindowDLG.c"
#include "songti16.c"
#include "songti12.c"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Graphic_Interface_Task_Handle;

uint8_t interface_index = MAIN_GRAPHIC_INDEX;			//界面索引：1为主界面
																													//					2为参数配置界面
																													//					3为采样配置界面
uint8_t modify_index = 0;					//参数修改索引
uint8_t EngineTimerCount = 5;			//禁止连续打火计时器

GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg0n_memdev, bg1n_memdev;								//显示存储变量		


/* Private variables ---------------------------------------------------------*/
char show_message[460];			//显示接收的消息

uint8_t debug_mode_parameter_init = 1;			//界面步骤索引
uint8_t blink_times = 0;										//闪烁次数
uint8_t blink_t = 0;												//光标闪烁

uint8_t debug_change_flag = 1;							//1表示调试界面和主界面的切换，0表示没切换

static double lat;//=-2.12345;							//纬度缓存
static double lng;//=-10.12345;							//经度缓存

uint16_t show_depth_div = 0;								//深度刷新分频因子，单位为百毫秒
uint16_t waveform_xaxis = 0;								//波形X轴

uint8_t RF_transmit_timer_toler = 0;				//显示发送补偿

uint8_t para_set_flag = 0;									//参数配置标志，0表示没经过参数配置，若收到串口数据，则为透传数据，1表示回读的数传数据

uint16_t key_protect_timer = 0;							//按键防粘连保护计数器

uint16_t wave_last_temp, wave_temp, wave_test_timer = 0;					//波形绘制暂存

uint32_t RSSI_Timer;

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
//  GUI_SelectLayer(0);

//	GUI_BMP_Draw(_acImage_0, 358, 226);				//Yunzhou logo
	
	GUI_SelectLayer(1);	
	bg1_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);
	bg1w_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);
	bg1n_memdev = GUI_MEMDEV_Create(0, 0, BG1N_XAXIS, BG1N_YAXIS);								//小块显存区
	
	GUI_SelectLayer(0);	
	bg0_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	bg0s_memdev = GUI_MEMDEV_Create(0, 0, DEBUG_BG0S_XAXIS, DEBUG_BG0S_YAXIS);
//	bg0n_memdev = GUI_MEMDEV_Create(0, 0, BG0N_XAXIS, BG0N_YAXIS);								//小块显存区
	
	interface_index = MAIN_GRAPHIC_INDEX;
//	interface_index = MAIN_PARAM1_INDEX;
	local_para_conf[4] = 0;							//常规显示模式
//	interface_index = DEBUG_MAIN_INTERFACE_INDEX;	
//	local_para_conf[4] = 1;							//debug模式下的界面显示
	
	
  /* Run the Graphic_Interface task */
  while (1)
  {			
		G_div++;												//屏幕显示计时器
		RF_transmit_timer++;						//无线传输计时器
			
		
		//调试模式显示------------------------------------------------------------------------------------------------------	
		if(local_para_conf[4] == 1)
		{			
			//索引号为1为主界面--------------------------------------------------
			if(interface_index == DEBUG_MAIN_INTERFACE_INDEX)
			{
				//100毫秒响应一次显示
				if(G_div >= DEBUG_MAIN_DIVISION)
				{
					G_div = 0;
					
					//调试模式显示刷新
					debug_mode_main();
//					USART_SendData(USART1, 0x88);
				}
				
				if(MENU_Button == 1)				//在主界面下按下menu键进入参数设置界面
				{
					interface_index = DEBUG_PARAM_INTERFACE_INDEX;
					debug_mode_parameter_init = 1;				//进入参数配置界面时的初始化标志
					LCD_clear();						//清屏幕
				}

				//在主界面下拨动 SAMPLE 采样 钮子开关进入采样配置界面（具有采水功能的船只）
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
					LCD_PWM_Config(ENABLE);								//液晶亮度修改					
					
					if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
					{
						save_Valve_parameter();			//保存阀门参数
					}									

					LCD_clear();						//清屏幕
					
					//调试模式判断---------------------------------------------------------------
					if(local_para_conf[4] == 1)				//保持调试模式
						interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//返回调试主界面	
					
					else
					{
						interface_index = MAIN_GRAPHIC_INDEX;										//返回正常主界面	
						debug_change_flag	= 1;					
					}					
					//调试模式判断===============================================================

					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断
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
		//调试模式显示======================================================================================================	
		
		
		//正常显示模式------------------------------------------------------------------------------------------------------
		else
		{
			//主界面显示-------------------------------------------------------
			if(interface_index == MAIN_GRAPHIC_INDEX)
			{								
				if(MENU_Button == 1)				//在主界面下按下menu键进入参数设置界面
				{
					interface_index = MAIN_PARAM1_INDEX;
					debug_mode_parameter_init = 1;				//进入参数配置界面时的初始化标志
					debug_change_flag	= 1; 								//初始化标志置1
					key_protect_timer = 0;								//粘连保护重新置零
					LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
					LCD_clear();						//清屏幕
				}				
				
				//在主界面下拨动 SAMPLE 采样 钮子开关进入采样配置界面（具有采水功能的船只）
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
							load_sample_config();					//读取采样参数
							LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
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
				
				
				//无线数传命令发送，调整后基本满足 200ms 一个包----------------------------
				if(RF_transmit_timer >= (RF_TRANSMIT_TIMES - RF_TRANSMIT_TIMES_TOLER - RF_transmit_timer_toler))					
				{
					RF_transmit_timer = 0;
					GenerateCommandBase64(TRUE);					//在主界面下发送数传控制命令
				}	
				
				
				//100毫秒响应一次显示
				if(G_div >= MAIN_GRAPHIC_DIVISION)
				{
					G_div = 0;
					main_graphic_show();
					
					RF_transmit_timer_toler = 3;					//为降低显示导致的数传发射延迟，将下次数传发送的时间缩短
				}	
			}
			//主界面显示=======================================================

			
			//进入电台参数配置界面---------------------------------------------
			else if(interface_index == MAIN_PARAM1_INDEX)
			{
				//100毫秒响应一次显示
				if(G_div >= MAIN_PARAM1_DIVISION)
				{
					G_div = 0;
					main_param_show();
				}	

				
				//menu键粘连保护
				key_protect_timer++;
				
				if(key_protect_timer < KEY_PROTECT_TIME)						//在保护时间内不响应对应按键
				{
					MENU_Button = 0;
				}
				else
				{
					key_protect_timer = KEY_PROTECT_TIME;
					
					//确认保存-------------------------------------------------
					if(OK_Button == 1)						//在参数配置界面下按下OK键后，保存当前参数并返回主调试界面
					{
						show_save_wait();						//显示存储警告信息
						save_parameter();						//保存当前参数于STM32的FLASH中
						save_RF_parameter();				//保存数传模块参数	
//						LCD_PWM_Config(ENABLE);								//液晶亮度修改					
											
						
						//调试模式判断---------------------------------------------------------------
						if(local_para_conf[4] == 1)				//保持调试模式
							interface_index = DEBUG_MAIN_INTERFACE_INDEX;						//返回调试主界面	
						
						else
						{
							interface_index = MAIN_GRAPHIC_INDEX;										//返回正常主界面	
							debug_change_flag	= 1;					
						}					
						//调试模式判断===============================================================
						
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();						//清屏幕

						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断
						
						para_set_flag = 1;						//参数配置后标记
					}
					
					
					//取消返回-------------------------------------------------
					else if(CANCEL_Button == 1)				//在参数配置界面下按下CANCEL键后，不保存当前参数并返回主界面
					{
//					not_save_RF_parameter();				//取消保存

						interface_index = MAIN_GRAPHIC_INDEX;						//返回主界面	
						debug_change_flag	= 1;													//界面初始化标记
						para_set_flag = 1;															//参数配置后标记
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();																		//清屏幕
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断
					}

					//菜单选择------------------------------------------------
					else if(MENU_Button == 1)							//在参数界面下按menu键进入参数设置2界面或联系信息界面
					{
						//ME300存在若干控制阀门
						if(local_para_conf[2] == USV_Boat_Type_ME300)						
						{
							interface_index = MAIN_PARAM2_INDEX;
						}
						
						//其他则进入联系信息界面
						else
						{
							interface_index = MAIN_CONTACT_INDEX;
						}
						
						debug_mode_parameter_init = 1;				//进入参数配置界面时的初始化标志
						debug_change_flag	= 1; 								//初始化标志置1
						
						key_protect_timer = 0;
						
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();						//清屏幕
					}							
				}						
			}			
			//进入电台参数配置界面=============================================			
			
			
			//进入ME300阀门配置界面--------------------------------------------
			else if(interface_index == MAIN_PARAM2_INDEX)
			{
				//100毫秒响应一次显示--------------------------------------
				if(G_div >= MAIN_PARAM1_DIVISION)
				{
					G_div = 0;
					main_valve_show();
				}		


				//menu键粘连保护
				key_protect_timer++;

				if(key_protect_timer < KEY_PROTECT_TIME)						//在保护时间内不响应对应按键
				{
					MENU_Button = 0;
				}
				else
				{
					key_protect_timer = KEY_PROTECT_TIME;
					
					//确认保存-------------------------------------------------
					if(OK_Button == 1)						//在参数配置界面下按下OK键后，保存当前参数并返回主调试界面
					{
						if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
						{
							save_Valve_parameter();			//保存阀门参数
						}	

						interface_index = MAIN_GRAPHIC_INDEX;						//返回主界面		
						debug_change_flag	= 1;													//界面初始化标记	
						para_set_flag = 1;															//参数配置后标记
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();																		//清屏幕
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断															
					}
					
					//取消返回-------------------------------------------------
					else if(CANCEL_Button == 1)				//在参数配置界面下按下CANCEL键后，不保存当前参数并返回主界面
					{
						interface_index = MAIN_GRAPHIC_INDEX;						//返回主界面		
						debug_change_flag	= 1;													//界面初始化标记	
						para_set_flag = 1;															//参数配置后标记
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();																		//清屏幕
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断												
					}
		
					//菜单选择------------------------------------------------
					else if(MENU_Button == 1)							//按menu键进入联系方式界面
					{		
						interface_index = MAIN_CONTACT_INDEX;
						
						debug_mode_parameter_init = 1;				//进入参数配置界面时的初始化标志
						debug_change_flag	= 1; 								//初始化标志置1
						
						key_protect_timer = 0;
						
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();						//清屏幕
					}							
				}			
			}			
			//进入ME300阀门配置界面============================================	


			//进入联系方式界面-------------------------------------------------
			else if(interface_index == MAIN_CONTACT_INDEX)	
			{
				//100毫秒响应一次显示------------------------------
				if(G_div >= MAIN_GRAPHIC_DIVISION)
				{
					G_div = 0;
					main_contact_show();
				}
				
				
				//menu键粘连保护
				key_protect_timer++;
				
				if(key_protect_timer < KEY_PROTECT_TIME)						//在保护时间内不响应对应按键
				{
					MENU_Button = 0;
				}
				else
				{
					key_protect_timer = KEY_PROTECT_TIME;
					
					
					//在联系方式界面下按下OK 或 CANCEL后均不保存参数，直接返回主界面
					if((OK_Button == 1) || (CANCEL_Button == 1))
					{
						interface_index = MAIN_GRAPHIC_INDEX;						//返回主界面		
						debug_change_flag	= 1;													//界面初始化标记	
						para_set_flag = 1;															//参数配置后标记
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();																		//清屏幕
						
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断							
					}
					
					//再次按下菜单键-------------------------------------
					else if(MENU_Button == 1) 
					{
						interface_index = MAIN_PARAM1_INDEX;
						debug_mode_parameter_init = 1;				//进入参数配置界面时的初始化标志
						debug_change_flag	= 1; 								//初始化标志置1
						
						key_protect_timer = 0;
						
						LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
						LCD_clear();						//清屏幕				
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//参数配置结束，允许中断							
					}					
				}
			}							
			//进入联系方式界面=================================================
			
			
			//进入采水参数配置界面---------------------------------------------
			else if(interface_index == MAIN_SAMPLE_INDEX)
			{
				//100毫秒响应一次显示
				if(G_div >= MAIN_GRAPHIC_DIVISION)
				{
					G_div = 0;
					main_sample_show();
				}

				if(OK_Button == 1)							//在参数配置界面下按下OK键后，保存当前参数并返回主调试界面
				{
					save_sample_config();																//保存当前采样参数配置
					interface_index = MAIN_GRAPHIC_INDEX;								//返回主界面
					
					NeedSendSampleCount = 10;		//发送10次采水命令
					GlobalVariable.IsManualSampling = TRUE;								//正在采样标志
					SampleInfo.IsBottleTaking[water_sample_conf[0] - 1] = TRUE;						//设置采样标号
					SampleInfo.VolumeTaken[water_sample_conf[0] - 1] = water_sample_conf[3];			//设置采样容量
					
					debug_change_flag	= 1;	
					LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
					LCD_clear();								//清屏幕					
				}		

				else if(CANCEL_Button == 1)				//在参数配置界面下按下CANCEL键后，不保存当前参数并返回主调试界面
				{
					interface_index = MAIN_GRAPHIC_INDEX;
					debug_change_flag	= 1;	
					LCD_PWM_Config(DISABLE);							//刷屏幕前关背光灯
					LCD_clear();						//清屏幕
				}						
			}	
			//进入采水参数配置界面=============================================			
		}		
		//正常显示模式======================================================================================================
		

//		//触摸屏坐标更新
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
			
			//数传连接超时
			if(BoatDataTimeoutCount > LINKTIMEOUT)
			{
				connect_ok = 0;
			}
			
			if(BoatDataTimeoutCount > 2 * BOATDATATIMEOUTVALUE)
			{
				BoatDataTimeoutCount = BOATDATATIMEOUTVALUE;
			}	

			//连接超时警报			
//			if((BoatDataTimeoutCount > 5) && (interface_index == DEBUG_MAIN_INTERFACE_INDEX))
			if((BoatDataTimeoutCount > 5) && (interface_index == MAIN_GRAPHIC_INDEX))
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


/*-------------------------------------------------------------------------
	函数名：main_valve_show
	功  能：阀门配置显示界面
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void main_valve_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;
	
	//执行阀门配置界面初始化
	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
		
		//绘制状态栏-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//绘画 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//绘制标题
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_DispStringAt(_famenshezhi[0], 54, 18);								//阀门设置
		}	
		else
		{
			GUI_DispStringAt("Valve Configuration", 54, 18);					
		}
		
		//绘制过渡窗口
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//绘画另外一侧状态栏
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);		


		//读取当前阀门参数
		load_Valve_parameter();	

		LCD_PWM_Config(ENABLE);							//初始化完毕，打开背光灯
	}
	
	
	//显示采水配置相关参数----------------------------------------
	//显示前清屏幕
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//显示阀门配置相关参数
	if(local_para_conf[6] == 1)						//中文
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_yihaofamen[0], 48, 17);										//1号阀门
		GUI_DispStringAt(":", 111, 17);		
		GUI_DispStringAt(_erhaofamen[0], 48, 42);										//2号阀门
		GUI_DispStringAt(":", 111, 42);		
		GUI_DispStringAt(_sanhaofamen[0], 48, 67);									//3号阀门
		GUI_DispStringAt(":", 111, 67);		
		GUI_DispStringAt(_sihaofamen[0], 48, 92);										//4号阀门
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
	
	//读取阀门配置相关参数	
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
	
	//绘画光标位置
	if(modify_index < 4)
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + modify_index* 25);			
	}
		
	//刷新LCD区域
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);	


	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//显示采样配置相关参数
	if(local_para_conf[6] == 1)						//中文
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_bengshuifamen[0], 48, 17);										//泵水阀门
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
	
	//读取阀门配置相关参数	
	if(ValveStatus[VALVE_QUANTITY - 1])
	{
		GUI_DispStringAt("ON", 128, 17);
	}
	else
	{
		GUI_DispStringAt("OFF", 128, 17);
	}
	
	//绘画光标位置
	if((modify_index >= 4) && (modify_index < 8))
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 4) * 25);			
	}
		
	//刷新LCD区域
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);			
}


/*-------------------------------------------------------------------------
	函数名：main_contact_show
	功  能：联系方式屏幕显示程序
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void main_contact_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;
	
	//执行阀门配置界面初始化
	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
		
		//绘制状态栏-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//绘画 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//绘制标题
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_DispStringAt(_lianxifangshi[0], 54, 18);								//联系方式
		}	
		else
		{
			GUI_DispStringAt("Contact Us", 54, 18);					
		}
		
		//绘制过渡窗口
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//绘画另外一侧状态栏
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);

		
		//显示地图信息
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		GUI_BMP_Draw(_acImage_20, 0, 0);											//公司地址地图

		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS + 20, 60);
		
		
		//显示二维码及公司logo
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
		GUI_SetBkColor(GUI_TRANSPARENT);

		GUI_BMP_Draw(_acImage_21, 10, 20);											//主页二维码
		GUI_BMP_Draw(_acImage_22, 118, 20);											//公司logo

		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 146);
		
		LCD_PWM_Config(ENABLE);							//初始化完毕，打开背光灯
	}
	
	
	//显示联系方式----------------------------------------
	//显示前清屏幕
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	
	//显示联系信息
	if(local_para_conf[6] == 1)						//中文
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt(_lianxidizhi[0], 8, 17);												//联系地址
		GUI_DispStringAt(":", 73, 17);
		
		GUI_SetColor(GUI_RED);		
		GUI_DispStringAt(_zhongguoguangdongshengzhuhaishi[0], 36, 42);										//中国广东省珠海市高新区
		GUI_DispStringAt(_nanfangruanjianyuan[0], 8, 63);																	//南方软件园D2-214
		
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt(_youzhengbianma[0], 8, 92);										//邮政编码
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
			
	//刷新LCD区域
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);

	
	//下一部分清屏
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	GUI_SetBkColor(GUI_TRANSPARENT);

	//显示联系信息
	if(local_para_conf[6] == 1)						//中文
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_SetColor(GUI_BLUE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_dianhuahaoma[0], 8, 17);											//电话号码
		GUI_DispStringAt(":", 73, 17);			
		GUI_SetColor(GUI_RED);	
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("+86(756)3626636", 86, 17);
		
		GUI_SetColor(GUI_BLUE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_chuanzhenhaoma[0], 8, 42);										//传真号码
		GUI_DispStringAt(":", 73, 42);			
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("+86(756)3626619", 86, 42);		
		
		GUI_SetColor(GUI_BLUE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_dianziyoujian[0], 8, 67);											//电子邮件
		GUI_DispStringAt(":", 73, 67);			
		GUI_SetColor(GUI_RED);	
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt("info@yunzhou-tech.com", 86, 67);	

		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetColor(GUI_BLUE);
		GUI_DispStringAt(_guanfangwangzhi[0], 8, 92);										//官方网址
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
			
	//刷新LCD区域
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);	
}


/*-------------------------------------------------------------------------
	函数名：main_sample_show
	功  能：采水设置屏幕显示程序
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void main_sample_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;
	
	//执行采水界面初始化
	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
		
		//绘制状态栏-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//绘画 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//绘制标题
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_DispStringAt(_caiyangshezhi[0], 54, 18);								//采样设置
		}	
		else
		{
			GUI_DispStringAt("Sampling Configuration", 50, 18);					
		}
		
		//绘制过渡窗口
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//绘画另外一侧状态栏
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);		


		//设置最大采样瓶号
		if(local_para_conf[2] <= 6)							//小型船最多4个瓶
		{
			water_sample_conf_max[0] = 4;
		}
		else if(local_para_conf[2] <= 11)				//中型船最多8个瓶
		{
			water_sample_conf_max[0] = 8;
		}		
		else 																		//大型船最多12个瓶
		{
			water_sample_conf_max[0] = 12;
		}			

		LCD_PWM_Config(ENABLE);							//初始化完毕，打开背光灯
	}
	
	//显示采水配置相关参数----------------------------------------
	//显示前清屏幕
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//显示采样配置相关参数
	if(local_para_conf[6] == 1)						//中文
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_caiyangpinghao[0], 48, 17);										//采样瓶号
		GUI_DispStringAt(":", 111, 17);		
		GUI_DispStringAt(_caiyangshendu[0], 48, 42);										//采样深度
		GUI_DispStringAt(":", 111, 42);		
		GUI_DispStringAt(_caiyangmoshi[0], 48, 67);											//采样模式
		GUI_DispStringAt(":", 111, 67);		
		GUI_DispStringAt(_caiyangrongliang[0], 48, 92);									//采样容量
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
	
	//读取采样配置相关参数
	for(temp_i = 0; temp_i < 4; temp_i++)
	{
		float_to_string(water_sample_conf_buf[temp_i], tempChar, 0, 0, 0, 0);
		GUI_DispStringAt(tempChar, 140, 17 + 25 * temp_i);
	}	
	
	//绘画光标位置
	if(modify_index < 4)
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + modify_index* 25);			
	}
		
	//刷新LCD区域
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);	


	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_TRANSPARENT);
	
	//显示采样配置相关参数
	if(local_para_conf[6] == 1)						//中文
	{
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Fontsongti16);
		
		GUI_DispStringAt(_qingximoshi[0], 48, 17);										//清洗模式
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
	
	//读取采样配置相关参数
	float_to_string(water_sample_conf_buf[4], tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 140, 17);
	
	//绘画光标位置
	if((modify_index >= 4) && (modify_index < 8))
	{
		GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 4) * 25);			
	}
		
	//刷新LCD区域
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);		
}
	


/*-------------------------------------------------------------------------
	函数名：main_param_show
	功  能：参数设置屏幕显示程序
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void main_param_show(void)
{
	char tempChar[30];	
	uint16_t temp_i = 0;

	//Step1: 调试与主界面切换第一次初始化  -----------------------------------------------------------------------
	if(debug_mode_parameter_init == 1)
	{
//		debug_change_flag = 0;

//		local_para_conf[6] = 0;
		
		LCD_clear();
		
		//绘制状态栏-----------------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_DrawGradientV (0, 0, DEBUG_BG0_XAXIS, 3, GUI_BLACK, GUI_WHITE);		
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		//绘画 HOME LOGO
		GUI_BMP_Draw(_acImage_0, 12, 3);				//Yunzhou logo
		
		//绘制标题
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();		
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_DispStringAt(_canshushezhi[0], 54, 18);								//参数设置
		}	
		else
		{
			GUI_DispStringAt("USV Configuration", 54, 18);					
		}
		
		//绘制过渡窗口
		GUI_DrawGradientV (0, 33, DEBUG_BG0_XAXIS, 36, GUI_WHITE, GUI_BLACK);
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
		
		//绘画另外一侧状态栏
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 3, DEBUG_BG0_XAXIS, 33);
		
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringAt(version_number, 180 - 60, 18);						//显示版本号
		GUI_DispStringAt(release_date, 180 - 8, 18);									//显示发布日期
				
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, DEBUG_BG0_XAXIS, 0);				
	
	
		//显示数传模块相关参数--------------------------------------
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tongxinpindao[0], 48, 17);								//通信频道
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_tongxinbianhao[0], 48, 42);								//通信编号
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_bendidizhi[0], 48, 67);										//本地地址
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_mudidizhi[0], 48, 92);										//目的地址
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
		
		//读取数传模块相关参数
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		
		for(temp_i = 0; temp_i < 4; temp_i++)
		{
			float_to_string(Xtend_900_para[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 128, 17 + 25 * temp_i);
		}		

		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);

		
		//下半部分存储设备内容
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_dizhiyanma[0], 48, 17);										//地址掩码
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_fashegonglv[0], 48, 42);									//发射功率
			GUI_DispStringAt(":", 111, 42);		
			GUI_DispStringAt(_diantaimoshi[0], 48, 67);									//电台模式
			GUI_DispStringAt(":", 111, 67);		
			GUI_DispStringAt(_diantaiwendu[0], 48, 92);									//电台温度
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
		
		//读取数传模块相关参数		
		float_to_string(Xtend_900_para[4], tempChar, 0, 0, 0, 0);				//地址掩码
		GUI_DispStringAt(tempChar, 128, 17);
		GUI_DispStringAt(get_TX_Power(Xtend_900_para[5]), 128, 42);			//电台发射功率
		float_to_string(Xtend_900_para[6], tempChar, 0, 0, 0, 0);				//API模式
		GUI_DispStringAt(tempChar, 128, 67);	
		
		GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
		float_to_string(Xtend_900_para[7], tempChar, 0, 0, 0, 0);				//电台温度
		GUI_DispStringAt(tempChar, 156, 92);
		
		GUI_SetFont(&GUI_Fontsongti12);			
		GUI_DispStringAt(_sheshidu[0], 159, 94);													//摄氏度 ℃
			
		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);	


		//配置遥控及USV参数------------------------------------------
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_pingmuliangdu[0], 48, 17);								//屏幕亮度
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_tuijinmoshi[0], 48, 42);									//推进模式
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_chuanxingxuanze[0], 48, 67);							//船型选择
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_duojizhongwei[0], 48, 92);								//舵机中位
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

		//读取遥控及船型参数
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);	
		
		float_to_string(local_para_conf[0], tempChar, 0, 0, 0, 0);						//屏幕亮度 			
		GUI_DispStringAt(tempChar, 128, 17);
		float_to_string(local_para_conf[1] + 1, tempChar, 0, 0, 0, 0);				//推进模式 			
		GUI_DispStringAt(tempChar, 128, 42);								
		GUI_DispStringAt(GetBoatTypeText(local_para_conf[2]), 128, 67);				//船型选择
		float_to_string(local_para_conf[3], tempChar, 0, 0, 0, 0);						//舵机中位			
		GUI_DispStringAt(tempChar, 128, 92);		
	
		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 46);
		
		
		//下半部参数
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tiaoshimoshi[0], 48, 17);										//调试模式
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_shengyinshezhi[0], 48, 42);									//声音设置
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_yuyanshezhi[0], 48, 67);										//语言设置
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_tongxinxieyi[0], 48, 92);										//通信协议
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
		
		GUI_DispStringAt(Get_On_Off(local_para_conf[4]), 128, 17);				//调试模式
		GUI_DispStringAt(Get_On_Off(local_para_conf[5]), 128, 42);				//声音设置

		if(local_para_conf[6] == 1)												//中文
		{
			GUI_SetFont(&GUI_Fontsongti16);
			GUI_DispStringAt(_zhongwen[0], 128, 67);				//中文
		}			
		else
		{
			GUI_SetFont(&GUI_Font16_1);
			GUI_DispStringAt("English", 128, 67);
		}
		
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("V2", 128, 92);									//通信协议
				
		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 146);			
		
		//进入初始化第二步
		debug_mode_parameter_init = 2;
		blink_times = ATCOMMAND_DELAY / 2;

		
		//绘制读取参数提示
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);	
		GUI_SetColor(GUI_TRANSPARENT);		
		GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	

		GUI_BMP_Draw(_acImage_14, 80, 0);											//感叹号图标

		GUI_SetColor(GUI_YELLOW);
		GUI_SetBkColor(GUI_TRANSPARENT);	
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
			GUI_SetFont(&GUI_Fontsongti16);	
			GUI_DispStringAt(_canshuduquzhong[0], 100, 60);				//参数读取中
			GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
			GUI_DispStringAt(_qingshaohou[0], 100, 85);						//请稍后
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
		
		
		LCD_PWM_Config(ENABLE);							//初始化完毕，打开背光灯
		
		//进入AT指令的间隔
		vTaskDelay(1200);
	}
	//Step1: 调试与主界面切换第一次初始化  =======================================================================	
	
	
	//Step2: 等待AT "+++"	指令返回的2秒	"OK"  --------------------------------------------------------------------
	else if(debug_mode_parameter_init == 2)
	{
		while(blink_times > 0)
		{
			if(blink_times == ATCOMMAND_DELAY / 2)
				enter_AT_Command();					//进入AT命令模式，发送 +++
			
//			//闪烁提示符号
//			GUI_SelectLayer(1);
//			GUI_MEMDEV_Select(bg1_memdev);	
//			GUI_MEMDEV_Clear(bg1_memdev);	
//			GUI_SetColor(GUI_TRANSPARENT);		
//			GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
//			
//			//闪烁效果
//			if(blink_times % 2 == 0)								//对2求余
//			{	
//				GUI_BMP_Draw(_acImage_14, 80, 0);											//感叹号图标
//		
//				GUI_SetColor(GUI_YELLOW);
//				GUI_SetBkColor(GUI_TRANSPARENT);	
//				
//				if(local_para_conf[6] == 1)						//中文
//				{
//					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
//					GUI_SetFont(&GUI_Fontsongti16);	
//					GUI_DispStringAt(_canshuduquzhong[0], 100, 60);				//参数读取中
//					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
//					GUI_DispStringAt(_qingshaohou[0], 100, 85);						//请稍后
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

			//保持750毫秒
			vTaskDelay(750);
			
			blink_times --;
		}
		
		debug_mode_parameter_init = 3;			//进入读取参数初始化第3步
	}
	//Step2: 等待AT "+++"	指令返回的2秒	"OK"  ====================================================================
	

	//Step3: 接收缓冲区及缓冲指针归零后发送查询指令  -------------------------------------------------------------
	else if(debug_mode_parameter_init == 3)
	{
		//清空接收缓冲区
		for(temp_i = 0; temp_i < USART1_RX_BUFFER_SIZE; temp_i++)
		{
			USART1_RX_Buffer[temp_i] = 0;
		}
		
		receive_pointer = 0;								//缓冲指针归零		
		
		//发送查询命令
		send_AT_inquiry();			
		
		//等待500毫秒命令数据返回
		vTaskDelay(500);
		
		//清除稍候图标
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);	
		GUI_SetColor(GUI_TRANSPARENT);		
		GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
		GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 130, 48);
		
		debug_mode_parameter_init = 4;			//进入读取参数初始化第4步
	}	
	//Step3: 接收缓冲区及缓冲指针归零后发送查询指令  =============================================================

	
	//Step4: 处理数传模块返回的数据  -----------------------------------------------------------------------------
	else if(debug_mode_parameter_init == 4)
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
		
		not_save_RF_parameter();				//数传模块退出命令模式
		
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);					//在参数配置时禁止中断		
	}
	//Step4: 处理数传模块返回的数据  =============================================================================
	

	//Step5: 参数配置修改状态  -----------------------------------------------------------------------------------
	else if(debug_mode_parameter_init == 5)		
	{
		//显示数传模块相关参数--------------------------------------
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tongxinpindao[0], 48, 17);								//通信频道
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_tongxinbianhao[0], 48, 42);								//通信编号
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_bendidizhi[0], 48, 67);										//本地地址
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_mudidizhi[0], 48, 92);										//目的地址
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
		
		//读取数传模块相关参数
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);
		
		for(temp_i = 0; temp_i < 4; temp_i++)
		{
			float_to_string(Xtend_900_para_buf[temp_i], tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 128, 17 + 25 * temp_i);
		}		

		//绘画光标位置
		if(modify_index < 4)
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + modify_index * 25);			
		}		
		
		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 46);

		
		//下半部分存储设备内容
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_dizhiyanma[0], 48, 17);										//地址掩码
			GUI_DispStringAt(":", 111, 17);		
			GUI_DispStringAt(_fashegonglv[0], 48, 42);									//发射功率
			GUI_DispStringAt(":", 111, 42);		
			GUI_DispStringAt(_diantaimoshi[0], 48, 67);									//电台模式
			GUI_DispStringAt(":", 111, 67);		
			GUI_DispStringAt(_diantaiwendu[0], 48, 92);									//电台温度
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
		
		//读取数传模块相关参数		
		float_to_string(Xtend_900_para_buf[4], tempChar, 0, 0, 0, 0);				//地址掩码
		GUI_DispStringAt(tempChar, 128, 17);
		GUI_DispStringAt(get_TX_Power(Xtend_900_para_buf[5]), 128, 42);			//电台发射功率
		float_to_string(Xtend_900_para_buf[6], tempChar, 0, 0, 0, 0);				//API模式
		GUI_DispStringAt(tempChar, 128, 67);	
		
		GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
		float_to_string(Xtend_900_para_buf[7], tempChar, 0, 0, 0, 0);				//电台温度
		GUI_DispStringAt(tempChar, 156, 92);
		
		GUI_SetFont(&GUI_Fontsongti12);			
		GUI_DispStringAt(_sheshidu[0], 159, 94);													//摄氏度 ℃

		//绘画光标位置
		if((modify_index >= 4) && (modify_index < 8))
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 4) * 25);			
		}
		
		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 146);	


		//配置遥控及USV参数------------------------------------------
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_pingmuliangdu[0], 48, 17);								//屏幕亮度
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_tuijinmoshi[0], 48, 42);									//推进模式
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_chuanxingxuanze[0], 48, 67);							//船型选择
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_duojizhongwei[0], 48, 92);								//舵机中位
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

		//读取遥控及船型参数
		GUI_SetColor(GUI_RED);
		GUI_SetFont(&GUI_Font16_1);	
		
		float_to_string(local_para_conf_buf[0], tempChar, 0, 0, 0, 0);						//屏幕亮度 			
		GUI_DispStringAt(tempChar, 128, 17);
		float_to_string(local_para_conf_buf[1] + 1, tempChar, 0, 0, 0, 0);				//推进模式 			
		GUI_DispStringAt(tempChar, 128, 42);								
		GUI_DispStringAt(GetBoatTypeText(local_para_conf_buf[2]), 128, 67);				//船型选择
		float_to_string(local_para_conf_buf[3], tempChar, 0, 0, 0, 0);						//舵机中位			
		GUI_DispStringAt(tempChar, 128, 92);		

		//绘画光标位置
		if((modify_index >= 8) && (modify_index < 12))
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 8) * 25);			
		}
		
		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 46);
		
		
		//下半部参数
		GUI_MEMDEV_Clear(bg0_memdev);					
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_TRANSPARENT);
		
		if(local_para_conf[6] == 1)						//中文
		{
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
			GUI_SetFont(&GUI_Fontsongti16);
			
			GUI_DispStringAt(_tiaoshimoshi[0], 48, 17);										//调试模式
			GUI_DispStringAt(":", 111, 17);	
			GUI_DispStringAt(_shengyinshezhi[0], 48, 42);									//声音设置
			GUI_DispStringAt(":", 111, 42);	
			GUI_DispStringAt(_yuyanshezhi[0], 48, 67);										//语言设置
			GUI_DispStringAt(":", 111, 67);	
			GUI_DispStringAt(_tongxinxieyi[0], 48, 92);										//通信协议
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
		
		GUI_DispStringAt(Get_On_Off(local_para_conf_buf[4]), 128, 17);				//调试模式
		GUI_DispStringAt(Get_On_Off(local_para_conf_buf[5]), 128, 42);				//声音设置

		if(local_para_conf_buf[6] == 1)												//中文
		{
			GUI_SetFont(&GUI_Fontsongti16);
			GUI_DispStringAt(_zhongwen[0], 128, 67);				//中文
		}			
		else
		{
			GUI_SetFont(&GUI_Font16_1);
			GUI_DispStringAt("English", 128, 67);
		}
						
		GUI_SetFont(&GUI_Font16_1);	
		GUI_DispStringAt("V2", 128, 92);							//通信协议
		
		//绘画光标位置
		if(modify_index >= 12)
		{
			GUI_BMP_Draw(_acImage_15, 8, 7 + (modify_index - 12) * 25);			
		}
				
		//刷新LCD区域
		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 146);			
	}
	//Step5: 参数配置修改状态  ===================================================================================		
}


/*-------------------------------------------------------------------------
	函数名：main_graphic_show
	功  能：主界面屏幕显示程序
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void main_graphic_show(void)
{
	char tempChar[30];	
	uint16_t temp_num = 0, last_temp_num = 0;
	int16_t rdbm = 0;

	
	//调试与主界面切换第一次初始化
	if(debug_change_flag == 1)
	{
		debug_change_flag = 0;	
		waveform_xaxis = 0;
		
		//绘制状态栏-----------------------------------------------
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
		
		//船型号文字
		GUI_SetFont(&GUI_Font10_1);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);	
		GUI_DispStringAt(GetBoatTypeText(local_para_conf[2]), 65, 18);	
//		GUI_SetColor(GUI_DARKRED);
		GUI_DrawRoundedRect(39, 7, 90, 28, 2);
		GUI_DrawRoundedRect(36, 4, 93, 31, 3);
		
		GUI_BMP_Draw(_acImage_0, 3, 3);				//Yunzhou logo
		
		//船型图片显示
		if(local_para_conf[2] <= 2)				//显示20船图标
		{
			GUI_BMP_Draw(_acImage_10, 97, 3);		
		}
		else if(local_para_conf[2] <= 6)				//显示30船图标
		{
			GUI_BMP_Draw(_acImage_1, 97, 3);		
		}
		else if(local_para_conf[2] <= 11)				//显示70船图标
		{
			GUI_BMP_Draw(_acImage_2, 97, 3);		
		}
		else if(local_para_conf[2] <= 15)				//显示120船图标
		{
			GUI_BMP_Draw(_acImage_3, 97, 3);		
		}
		else if(local_para_conf[2] <= 18)				//显示300船图标
		{
			GUI_BMP_Draw(_acImage_4, 95, 3);		
		}		

		GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 176, 0);

		
		//显示控制模式
		GUI_SelectLayer(0);
		GUI_MEMDEV_Select(bg0_memdev);	
		GUI_MEMDEV_Clear(bg0_memdev);	
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();
		GUI_SetColor(GUI_DARKBLUE);
		GUI_SetBkColor(GUI_WHITE);
		
		if(local_para_conf[6] == 1)		//中文
		{
			if(local_para_conf[1] == 0)				//单推
				GUI_DispStringAt(_dantui[0], 0, 8);

			else
				GUI_DispStringAt(_shuangtui[0], 0, 8);	//双推
			
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 15, 10);
		}
		else
		{
			if(local_para_conf[1] == 0)				//单推
				GUI_DispStringAt("Single", 0, 8);

			else
				GUI_DispStringAt("Double", 0, 8);				//双推

			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 2, 10);			
		}	

		
		//绘制右下角动态显示区
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
				//绘制深度显示背景
				GUI_SelectLayer(0);
				GUI_MEMDEV_Select(bg0_memdev);	
				GUI_MEMDEV_Clear(bg0_memdev);
				GUI_SetColor(GUI_TRANSPARENT);
				GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);
				
				GUI_BMP_Draw(_acImage_13, 30, 0);													//海洋背景
							
				
				//绘画深度刻度
				GUI_SetColor(GUI_BLUE);
				GUI_SetBkColor(GUI_TRANSPARENT);
				GUI_SetFont(&GUI_Font8_1);
				
				for(temp_num = 0; temp_num < 8; temp_num++)
				{
					GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
					float_to_string(temp_num * 10, tempChar, 1, 0, 0, 0);
					GUI_DispStringAt(tempChar, 28, 18 + temp_num * 10);
					
					GUI_SetLineStyle(GUI_LS_DOT);																				//设置点虚线
					GUI_DrawLine(30, 18 + temp_num * 10, 218, 18 + temp_num * 10);					
				}
								
				GUI_SetLineStyle(GUI_LS_SOLID);					//切换回实线
								
				GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 160);			
				break;
			}
				
			default:
			{	
				break;
			}
		}

		LCD_PWM_Config(ENABLE);							//初始化完毕，打开背光灯
	}

	
	//显示状态控件-------------------------------------------------------------------------
	//显示前清屏幕
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1w_memdev);	
	GUI_MEMDEV_Clear(bg1w_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);	
	
	//摇杆控制模式选择显示-----------------------
	if(MODE_Button == CONTROL_MODE_MANUAL)			//单摇杆模式
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
		
		if(local_para_conf[6] == 1)		//中文
			GUI_DispStringAt(_zidong[0], 3, 15);	
		
		else													//英文
			GUI_DispStringAt("Auto", 3, 15);
	}	
	
//	//数传连接状态显示------------------------
//	if((BoatDataTimeoutCount <= 5) && (connect_ok == 1))						
//	{
//		GUI_BMP_Draw(_acImage_6, 43, 3);				//连接成功显示
//	}
//	else
//	{
//		GUI_BMP_Draw(_acImage_7, 43, 3);				//连接失败显示
//	}
	
	//警报灯状态-------------------------------
	if(ALARM_Button == 1)
	{
		GUI_BMP_Draw(_acImage_9, 85, 0);				//警报灯图标
	}
	else
	{
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(85, 0, 29 + 85, 30);	
	}

	
	//叠加信道指示--------------------------------
	GUI_SetFont(&GUI_Font10_1);	
	GUI_SetColor(GUI_RED);	
	GUI_SetBkColor(GUI_TRANSPARENT);	
	float_to_string(Xtend_900_para[0], tempChar, 0, 0, 0, 0);
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(tempChar, 45, 22);	
	
	
	//信号强度判定
//	if((RSSI_Interrupt_flag == 0) && (para_set_flag == 0) && (RSSI_Timer > 3))							//接收到数据但没有信号强度中断，表示信号满格，为-40dbm				
//	{
//		rdbm = -40;
//		RSSI_Timer = 0;
//	}
//	else if((RSSI_Interrupt_flag == 0) && (para_set_flag == 1))					//表示接收到数传模块命令模式下的回读数据
//	{
//		rdbm = -113;
//	}
//	else																																//表示接收到数传透传数据，信号不满格
//	{
		rdbm = DutyCycle * 2 / 3 - 113;	
//	}
		
	float_to_string(rdbm, tempChar, 0, 0, 0, 0);		
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
//	GUI_DispStringAt("The RSSI is:", 30, 91);
	GUI_DispStringAt(tempChar, 76, 22);
	
	GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 56, 3);
	
	
	//数传连接状态显示------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0s_memdev);	
	GUI_MEMDEV_Clear(bg0s_memdev);
	
	if((BoatDataTimeoutCount <= 5) && (connect_ok == 1))		
	{		
		//根据RSSI显示信号强度		大于-60dbm表示信号好，-80 ~ -60dbm表示信号一般， -80以下表示信号差
		if(rdbm >= -65)
		{
			GUI_BMP_Draw(_acImage_16, 3, 0);				//信号好
		}
		else if((rdbm >= -85) && (rdbm < -65))
		{
			GUI_BMP_Draw(_acImage_17, 3, 0);				//信号一般		
		}
		else
		{
			GUI_BMP_Draw(_acImage_18, 3, 0);				//信号差			
		}
	}
	else
	{
		GUI_BMP_Draw(_acImage_19, 3, 0);				//连接失败显示
	}
	GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 96, 3);
	
	
	//采水中显示---------------------------------	
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0s_memdev);	
	GUI_MEMDEV_Clear(bg0s_memdev);	

	if(GlobalVariable.IsManualSampling == TRUE)
	{
		GUI_BMP_Draw(_acImage_12, 0, 0);				//显示采水图标
	}
	else
	{
		GUI_SetColor(GUI_WHITE);
		GUI_FillRect(0, 0, 18, 30);			
	}
	GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 480 - 128, 3);
	
	
	//遥控器电池电量显示---------------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0s_memdev);	
	GUI_MEMDEV_Clear(bg0s_memdev);	
	GUI_BMP_Draw(_acImage_8, 0, 1);				//电池背景图标

	//电池电量显示映射范围为5 ~ 32
	if(Battery_Power_Percent <= 20)				//少于20%电量
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * Battery_Power_Percent / 100, 15, GUI_BLACK, GUI_RED);
		GUI_DrawGradientV(5, 16, 5 + 27 * Battery_Power_Percent / 100, 26, GUI_RED, GUI_BLACK);
	}	
	else if(Battery_Power_Percent <= 40)				//少于40%电量
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
	
	
	//USV电量映射----------------------------------------------------
	GUI_MEMDEV_Clear(bg0s_memdev);
	GUI_BMP_Draw(_acImage_8, 0, 1);				//电池背景图标
	
	if(BoatData.BatLife >= 100)
	{
		BoatData.BatLife = 100;
	}
	
	if(BoatData.BatLife <= 20)				//少于20%电量
	{
		GUI_DrawGradientV(5, 3, 5 + 27 * BoatData.BatLife / 100, 15, GUI_BLACK, GUI_RED);
		GUI_DrawGradientV(5, 16, 5 + 27 * BoatData.BatLife / 100, 26, GUI_RED, GUI_BLACK);
	}	
	else if(BoatData.BatLife <= 40)				//少于40%电量
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
	
	
	//显示遥控电量文字内容
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
	
	
	//显示USV电量文字内容
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
	//显示状态控件==============================================================================
	
	
	//显示操作反馈区----------------------------------------------------------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	
	//画板框
	GUI_SetColor(GUI_WHITE);
	GUI_DrawRoundedRect(8, 16, 250 - 8, 100 - 5, 3);
	
	//自动模式显示--------------------------------------------------------------------
	if(MODE_Button == CONTROL_MODE_AUTO)
	{
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_SetColor(GUI_BLUE);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_UC_SetEncodeUTF8();
		GUI_SetBkColor(GUI_BLACK);
		
//		local_para_conf[6] = 0;
		
		//画板框标题
		if(local_para_conf[6] == 1)
		{
			GUI_DispStringAt(_zhinengrenwu[0], 125, 16);									//智能任务
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
			case 0:																											//未设置导航任务
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
			case 65535:	 																								//导航任务已结束
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
			case 65534:										//采样中
			{
				GlobalVariable.IsManualSampling = TRUE;
				
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_zhengzaicaiyangzhi[0], 85, 53);							//正在采样至
					float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//瓶号
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 138, 53);	
					GUI_DispStringAt(_haoping[0], 150, 53);												//号瓶
				}
				else 
				{
					GUI_DispStringAt("Sampling bottle", 100, 53);								
					float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//瓶号
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 180, 53);						
				}
				break;
			}
////			case 65533://在线监测中
////			{
////				LCD_ClearArea(30, 79, 82, 120);
////				LCD_WR_L_HanZi_String(30, 79 , zheng zai jian ce);	//
////				//LCD_WR_L_HanZi_String(30, 120 , zhi ); 	//
////				break;
////			}
			case 65532:																								//导航已被取消
			{
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_daohangquxiao[0], 125, 53);							//导航取消
				}	
				else 
				{
					GUI_DispStringAt("Mission cancelled", 125, 53);							
				}
				
				break;
			}
			case 65531:																								//导航已被暂停
			{
				if(local_para_conf[6] == 1)
				{
					GUI_DispStringAt(_daohangzanting[0], 125, 53);							//导航暂停				
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
						GUI_DispStringAt(_zhengzaidaohangzhi[0], 85, 53);							//正在导航至							
						float_to_string(BoatData.Status, tempChar, 0, 0, 0, 0);				//测量点
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 140, 53);	
						GUI_DispStringAt(_haodian[0], 156, 53);												//号点					
					}
					else if (BoatData.Status >= 32768 && BoatData.Status <= 32768 + 255)
					{
						GUI_DispStringAt(_zhengzaijiancezhi[0], 85, 53);							//正在监测至							
						float_to_string(BoatData.Status - 32768, tempChar, 0, 0, 0, 0);				//测量点
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 140, 53);	
						GUI_DispStringAt(_haodian[0], 156, 53);												//号点							
					}
					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //采样监测
					{
						GUI_DispStringAt(_caiyangjiance[0], 85, 46);							//采样监测						
						float_to_string(BoatData.Status - 49152, tempChar, 0, 0, 0, 0);				//测量点
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 132, 46);	
						GUI_DispStringAt(_haodian[0], 148, 46);												//号点		

						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_caiyangzhi[0], 85, 70);											//采样至		
						float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//采样瓶
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 118, 70);	
						GUI_DispStringAt(_haoping[0], 128, 70);												//号瓶								
					}
				}
				else
				{
					if (BoatData.Status >= 1 && BoatData.Status <= 255)
					{
						GUI_DispStringAt("Moving to", 100, 53);												//正在导航
						float_to_string(BoatData.Status, tempChar, 0, 0, 0, 0);				//测量点
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 150, 53);								
					}
					else if (BoatData.Status >= 32768 && BoatData.Status <= 32768 + 255)
					{
						GUI_DispStringAt("Monitoring", 100, 53);												//正在监测
						float_to_string(BoatData.Status - 32768, tempChar, 0, 0, 0, 0);				//测量点
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 152, 53);
					}
					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //采样监测
					{
						GUI_DispStringAt("Sampling monitoring", 110, 46);												//正在监测
						float_to_string(BoatData.Status - 49152, tempChar, 0, 0, 0, 0);				//测量点
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 203, 46);						
						
						GUI_DispStringAt("Sampling", 68, 70);																//采样至
						float_to_string(BoatData.Bottle, tempChar, 0, 0, 0, 0);				//测量点
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 148, 70);								
					}
				}
				break;
			}
		}
	}
	//自动模式显示====================================================================

	
	//单摇杆模式显示------------------------------------------------------------------
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
		
		//画板框标题
		if(local_para_conf[6] == 1)
		{
			GUI_DispStringAt(_youshoukongzhi[0], 125, 16);									//右手控制
		}
		else
		{
			GUI_DispStringAt("Right Hand Control", 125, 16);
		}

		
		GUI_SetColor(GUI_RED);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		
		switch (local_para_conf[2])
		{
			case USV_Boat_Type_LE2000:																				//LE2000操作反馈
			case USV_Boat_Type_LE3000:																				//LE3000操作反馈
			{
				break;
			}
				
			default:
			{
				if(local_para_conf[6] == 1)																		//中文显示
				{
					if(local_para_conf[2] == USV_Boat_Type_MC120)
					{
						//显示水泵反馈读数
						GUI_DispStringAt(_shuibeng[0], 36, 46);												//水泵							
					}
					else if(local_para_conf[2] == USV_Boat_Type_ME300)
					{
						//显示油门反馈读数
						GUI_DispStringAt(_youmen[0], 36, 46);												//油门
					}
					else
					{
						//显示档位反馈读数
						GUI_DispStringAt(_dangwei[0], 36, 46);												//油门						
					}

					GUI_DispStringAt(":", 54, 46);

					//旋钮ADC映射范围
					temp_num = Knob_Avg;
					
					if(temp_num >= MAX_KNOB_VALUE)
					{
						temp_num = MAX_KNOB_VALUE;
					}
					
					float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//旋钮百分比
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 68, 46);	
					
					
					//显示摇杆控制反馈读数--------------------------------------
					//推进百分比--------------------------
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt(_tuijin[0], 36, 70);												//推进
					GUI_DispStringAt(":", 54, 70);					

					temp_num = RightSpeedCommand;
					
					if(temp_num >= FORWARD_NUM)
					{
						temp_num = temp_num - MIDDLE_NUM;
						GUI_DispStringAt(_qianjin[0], 105, 70);												//前进					
					}
					else if(temp_num <= BACKWARD_NUM)
					{
						temp_num = MIDDLE_NUM - temp_num;
						GUI_DispStringAt(_houtui[0], 105, 70);												//后退					
					}
					else
					{
						temp_num = 0;
					}
					
					float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 68, 70);						
					
					
					//舵角百分比-----------------------------		
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt(_duojiao[0], 145, 70);													//舵角
					GUI_DispStringAt(":", 163, 70);									

					temp_num = RightRudderCommand;

					//ME300舵角
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
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右	
							
							float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
						}
						else if((temp_num <= 1540) && (temp_num >= 1110))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左			
							
							float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
						}						
						
					}
					
					//其他舵角
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
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左		
							
							float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
						}
						else if((temp_num <= 1540) && (temp_num >= 1010))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右			
							
							float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
						}						
					}

					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 177, 70);						
					
//					//显示摇杆控制反馈读数======================================					
//					printf("RRS: %d, %d,  LRS: %d, %d\n", RightRudderCommand, RightSpeedCommand, LeftRudderCommand, LeftSpeedCommand);
				}
				
				else																															//英文
				{
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					
					if(local_para_conf[2] == USV_Boat_Type_MC120)
					{
						//显示水泵反馈读数
						GUI_DispStringAt("Water  pump:", 20, 46);											//水泵							
					}
					else if(local_para_conf[2] == USV_Boat_Type_ME300)
					{
						//显示油门反馈读数
						GUI_DispStringAt("Accelerator:", 20, 46);											//油门
					}
					else
					{
						//显示档位反馈读数
						GUI_DispStringAt("Speed limit:", 20, 46);											//档位					
					}

					//旋钮ADC映射范围
					temp_num = Knob_Avg;
					
					if(temp_num >= MAX_KNOB_VALUE)
					{
						temp_num = MAX_KNOB_VALUE;
					}
					
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//旋钮百分比
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 136, 46);	
					
					
					//显示摇杆控制反馈读数--------------------------------------
					//推进百分比--------------------------
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt("Spe", 36, 70);												//推进
					GUI_DispStringAt(":", 54, 70);					

					temp_num = RightSpeedCommand;
					
					if(temp_num >= FORWARD_NUM)
					{
						temp_num = temp_num - MIDDLE_NUM;
						GUI_DispStringAt(_qianjin[0], 105, 70);												//前进					
					}
					else if(temp_num <= BACKWARD_NUM)
					{
						temp_num = MIDDLE_NUM - temp_num;
						GUI_DispStringAt(_houtui[0], 105, 70);												//后退					
					}
					else
					{
						temp_num = 0;
					}
					
					float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 68, 70);						
					
					
					//舵角百分比-----------------------------		
					GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
					GUI_DispStringAt("Rud", 145, 70);													//舵角
					GUI_DispStringAt(":", 163, 70);									

					temp_num = RightRudderCommand;
					
					//ME300舵角
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
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右	
							
							float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
						}
						else if((temp_num <= 1540) && (temp_num >= 1110))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左		
							
							float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
						}						
						
					}
					
					//其他舵角
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
							GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左		
							
							float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
						}
						else if((temp_num <= 1540) && (temp_num >= 1010))
						{
							temp_num = 1560 - temp_num;
							GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右			
							
							float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
						}
						else
						{
							temp_num = 0;
							float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
						}						
					}

					GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
					GUI_DispStringAt(tempChar, 177, 70);					
				}
				
				break;
			}														
		}					
	}
	//单摇杆模式显示==================================================================
	
	
	//双摇杆模式显示------------------------------------------------------------------
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
		
		//画板框标题
		if(local_para_conf[6] == 1)				//中文
		{
			if(local_para_conf[1] == 0)			//单推
			{
				GUI_DispStringAt(_zuosuyouduo[0], 125, 16);									//左速右舵
			}
			else														//双推
			{
				GUI_DispStringAt(_zuoyouchasu[0], 125, 16);									//左右差速
			}						
		}
		else                              //英文
		{
			if(local_para_conf[1] == 0)			//单推
			{
				GUI_DispStringAt("LH-Speed  RH-Rudder", 125, 16);									//左速右舵
			}
			else														//双推
			{
				GUI_DispStringAt("Differential Control", 125, 16);									//左右差速
			}			
		}

		
		GUI_SetColor(GUI_RED);
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		
		switch (local_para_conf[2])
		{
			case USV_Boat_Type_LE2000:																				//LE2000操作反馈
			case USV_Boat_Type_LE3000:																				//LE3000操作反馈
			{
//				break;
			}
				
			default:
			{
				if(local_para_conf[6] == 1)																		//中文显示
				{
					//单推  左速右舵---------------------------------------------					
					if(local_para_conf[1] == 0)			
					{
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//显示水泵反馈读数
							GUI_DispStringAt(_shuibeng[0], 36, 46);												//水泵							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//显示油门反馈读数
							GUI_DispStringAt(_youmen[0], 36, 46);												//油门
						}
						else
						{
							//显示档位反馈读数
							GUI_DispStringAt(_dangwei[0], 36, 46);												//油门						
						}

						GUI_DispStringAt(":", 54, 46);

						//旋钮ADC映射范围
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//旋钮百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 46);	
						
						
						//显示摇杆控制反馈读数--------------------------------------
						//推进百分比--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_tuijin[0], 36, 70);												//推进
						GUI_DispStringAt(":", 54, 70);					

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 70);												//前进					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 70);												//后退					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 70);						
						
						
						//舵角百分比-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_duojiao[0], 145, 70);													//舵角
						GUI_DispStringAt(":", 163, 70);									

						temp_num = RightRudderCommand;
						
						//ME300舵角
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
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左		
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}									
						}
						
						//其他舵角
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
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}							
						}

						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 70);					
					}
					//单推  左速右舵=============================================
					
					
					//双推  左右差速---------------------------------------------
					else														
					{
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//显示水泵反馈读数
							GUI_DispStringAt(_shuibeng[0], 36, 36);												//水泵							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//显示油门反馈读数
							GUI_DispStringAt(_youmen[0], 36, 36);												//油门
						}
						else
						{
							//显示档位反馈读数
							GUI_DispStringAt(_dangwei[0], 36, 36);												//油门						
						}

						GUI_DispStringAt(":", 54, 36);

						//旋钮ADC映射范围
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//旋钮百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 36);	
						
						
						//显示摇杆控制反馈读数--------------------------------------
						//左推进百分比--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_zuojin[0], 36, 56);												//左进
						GUI_DispStringAt(":", 54, 56);							

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 56);												//前进					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 56);												//后退					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 56);						
						
						
						//左舵角百分比-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_zuoduo[0], 36, 76);												//左舵
						GUI_DispStringAt(":", 54, 76);									

						temp_num = LeftRudderCommand;

						//ME300舵角
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
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//转右	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//转左			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}															
						}
						
						//其他船舵角映射
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
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//转左		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}							
						}
						
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 76);

						
						//右推进百分比--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_youjin[0], 145, 56);												//右进
						GUI_DispStringAt(":", 163, 56);							

						temp_num = RightSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 214, 56);												//前进					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 214, 56);												//后退					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 56);		
						
						
						//右舵角百分比-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt(_youduo[0], 145, 76);													//右舵
						GUI_DispStringAt(":", 163, 76);									

						temp_num = RightRudderCommand;
						
						//ME300舵角
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
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//转左		
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}							
						}
						
						//其他船舵角
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
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//转左		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}						
						}
						
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 76);							
					}
					//双推  左右差速=============================================			
						
					
				}
				
				else																															//英文
				{
					//单推  左速右舵---------------------------------------------					
					if(local_para_conf[1] == 0)			
					{
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//显示水泵反馈读数
							GUI_DispStringAt("Water  pump:", 20, 46);											//水泵							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//显示油门反馈读数
							GUI_DispStringAt("Accelerator:", 20, 46);											//油门
						}
						else
						{
							//显示档位反馈读数
							GUI_DispStringAt("Speed limit:", 20, 46);											//档位					
						}

						//旋钮ADC映射范围
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//旋钮百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 136, 46);	
						
						
						//显示摇杆控制反馈读数--------------------------------------
						//推进百分比--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("Spe", 36, 70);												//推进
						GUI_DispStringAt(":", 54, 70);					

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 70);												//前进					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 70);												//后退					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 70);						
						
						
						//舵角百分比-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("Rud", 145, 70);													//舵角
						GUI_DispStringAt(":", 163, 70);									

						temp_num = RightRudderCommand;
						
						//ME300舵角
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
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左		
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}									
						}
						
						//其他舵角
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
								GUI_DispStringAt(_xiangzuo[0], 214, 70);												//转左		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 70);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}							
						}

						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 70);					
					}
					//单推  左速右舵=============================================
					
					
					//双推  左右差速---------------------------------------------
					else														
					{
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						
						if(local_para_conf[2] == USV_Boat_Type_MC120)
						{
							//显示水泵反馈读数
							GUI_DispStringAt("Water  Pump:", 20, 36);												//水泵							
						}
						else if(local_para_conf[2] == USV_Boat_Type_ME300)
						{
							//显示油门反馈读数
							GUI_DispStringAt("Accelerator:", 20, 36);												//油门
						}
						else
						{
							//显示档位反馈读数
							GUI_DispStringAt("Speed limit:", 20, 36);												//档位						
						}


						//旋钮ADC映射范围
						temp_num = Knob_Avg;
						
						if(temp_num >= MAX_KNOB_VALUE)
						{
							temp_num = MAX_KNOB_VALUE;
						}
						
						float_to_string(temp_num * 100 / MAX_KNOB_VALUE, tempChar, 0, 1, 0, 0);				//旋钮百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 136, 36);	
						
						
						//显示摇杆控制反馈读数--------------------------------------
						//左推进百分比--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("LSp", 36, 56);												//左进
						GUI_DispStringAt(":", 54, 56);							

						temp_num = LeftSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 105, 56);												//前进					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 105, 56);												//后退					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 56);						
						
						
						//左舵角百分比-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("LRu", 36, 76);												//左舵
						GUI_DispStringAt(":", 54, 76);									

						temp_num = LeftRudderCommand;

						//ME300舵角
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
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//转右	
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//转左			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}															
						}
						
						//其他船舵角映射
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
								GUI_DispStringAt(_xiangzuo[0], 105, 76);												//转左		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 105, 76);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}							
						}
						
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 68, 76);

						
						//右推进百分比--------------------------
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("RSp", 145, 56);												//右进
						GUI_DispStringAt(":", 163, 56);							

						temp_num = RightSpeedCommand;
						
						if(temp_num >= FORWARD_NUM)
						{
							temp_num = temp_num - MIDDLE_NUM;
							GUI_DispStringAt(_qianjin[0], 214, 56);												//前进					
						}
						else if(temp_num <= BACKWARD_NUM)
						{
							temp_num = MIDDLE_NUM - temp_num;
							GUI_DispStringAt(_houtui[0], 214, 56);												//后退					
						}
						else
						{
							temp_num = 0;
						}
						
						float_to_string(temp_num * 100 / MIDDLE_NUM, tempChar, 0, 1, 0, 0);				//推进百分比
						GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
						GUI_DispStringAt(tempChar, 177, 56);		
						
						
						//右舵角百分比-----------------------------		
						GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
						GUI_DispStringAt("RRu", 145, 76);													//右舵
						GUI_DispStringAt(":", 163, 76);									

						temp_num = RightRudderCommand;
						
						//ME300舵角
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
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//转左		
								
								float_to_string(temp_num * 100 / (2100 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1110))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1110), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
							}							
						}
						
						//其他船舵角
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
								GUI_DispStringAt(_xiangzuo[0], 214, 76);												//转左		
								
								float_to_string(temp_num * 100 / (2000 - 1560), tempChar, 0, 1, 0, 0);				//舵角百分比						
							}
							else if((temp_num <= 1540) && (temp_num >= 1010))
							{
								temp_num = 1560 - temp_num;
								GUI_DispStringAt(_xiangyou[0], 214, 76);												//转右			
								
								float_to_string(temp_num * 100 / (1560 - 1010), tempChar, 0, 1, 0, 0);				//舵角百分比	
							}
							else
							{
								temp_num = 0;
								float_to_string(temp_num, tempChar, 0, 1, 0, 0);				//舵角百分比	
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
	//双摇杆模式显示==================================================================
	
	//刷新操作反馈区
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 43);	
	//显示操作反馈区============================================================================

	
	//显示状态监测区----------------------------------------------------------------------------
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	

//	local_para_conf[6] = 0;
	
	//画板框
	GUI_SetColor(GUI_WHITE);
	GUI_DrawRoundedRect(8, 16, 250 - 8, 100 - 2, 3);	

	//画板框标题
	GUI_SetColor(GUI_BLUE);
	GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);

	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_zhuangtaijiance[0], 125, 16);									//智能任务
	}
	else																//英文
	{
		GUI_DispStringAt("USV Status", 125, 16);
	}

	//显示卫星定位信息--------------------------------------------
	GUI_SetFont(&GUI_Fontsongti12);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_UC_SetEncodeUTF8();
	GUI_SetColor(GUI_RED);
	
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_weizhi[0], 16, 36);									//位置	
		GUI_DispStringAt(":", 41, 36);									
	}
	else
	{
		GUI_DispStringAt("GPS:", 16, 36);		
	}

	//缓存经度与纬度
	lat = BoatData.DoubleLatitude;
	lng = BoatData.DoubleLongitude;

//	lat = 88.55 / 1000;
//	lng = 120.66 / 1000;

	//纬度------------------------------------
	//北纬
	if (lat > 0)
	{	//-90,90
		GUI_DispStringAt("N", 53, 36);			//N		
	}
	//南纬
	else 
	{
		lat = -lat;		
		GUI_DispStringAt("S", 53, 36);			//S	
	}
	
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
//	float_to_string(lat * 1000, tempChar, 4, 0, 0, 0);
	float_to_string(lat, tempChar, 4, 0, 0, 0);	
	GUI_DispStringAt(tempChar, 108, 36);	
	GUI_DispStringAt(_du[0], 110, 36);									//度 °		

	//经度-------------------------------------
	//东经
	if (lng > 0)
	{//-180,180
		GUI_DispStringAt("E", 136, 36);			//E	
	}
	//西经
	else 
	{
		lng = -lng;		
		GUI_DispStringAt("W", 136, 36);			//W	
	}	
	
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
//	float_to_string(lng * 1000, tempChar, 4, 0, 0, 0);
	float_to_string(lng, tempChar, 4, 0, 0, 0);
	GUI_DispStringAt(tempChar, 196, 36);		
	GUI_DispStringAt(_du[0], 198, 36);									//度 °				

//	BoatData.Speed = 233;
	
	//速度--------------------------------------
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_sudu[0], 16, 52);									//速度
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
	//显示卫星定位信息============================================

//	local_para_conf[6] = 0;
	
	//显示惯性状态------------------------------------------------
	//艘摇  Yaw----------------------------------
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_shouyao[0], 129, 52);									//艏摇 
		GUI_DispStringAt(":", 25 + 129, 52);									
	}
	else
	{
		GUI_DispStringAt("Yaw:", 129, 52);		
	}	
	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	float_to_string(((int16_t) BoatData.Yaw) / 10, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 168, 52);	
	
	
	//横摇  Yaw----------------------------------	
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_hengyao[0], 16, 68);									//横摇
		GUI_DispStringAt(":", 41, 68);									
	}
	else
	{
		GUI_DispStringAt("Roll:", 16, 68);		
	}	
	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	float_to_string(((int16_t) BoatData.Roll) / 10, tempChar, 0, 0, 0, 0);
	GUI_DispStringAt(tempChar, 55, 68);
	
	
	//纵摇  Yaw----------------------------------
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_zongyao[0], 129, 68);									//纵摇
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
	//显示惯性状态================================================

	
	//显示温度监测------------------------------------------------
	//水温  WaterTemp----------------------------------	
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_shuiwen[0], 16, 84);									//水温
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
	GUI_DispStringAt(_sheshidu[0], 88, 84);										//℃  摄氏度


	//船温  InternalTemp----------------------------------	
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_DispStringAt(_chuanwen[0], 129, 84);									//船温
		GUI_DispStringAt(":", 25 + 129, 84);									
	}
	else
	{
		GUI_DispStringAt("I T:", 129, 84);											//船内温度
	}	
	
//	BoatData.InternalTemp = 132.5 / 10;
	
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);
	float_to_string(BoatData.InternalTemp, tempChar, 1, 0, 0, 0);
	GUI_DispStringAt(tempChar, 199, 84);
	GUI_DispStringAt(_sheshidu[0], 201, 84);										//℃  摄氏度	
	//显示温度监测================================================
	
	//刷新状态监测区
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 150);	
	//显示状态监测区============================================================================
	
	
	//显示采样监测任务信息----------------------------------------------------------------------
//	uint8_t number_min = MinBottleNumber;
	uint8_t number_max;
	int color_temp;
	
	//显示采样监测文字
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1w_memdev);	
	GUI_MEMDEV_Clear(bg1w_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);	

	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_DARKBLUE);
	
	if(local_para_conf[6] == 1)					//中文
	{
		GUI_SetFont(&GUI_Fontsongti16);
		GUI_DispStringAt(_caiyangyujiance[0], 16, 12);										//采样 & 监测		
	}	
	else
	{
		GUI_SetFont(&GUI_Fontsongti12);
		GUI_DispStringAt("RT Monitoring", 16, 12);												//实时监测								
	}

	GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 250, 44);


	//显示采样信息-------------------------------------------
	//显示采样监测背景
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(bg0_memdev);	
	GUI_MEMDEV_Clear(bg0_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG0_XAXIS, DEBUG_BG0_YAXIS);	
	
	GUI_DrawGradientRoundedH(8, 2, 116, 22, 3, GUI_RED, GUI_ORANGE);				//采样 & 监测 背景条

	
	//绘制采样瓶背景
	switch (local_para_conf[2])
	{
		//绘制 1 ~ 4 号瓶
		case USV_Boat_Type_SF20:																			
		case USV_Boat_Type_SS20:
		case USV_Boat_Type_ESM20:
		case USV_Boat_Type_SS30:
		case USV_Boat_Type_ESM30:
		case USV_Boat_Type_ES30:
		case USV_Boat_Type_ESMH30:			
		{
			GUI_BMP_Draw(_acImage_11, 8 + 10, 28);				//采样瓶背景  1 ~ 4			
			Draw_Empty_Bottle(8 + 10);
			
			number_max = 4;
			
			break;
		}


		//绘制 5 ~ 8 号瓶
		case USV_Boat_Type_MC70:
		case USV_Boat_Type_MM70:
		case USV_Boat_Type_MS70:
		case USV_Boat_Type_ME70:
		case USV_Boat_Type_MMH70:
		{
			GUI_BMP_Draw(_acImage_11, 8 + 10, 28);					//采样瓶背景  1 ~ 4
			GUI_BMP_Draw(_acImage_11, 78 + 10, 28);				//采样瓶背景  5 ~ 8
			Draw_Empty_Bottle(8 + 10);
			Draw_Empty_Bottle(78 + 10);	
			
			number_max = 8;

			break;
		}

		//绘制 9 ~ 12 号瓶		
		case USV_Boat_Type_MC120:	
		case USV_Boat_Type_ME120:	
		case USV_Boat_Type_MS120:	
		case USV_Boat_Type_MM120:	
		case USV_Boat_Type_ME300:
		{
			GUI_BMP_Draw(_acImage_11, 8 + 10, 28);					//采样瓶背景  1 ~ 4
			GUI_BMP_Draw(_acImage_11, 78 + 10, 28);				//采样瓶背景  5 ~ 8
			GUI_BMP_Draw(_acImage_11, 148 + 10, 28);				//采样瓶背景   9 ~ 12
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

	//显示采水容量
	for(temp_num = MinBottleNumber; temp_num <= number_max; temp_num++)
	{
		//显示已经采样好的容量
		if(SampleInfo.IsBottleTaking[temp_num - 1] == FALSE)
		{
			color_temp = GUI_BLUE;
		}
		
		//显示正在采样的容量
		else
		{
			color_temp = GUI_GREEN;
		}
		
		//3组采样瓶
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
	//显示采样信息===========================================
		
	//刷新采样监测任务区
	GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 250, 44);	
	//显示采样监测任务信息======================================================================
	
	
	//显示深度监测任务信息----------------------------------------------------------------------
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1w_memdev);	
	GUI_MEMDEV_Clear(bg1w_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, DEBUG_BG1W_XAXIS, DEBUG_BG1W_YAXIS);		

	GUI_SetColor(GUI_RED);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);	
	GUI_SetFont(&GUI_Fontsongti12);
	
	if(local_para_conf[6] == 1)									//中文
	{
		GUI_DispStringAt(_shendu[0], 8, 8);										//深度
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
	
	//绘画深度波形-------------------------------------
	show_depth_div++;
	
	if(show_depth_div >= SHOW_DEPTH_TIME)					//0.5秒刷新一次
	{
		show_depth_div = 0;
		
		//刷新X轴位置
		GUI_SelectLayer(1);
		GUI_MEMDEV_Select(bg1_memdev);	
		GUI_MEMDEV_Clear(bg1_memdev);	
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(30 + waveform_xaxis + 1, 0, 30 + waveform_xaxis + 4, DEBUG_BG1_YAXIS);		
	
		//读取当前深度后转换为显示输出
//		temp_num = (uint16_t) (BoatData.Depth * 100);
		wave_temp = (uint16_t) (BoatData.Depth);
		
		if(wave_temp >= SHOW_MAX_DEPTH)
		{
			wave_temp = SHOW_MAX_DEPTH; 
		}
		
		//绘画当前位置深度直线
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
	//显示深度监测任务信息======================================================================
	
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
	
	LCD_PWM_Config(ENABLE);
								
	
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
	
//	GUI_BMP_Draw(_acImage_0, 140, 10 + 32);				//Yunzhou logo
	
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
		GUI_DispStringAt("language:", 5, 8 + 72);								//语言设置
		GUI_DispStringAt("Version:", 5, 8 + 84);										//版本号
		GUI_DispStringAt(version_number, 110, 8 + 84);		
		
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
		GUI_DispStringAt("language:", 5, 8 + 72);								//语言设置
		GUI_DispStringAt("Version:", 5, 8 + 84);										//版本号
		GUI_DispStringAt(version_number, 110, 8 + 84);		

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

	if(debug_mode_parameter_init == 1)
	{
		debug_mode_parameter_init = 0;
		
		LCD_clear();
	}
	
	
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
  * @brief  Draw_Empty_Bottle 画采样瓶胆心
  * @param  start_pix： 起始像素点
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
	函数名：show_save_wait
	功  能：参数配置保存等待警告
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void show_save_wait(void)
{
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);		
	GUI_FillRect(0, 0, DEBUG_BG1_XAXIS, DEBUG_BG1_YAXIS);	
			
	GUI_BMP_Draw(_acImage_14, 80, 0);											//感叹号图标
		
	GUI_SetColor(GUI_YELLOW);
	GUI_SetBkColor(GUI_TRANSPARENT);	
				
	if(local_para_conf[6] == 1)						//中文
	{
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);	
		GUI_SetFont(&GUI_Fontsongti16);	
		GUI_DispStringAt(_canshucunchuzhong[0], 100, 60);				//参数存储中
		GUI_SetTextAlign(GUI_TA_CENTER | GUI_TA_VCENTER);
		GUI_DispStringAt(_qingwuduandian[0], 100, 85);						//请勿断电
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
	else if(unit == 3)
	{
		str[i++] = '#';							//米单位
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
