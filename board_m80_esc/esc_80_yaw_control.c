#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "esc_box.h"
#include "stm32f10x_tim.h"	


//前提是，adc 与 角度 的关系是线性的
// arm9 -> angle -> adc_value -> compare and change yaw 
// 电流决定角度的真实范围大小
// 为了防止硬件改动，将向左向右变换为adc value 的大小变化 
// need to sure two thing:
// 1,角度adc变化时，对应主控角度[1000,2000]的关系  YAW_ANGLE_ADD_ANGLE_ADC_ADD
// 2,要让角度adc变化，推杆的控制方向    YAW_FORWARD_ANGLE_ADC_ADD

#define YAW_FORWARD_ANGLE_ADC_ADD 1
#define YAW_ANGLE_ADD_ANGLE_ADC_ADD 1 //angle is from [1000,2000] left->right

#define _YAW_ANGLE_DIFF_RANGE 10 // 当前角度ADC与期望的角度的ADC 之间的差值区间
#define _YAW_TIMER_CHECK_MS 20 // each 20ms check angle task
#define _YAW_ADC_UPDATE_CHECK_MIN_HZ 5 // 每次 执行上面的 angle check task 时，adc 数据的最少更新次数, 可以在ADC配置里看更新频率



// 推杆最大角度ADC值范围  TODO config
#define YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE 2048
#define YAW_DEFAULT_ANGLE_MAX_ADC_VALUE (2048+512)
#define YAW_DEFAULT_ANGLE_MIN_ADC_VALUE (2048-512)
#define YAW_DEFAULT_ANGLE_FAILSAFE_MAX_ADC_VALUE (2048+1024)
#define YAW_DEFAULT_ANGLE_FAILSAFE_MIN_ADC_VALUE (2048-1024)
u16 yawAngleLowPartAdcValueR = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE - YAW_DEFAULT_ANGLE_MIN_ADC_VALUE;
u16 yawAngleHightPartAdcValueR = YAW_DEFAULT_ANGLE_MAX_ADC_VALUE - YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE ;

// 当前的角度adc 值
volatile u16 yawAngleAdcValue = 0;

// 主控发过来的,要求的 角度
#define YAW_DEFAULE_MIDDLE_ANGLE  1500 // 1000 - 1500 - 2000
#define YAW_DEFAULE_MAX_ANGLE 2000 // most right
#define YAW_DEFAULE_MIN_ANGLE 1000 // most Left
float yawExpectAngle = YAW_DEFAULE_MIDDLE_ANGLE;
u16 yawExpectAngleAdcValue = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE;




// 推杆电流ADC值最大范围
#define YAW_CURRENT_MAX_ADC_VALUE 3072 //4096 
// 当前电流adc 值
volatile u16 yawCurrentAdcValue = 0;


// 初始化时，自检
int yawControlInited = 0; 
systick_time_t _yaw_t;


// 出现 failsafe 时的时间
uint32_t _failsafe_time_ms = 0;












#define _yaw_control_shutdown() 	GPIO_ResetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN)
#define _yaw_control_poweron()  GPIO_SetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN);
void _yaw_control_forward()
{
	_yaw_control_shutdown();
	GPIO_SetBits(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN);
	GPIO_ResetBits(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN);
	_yaw_control_poweron();
}
void _yaw_control_back()
{
	_yaw_control_shutdown();
	GPIO_ResetBits(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN);
	GPIO_SetBits(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN);
	_yaw_control_poweron();
}

