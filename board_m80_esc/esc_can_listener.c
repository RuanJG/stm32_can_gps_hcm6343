#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "esc_box.h"



/* 主控发往电调
//
1. yaw pitch
speed =[0-2000-4000] ;  yaw = [1000-1500-2000]
[id=0][yaw_L][yaw_H][speed_L][speedH]

2. dam switch cmd
	[id = 1] [dam_addr_id] [num_id] [cmd 0-3] [msL] [msH] 
	
	dam_addr_id:  继电器板的485地址，应该是从 0x01 -> 0xB, 具体查表
	numid:  4路继电器板 [1，4]    16路继电器板 [1，16]
	cmd : DAM_CMD_FLASH_ON 3 ; DAM_CMD_FLASH_OFF 2 ; DAM_CMD_ON 1 ; DAM_CMD_OFF 0；  flash on 表示在原来关了的状态下，打开开关，廷时一段时间后，再关闭。
	ms: 0<ms < 20000ms ,只用在 flash on 或 flash off 命令中，用于设置廷时时间，单位是ms，最大不能设置 超过 20000




//******** 电调发往主控

1. report
	// th11sb_head,th11sb_tail; 0xa 0xb
	unsigned short wet ;  单位 0.1  
	unsigned short tempture;  单位 0.1 
	[id=0xa][wetL][wetH][temL][temH] //前舱
	[id=0xb][][][][] // 后舱

  //继电器 
	status 每一Bit代表一路开关的状态，如0x1表示第一路是打开的
	dam4_02 表示485地址是0x02的 4路继电器 
	dam4_02 ： 	[0x2][statusL][statusH] 
	dam4_04 ：	[0x4][][]    
	dam4_05 ：	[0x05][][] 
	dam16_08 ：	[0x08][][] 
	dam4_09 ：	[0x09][][] 

  //电压采集板： 每一个板有6路电压采样	unsigned short adc[6];
	powerAdc6_01 表示  486地址为0x01的 6路电压采样板 
	
	powerAdc6_01
	[0x10][adc0L][adc0H] [adc1_L][adc1_H] [adc2_L][adc2_H] 
	[0x11][adc3L][adc3H] [adc4_L][adc4_H] [adc5_L][adc5_H]
	powerAdc6_06
	[0x60][adc0][adc1][adc2] 
	[0x61][adc3][adc4][adc5]
	powerAdc6_07
	[0x70][adc0][adc1][adc2] 
	[0x71][adc3][adc4][adc5]

  //转速脉冲表;   pgw636_03   485地址为0x03 的 转速脉冲表;
	// 测量值数据范围是 -8388608  ~  8388607  4个字节


   pgw636_03
	[0x03][Byte0][Byte1][Byte2][Byte3]  ==> 0xByte3Byte2Byte1Byte0

  // 当前电调测到的电流（单位A），舵机角度（1000-1500-2000）， 油量（0-100）单位% 
	[0x1a][currentL][currentH] [angleL][angleH] [oilL][oilH]
	

*/



#define CAN1_LISTENER_LOST_CONNECT_MAX_MS 5000 // 5s
#define MAIN_CONTROLLER_CAN_ID 0x10
CanRxMsg RxMsg;
volatile uint32_t can1_lost_connect_counter = 0;



			/*
			#define DAM_CMD_FLASH_ON 3
			#define DAM_CMD_FLASH_OFF 2
			#define DAM_CMD_ON 1
			#define DAM_CMD_OFF 0
			*/
			//void Rtu_485_Dam_Cmd(unsigned char addr_id, unsigned char num_id, unsigned int cmd, unsigned int ms)
			//can : [id=1] [dam_addr_id 1-0xb] [num_id 1-4/1-16] [cmd 0-3] [msL] [msH] 0<ms < 20000ms
void handle_dam_can_cmd( CanRxMsg * msg)
{
	unsigned char dam_id,num_id,cmd;
	unsigned short ms;
	
	dam_id = msg->Data[1];
	num_id = msg->Data[2];
	cmd = msg->Data[3];
	ms = (msg->Data[4] | (msg->Data[5]<<8));
	
	if( ms < 20000 && cmd  <= DAM_CMD_FLASH_ON ){
		Rtu_485_Dam_Cmd(dam_id,num_id,cmd,ms);
	}
}



