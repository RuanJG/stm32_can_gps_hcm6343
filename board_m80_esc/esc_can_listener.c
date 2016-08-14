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



#define CAN1_LISTENER_LOST_CONNECT_MAX_MS 5000 // 5s
#define MAIN_CONTROLLER_CAN_ID 0x10
CanRxMsg RxMsg;
volatile uint32_t can1_lost_connect_counter = 0;









void Listen_Can1()
{
	unsigned short yaw, pump_pitch;
	if( Can1_Get_CanRxMsg(&RxMsg) ){
		switch ( RxMsg.Data[0] )
		{
			case 0x0:
			{
				if( RxMsg.Data[0] == 0x0 && RxMsg.DLC == 5){
					yaw = RxMsg.Data[1] | (RxMsg.Data[2]<<8);
					pump_pitch = RxMsg.Data[3] | (RxMsg.Data[4]<<8);
					
					if( yaw <= YAW_DEFAULE_MAX_ANGLE && yaw >= YAW_DEFAULE_MIN_ANGLE ){
						Esc_Yaw_Control_SetAngle(yaw);
					}
					
					if( pump_pitch >=0 && pump_pitch< 2000){
						Esc_Pump_Pitch_Forward();
					}else if( pump_pitch == 2000){
						Esc_Pump_Pitch_Middle();
					}else if( pump_pitch >2000 && pump_pitch <=4000){
						Esc_Pump_Pitch_Back();
					}
				}
				break;
			}
			
			// send dam cmd  
			/*
			#define DAM_CMD_FLASH_ON 3
			#define DAM_CMD_FLASH_OFF 2
			#define DAM_CMD_ON 1
			#define DAM_CMD_OFF 0
			*/
			//void Rtu_485_Dam_Cmd(unsigned char addr_id, unsigned char num_id, unsigned int cmd);
			
		}
		
		can1_lost_connect_counter=0;

	}
}

void Can1_Listener_Check_connect_event()
{
	//check connect
	if( can1_lost_connect_counter < CAN1_LISTENER_LOST_CONNECT_MAX_MS ){
		can1_lost_connect_counter += CAN1_LISTENER_REPORT_STATUS_MS;
	}else{
		logd("lost can1 connect\r\n");
	}
}