#if YAW_FORWARD_ANGLE_ADC_ADD
#define  _yaw_control_reduce_angle_adc_value() _yaw_control_back()
#define  _yaw_control_add_angle_adc_value() _yaw_control_forward()
#else
#define  _yaw_control_reduce_angle_adc_value() _yaw_control_forward()
#define  _yaw_control_add_angle_adc_value() _yaw_control_back()
#endif
// 获取当前运动方向状态
#define _YAW_ANGLE_ADC_ADD_DIRECTION 1
#define _YAW_ANGLE_ADC_REDUCE_DIRECTION 2
#define _YAW_ANGLE_ADC_STOP_DIRECTION 3 //may be stop and gpio not init
int _yaw_control_get_direction()
{
	char ctrl3,ctrl4,status,pwmb;
	ctrl3 = GPIO_ReadOutputDataBit(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN) ;
	ctrl4 = GPIO_ReadOutputDataBit(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN) ;
	
	
	status = ctrl4<<1 | ctrl3;
	
#if YAW_FORWARD_ANGLE_ADC_ADD
	if( status == 1) return _YAW_ANGLE_ADC_ADD_DIRECTION; // forwared -> adc++ drirect
	if( status == 2) return _YAW_ANGLE_ADC_REDUCE_DIRECTION; // back -> adc-- drirect
#else
	if( status == 1) return _YAW_ANGLE_ADC_REDUCE_DIRECTION; // forwared -> adc-- drirect
	if( status == 2) return _YAW_ANGLE_ADC_ADD_DIRECTION; // back -> adc++ drirect
#endif
	
	if( status == 0 || status == 0x3  || pwmb == 0) return _YAW_ANGLE_ADC_STOP_DIRECTION;
	return _YAW_ANGLE_ADC_STOP_DIRECTION;
}

int _is_yaw_control_stoped()
{
	char ctrl3,ctrl4,status,pwmb;
	ctrl3 = GPIO_ReadOutputDataBit(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN) ;
	ctrl4 = GPIO_ReadOutputDataBit(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN) ;
	pwmb = GPIO_ReadOutputDataBit(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN);
	
	status = ctrl4<<1 | ctrl3;
	if( status == 0 || status == 0x3  || pwmb == 0) return 1;
	
	return 0;
}






u16 _yaw_control_angle_to_adc(uint16_t angle)
{
	uint32_t res;

	res = YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE ;
	if( angle == YAW_DEFAULE_MIDDLE_ANGLE ) return res;
	
#if YAW_ANGLE_ADD_ANGLE_ADC_ADD 
	//forward push -> turn right -> angle ++  ==  angle_adc++
	if( angle > YAW_DEFAULE_MIDDLE_ANGLE ){
		res += yawAngleHightPartAdcValueR*(angle-YAW_DEFAULE_MIDDLE_ANGLE)/(YAW_DEFAULE_MAX_ANGLE-YAW_DEFAULE_MIDDLE_ANGLE);
	}else{
		res -= yawAngleLowPartAdcValueR*(YAW_DEFAULE_MIDDLE_ANGLE - angle)/(YAW_DEFAULE_MIDDLE_ANGLE-YAW_DEFAULE_MIN_ANGLE);
	}
#else
		//forward push -> turn right -> angle ++  ==  angle_adc--
	if( angle > YAW_DEFAULE_MIDDLE_ANGLE ){
		res -= yawAngleLowPartAdcValueR*(angle-YAW_DEFAULE_MIDDLE_ANGLE)/(YAW_DEFAULE_MAX_ANGLE-YAW_DEFAULE_MIDDLE_ANGLE);
	}else{
		res += yawAngleHightPartAdcValueR*(YAW_DEFAULE_MIDDLE_ANGLE - angle)/(YAW_DEFAULE_MIDDLE_ANGLE-YAW_DEFAULE_MIN_ANGLE);
	}
#endif
	logd_uint("new angle:",angle);
	logd_uint("to adc:",res);
	return res;
}

int is_reach_expect_value(uint16_t angle_adc_value)
{
	int diff ;
	
	diff = (yawAngleAdcValue > angle_adc_value) ? (yawAngleAdcValue - angle_adc_value) : (angle_adc_value - yawAngleAdcValue);
	
	if( diff <= _YAW_ANGLE_DIFF_RANGE )
	{
		return 1;
	}
	return 0;
}



/*
// 当前方向状态，向左或右转动中，或停止
volatile uint8_t _yaw_current_direction = _YAW_STOP; 
void _yaw_control_change_direction( int direction )
{
	if( direction == _YAW_TURN_LEFT){
		//_yaw_control_forward();
		_yaw_control_back();
		_yaw_current_direction = _YAW_TURN_LEFT;
	}else if( direction == _YAW_TURN_RIGHT )
	{
		_yaw_control_forward();
		//_yaw_control_back();
		_yaw_current_direction = _YAW_TURN_RIGHT;
	}else if( direction == _YAW_STOP ){
		_yaw_control_shutdown();
		_yaw_current_direction = _YAW_STOP;
	}
}
*/









