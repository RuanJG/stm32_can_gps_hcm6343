/*-------------------------------------------------------------------------
工程名称：摇杆、旋钮及电池处理应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "joystick_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Joystick_Task_Handle;

u16 RightRudderCommand;				//右舵机舵角映射
u16 RightSpeedCommand;				//右推进速度映射
u16 LeftRudderCommand;				//左舵机舵角映射
u16 LeftSpeedCommand;					//左推进速度映射

//旋钮键值映射
u16 KnobValue;									//旋钮采样后的处理值	---  喷水
u16 SpeedFactor;								//旋钮采样后的处理值	---	 速度因子或油门

int xx;
int yy;
int adcOffset = 100;
double doublespeed;

u8 ControllerType = 2;							//摇杆模式，是否叠加旋钮判定


/**
  * @brief  Joystick task
  * @param  pvParameters not used
  * @retval None
  */
void Joystick_Task(void * pvParameters)
{
	uint16_t J_Temp = 0, i_temp = 0;
	int count_temp = 0;
	uint16_t Battery_temp[10];
	uint16_t Battery_pointer = 0;

//	//等待200毫秒板载驱动及液晶屏初始化
//	vTaskDelay(200);	
//	GPIO_SetBits(GPIOF, GPIO_Pin_6);			//常亮液晶屏
	
//	LCD_PWM_Config();						//液晶屏幕PWM调节
//	Buzzer_PWM_Config();
	
  /* Run the Joystick task */
  while (1)
  {
		/*ADC采样处理-----------------------------------------------------------------------------------*/
		//电池电量均值----------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][0];
//		}		
//		Battery_Power_Avg = count_temp / ADC_BUFFER_SIZE;					//电池电量均值	Bat_ADC

		
		
		//左侧摇杆 X 轴采样均值----------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][1];
//		}		
//		Left_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//左侧摇杆 X 轴采样均值		RCKLY_ADC
		Left_Joystick_X_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 1);
		
		
		//电位器旋钮均值----------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][2];
//		}		
//		Knob_Avg = count_temp / ADC_BUFFER_SIZE;					//电位器旋钮均值		Wheel_ADC
		Knob_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 2);
		
		
		//左侧摇杆 Y 轴采样均值---------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][3];
//		}		
//		Left_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//左侧摇杆 Y 轴采样均值		RCKLX_ADC
		Left_Joystick_Y_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 3);
		
		
		//右侧摇杆 X 轴采样均值---------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][4];
//		}		
//		Right_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//右侧摇杆 X 轴采样均值		RCKRY_ADC
		Right_Joystick_X_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 4);

		
		//右侧摇杆 Y 轴采样均值---------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][5];
//		}		
//		Right_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//右侧摇杆 X 轴采样均值		RCKRY_ADC		
		Right_Joystick_Y_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 5);
		/*ADC采样处理-----------------------------------------------------------------------------------*/		
		
		J_Temp++;
				
		if(J_Temp >= 10)
		{
			J_Temp = 0;
		
			//电量低速处理			
			Battery_temp[Battery_pointer] = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 0) & 0x0fff;
//			printf("%d\n", Battery_temp[Battery_pointer]);
			
			Battery_pointer++;
			
			if(Battery_pointer >= 10)
			{
				Battery_pointer = 0;
			}
			
			Battery_Power_Avg = FindMax(Battery_temp, 10);
			
			/*
					电池标称10.8 ~ 12.3V，通过1M及270K电阻分压后为2.30 ~ 2.61V，ADC换算为 2854 ~ 3238
					电池电量粗劣的计算公式为：1-(4.2-x)/(4.2-3.6)
			*/
			Battery_Power_Percent = 100 - (3238 - Battery_Power_Avg) * 100 / (3238 - 2854);
			
			if(Battery_Power_Percent <= 0)
			{
				Battery_Power_Percent = 0;
			}
			else if(Battery_Power_Percent >= 100)
			{
				Battery_Power_Percent = 100;
			}			
			
//			printf("%d\n", Battery_Power_Avg);
		}

		
		//旋钮键值映射
		CountKnobValue();
		//新舵角推进计算
		count_angle_speed_right();
		count_angle_speed_left();		
		
		vTaskDelay(10);
	}
}


