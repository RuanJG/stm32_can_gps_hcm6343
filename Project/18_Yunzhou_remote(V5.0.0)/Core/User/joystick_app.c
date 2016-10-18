/*-------------------------------------------------------------------------
工程名称：摇杆、旋钮及电池处理应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


#include "joystick_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Joystick_Task_Handle;


/**
  * @brief  Joystick task
  * @param  pvParameters not used
  * @retval None
  */
void Joystick_Task(void * pvParameters)
{
	uint16_t J_Temp = 0, i_temp = 0;
	int count_temp = 0;

	//等待200毫秒板载驱动及液晶屏初始化
	vTaskDelay(200);	
	GPIO_SetBits(GPIOF, GPIO_Pin_6);			//常亮液晶屏
	
  /* Run the Joystick task */
  while (1)
  {
		/*ADC采样处理-----------------------------------------------------------------------------------*/
		//电池电量均值----------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][0];
		}		
		Battery_Power_Avg = count_temp / ADC_BUFFER_SIZE;					//电池电量均值	Bat_ADC
		
		//左侧摇杆 X 轴采样均值----------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][1];
		}		
		Left_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//左侧摇杆 X 轴采样均值		RCKLY_ADC
		
		//电位器旋钮均值----------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][2];
		}		
		Knob_Avg = count_temp / ADC_BUFFER_SIZE;					//电位器旋钮均值		Wheel_ADC
		
		//左侧摇杆 Y 轴采样均值---------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][3];
		}		
		Left_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//左侧摇杆 Y 轴采样均值		RCKLX_ADC
		
		//右侧摇杆 X 轴采样均值---------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][4];
		}		
		Right_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//右侧摇杆 X 轴采样均值		RCKRY_ADC
		
		//右侧摇杆 Y 轴采样均值---------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][5];
		}		
		Right_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//右侧摇杆 X 轴采样均值		RCKRY_ADC		
		/*ADC采样处理-----------------------------------------------------------------------------------*/		
		
		J_Temp++;
				
		if(J_Temp >= 10)
		{
			J_Temp = 0;
			
			printf("bbbb\n");
		}
		
		vTaskDelay(10);
	}
}
