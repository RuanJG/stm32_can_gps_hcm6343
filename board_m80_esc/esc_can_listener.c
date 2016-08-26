#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "esc_box.h"



/* ���ط������
//
1. ������ת��Ŀ��� 
	//speed < 1500 ʱ�������·�����������ƣ�speed =[1500,2500] �������м䣬�������£�speed=(2500,4000]ʱ���������ϣ�������ǰ
	//yaw ��1500ʱ���м䣬С��1500ת�󣬴���1500ת��
	
	speed =[0-2000-4000] ;  yaw = [1000-1500-2000]
	[id=0][yaw_L][yaw_H][speed_L][speedH]



2. �̵������ؿ���
	[id = 1] [dam_addr_id] [num_id] [cmd 0-3] [msL] [msH] 
	
	dam_addr_id:  �̵������485��ַ��Ӧ���Ǵ� 0x01 -> 0xB, ������
	numid:  4·�̵����� [1��4]    16·�̵����� [1��16]�� ����ÿһ·�Ĺ��ܣ�����������
	cmd : DAM_CMD_FLASH_ON 3 ; DAM_CMD_FLASH_OFF 2 ; DAM_CMD_ON 1 ; DAM_CMD_OFF 0��  flash on ��ʾ��ԭ�����˵�״̬�£��򿪿��أ�͢ʱһ��ʱ����ٹرա�
	ms: 100<ms < 20000ms ,ֻ���� flash on �� flash off �����У���������͢ʱʱ�䣬��λ��ms����������� ���� 20000




//******** �����������

1. report
	// th11sb_head,th11sb_tail; 0xa 0xb
	unsigned short wet ;  ��λ 0.1  �磺wet = 501  501*0.1 = 50.1 
	unsigned short tempture;  ��λ 0.1   tempture =381  381*0.1=38.1���϶�
	
	can frame: [id=0xa][wetL][wetH][temL][temH] //ǰ��
	can frame: [id=0xb][][][][] // ���

  //�̵��� 
	status ÿһBit����һ·���ص�״̬����0x1��ʾ��һ·�Ǵ򿪵�
	dam4_02 ��ʾ485��ַ��0x02�� 4·�̵��� 
	
	
	dam4_02 �� 	������뷢����
		//��DAM���ϵ�����Ž�����·��� input2:����������ѹ���� input3:��������ˮ�±���
		//��һ·���أ�������Ĺرգ�����1�룬�رշ����
		//�ڶ�·���أ���������ϵ磬����3�룬���������
		//����·���أ��������Ĵ�𿪹أ�����500ms�����������
		//����·���أ��������Ŀ��أ��պϿ��أ��������ϵ磬�Ͽ����أ��������ر�
		[0x2][statusL][statusH][input1][input2][input3][input4]   
	
	
	dam4_04 ��	������ �ϵ���ƣ�
		ע����ͬһʱ���ڣ�ֻ�ܴ��ҿ��ƻ�����ƣ���Ҫ���ҿ��ƣ�Ҫ�ȹ������
		ע���Ƚ��г�磬���10����󣬲����ϵ�
		//��һ·���أ��������ϵ磬�պϿ����ϵ�
		//�ڶ�·���أ��������磬�պϳ��
		//����·���أ�����������ƿ�����رգ��պϿ���ʹ��
		//����·���أ��������ҿ��ƿ�����رգ��պϿ���ʹ��
		[0x4][statusL][statusH] 
		
	dam4_05 ��	������ ���Ʒ���  
		ע��ĳһʱ���ڣ�ֻ�ܿ���ǰ������ˣ��磬�ڿ���ǰ������ʱ��Ҫ�رպ��˷���
		//��һ·���أ�����������Ƶ�ǰ�����򣬱պϿ���ʹ��
		//�ڶ�·���أ�����������Ƶĺ��˷��򣬱պϿ���ʹ��
		//����·���أ��������ҿ��Ƶ�ǰ�����򣬱պϿ���ʹ��
		//����·���أ��������ҿ��Ƶĺ��˷��򣬱պϿ���ʹ��
		[0x05][statusL][statusH]
		
	
	dam16_08 ��	DC����� 
		//��һ·���أ�����֧��UP  , �պϿ���ʹ��
		//�ڶ�·���أ�����֧��DOWN�� �պϿ���ʹ��
		//����·���أ�
		//����·���أ������״�պϿ���ʹ��
		//����·���أ�ת�������õ�Դ
		//�ڰ�·���أ�KE4
		//�ھ�·���أ�����
		//��ʮ·���أ�ǰ�շ��
		//��ʮһ·���أ���շ��
		//��12·���أ��յױ�
		//��13·���أ�4G�״�
		//��14·���أ�����ͷ
		//��15·���أ�������
		//��16·���أ����أ�GPS
	[0x08][statusL][statusH] 
	
	
	dam4_09 ��	
		//��һ·��ǰ������
		//�ڶ�·���ನ��
		//����·�� LET
		//����·�����ػ�
	[0x09][statusL][statusH]
	





  //��ѹ�ɼ��壺 ÿһ������6·��ѹ����	unsigned short adc[6];
	powerAdc6_01 ��ʾ  486��ַΪ0x01�� 6·��ѹ������ 
	
	powerAdc6_01��AC�����  adc[0]=DC_12V  adc[4]= AC_I_VOUT  adc[5] = AC_V_VOUT
	��һ��can����[0x10][adc0L][adc0H] [adc1_L][adc1_H] [adc2_L][adc2_H] 
	�ڶ���can����[0x11][adc3L][adc3H] [adc4_L][adc4_H] [adc5_L][adc5_H]
	
	powerAdc6_06 �������䣬adc[0]=12V_IN adc[1]=12V_OUT  adc[4]=I_Right adc[5]=I_LEFT
	��һ��can����[0x60][adc0L][adc0H] [adc1_L][adc1_H] [adc2_L][adc2_H]
	�ڶ���can����[0x61][adc3L][adc3H] [adc4_L][adc4_H] [adc5_L][adc5_H]
	
	
	powerAdc6_07�� DC���   adc[0]= BAT_12V adc[1]=12V_EXT adc[2]=24V_EXT adc[3]=24V_BAT adc[4]=I_12V adc[5]=I_24V
	��һ��can����[0x70][adc0L][adc0H] [adc1_L][adc1_H] [adc2_L][adc2_H]
	�ڶ���can����[0x71][adc3L][adc3H] [adc4_L][adc4_H] [adc5_L][adc5_H]

  

   pgw636_03��
		//ת�������;   pgw636_03   485��ַΪ0x03 �� ת�������;
		// ����ֵ���ݷ�Χ�� -8388608  ~  8388607  4���ֽ�
		//ת�ٱ� ת�����������ֵ�Ĺ�ϵ���������ܵõ����
	[0x03][Byte0][Byte1][Byte2][Byte3]  ==> int = 0x Byte3Byte2Byte1Byte0




  // ��ǰ����⵽�� ת���Ƹ˵� ��������λA��������Ƕȣ�1000-1500-2000���� ������0-100����λ% 
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
				reportBuffer[4] = dam4_02.input[1] ; //����������ѹ����
				reportBuffer[5] = dam4_02.input[2] ; //��������ˮ�±���
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