/*-------------------------------------------------------------------------
	函数名：LowPassFilter
	功  能：数字低频滤波
	参  数：buffer 	待滤波数组（二维）
					length	数组长度
					subpara 二维数组第二维度索引
	返回值：无符号16位滤波值
-------------------------------------------------------------------------*/
uint16_t LowPassFilter(uint16_t buffer[ADC_BUFFER_SIZE][6], uint16_t length, uint8_t subpara)
{
	uint8_t t1 = 0;
	uint16_t t, temp;

	do			//将数据升序排列
	{	
		t1 = 0;	
	  
		for(t = 0; t < length - 1; t++)
		{
			//将数据升序排列
			if((buffer[t][subpara] & 0x0fff) > (buffer[t+1][subpara] & 0x0fff))	//升序排列
			{
				temp = buffer[t+1][subpara] & 0x0fff;
				buffer[t+1][subpara] = buffer[t][subpara] & 0x0fff;
				buffer[t][subpara] = temp;
				t1 = 1; 
			} 		
		}
	}while(t1); 
	
	return ((buffer[(length >> 1) - 1][subpara] + buffer[(length >> 1)][subpara] + buffer[(length >> 1) - 2][subpara] + buffer[(length >> 1) + 1][subpara]) >> 2) & 0x0fff;		
}


/*-------------------------------------------------------------------------
	函数名：FindMax
	功  能：搜索最大值
	参  数：buffer 	待搜索数组
					length	数组长度
	返回值：无符号16位最大值
-------------------------------------------------------------------------*/
uint16_t FindMax(uint16_t * buffer, uint16_t length)
{
	uint16_t temp;
	uint8_t i;
	
	temp = buffer[0] & 0x0fff;
	
	for(i = 0; i < length; i++)
	{
		if((buffer[i] & 0x0fff) >= temp)
		{
			temp = buffer[i] & 0x0fff;
		}
	}
	
	return temp;
}


/*-------------------------------------------------------------------------
	函数名：CountKnobValue
	功  能：旋钮键值映射
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void CountKnobValue(void)
{
	int tempvalue = Knob_Avg;
	
	if(tempvalue >= MIN_KNOB_VALUE)
	{
		if(tempvalue > MAX_KNOB_VALUE)
		{
			tempvalue = MAX_KNOB_VALUE;
		}
		
		KnobValue = 2000 + (tempvalue - MIN_KNOB_VALUE) * 2000 / (MAX_KNOB_VALUE - MIN_KNOB_VALUE);
		SpeedFactor = (tempvalue - MIN_KNOB_VALUE) * 100 / (MAX_KNOB_VALUE - MIN_KNOB_VALUE);
	}
	else 
	{
		KnobValue = 2000;//停止
		SpeedFactor = 0;
	}
}


/*-------------------------------------------------------------------------
	函数名：count_angle_speed_right
	功  能：右摇杆舵角与速度映射
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void count_angle_speed_right(void)
{
	u8 debugbuffer[32];
	
	int speed;
	//double doublespeed;
	
	double angle;

	//右摇杆舵角映射--------------------------------------------------------------------------------------------------		
//	xx = ADC.ADC_2 - ADC.ADC_Middle_2;						//RJX		
	xx = ((int)(Right_Joystick_X_axis_Avg)) - DEFAULT_MIDDLE_VALUE;
	/*
	if(ADC.ADC_2 >= ADC.ADC_Middle_2)
	{
		xx = ADC.ADC_2 - ADC.ADC_Middle_2;//左右摇杆
	}
	else 
	{
		xx = -(ADC.ADC_Middle_2 - ADC.ADC_2);//左右摇杆
	}
	*/
	if(xx > 2000)
		xx = 2000;
	else if(xx < -2000)
		xx = -2000;

	//设置舵角
	if(xx > adcOffset || xx < -adcOffset){
		//angle = 1500 + xx/4;//舵机1000-2000, 其中1500中位
		//angle = GlobalVariable.RudderMiddle*10 + xx/4;//舵机1000-2000, 其中1500中位
		//angle = GlobalVariable.RudderMiddle*10 + (xx-adcOffset)/4;//舵机1000-2000, 其中1500中位
		angle = (xx - adcOffset) / 4;
		if(xx > adcOffset)
			angle = angle * angle / 500;
		else
			angle = -angle * angle / 500;
		if(local_para_conf[2] != USV_Boat_Type_ME300)
		{
			angle = local_para_conf[3] * 10 + angle;		//舵机1000-2000, 其中1500中位
		}
		else
		{
			angle = local_para_conf[3] * 10 - angle;		//舵机1000-2000, 其中1500中位
		}
	}
	else {
		angle = local_para_conf[3] * 10;
	}
	RightRudderCommand = angle;
	//右摇杆舵角映射==================================================================================================
	
	
	//右摇杆速度映射--------------------------------------------------------------------------------------------------
