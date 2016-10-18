/*-------------------------------------------------------------------------
工程名称：按键采样应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "keyboard_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Keyboard_Task_Handle;

//按键及钮子开关相关全局变量
uint8_t MENU_Button = 0;				//MENU 菜单键，0表示没有按下，1表示按下
uint8_t OK_Button = 0;					//OK 确认键，0表示没有按下，1表示按下
uint8_t CANCEL_Button = 0;			//CANCEL 取消键，0表示没有按下，1表示按下
uint8_t ALARM_Button = 0;				//ALARM 警报警灯钮子开关，0表示没有按下，1表示按下
uint8_t ALARM_Button_Previous = 0;				//前一次的警报警灯按键状态
uint8_t SAMPLE_Button = 0;			//SAMPLE 采样开关，0表示没有按下，1表示按下
uint8_t MODE_Button = 0;				//MODE 模式开关，0表示模式0，1表示模式1，2表示模式2
uint8_t MODE_Button_Previous = 0;				//前一次的控制模式，用于判定某些指令是否发送

//摇杆方向表征
uint8_t left_joystick_ud = 0;				//左侧摇杆上下方向指示，0代表中位，1代表上拉杆，2代表下拉杆
uint8_t left_joystick_lr = 0;				//左侧摇杆左右方向指示，0代表中位，1代表左拉杆，2代表右拉杆
uint8_t right_joystick_ud = 0;			//右侧摇杆上下方向指示，0代表中位，1代表上拉杆，2代表下拉杆
uint8_t right_joystick_lr = 0;			//右侧摇杆左右方向指示，0代表中位，1代表左拉杆，2代表右拉杆


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
	
	uint16_t K_div;						//按键分频系数，慢速
	uint16_t KQ_div;					//							快速
	
	
	/* Run the Keyboard task */
  while (1)
  {	
		/*按键及钮子开关采样-----------------------------------------------------------------------------------*/
		//B_MENU 菜单键采样------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == 0)
		{
			menu_temp++;
			
			//抗抖动处理
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
		
		//B_OK 确认键采样------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13) == 0)
		{
			ok_temp++;
			
			//抗抖动处理
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

		//B_CANCEL 取消键采样------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12) == 0)
		{
			cancel_temp++;
			
			//抗抖动处理
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
		
		//S_Alarm 警报警灯键采样----------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0)
		{
			alarm_temp++;
			
			//抗抖动处理
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

		//S_Sample 采样键采样----------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
		{
			sample_temp++;
			
			//抗抖动处理
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

		//S_MODE2 手动单推模式键采样----------------------------------------------
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		{
			modeup_temp++;
			
			//抗抖动处理
			if(modeup_temp >= 2)
			{
				modeup_temp = 2;				
				MODE_Button = 0;
			}
		}
		//S_MODE1 自动模式键采样----------------------------------------------
		else if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
		{
			modedown_temp++;
			
			//抗抖动处理
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
		/*按键及钮子开关采样===================================================================================*/

		
		/*摇杆按键映射-----------------------------------------------------------------------------------------*/
		//左侧摇杆上下映射-----------------------------------------------------
		if(Left_Joystick_Y_axis_Avg > UP_LEFT_THRESHOLD)
			left_joystick_ud = 1;
		
		else if(Left_Joystick_Y_axis_Avg < DOWN_RIGHT_THRESHOLD)
			left_joystick_ud = 2;
		
		else
			left_joystick_ud = 0;


		//左侧摇杆左右映射-----------------------------------------------------
		if(Left_Joystick_X_axis_Avg > UP_LEFT_THRESHOLD)		
			left_joystick_lr = 1;
		
		else if(Left_Joystick_X_axis_Avg < DOWN_RIGHT_THRESHOLD)
			left_joystick_lr = 2;
		
		else
			left_joystick_lr = 0;		


		//右侧摇杆上下映射-----------------------------------------------------
		if(Right_Joystick_Y_axis_Avg > UP_LEFT_THRESHOLD)
			right_joystick_ud = 1;
		
		else if(Right_Joystick_Y_axis_Avg < DOWN_RIGHT_THRESHOLD)
			right_joystick_ud = 2;
		
		else
			right_joystick_ud = 0;


		//右侧摇杆左右映射-----------------------------------------------------
		if(Right_Joystick_X_axis_Avg > UP_LEFT_THRESHOLD)
			right_joystick_lr = 1;
		
		else if(Right_Joystick_X_axis_Avg < DOWN_RIGHT_THRESHOLD)
			right_joystick_lr = 2;
		
		else
			right_joystick_lr = 0;		
		/*摇杆按键映射=========================================================================================*/
		
//		USART_SendData(USART2, 0x89);

		
		K_div++;
		
		//1000毫秒响应一次
		if(K_div > 1000)
		{
			K_div = 0;
			
			//参数配置时修改索引修改---------------------------------------------------------------------------------			
			//索引减操作，即左摇杆往上拨
			if((interface_index == 2) && (left_joystick_ud == 1))
			{
				//判断索引是否到顶，不到顶减一处理
				if(modify_index > 0)
				{
					modify_index--;
				}
				
				//到顶指向索引底部
				else
				{
					if(local_para_conf[2] == USV_Boat_Type_ME300)						//ME300存在若干控制阀门
					{
						modify_index = PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY + VALVE_QUANTITY - 1;
					}
					else
					{
						modify_index = PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY - 1;
					}
				}
			}

			//索引加操作，即左摇杆往下拨
			else if((interface_index == 2) && (left_joystick_ud == 2))
			{
				//判断索引是否到底，不到底加一处理
				//ME300存在若干控制阀门----------------------------------------------
				if(local_para_conf[2] == USV_Boat_Type_ME300)						
				{
					if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY + VALVE_QUANTITY - 1)
					{
						modify_index++;
					}
					
					//到底指向索引顶部
					else
					{
						modify_index = 0;
					}		
				}					
		
				//非ME300没有阀门显示设置---------------------------------------------
				else
				{
					if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY - 1)
					{
						modify_index++;
					}
					
					//到底指向索引顶部
					else
					{
						modify_index = 0;
					}					
				}
			}
			//参数配置时修改索引修改=================================================================================
			
			
			//采样配置时修改索引修改---------------------------------------------------------------------------------			
			//索引减操作，即左摇杆往上拨
			if((interface_index == 3) && (left_joystick_ud == 1))
			{
				//判断索引是否到顶，不到顶减一处理
				if(modify_index > 0)
				{
					modify_index--;
				}
				
				//到顶指向索引底部
				else
				{
					modify_index = WATER_SAMPLE_PARAMETER - 1;
				}
			}

			//索引加操作，即左摇杆往下拨
			else if((interface_index == 3) && (left_joystick_ud == 2))
			{
				//判断索引是否到底，不到底加一处理
				if(modify_index < WATER_SAMPLE_PARAMETER - 1)
				{
					modify_index++;
				}
				
				//到底指向索引顶部
				else
				{
					modify_index = 0;
				}				
			}
			//采样配置时修改索引修改=================================================================================

			
			//当前索引下的参数慢速修改（参数配置）-------------------------------------------------------------------------------
			//参数加操作，即右摇杆往上拨
			if((interface_index == 2) && (right_joystick_ud == 1))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 索引时
				//数传模块参数索引-----------------------------------------------------------------------
				if(modify_index < PARAMETER_QUANTITY)
				{
					//判断当前参数是否到顶，到顶后重新赋值至底部
					if(Xtend_900_para_buf[modify_index] >= Xtend_900_para_max[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_min[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]++;
					}
				}	

				
				//本地参数索引-----------------------------------------------------------------------
				else if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY)
				{
					//判断当前参数是否到顶，到顶后重新赋值至底部
					if(local_para_conf_buf[modify_index - PARAMETER_QUANTITY] >= local_para_conf_max[modify_index - PARAMETER_QUANTITY])
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY] = local_para_conf_min[modify_index - PARAMETER_QUANTITY];
					}
					
					else
					{
						local_para_conf_buf[modify_index - PARAMETER_QUANTITY]++;
					}					
				}	

				
				//阀门参数索引-----------------------------------------------------------------------
				else
				{
					//判断当前参数是否到顶，到顶后重新赋值至底部
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


			//参数减操作，即右摇杆往下拨
			else if((interface_index == 2) && (right_joystick_ud == 2))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 索引时
				//数传模块参数索引-----------------------------------------------------------------------
				if(modify_index < PARAMETER_QUANTITY)
				{
					//判断当前参数是否到底，到底后重新赋值至顶部
					if(Xtend_900_para_buf[modify_index] <= Xtend_900_para_min[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_max[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]--;
					}
				}	

				
				//本地参数索引-----------------------------------------------------------------------
				else if(modify_index < PARAMETER_QUANTITY + LOCAL_PARAMETER_QUANTITY)
				{
					//判断当前参数是否到底，到底后重新赋值至顶部
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
			//当前索引下的参数慢速修改（参数配置）===============================================================================
			
			
			//当前索引下的参数慢速修改（采样配置）-------------------------------------------------------------------------------
			//参数加操作，即右摇杆往上拨
			if((interface_index == 3) && (right_joystick_ud == 1))
			{
				//判断当前参数是否到顶，到顶后重新赋值至底部
				if(water_sample_conf_buf[modify_index] >= water_sample_conf_max[modify_index])
				{
					water_sample_conf_buf[modify_index] = water_sample_conf_min[modify_index];
				}
				
				else
				{
					water_sample_conf_buf[modify_index]++;
				}								
			}


			//参数减操作，即右摇杆往下拨
			else if((interface_index == 3) && (right_joystick_ud == 2))
			{
				//判断当前参数是否到底，到底后重新赋值至顶部
				if(water_sample_conf_buf[modify_index] <= water_sample_conf_min[modify_index])
				{
					water_sample_conf_buf[modify_index] = water_sample_conf_max[modify_index];
				}
				
				else
				{
					water_sample_conf_buf[modify_index]--;
				}
			}											
			//当前索引下的参数慢速修改（采样配置）===============================================================================
		}
		
		
		//2毫秒响应一次
		KQ_div++;
		
		if(KQ_div >= 2)
		{
			KQ_div = 0;
			
			//当前索引下的参数快速修改-------------------------------------------------------------------------------
			//参数加操作，即右摇杆往左拨
			if((interface_index == 2) && (right_joystick_lr == 1))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 索引时
				if(modify_index < PARAMETER_QUANTITY)
				{
					//判断当前参数是否到顶，到顶后重新赋值至底部
					if(Xtend_900_para_buf[modify_index] >= Xtend_900_para_max[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_min[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]++;
					}
				}	

				
				//其他索引时
				else
				{
					//判断当前参数是否到顶，到顶后重新赋值至底部
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


			//参数减操作，即右摇杆往右拨
			else if((interface_index == 2) && (right_joystick_lr == 2))
			{
				// 0 ~ PARAMETER_QUANTITY - 1 索引时
				if(modify_index < PARAMETER_QUANTITY)
				{
					//判断当前参数是否到底，到底后重新赋值至顶部
					if(Xtend_900_para_buf[modify_index] <= Xtend_900_para_min[modify_index])
					{
						Xtend_900_para_buf[modify_index] = Xtend_900_para_max[modify_index];
					}
					
					else
					{
						Xtend_900_para_buf[modify_index]--;
					}
				}	

				
				//其他索引时
				else
				{
					//判断当前参数是否到底，到底后重新赋值至顶部
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
			//当前索引下的参数快速修改===============================================================================			
		}

		
		//模式切换时标志位设置----------------------------------------------------------------------
		if(MODE_Button_Previous != MODE_Button)
		{
			//若拨杆到自动采水状态，则发送若干次自动指令
			if(MODE_Button == CONTROL_MODE_AUTO)
			{
				NeedSendAutoModeCommandCount = 2;
//				USART_SendData(USART2, 0x98);
			}			
		}
		
		MODE_Button_Previous = MODE_Button;
		//模式切换时标志位设置======================================================================
				

		//警报警灯开关切换时标志位设置--------------------------------------------------------------
		if(ALARM_Button_Previous != ALARM_Button)
		{
			NeedSendSpeaker = MAX_SPEAKER_SEND_COUNT;
			NeedSendAlertLED = MAX_ALERT_SEND_COUNT;
		}	

		ALARM_Button_Previous = ALARM_Button;
		//警报警灯开关切换时标志位设置==============================================================


		vTaskDelay(1);
	}
}