//failsafe 当电流与角度超出范围时，要做保护, ADC 没有更新时，也要做停止操作
volatile unsigned char failsafe_AngleOverFlow = 0;
volatile unsigned char failsafe_CurrentOverFlow = 0;
volatile unsigned char failsafe_AdcUpdateError = 0;

int _is_failsafe()
{
	if( failsafe_AngleOverFlow || failsafe_CurrentOverFlow || failsafe_AdcUpdateError )
		return 1;
	
	return 0;
}

void yaw_control_failsafe()
{
	_yaw_control_shutdown();
	_failsafe_time_ms = get_system_ms();
}

//call by adc dma irq or loop in main for listen the current and angle 
volatile uint32_t _esc_yaw_check_adc_hz = 0;
int _check_current_failsafe()
{
	if( yawCurrentAdcValue >= YAW_CURRENT_MAX_ADC_VALUE )
		return 1;
	return 0;
}
int _check_angle_failsafe()
{
	if( yawAngleAdcValue >= YAW_DEFAULT_ANGLE_FAILSAFE_MAX_ADC_VALUE  || yawAngleAdcValue <= YAW_DEFAULT_ANGLE_FAILSAFE_MIN_ADC_VALUE)
		return 1;
	return 0;
}
void _esc_yaw_adc_update_event()
{
	// update adc and check error
	yawCurrentAdcValue = Get_ISA_Adc_value();
	if( 1 == _check_current_failsafe() )
	{
		failsafe_CurrentOverFlow = 1; 
		yaw_control_failsafe();
	}
	
	yawAngleAdcValue = Get_PUMP_ANGLE_Adc_value();
	if( 1 == _check_angle_failsafe() )
	{
		failsafe_AngleOverFlow = 1;
		yaw_control_failsafe();
	}
	
	_esc_yaw_check_adc_hz++;
	
	// check angle 
	if( yawControlInited == 1 && 0 == _is_failsafe() ){
		if( 1 == is_reach_expect_value(yawExpectAngleAdcValue) )
		{
			_yaw_control_shutdown();
		}
	}		
}






int _yaw_control_move_to_expect_adc(uint16_t angle_adc_value)
{
	// -1 error 0 running 1 ok
	
	int direct ;
	
	//检查是否己经达到
	if( 1 == is_reach_expect_value(angle_adc_value) )
	{
		_yaw_control_shutdown();
		return 1;
	}
	
	// no reach , so check and do sport
	
	//check failsafe
	//if( 1 == failsafe_AdcUpdateError ) return -1;
	if( 1 == _is_failsafe() )
			return -1;

	direct  = _yaw_control_get_direction();
	if( yawExpectAngleAdcValue > yawAngleAdcValue ){
		/*
		//check failsafe
		if( failsafe_AngleOverFlow == 1 || failsafe_CurrentOverFlow == 1 ){
			if( _YAW_ANGLE_ADC_ADD_DIRECTION  ==  direct ) // last move is add direction
				return -1; //stop do nothing
		}*/
		//if is moving , should stop and change direction , or do nothing
		if( 0 == _is_yaw_control_stoped() ){
			if( _YAW_ANGLE_ADC_ADD_DIRECTION  ==  direct ){ // last move is add direction
				return 0 ; //stop do nothing
			}else{
				_yaw_control_shutdown();
				delay_us(2000);
			}
		}
		//start moving
		_yaw_control_add_angle_adc_value();
	}else{
		/*
		if( failsafe_AngleOverFlow == 1 || failsafe_CurrentOverFlow == 1 ){
			if( _YAW_ANGLE_ADC_REDUCE_DIRECTION  ==  _yaw_control_get_direction() ) // last move is add direction
				return 0 ; //stop do nothing
		}*/
		//if is moving , should stop and change direction , or do nothing
		if( 0 == _is_yaw_control_stoped() ){
			if( _YAW_ANGLE_ADC_REDUCE_DIRECTION  ==  direct ){ // last move is add direction
				return 0 ; //stop do nothing
			}else{
				_yaw_control_shutdown();
				delay_us(2000);
			}
		}
		_yaw_control_reduce_angle_adc_value();
	}
}




void Esc_Yaw_Control_SetAngle(uint16_t  angle)
{
	//上层设置期望的角度
	if( angle <= YAW_DEFAULE_MAX_ANGLE || angle >= YAW_DEFAULE_MIN_ANGLE )
	{
		yawExpectAngle = angle;
		yawExpectAngleAdcValue = _yaw_control_angle_to_adc(angle);
	}
}



