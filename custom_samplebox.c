#include "custom_samplebox.h"
#include "custom.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f10x.h"
#include "GPIO_STM32F10x.h"
#include "RTE_Components.h"
#include "RTE_Device.h"
#include "systick.h"
#include "protocol.h"
#include "can1.h"



//motorA的位置 （0，1，2), 0:中位，1：valve1 ， 2： valve2
#define MOTORA_VALVE1_POS 1
#define MOTORA_VALVE2_POS 2
#define MOTORA_MIDDLE_POS 0
volatile unsigned char samplebox_motorA_pos = MOTORA_MIDDLE_POS;

#define MOTORB_VALVE3_POS 1
#define MOTORB_VALVE4_POS 2
#define MOTORB_MIDDLE_POS 0
volatile unsigned char samplebox_motorB_pos = MOTORB_MIDDLE_POS;


//是否初始化到中位
volatile unsigned char samplebox_valve_inited = 0;

systick_time_t report_timer;






/*


采样盒:
	先检查ID和CAN1 ID是否与自己相符，执行命令。所有的采样盒的 CAN1 ID都是一样的，通过ID来区分
	协议最大支持8个瓶，但目前采样箱只有4个



主控发过来的命令是：

1，设置阀：[ CAN1 ID - 1Byte] [ ID -1Byte ] [  开关状态与阀的序号（1，2，3，4）-1 byte]
	开关状态与序号： 
	bit7      bit6 bit5 bit4 bit3 bit2 bit1 bit0  
	1开0关			阀的序号（ 1-8 ）
	如果bit7 = 0  要关闭时， 序号为0 ，表示全关 
	
	ID ： 拨码键的值
	
	CAN1 ID： 采样盒的CANID
	
	
2，间隔1秒给主控发状态包，当接收到上面的设置命令后，设置后，马上回应一个状态包

	[ CAN1 ID - 1Byte]，[ ID -1Byte], [ 每个阀的开关状态-1Byte]
	
	ID ： 拨码键的值
	
	CAN1 ID： 采样盒的CANID
	
	每个阀的开关状态：  bit0     bit1    bit2    bit3    bit4    bit5    bit6    bit7
					   1号瓶    2号瓶   3号瓶    4号瓶  5号瓶   6号瓶    7号瓶   8号瓶
					   
						1：打开 0：关闭



*/


#define VALVE_MOTOR_TURN_MIDDLE_US   500000

void _motorA_turn_to_valve12(int num)
{
	if( num == 1 ){
		GPIO_PinWrite(GPIOB, 12, 1);
		GPIO_PinWrite(GPIOB, 13, 0);
	}else if( num == 2 ){
		GPIO_PinWrite(GPIOB, 12, 0);
		GPIO_PinWrite(GPIOB, 13, 1);
	}
}
void _motorA_stop()
{
	GPIO_PinWrite(GPIOB, 12, 0);
	GPIO_PinWrite(GPIOB, 13, 0);
}

void _motorB_turn_to_valve34(int num)
{
	if( num == 3 ){
		GPIO_PinWrite(GPIOB, 14, 1);
		GPIO_PinWrite(GPIOB, 15, 0);
	}else if( num == 4){
		GPIO_PinWrite(GPIOB, 14, 0);
		GPIO_PinWrite(GPIOB, 15, 1);
	}
}
void _motorB_stop()
{
	GPIO_PinWrite(GPIOB, 14, 0);
	GPIO_PinWrite(GPIOB, 15, 0);
}



void samplebox_turn_off_valve12()
{
	// let motor A turn to middle , valve 1 2 will close 
	
	int num;
	
	if( samplebox_motorA_pos == MOTORA_VALVE1_POS )
	{
		num = 2;
	}else if( samplebox_motorA_pos == MOTORA_VALVE2_POS ){
		num = 1;
	}else{
		return;
	}
	
	_motorA_turn_to_valve12(num);
	delay_us( VALVE_MOTOR_TURN_MIDDLE_US );
	_motorA_stop();
	samplebox_motorA_pos = MOTORA_MIDDLE_POS;	
	
}