//	yy = ADC.ADC_3 - ADC.ADC_Middle_3;//前后摇杆
	yy = ((int)(Right_Joystick_Y_axis_Avg)) - DEFAULT_MIDDLE_VALUE;	
	
	if((yy > adcOffset || yy < -adcOffset)|| (xx > adcOffset || xx < -adcOffset)){
		//调整速度值,通过平方根函数,增加低速的加速度,减高速的加速度//2000的平方根为44.7; 如果用45则会产生比2000大的数
		//doublespeed = sqrt(sqrt(yy))*303.03;
		//doublespeed = sqrt(sqrt(sqrt(yy*yy+xx*xx)))*303;
		//doublespeed = sqrt(yy)*44;
		doublespeed = sqrt(sqrt(yy*yy+xx*xx))*44;

		//负整数赋值会变成很大的无符号整数,这里确保小于2000
		if(doublespeed > 2000)
		{
			doublespeed = 2000;
		}
		
		if(doublespeed >= adcOffset)
		{
			//doublespeed = (doublespeed - adcOffset) / 2;
				
			if(ControllerType == CONTROLLER_TYPE_2)
			{
				switch(local_para_conf[2])
				{
					case USV_Boat_Type_MC70://旋钮用作喷水
					case USV_Boat_Type_MC120://旋钮用作喷水
					case USV_Boat_Type_ME300://旋钮用作油门
						if(yy >= 0){
							//speed = 3000 + doublespeed/2;//50%以上才可以转动
							speed = 2000 + doublespeed;
						}
						else {
							//speed = 1000 - doublespeed/2;//50%以上才可以转动
							speed = 2000 - doublespeed;
						}
						break;
					default://20141114旋钮用作调速
						if(SpeedFactor > 0)
						{
							doublespeed = doublespeed * SpeedFactor / 100;
							if(yy >= 0){
								//speed = 3000 + doublespeed;//50%以上才可以转动
								speed = 2000 + doublespeed;
							}
							else {
								//speed = 1000 - doublespeed;//50%以上才可以转动
								speed = 2000 - doublespeed;
							}
						}
						else {
							speed = 2000;
						}
						break;
				}
			}
			else 
			{
				if(yy >= 0){
					//speed = 3000 + doublespeed/2;//50%以上才可以转动
					speed = 2000 + doublespeed;
				}
				else {
					//speed = 1000 - doublespeed/2;//50%以上才可以转动
					speed = 2000 - doublespeed;
				}
			}
		}
		else {
			speed = 2000;
		}
	}
	else {
			speed = 2000;
	}

	//设置速度值
	RightSpeedCommand = speed;//推进正0-2000,停2000,反2000-4000, 正转最大0, 反转最大4000
	//右摇杆速度映射==================================================================================================
}


