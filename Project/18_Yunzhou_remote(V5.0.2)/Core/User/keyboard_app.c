/*-------------------------------------------------------------------------
工程名称：按键采样应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

#include "keyboard_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Keyboard_Task_Handle;

//按键及钮子开关相关全局变量
uint8_t MENU_Button = 0;				//MENU 菜单键，0表示没有按下，1表示按下
uint8_t OK_Button = 0;					//OK 确认键，0表示没有按下，1表示按下
uint8_t CANCEL_Button = 0;			//CANCEL 取消键，0表示没有按下，1表示按下
uint8_t ALARM_Button = 0;				//ALARM 警报警灯钮子开关，0表示没有按下，1表示按下
uint8_t SAMPLE_Button = 0;			//SAMPLE 采样开关，0表示没有按下，1表示按下
uint8_t MODE_Button = 0;				//MODE 模式开关，0表示模式0，1表示模式1，2表示模式2


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
		
		/*按键及钮子开关采样-----------------------------------------------------------------------------------*/
		
		vTaskDelay(10);
	}
}
