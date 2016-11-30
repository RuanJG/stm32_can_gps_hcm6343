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

typedef struct _samplebox_valve_t {
	GPIO_TypeDef *Gpio;
	uint32_t pinNum;
	unsigned int  offvalue;
}samplebox_valve_t;

//���8��
samplebox_valve_t samplebox_valves[]={
	{ GPIOB, 12 , 0 },//valve1
	{ GPIOB, 13 , 0 },//valve2
	{ GPIOB, 14 , 0 },//valve3
	{ GPIOB, 15 , 0 },//valve4
};

#define SAMPLE_BOX_COUNT (sizeof(samplebox_valves)/sizeof(samplebox_valve_t))
	
unsigned char samplebox_valve_status=0;

systick_time_t report_timer;





/*
*     

led1: 			heart packget send reference led 
led2 led3 :		00:��һ�������ˣ�01���ڶ������򿪣�10����������11�����ĸ�

*
*/


#define Led1_On() GPIO_PinWrite(GPIOA, 11, 0)
#define Led1_Off() GPIO_PinWrite(GPIOA, 11, 1)
#define Led1_troggle()  GPIO_PinWrite(GPIOA, 11, GPIO_PinRead(GPIOA,11)==1? 0:1  )

#define Led2_On() GPIO_PinWrite(GPIOA, 11, 0)
#define Led2_Off() GPIO_PinWrite(GPIOA, 11, 1)

#define Led3_On() GPIO_PinWrite(GPIOA, 11, 0)
#define Led3_Off() GPIO_PinWrite(GPIOA, 11, 1)

void sample_box_led_gpio_init()
{
	GPIO_PortClock(GPIOA,true);
	GPIO_PinConfigure (GPIOA, 11, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
	
	
	Led1_On();
	Led2_On();
	Led3_On();
}

/*


������:
	�ȼ��ID��CAN1 ID�Ƿ����Լ������ִ��������еĲ����е� CAN1 ID����һ���ģ�ͨ��ID������
	Э�����֧��8��ƿ����Ŀǰ������ֻ��4��



���ط������������ǣ�

1�����÷���[ CAN1 ID - 1Byte] [ ID -1Byte ] [  ����״̬�뷧����ţ�1��2��3��4��-1 byte]
	����״̬����ţ� 
	bit7      bit6 bit5 bit4 bit3 bit2 bit1 bit0  
	1��0��			������ţ� 1-8 ��
	
	ID �� �������ֵ
	
	CAN1 ID�� �����е�CANID
	
	
2�����1������ط�״̬���������յ������������������ú����ϻ�Ӧһ��״̬��

	[ CAN1 ID - 1Byte]��[ ID -1Byte], [ ÿ�����Ŀ���״̬-1Byte]
	
	ID �� �������ֵ
	
	CAN1 ID�� �����е�CANID
	
	ÿ�����Ŀ���״̬��  bit0     bit1    bit2    bit3    bit4    bit5    bit6    bit7
					   1��ƿ    2��ƿ   3��ƿ    4��ƿ  5��ƿ   6��ƿ    7��ƿ   8��ƿ
					   
						1���� 0���ر�



*/



//get this sample box id form gpio key board
unsigned char sample_box_get_id()
{
	return 0;
}


void sample_box_gpio_init()
{
	int valve_count,index;
	
	valve_count = SAMPLE_BOX_COUNT;
	for( index = 0; index < valve_count ; index++)
	{
		GPIO_PortClock(samplebox_valves[index].Gpio,true);
		GPIO_PinConfigure (samplebox_valves[index].Gpio, samplebox_valves[index].pinNum, GPIO_OUT_PUSH_PULL, GPIO_MODE_OUT50MHZ);
		GPIO_PinWrite(samplebox_valves[index].Gpio, samplebox_valves[index].pinNum, samplebox_valves[index].offvalue);
	}
	
	sample_box_led_gpio_init();

	GPIO_AFConfigure (AFIO_SWJ_FULL_NO_NJTRST);
	
}


//num : �������
//onoff: 1->�򿪷��� 0->�رշ�
void samplebox_set_valve(unsigned int num, unsigned int onoff)
{
	int value,valve_count,index;
	
	valve_count = SAMPLE_BOX_COUNT;
	index = num -1;
	
	if( index < valve_count )
	{
		
		if( onoff == 0 )
		{
			value = samplebox_valves[index].offvalue == 0 ? 0:1;
		}else
		{
			value = samplebox_valves[index].offvalue == 0 ? 1:0;
		}
		GPIO_PinWrite(samplebox_valves[index].Gpio, samplebox_valves[index].pinNum, value);
	}
}


unsigned int samplebox_get_valve_status()
{
	int valve_count,index;
	unsigned int status=0, tmp;
	
	valve_count = SAMPLE_BOX_COUNT;
	
	for( index = 0; index < valve_count ; index++)
	{
		tmp = GPIO_PinRead(samplebox_valves[index].Gpio, samplebox_valves[index].pinNum);
		tmp = samplebox_valves[index].offvalue == 0 ? ( tmp==0?0:1 ):(tmp==1?0:1);
		status |= (tmp << index);
	}	
	return status;
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

void samplebox_send_status_packget( unsigned char status)
{
	unsigned char buffer[3];
	unsigned char data[16];
	int len;
	
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
	samplebox_valve_status = samplebox_get_valve_status();

	systick_time_start(&report_timer, 1000);
}


void samplebox_loop()
{
	if( check_systick_time(&report_timer) )
	{
		samplebox_valve_status = samplebox_get_valve_status();
		samplebox_send_status_packget(samplebox_valve_status);
		Led1_troggle();
	}
	
}