/*-------------------------------------------------------------------------
	函数名：count_angle_speed_left
	功  能：左摇杆舵角与速度映射
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void count_angle_speed_left(void)
{
	u8 debugbuffer[32];
	
	int speed;
	//double doublespeed;
	
	double angle;

	//左摇杆舵角映射--------------------------------------------------------------------------------------------------	
//	xx = ADC.ADC_0 - ADC.ADC_Middle_0;
	xx = ((int)(Left_Joystick_X_axis_Avg)) - DEFAULT_MIDDLE_VALUE;
	/*
	if(ADC.ADC_0 >= ADC.ADC_Middle_0)
	{
		xx = ADC.ADC_0 - ADC.ADC_Middle_0;//左右摇杆
	}
	else 
	{
		xx = -(ADC.ADC_Middle_0 - ADC.ADC_0);//左右摇杆
	}
	*/
	
	if(xx>2000)
		xx = 2000;
	else if(xx<-2000)
		xx = -2000;

	//设置舵角
	if(xx > adcOffset || xx < -adcOffset){
		//angle = 1500 + xx/4;//舵机1000-2000, 其中1500中位
		//angle = GlobalVariable.RudderMiddle*10 + xx/4;//舵机1000-2000, 其中1500中位
		//angle = GlobalVariable.RudderMiddle*10 + (xx-adcOffset)/4;//舵机1000-2000, 其中1500中位
		angle = (xx-adcOffset)/4;
		if(xx > adcOffset)
			angle = angle*angle/500;
		else
			angle = -angle*angle/500;
		if(local_para_conf[2] != USV_Boat_Type_ME300)
		{
			angle = local_para_conf[3] * 10 + angle;					//舵机1000-2000, 其中1500中位
		}
		else
		{
			angle = local_para_conf[3] * 10 - angle;			//舵机1000-2000, 其中1500中位
		}
	}
	else {
		angle = local_para_conf[3] * 10;
	}
	LeftRudderCommand = angle;
	//左摇杆舵角映射==================================================================================================
	
	
	//左摇杆速度映射--------------------------------------------------------------------------------------------------
//	yy = ADC.ADC_1 - ADC.ADC_Middle_1;//前后摇杆
	yy = ((int)(Left_Joystick_Y_axis_Avg)) - DEFAULT_MIDDLE_VALUE;
	if((yy > adcOffset || yy < -adcOffset) || (xx > adcOffset || xx < -adcOffset)){
		//调整速度值,通过平方根函数,增加低速的加速度,减高速的加速度//2000的平方根为44.7; 如果用45则会产生比2000大的数
		//doublespeed = sqrt(sqrt(yy))*303.03;
		//doublespeed = sqrt(sqrt(sqrt(yy*yy+xx*xx)))*303;
		//doublespeed = sqrt(yy)*44;
		doublespeed = sqrt(sqrt(yy*yy+xx*xx))*44;

		//负整数赋值会变成很大的无符号整数,这里确保小于2000
		if(doublespeed>2000)
		{
			doublespeed = 2000;
		}
		
		if(doublespeed >= adcOffset)
		{
			//doublespeed = (doublespeed - adcOffset) / 2;
				
			if(ControllerType == CONTROLLER_TYPE_2)
			{
				switch(local_para_conf[2])
				{
					case USV_Boat_Type_MC70://旋钮用作喷水
					case USV_Boat_Type_MC120://旋钮用作喷水
					case USV_Boat_Type_ME300://旋钮用作油门
						if(yy >= 0){
							//speed = 3000 + doublespeed/2;//50%以上才可以转动
							speed = 2000 + doublespeed;
						}
						else {
							//speed = 1000 - doublespeed/2;//50%以上才可以转动
							speed = 2000 - doublespeed;
						}
						break;
					default://20141114旋钮用作调速
						if(SpeedFactor > 0)
						{
							doublespeed = doublespeed * SpeedFactor / 100;
							if(yy >= 0){
								//speed = 3000 + doublespeed;//50%以上才可以转动
								speed = 2000 + doublespeed;
							}
							else {
								//speed = 1000 - doublespeed;//50%以上才可以转动
								speed = 2000 - doublespeed;
							}
						}
						else {
							speed = 2000;
						}
						break;
				}
			}
			else 
			{
				if(yy >= 0){
					//speed = 3000 + doublespeed/2;//50%以上才可以转动
					speed = 2000 + doublespeed;
				}
				else {
					//speed = 1000 - doublespeed/2;//50%以上才可以转动
					speed = 2000 - doublespeed;
				}
			}
		}
		else {
			speed = 2000;
		}
	}
	else {
			speed = 2000;
	}
	//设置速度值
	LeftSpeedCommand = speed;			//推进正0-2000,停2000,反2000-4000, 正转最大0, 反转最大4000
	//左摇杆速度映射==================================================================================================
}
