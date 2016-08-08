#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "esc_box.h"
#include "stm32f10x_tim.h"	


// 推杆最大角度范围
#define YAW_DEFAULE_MIDDLE_ANGLE 90
#define YAW_DEFAULE_LEFT_MAX_ANGLE 135
#define YAW_DEFAULE_RIGHT_MAX_ANGLE 45

float yawExpectAngle = YAW_DEFAULE_MIDDLE_ANGLE;
float yawLeftMaxAngle = YAW_DEFAULE_LEFT_MAX_ANGLE;
float yawRightMaxAngle = YAW_DEFAULE_RIGHT_MAX_ANGLE;

// 推杆电流ADC检测最大范围
#define YAW_CURRENT_MAX_ADC_VALUE 4096
unsigned short yawCurrentAdcValue = 0;


void Esc_Yaw_Control_Configure()
{
	
	
}