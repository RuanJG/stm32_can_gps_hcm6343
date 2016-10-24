/*-------------------------------------------------------------------------
工程名称：遥控器平台ADC驱动程序文档
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150526 赵铭章    5.0.0		新建立
																		PC0 CH10	Bat_ADC 	电池电压采样
																		PC1 CH11	RCKLY_ADC 	左摇杆X轴
																		PC2 CH12	Wheel_ADC		电位器旋钮
																		PC3 CH13	RCKLX_ADC		左摇杆Y轴
																		PC4	CH14	RCKRY_ADC		右摇杆X轴
																		PC5	CH15	RCKRX_ADC		右摇杆Y轴
					
					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SETADC_H
#define __SETADC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//#define ADC1_DR_Address    ((u32)0x4001204C)
#define ADC_BUFFER_SIZE 50	//300, 200, 100


/* Public variables ---------------------------------------------------------*/
extern uint16_t ADCValue[ADC_BUFFER_SIZE][6];
extern uint16_t Battery_Power_Avg;									//电池电压均值
extern uint16_t Left_Joystick_X_axis_Avg;						//左侧摇杆 X 轴采样均值
extern uint16_t Left_Joystick_Y_axis_Avg;						//左侧摇杆 Y 轴采样均值
extern uint16_t Right_Joystick_X_axis_Avg;					//右侧摇杆 X 轴采样均值
extern uint16_t Right_Joystick_Y_axis_Avg;					//右侧摇杆 Y 轴采样均值
extern uint16_t Knob_Avg;														//电位器旋钮均值
extern int8_t Battery_Power_Percent;							//电池电量换算百分比形式


/*-------------------------------------------------------------------------
	函数名：ADC1_CH6_DMA_Config
	功  能：初始化各ADC口
						PC0 CH10	Bat_ADC 	电池电压采样
						PC1 CH11	RCKLY_ADC 	左摇杆X轴
						PC2 CH12	Wheel_ADC		电位器旋钮
						PC3 CH13	RCKLX_ADC		左摇杆Y轴
						PC4	CH14	RCKRY_ADC		右摇杆X轴
						PC5	CH15	RCKRX_ADC		右摇杆Y轴
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ADC1_CH6_DMA_Config(void);


#endif











