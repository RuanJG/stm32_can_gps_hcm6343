#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "esc_box.h"



/* 主控发往电调
//
1. 翻斗与转向的控制 
	//speed < 1500 时翻斗向下翻，动力向后推，speed =[1500,2500] 翻斗在中间，动力向下，speed=(2500,4000]时，翻斗向上，动力向前
	//yaw 在1500时在中间，小于1500转左，大于1500转右
	
	speed =[0-2000-4000] ;  yaw = [1000-1500-2000]
	[id=0][yaw_L][yaw_H][speed_L][speedH]



2. 继电器开关控制
	[id = 1] [dam_addr_id] [num_id] [cmd 0-3] [msL] [msH] 
	
	dam_addr_id:  继电器板的485地址，应该是从 0x01 -> 0xB, 具体查表
	numid:  4路继电器板 [1，4]    16路继电器板 [1，16]， 具体每一路的功能，看下面描述
	cmd : DAM_CMD_FLASH_ON 3 ; DAM_CMD_FLASH_OFF 2 ; DAM_CMD_ON 1 ; DAM_CMD_OFF 0；  flash on 表示在原来关了的状态下，打开开关，廷时一段时间后，再关闭。
	ms: 100<ms < 20000ms ,只用在 flash on 或 flash off 命令中，用于设置廷时时间，单位是ms，最大不能设置 超过 20000




//******** 电调发往主控

1. report
	// th11sb_head,th11sb_tail; 0xa 0xb
	unsigned short wet ;  单位 0.1  如：wet = 501  501*0.1 = 50.1 
	unsigned short tempture;  单位 0.1   tempture =381  381*0.1=38.1摄氏度
	
	can frame: [id=0xa][wetL][wetH][temL][temH] //前舱
	can frame: [id=0xb][][][][] // 后舱

  //继电器 
	status 每一Bit代表一路开关的状态，如0x1表示第一路是打开的
	dam4_02 表示485地址是0x02的 4路继电器 
	
	
	dam4_02 ： 	发电机与发动机
		//该DAM板上的输入脚接了两路检测 input2:发动机低油压报警 input3:发动机高水温报警
		//第一路开关：发电机的关闭，闪连1秒，关闭发电机
		//第二路开关：发电机的上电，闪连3秒，开启发电机
		//第三路开关：发动机的打火开关，闪连500ms，发动机打火
		//第四路开关：发动机的开关，闭合开关，发动机上电，断开开关，发动机关闭
		[0x2][statusL][statusH][input1][input2][input3][input4]   
	
	
	dam4_04 ：	侧推箱 上电控制，
		注：在同一时间内，只能打开右控制或左控制，如要打开右控制，要先关左控制
		注：先进行充电，充电10多秒后，才能上电
		//第一路开关：侧推箱上电，闭合开关上电
		//第二路开关：侧推箱充电，闭合充电
		//第三路开关：侧推箱左控制开启与关闭，闭合开关使能
		//第四路开关：侧推箱右控制开启与关闭，闭合开关使能
		[0x4][statusL][statusH] 
		
	dam4_05 ：	侧推箱 控制方向  
		注：某一时间内，只能开启前进或后退，如，在开启前进方向时，要关闭后退方向
		//第一路开关：侧推箱左控制的前进方向，闭合开关使能
		//第二路开关：侧推箱左控制的后退方向，闭合开关使能
		//第三路开关：侧推箱右控制的前进方向，闭合开关使能
		//第四路开关：侧推箱右控制的后退方向，闭合开关使能
		[0x05][statusL][statusH]
		
	
	dam16_08 ：	DC配电箱 
		//第一路开关：天线支架UP  , 闭合开关使能
		//第二路开关：天线支架DOWN， 闭合开关使能
		//第三路开关：
		//第六路开关：激光雷达，闭合开关使能
		//第七路开关：转向助力泵电源
		//第八路开关：KE4
		//第九路开关：翻斗
		//第十路开关：前舱风机
		//第十一路开关：后舱风机
		//第12路开关：舱底泵
		//第13路开关；4G雷达
		//第14路开关：摄像头
		//第15路开关：交换机
		//第16路开关：主控，GPS
	[0x08][statusL][statusH] 
	
	
	dam4_09 ：	
		//第一路：前视声纳
		//第二路：多波束
		//第三路： LET
		//第四路：工控机
	[0x09][statusL][statusH]
	





  //电压采集板： 每一个板有6路电压采样	unsigned short adc[6];
	powerAdc6_01 表示  486地址为0x01的 6路电压采样板 
	
	powerAdc6_01：AC配电箱  adc[0]=DC_12V  adc[4]= AC_I_VOUT  adc[5] = AC_V_VOUT
	第一个can包：[0x10][adc0L][adc0H] [adc1_L][adc1_H] [adc2_L][adc2_H] 
	第二个can包：[0x11][adc3L][adc3H] [adc4_L][adc4_H] [adc5_L][adc5_H]
	
	powerAdc6_06 ：侧推箱，adc[0]=12V_IN adc[1]=12V_OUT  adc[4]=I_Right adc[5]=I_LEFT
	第一个can包：[0x60][adc0L][adc0H] [adc1_L][adc1_H] [adc2_L][adc2_H]
	第二个can包：[0x61][adc3L][adc3H] [adc4_L][adc4_H] [adc5_L][adc5_H]
	
	
	powerAdc6_07： DC配电   adc[0]= BAT_12V adc[1]=12V_EXT adc[2]=24V_EXT adc[3]=24V_BAT adc[4]=I_12V adc[5]=I_24V
	第一个can包：[0x70][adc0L][adc0H] [adc1_L][adc1_H] [adc2_L][adc2_H]
	第二个can包：[0x71][adc3L][adc3H] [adc4_L][adc4_H] [adc5_L][adc5_H]

  

   pgw636_03：
		//转速脉冲表;   pgw636_03   485地址为0x03 的 转速脉冲表;
		// 测量值数据范围是 -8388608  ~  8388607  4个字节
		//转速表 转速与读到的数值的关系经测量才能得到结果
	[0x03][Byte0][Byte1][Byte2][Byte3]  ==> int = 0x Byte3Byte2Byte1Byte0




  // 当前电调测到的 转向推杆的 电流（单位A），舵机角度（1000-1500-2000）， 油量（0-100）单位% 
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
					//logd_uint("yaw=",yaw);
					if( yaw <= YAW_DEFAULE_MAX_ANGLE && yaw >= YAW_DEFAULE_MIN_ANGLE ){
						Esc_Yaw_Control_SetAngle(yaw);
					}
					
					// middle is 2000 dead range 500 25%
					//logd_uint("pitch=",pump_pitch);
					if( pump_pitch >= 0 || pump_pitch <= 4000)
					{
						if( pump_pitch >=0 && pump_pitch< 1500){
							Esc_Pump_Pitch_Back();
						}else if( pump_pitch >2500 && pump_pitch <=4000){
							Esc_Pump_Pitch_Forward();
						}else{
							Esc_Pump_Pitch_Middle();
						}
						
						Ke4_Set_Speed(pump_pitch/8+500);
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
	static unsigned char step=0;
	uint8_t lim_status ;
	
	lim_status = get_esc_limit_gpio_status();
	if( lim_status != 0){
		logd_uint("Alarm Gpio=",lim_status);
		//Alarm_limit_Position();
		reportBuffer[0]= 0xa1;
		reportBuffer[1]= lim_status;
		Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,2,CAN_ID_EXT, CAN_RTR_DATA);
	}
	switch(step)
	{
		case 0:
		{
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
			step++;
			break;
		}
		case 1:
		{
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

			step++;
			break;
		}
		case 2:
		{
			////  //dam4_02,dam4_04,dam4_05,dam16_08,dam4_09;
			//unsigned short status;
			//[0x2][statusL][statusH][input1][input2][input3][input4] 
			//[0x4][statusL][statusH]    [0x05][statusL][statusH]     [0x08][statusL][statusH]      [0x09][statusL][statusH] 
			if( dam4_02.updated ){
				reportBuffer[0]= 0x2;
				reportBuffer[1]= (dam4_02.status & 0xff); reportBuffer[2]= ((dam4_02.status>>8)& 0xff);
				reportBuffer[3] = dam4_02.input[0] ; 
				reportBuffer[4] = dam4_02.input[1] ; //发动机低油压报警
				reportBuffer[5] = dam4_02.input[2] ; //发动机高水温报警
				reportBuffer[6] = dam4_02.input[3] ; 
				//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
				//Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
				Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,7,CAN_ID_EXT, CAN_RTR_DATA);
			}
			if( dam4_04.updated ){
				reportBuffer[0]= 0x4;
				reportBuffer[1]= (dam4_04.status & 0xff); reportBuffer[2]= ((dam4_04.status>>8)& 0xff);
				//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
				Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
			}
			step++;
			break;	
		}
		case 3:
		{
			if( dam16_08.updated ){
				reportBuffer[0]= 0x8;
				reportBuffer[1]= (dam16_08.status & 0xff); reportBuffer[2]= ((dam16_08.status>>8)& 0xff);
				//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
				Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
			}
			step++;
			break;
		}
		case 4:
		{
			if( dam4_09.updated ){
				reportBuffer[0]= 0x9;
				reportBuffer[1]= (dam4_09.status & 0xff); reportBuffer[2]= ((dam4_09.status>>8)& 0xff);
				//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
				Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
			}
			if( dam4_05.updated ){
				reportBuffer[0]= 0x5;
				reportBuffer[1]= (dam4_05.status & 0xff); reportBuffer[2]= ((dam4_05.status>>8)& 0xff);
				//Can1_Send(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3);
				Can1_Send_Ext(MAIN_CONTROLLER_CAN_ID ,reportBuffer,3,CAN_ID_EXT, CAN_RTR_DATA);
			}
			step++;
			break;
		}
		case 5:
		{
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
			step++;
			break;
		}
		case 6:
		{
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
			step++;
			break;
		}
		case 7:
		{
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
			step++;
			break;
		}
		case 8:
		{
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
			step++;
			break;
		}
		
		default:
		{
			step=0;
			break;
		}
	}
}
int is_Can1_Lost_Connect()
{
	if( can1_lost_connect_counter >= CAN1_LISTENER_LOST_CONNECT_MAX_MS )
		return 1;
	return 0;
}