int _yaw_control_initFunc()
{
	static char step = 0;
	int res;
	
	switch(step){
		case 0:
		{
			//goto middle
			res = _yaw_control_move_to_expect_adc( YAW_DEFAULT_ANGLE_MIDDLE_ADC_VALUE );
			if( 1 == res ){
				step++;
			}else {
				logd("goto mid error\r\n");
				return res;
			}				
			break;
		}
		case 1:
		{
			//goto min side
			if( 1 == _yaw_control_move_to_expect_adc( YAW_DEFAULT_ANGLE_MIN_ADC_VALUE )){
				step++;
			}else {
				logd("goto max error\r\n");
				return res;
			}	
			break;
		}
		case 2:
		{
			//goto max side
			if( 1 == _yaw_control_move_to_expect_adc( YAW_DEFAULT_ANGLE_MAX_ADC_VALUE )){
				_yaw_control_move_to_expect_adc(yawExpectAngleAdcValue);
				return 1;
			}else {
				logd("goto min error\r\n");
				return res;
			}	
			break;
		}
	}
	return 0;
}


void _try_go_back_in_failsafe()
{
		int res , direct;
		uint32_t now_time_ms;
				if( 0 == _is_yaw_control_stoped() ){
					//moving, mean that , we have done failsafe opera
					if( 0== _check_current_failsafe() && 0 == _check_angle_failsafe() ){
						failsafe_CurrentOverFlow = 0;
						failsafe_AngleOverFlow = 0;
						_yaw_control_shutdown();
					}
				}else{
					// failsafe , so we have to do failsafe opera
					now_time_ms = get_system_ms();
					if( (now_time_ms - _failsafe_time_ms) < 500  || 1 == _check_current_failsafe()) 
					{// try to moving after 0.5s and current is under safe range
						return ;
					}
					failsafe_CurrentOverFlow = 0; // if just current overflow , it will not into _try_go_back_in_failsafe next time
					
					if( failsafe_AngleOverFlow )
					{
						direct  = _yaw_control_get_direction();
						if( _YAW_ANGLE_ADC_ADD_DIRECTION  ==  direct ) // last move is add direction
							_yaw_control_reduce_angle_adc_value();
						else if( _YAW_ANGLE_ADC_REDUCE_DIRECTION  ==  direct ){
							_yaw_control_add_angle_adc_value();
						}else{
							logd("unknow status\r\n");
						}
					}

				}
}
void Esc_Yaw_Control_Event()
{
	int res , direct;
	
	
	if ( check_systick_time(&_yaw_t) )
	{
		// check adc update
		if( _esc_yaw_check_adc_hz < _YAW_ADC_UPDATE_CHECK_MIN_HZ ) //2ms once update , 20 ms should be 10
		{
			failsafe_AdcUpdateError = 1;
			yaw_control_failsafe();
			_esc_yaw_check_adc_hz = 0;
			logd("adc update error\r\n");
			return ; // stop , do nothing
		}else{
			failsafe_AdcUpdateError = 0;
			_esc_yaw_check_adc_hz = 0;
		}
		
		//init 
		if( yawControlInited == 0 )
		{
			res = _yaw_control_initFunc();
			if( 1 == res )
				yawControlInited = 1;
			else if( res == -1 ){
				logd("init error\r\n");
			}
		}else{
			//check failsafe
			if( failsafe_AngleOverFlow == 1 || failsafe_CurrentOverFlow == 1 ){
				//try to return to normal status
				_try_go_back_in_failsafe();
			}else{
				//goto  expect angle
				_yaw_control_move_to_expect_adc(yawExpectAngleAdcValue);
			}
		}
	}
}


void Esc_Yaw_Control_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_CTRL3_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_CTRL3_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_CTRL4_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_CTRL4_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = H_BRIDGE_A_PWMB_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(H_BRIDGE_A_PWMB_GPIO_BANK, &GPIO_InitStructure);
	
	//先关闭，等ADC稳定后再开始自检
	_yaw_control_shutdown();
	GPIO_ResetBits(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN);
	GPIO_ResetBits(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN);

	Esc_ADC_Configuration (_esc_yaw_adc_update_event);
	
	systick_time_start(&_yaw_t,_YAW_TIMER_CHECK_MS); //每10ms运行一次检查
}