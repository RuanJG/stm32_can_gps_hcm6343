#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "esc_box.h"



/*
//*********** recive data type:
speed =[0-2000-4000] ;  yaw = [1000-1500-2000]
[id=0][yaw_L][yaw_H][speed_L][speedH]

//******** send data
1. report
	// th11sb_head,th11sb_tail; 0xa 0xb
	unsigned short wet ;  单位 0.1  
	unsigned short tempture;  单位 0.1 
	[id=0xa][wetL][wetH][temL][temH] //前舱
	[id=0xb][][][][] // 后舱

  //dam4_02,dam4_04,dam4_05,dam16_08,dam4_09;
	unsigned short status;
	[0x2][statusL][statusH] [0x4][][]    [0x05][][] [0x08][][] [0x09][][] 

  //powerAdc6_01,powerAdc6_06,powerAdc6_07;
	unsigned short adc[6];
	[0x10][adc0][adc1][adc2] [11][adc3][adc4][adc5]
	[0x60][adc0][adc1][adc2] [61][adc3][adc4][adc5]
	[0x70][adc0][adc1][adc2] [71][adc3][adc4][adc5]

  //pgw636_03;
	int curren_speed;  //-8388608  ~  8388607
	int max_speed;
	int min_speed;

	[0x03][B0-7][B8-15][B16-23][B24-31]

  // yaw control 
	u16 yawCurrentAdcValue yawAngleAdcValue oil_mass_adc_value
	[0x1a][currentL][currentH] [angleL][angleH] [oilL][oilH]
	//
*/



CanRxMsg RxMsg;
void Listen_Can1()
{
	if( Can1_Get_CanRxMsg(&RxMsg) ){
		
	}
	
	
	
}