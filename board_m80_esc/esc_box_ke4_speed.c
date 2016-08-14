#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "esc_box.h"

#define KE4_SPEED_MAX 1000
#define KE4_SPEED_MIN 500
#define KE4_CAN_ID 0x0
volatile uint16_t _ke4_speed = KE4_SPEED_MIN; //[500 - 0] or [500 - 1000]
unsigned char ke4_cmd[8] = {0x00 , 0x00, 0x03, 0x7D,  0x7F, 0x01,  0x00, 0x00 };
void Ke4_Speed_Control_Loop()
{
	if( _ke4_speed >= KE4_SPEED_MIN && _ke4_speed <= KE4_SPEED_MAX){
		ke4_cmd[0] = ((_ke4_speed>>8)&0xff);
		ke4_cmd[1] = (_ke4_speed&0xff);
		Can1_Send_Ext(KE4_CAN_ID, ke4_cmd, 8, CAN_ID_EXT, CAN_RTR_DATA);
	}
}
void Ke4_Set_Speed(uint16_t speed)
{
	if( speed >= KE4_SPEED_MIN && speed <= KE4_SPEED_MAX){
		_ke4_speed = speed;
	}
}