void samplebox_turn_on_valve12(int num)
{
	unsigned int us;
	
	samplebox_turn_off_valve34();
	
	if( samplebox_motorA_pos == MOTORA_MIDDLE_POS )
	{
		us = VALVE_MOTOR_TURN_MIDDLE_US*2;  // to make sure turn to the end
		
	}else {
		
		if( (num == 1 && samplebox_motorA_pos == MOTORA_VALVE1_POS)  
			||  (num == 2 && samplebox_motorA_pos == MOTORA_VALVE2_POS) )
		{
			us = VALVE_MOTOR_TURN_MIDDLE_US/2;  //0
		}else{
			us = VALVE_MOTOR_TURN_MIDDLE_US*3;  //2
		}
		
	}
	
	_motorA_turn_to_valve12(num);
	delay_us( us );
	//? should be stop the motor 
	_motorA_stop();
	samplebox_motorA_pos = (num == 1)? MOTORA_VALVE1_POS:MOTORA_VALVE2_POS;
}


void samplebox_turn_off_valve34()
{
	// let motor B turn to middle , valve 1 2 will close 
	
	int num;
	
	if( samplebox_motorB_pos == MOTORB_VALVE3_POS )
	{
		num = 4;
	}else if( samplebox_motorB_pos == MOTORB_VALVE4_POS ){
		num = 3;
	}else{
		return;
	}
	
	_motorB_turn_to_valve34(num);
	delay_us( VALVE_MOTOR_TURN_MIDDLE_US );
	_motorB_stop();
	samplebox_motorB_pos = MOTORB_MIDDLE_POS;	
}

void samplebox_turn_on_valve34(int num)
{
	unsigned int us;
	
	samplebox_turn_off_valve12();
	
	if( samplebox_motorB_pos == MOTORB_MIDDLE_POS )
	{
		us = VALVE_MOTOR_TURN_MIDDLE_US*2;  // to make sure turn to the end
		
	}else {
		
		if( (num == 3 && samplebox_motorB_pos == MOTORB_VALVE3_POS)  
			||  (num == 4 && samplebox_motorB_pos == MOTORB_VALVE4_POS) )
		{
			us = VALVE_MOTOR_TURN_MIDDLE_US/2;  //0
		}else{
			us = VALVE_MOTOR_TURN_MIDDLE_US*3;  //2
		}
		
	}
	
	_motorB_turn_to_valve34(num);
	delay_us( us );
	//? should be stop the motor 
	_motorB_stop();
	samplebox_motorB_pos = (num == 3)? MOTORB_VALVE3_POS:MOTORB_VALVE4_POS;
}