unsigned char reportBuffer[8];
void Can1_Listener_Report_Event()
{
	unsigned int tmp;
	

	
	//report oil current angle adc
	reportBuffer[0]= 0x1a;
	tmp = Esc_Yaw_Control_GetCurrentAdc();
	reportBuffer[1] = (tmp&0xff); reportBuffer[2] = ((tmp>>8) &0xff);
	tmp = Esc_Yaw_Control_GetAngleAdc();
	reportBuffer[3] = (tmp&0xff); reportBuffer[4] = ((tmp>>8) &0xff);
	tmp = Esc_Yaw_Control_GetOilMassAdc();
	reportBuffer[5] = (tmp&0xff); reportBuffer[6] = ((tmp>>8) &0xff);
	Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
	
	//// th11sb_head,th11sb_tail; 0xa 0xb
	if( th11sb_head.updated ){
		reportBuffer[0]= 0xa;
		reportBuffer[1]= (th11sb_head.wet& 0xff); reportBuffer[2]= ((th11sb_head.wet>>8)& 0xff);
		reportBuffer[3]= (th11sb_head.tempture & 0xff); reportBuffer[4]= ((th11sb_head.tempture >>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5);
	}
	if( th11sb_tail.updated ){
		reportBuffer[0]= 0xa;
		reportBuffer[1]= (th11sb_tail.wet& 0xff); reportBuffer[2]= ((th11sb_tail.wet>>8)& 0xff);
		reportBuffer[3]= (th11sb_tail.tempture & 0xff); reportBuffer[4]= ((th11sb_tail.tempture >>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5);
	}
	
	////  //dam4_02,dam4_04,dam4_05,dam16_08,dam4_09;
	//unsigned short status;
	//[0x2][statusL][statusH] [0x4][][]    [0x05][][] [0x08][][] [0x09][][] 
	if( dam4_02.updated ){
		reportBuffer[0]= 0x2;
		reportBuffer[1]= (dam4_02.status & 0xff); reportBuffer[2]= ((dam4_02.status>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
	}
	if( dam4_04.updated ){
		reportBuffer[0]= 0x4;
		reportBuffer[1]= (dam4_04.status & 0xff); reportBuffer[2]= ((dam4_04.status>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
	}
	if( dam4_05.updated ){
		reportBuffer[0]= 0x5;
		reportBuffer[1]= (dam4_05.status & 0xff); reportBuffer[2]= ((dam4_05.status>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
	}
	if( dam16_08.updated ){
		reportBuffer[0]= 0x8;
		reportBuffer[1]= (dam16_08.status & 0xff); reportBuffer[2]= ((dam16_08.status>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
	}
	if( dam4_09.updated ){
		reportBuffer[0]= 0x9;
		reportBuffer[1]= (dam4_09.status & 0xff); reportBuffer[2]= ((dam4_09.status>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
	}
	
	
	
	
	
  //powerAdc6_01,powerAdc6_06,powerAdc6_07;
	/*unsigned short adc[6];
	[0x10][adc0][adc1][adc2] [11][adc3][adc4][adc5]
	[0x60][adc0][adc1][adc2] [61][adc3][adc4][adc5]
	[0x70][adc0][adc1][adc2] [71][adc3][adc4][adc5]*/
	
	if( powerAdc6_01.updated ){
		reportBuffer[0]= 0x10;
		reportBuffer[1]= (powerAdc6_01.adc[0] & 0xff); reportBuffer[2]= ((powerAdc6_01.adc[0]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_01.adc[1] & 0xff); reportBuffer[4]= ((powerAdc6_01.adc[1]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_01.adc[2] & 0xff); reportBuffer[6]= ((powerAdc6_01.adc[2]>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		reportBuffer[0]= 0x11;
		reportBuffer[1]= (powerAdc6_01.adc[3] & 0xff); reportBuffer[2]= ((powerAdc6_01.adc[3]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_01.adc[4] & 0xff); reportBuffer[4]= ((powerAdc6_01.adc[4]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_01.adc[5] & 0xff); reportBuffer[6]= ((powerAdc6_01.adc[5]>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
	}
	if( powerAdc6_06.updated ){
		reportBuffer[0]= 0x60;
		reportBuffer[1]= (powerAdc6_06.adc[0] & 0xff); reportBuffer[2]= ((powerAdc6_06.adc[0]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_06.adc[1] & 0xff); reportBuffer[4]= ((powerAdc6_06.adc[1]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_06.adc[2] & 0xff); reportBuffer[6]= ((powerAdc6_06.adc[2]>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		reportBuffer[0]= 0x61;
		reportBuffer[1]= (powerAdc6_06.adc[3] & 0xff); reportBuffer[2]= ((powerAdc6_06.adc[3]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_06.adc[4] & 0xff); reportBuffer[4]= ((powerAdc6_06.adc[4]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_06.adc[5] & 0xff); reportBuffer[6]= ((powerAdc6_06.adc[5]>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
	}
	if( powerAdc6_07.updated ){
		reportBuffer[0]= 0x70;
		reportBuffer[1]= (powerAdc6_07.adc[0] & 0xff); reportBuffer[2]= ((powerAdc6_07.adc[0]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_07.adc[1] & 0xff); reportBuffer[4]= ((powerAdc6_07.adc[1]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_07.adc[2] & 0xff); reportBuffer[6]= ((powerAdc6_07.adc[2]>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		reportBuffer[0]= 0x71;
		reportBuffer[1]= (powerAdc6_07.adc[3] & 0xff); reportBuffer[2]= ((powerAdc6_07.adc[3]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_07.adc[4] & 0xff); reportBuffer[4]= ((powerAdc6_07.adc[4]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_07.adc[5] & 0xff); reportBuffer[6]= ((powerAdc6_07.adc[5]>>8)& 0xff);
		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
	}

  /*pgw636_03;
	int curren_speed;  //-8388608  ~  8388607
	[0x03][B0-7][B8-15][B16-23][B24-31] */
	
	if( pgw636_03.updated ){
		reportBuffer[0]= 0x03;
		reportBuffer[1]= (pgw636_03.curren_speed & 0xff);
		reportBuffer[2]= ((pgw636_03.curren_speed>>8) &0xff);
		reportBuffer[3]= ((pgw636_03.curren_speed>>16) &0xff);
		reportBuffer[4]= ((pgw636_03.curren_speed>>24) &0xff);

		Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5);
	}
	
	
}
int is_Can1_Lost_Connect()
{
	if( can1_lost_connect_counter >= CAN1_LISTENER_LOST_CONNECT_MAX_MS )
		return 1;
	return 0;
}