void Listen_Can1()
{
	unsigned short yaw, pump_pitch;
	if( Can1_Get_CanRxMsg(&RxMsg) ){
		switch ( RxMsg.Data[0] )
		{
			case 0x0:
			{
				// 
				if( RxMsg.Data[0] == 0x0 && RxMsg.DLC == 5){
					yaw = RxMsg.Data[1] | (RxMsg.Data[2]<<8);
					pump_pitch = RxMsg.Data[3] | (RxMsg.Data[4]<<8);
					
					if( yaw <= YAW_DEFAULE_MAX_ANGLE && yaw >= YAW_DEFAULE_MIN_ANGLE ){
						Esc_Yaw_Control_SetAngle(yaw);
					}
					
					// middle is 2000 dead range 200 10%
					if( pump_pitch >=0 && pump_pitch< 1800){
						Esc_Pump_Pitch_Back();
					}else if( pump_pitch >2200 && pump_pitch <=4000){
						Esc_Pump_Pitch_Forward();
					}else{
						Esc_Pump_Pitch_Middle();
					}
				}
				break;
			}
			
			// send dam cmd  

			case 1:
			{
				handle_dam_can_cmd( &RxMsg);
				break;
			}
			
			
			
			
			// volatile uint16_t ke4_speed = 500;
			
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
	tmp = Esc_Get_Current_Value();
	reportBuffer[1] = (tmp&0xff); reportBuffer[2] = ((tmp>>8) &0xff);
	tmp = Esc_Get_Angle_Value();
	reportBuffer[3] = (tmp&0xff); reportBuffer[4] = ((tmp>>8) &0xff);
	tmp = Esc_Get_Oil_Mass_Value();
	reportBuffer[5] = (tmp&0xff); reportBuffer[6] = ((tmp>>8) &0xff);
	//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
	Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
	
	//// th11sb_head,th11sb_tail; 0xa 0xb
	if( th11sb_head.updated ){
		reportBuffer[0]= 0xa;
		reportBuffer[1]= (th11sb_head.wet& 0xff); reportBuffer[2]= ((th11sb_head.wet>>8)& 0xff);
		reportBuffer[3]= (th11sb_head.tempture & 0xff); reportBuffer[4]= ((th11sb_head.tempture >>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5,CAN_ID_EXT, CAN_RTR_DATA);
	}
	if( th11sb_tail.updated ){
		reportBuffer[0]= 0xa;
		reportBuffer[1]= (th11sb_tail.wet& 0xff); reportBuffer[2]= ((th11sb_tail.wet>>8)& 0xff);
		reportBuffer[3]= (th11sb_tail.tempture & 0xff); reportBuffer[4]= ((th11sb_tail.tempture >>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5,CAN_ID_EXT, CAN_RTR_DATA);
	}
	
	////  //dam4_02,dam4_04,dam4_05,dam16_08,dam4_09;
	//unsigned short status;
	//[0x2][statusL][statusH] [0x4][][]    [0x05][][] [0x08][][] [0x09][][] 
	if( dam4_02.updated ){
		reportBuffer[0]= 0x2;
		reportBuffer[1]= (dam4_02.status & 0xff); reportBuffer[2]= ((dam4_02.status>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
	}
	if( dam4_04.updated ){
		reportBuffer[0]= 0x4;
		reportBuffer[1]= (dam4_04.status & 0xff); reportBuffer[2]= ((dam4_04.status>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
	}
	if( dam4_05.updated ){
		reportBuffer[0]= 0x5;
		reportBuffer[1]= (dam4_05.status & 0xff); reportBuffer[2]= ((dam4_05.status>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
	}
	if( dam16_08.updated ){
		reportBuffer[0]= 0x8;
		reportBuffer[1]= (dam16_08.status & 0xff); reportBuffer[2]= ((dam16_08.status>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
	}
	if( dam4_09.updated ){
		reportBuffer[0]= 0x9;
		reportBuffer[1]= (dam4_09.status & 0xff); reportBuffer[2]= ((dam4_09.status>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
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
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
		reportBuffer[0]= 0x11;
		reportBuffer[1]= (powerAdc6_01.adc[3] & 0xff); reportBuffer[2]= ((powerAdc6_01.adc[3]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_01.adc[4] & 0xff); reportBuffer[4]= ((powerAdc6_01.adc[4]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_01.adc[5] & 0xff); reportBuffer[6]= ((powerAdc6_01.adc[5]>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
	}
	if( powerAdc6_06.updated ){
		reportBuffer[0]= 0x60;
		reportBuffer[1]= (powerAdc6_06.adc[0] & 0xff); reportBuffer[2]= ((powerAdc6_06.adc[0]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_06.adc[1] & 0xff); reportBuffer[4]= ((powerAdc6_06.adc[1]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_06.adc[2] & 0xff); reportBuffer[6]= ((powerAdc6_06.adc[2]>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
		reportBuffer[0]= 0x61;
		reportBuffer[1]= (powerAdc6_06.adc[3] & 0xff); reportBuffer[2]= ((powerAdc6_06.adc[3]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_06.adc[4] & 0xff); reportBuffer[4]= ((powerAdc6_06.adc[4]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_06.adc[5] & 0xff); reportBuffer[6]= ((powerAdc6_06.adc[5]>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
	}
	if( powerAdc6_07.updated ){
		reportBuffer[0]= 0x70;
		reportBuffer[1]= (powerAdc6_07.adc[0] & 0xff); reportBuffer[2]= ((powerAdc6_07.adc[0]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_07.adc[1] & 0xff); reportBuffer[4]= ((powerAdc6_07.adc[1]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_07.adc[2] & 0xff); reportBuffer[6]= ((powerAdc6_07.adc[2]>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
		reportBuffer[0]= 0x71;
		reportBuffer[1]= (powerAdc6_07.adc[3] & 0xff); reportBuffer[2]= ((powerAdc6_07.adc[3]>>8)& 0xff);
		reportBuffer[3]= (powerAdc6_07.adc[4] & 0xff); reportBuffer[4]= ((powerAdc6_07.adc[4]>>8)& 0xff);
		reportBuffer[5]= (powerAdc6_07.adc[5] & 0xff); reportBuffer[6]= ((powerAdc6_07.adc[5]>>8)& 0xff);
		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
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

		//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5);
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,5,CAN_ID_EXT, CAN_RTR_DATA);
	}
	
	
}
int is_Can1_Lost_Connect()
{
	if( can1_lost_connect_counter >= CAN1_LISTENER_LOST_CONNECT_MAX_MS )
		return 1;
	return 0;
}