void sample_box_valve_gpio_init()
{
	GPIO_PortClock(GPIOB,true);
	GPIO_PinConfigure (GPIOB, 12, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinConfigure (GPIOB, 13, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinConfigure (GPIOB, 14, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinConfigure (GPIOB, 15, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
}

//num : 阀的序号,  0 表示全关
//onoff: 1->打开阀； 0->关闭阀
void samplebox_set_valve(unsigned int num, unsigned int onoff)
{
	if( samplebox_valve_inited == 0 ) return;
	
	if( num == 1 || num ==2 )
	{
		if( onoff == 1 ) //turn on
			samplebox_turn_on_valve12(num);
		else
			samplebox_turn_off_valve12();
	}
	
	if( num == 3 || num ==4 )
	{
		if( onoff == 1 ) //turn on
			samplebox_turn_on_valve34(num);
		else
			samplebox_turn_off_valve34();
	}
	
	if( num == 0 && onoff == 0){
		samplebox_turn_off_valve34();
		samplebox_turn_off_valve12();
	}
	
	
	samplebox_send_status_packget();

}

unsigned char samplebox_get_valve_status()
{
	unsigned char status12;
	unsigned char status34;
	
	//status :   bit0:valve1   bit1:valve2   bit2:valve3	bit3:valve4
	
	if( samplebox_motorA_pos == MOTORA_MIDDLE_POS ){
		status12 = 0;
	}else{
		status12 = samplebox_motorA_pos==MOTORA_VALVE1_POS ? 1:2;
	}
	
	if( samplebox_motorB_pos == MOTORB_MIDDLE_POS ){
		status34 = 0;
	}else{
		status34 = samplebox_motorA_pos==MOTORB_VALVE3_POS ? 1:2;
	}
	
	return (status12|(status34<<2));
	
}



void samplebox_valve_init()
{
	_motorA_turn_to_valve12(1);
	_motorB_turn_to_valve34(3);
	delay_us( VALVE_MOTOR_TURN_MIDDLE_US * 4 );
	samplebox_motorA_pos = MOTORA_VALVE1_POS;
	samplebox_motorB_pos = MOTORB_VALVE3_POS;
	
	samplebox_turn_off_valve34();
	samplebox_turn_off_valve12();
	
	
	samplebox_valve_inited = 1;
}









/*
*     

led1: 			heart packget send reference led 
led2 led3 :		00:第一个阀打开了，01：第二个阀打开，10：第三个，11：第四个

*
*/


#define Led1_On() GPIO_PinWrite(GPIOA, 3, 0)
#define Led1_Off() GPIO_PinWrite(GPIOA, 3, 1)
#define Led1_troggle()  GPIO_PinWrite(GPIOA, 3, GPIO_PinRead(GPIOA,3)==1? 0:1  )

#define Led2_On() GPIO_PinWrite(GPIOA, 4, 0)
#define Led2_Off() GPIO_PinWrite(GPIOA, 4, 1)

#define Led3_On() GPIO_PinWrite(GPIOA, 5, 0)
#define Led3_Off() GPIO_PinWrite(GPIOA, 5, 1)

void sample_box_led_gpio_init()
{
	GPIO_PortClock(GPIOA,true);
	GPIO_PinConfigure (GPIOA, 3, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinConfigure (GPIOA, 4, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	GPIO_PinConfigure (GPIOA, 5, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	
	Led1_On();
	Led2_On();
	Led3_On();
}






/*

****************  ID  mode1 is bit0 , mode2 is bit1
****************  ID value is [0,3]
*/

void sample_box_id_gpio_init()
{
	GPIO_PortClock(GPIOB,true);
	GPIO_PinConfigure (GPIOB, 6, GPIO_IN_FLOATING, GPIO_MODE_INPUT);//mode1
	GPIO_PinConfigure (GPIOB, 7, GPIO_IN_FLOATING, GPIO_MODE_INPUT);//mode2
}
//get this sample box id form gpio key board
unsigned char sample_box_get_id()
{
	unsigned char  id = GPIO_PinRead(GPIOB, 6) | (GPIO_PinRead(GPIOB, 7)<<1);
	return 0;
}


















void sample_box_gpio_init()
{
	int valve_count,index;
	
	sample_box_valve_gpio_init();
	sample_box_led_gpio_init();
	sample_box_id_gpio_init();
	
	GPIO_AFConfigure (AFIO_SWJ_FULL_NO_NJTRST);
	
}



void samplebox_excute_cmd(uint8_t *cmd, uint32_t size)
{
	unsigned char onoff;
	unsigned char num;
	
	if( cmd[0] != CUSTOM_CAN1_ID || cmd[1] != sample_box_get_id() )
		return ;
	
	onoff = cmd[2]>>7;
	num = cmd[2]&0x7f;
	
	
	samplebox_set_valve(num,onoff);
	
}



void samplebox_send_status_packget()
{
	unsigned char buffer[3];
	unsigned char data[16];
	int len;
	unsigned char status = samplebox_get_valve_status();
	
	buffer[0] = CUSTOM_CAN1_ID;
	buffer[1] = sample_box_get_id();
	buffer[2] = status;
	
	len = Pack_Data(buffer, 3, data, 16);
	if( len > 0 )
	{
		CAN1_Send(MAIN_CONTROLLER_CAN_ID,data,len);
	}
}




void samplebox_init()
{
	sample_box_gpio_init();
	samplebox_valve_init();

	systick_time_start(&report_timer, 1000);
}


void samplebox_loop()
{
	if( check_systick_time(&report_timer) )
	{
		samplebox_send_status_packget();
		Led1_troggle();
	}
	
}
