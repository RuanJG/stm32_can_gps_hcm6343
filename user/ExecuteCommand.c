#include "stm32f10x.h"
#include "global.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	

u8 CAN_TX(CanTxMsg* TxMessage);
//void SetSamplePump(u8 pumpValue);
//void SetSampleValve(u8 Bottle);

//extern u8 Command[30];
//extern u8 CommandLength;
//extern int CommandIndex;

u8 Arm9Started = FALSE;

//static u8 pumpValue = 0;
static u8 sampleValue = 2;
//static u8 sampleValue1 = 2;
//static u8 rod_down = 1;			//0:up, 1:down

static CanTxMsg TxMessage2;        //外挂桨1
static CanTxMsg TxMessage3;        //外挂桨2

void SetSamplePump(u8 pumpValue)
{
	TxMessage2.StdId = 0x12;//0x12 是推进器can总线的地址
	TxMessage2.DLC = 2;
	TxMessage2.Data[0] = 0x09;
	TxMessage2.Data[1] = pumpValue; 	
	CAN_TX(&TxMessage2);
}

void SetSampleValve(u8 Bottle)  //增加采样箱CAN总线控制
{
  TxMessage3.StdId = 0x06;//0x06 是采样箱can总线的地址
	TxMessage3.DLC = 2;
	TxMessage3.Data[0] = 0x08;
	TxMessage3.Data[1] = Bottle;//Bottle - 100; 	
	CAN_TX(&TxMessage3);
}

static CanTxMsg TxMessage7;        //收放进水管
void SendSamplePipeLenCmd(u8 cmd)
{
	//cmd -> stop 33; pull 17; push 65
	TxMessage7.StdId = 0x15;//0x15 是监测盒子的ID，分层抽样控制功能也添加在其上
	TxMessage7.DLC = 2;
	TxMessage7.Data[0] = 0x07;
	TxMessage7.Data[1] = cmd;
	CAN_TX(&TxMessage7);
}
void test_sample_pipe_len_cmd()
{
	static u8 i=0;
	u8 cmds[3]={17,65,33};

	SendSamplePipeLenCmd(cmds[i++]);
	if( i >= 3 ) i=0;
}

void ExecuteValve(u8 valve_value)
{
	if((valve_value & 0x02) != 0)
	{
		VALVE12_1SET;
	}
	else{
		VALVE12_1RESET;
	}
	if((valve_value & 0x04) != 0)
	{
		VALVE12_2SET;
	}
	else{
		VALVE12_2RESET;
	}
	if((valve_value & 0x08) != 0)
	{
		VALVE34_1SET;
	}
	else{
		VALVE34_1RESET;
	}
	if((valve_value & 0x10) != 0)
	{
		VALVE34_2SET;
	}
	else{
		VALVE34_2RESET;
	}
	if((valve_value & 0x01) != 0)
	{
		TxMessage2.StdId = 0x12;
		TxMessage2.DLC = 2;
		TxMessage2.Data[0] = 0x09;
		TxMessage2.Data[1] = 17; 	//正转
		CAN_TX(&TxMessage2);
	}
	else{
		TxMessage2.StdId = 0x12;
		TxMessage2.DLC = 2;
		TxMessage2.Data[0] = 0x09;
		TxMessage2.Data[1] = 33; 	//关闭
		CAN_TX(&TxMessage2);
	}
}

void ExecuteCommand(u8 *commamnd, int commamnd_len)
{
//	//***** Control rudder, propeller, send command to valve***********/					  			
	int cmdIndex = 0;
	u16 cmdChecksum = 0; 
	u16 receivedChecksum;
	
	for (cmdIndex = 0; cmdIndex < commamnd_len-2; cmdIndex++)
	{
			cmdChecksum += commamnd[cmdIndex];
	}
	cmdChecksum |= 0x8080;
	receivedChecksum = ((commamnd[commamnd_len-2]*256) + commamnd[commamnd_len-1]) | 0x8080;
	
	if(cmdChecksum == receivedChecksum)
	{
			// command index 0:rudder 1:propeller 2:alarm lamp 3:alarm clock 4:cam power 5:valve 6:pump 7:收放杆			
			// 与外挂桨板通信
			for (cmdIndex = 0; cmdIndex <= commamnd_len-3;)
			{
				switch(commamnd[cmdIndex])
				{
					case 0://舵机角度
						 cmdIndex += 2;
						break;
					case 1://外挂桨指令
						TxMessage2.StdId = 0x12;
						TxMessage2.DLC = 4;
						TxMessage2.Data[0] = 0x10;
						TxMessage2.Data[1] = commamnd[1]; 	//rudder
						TxMessage2.Data[2] = commamnd[3];  	//propeller
						TxMessage2.Data[3] = TxMessage2.Data[2]^TxMessage2.Data[1];	
						CAN_TX(&TxMessage2);
						cmdIndex += 2;
						break;
					case 0x11:// 新舵角推进,单摇杆控制所有舵机和推进
					case 0x12://MC70喷水
					case 0x13://双遥杆的左,控制左舵机和推进
					case 0x14://双遥杆的右,控制右舵机和推进
						TxMessage2.StdId = 0x12;
						TxMessage2.DLC = 6;
						TxMessage2.Data[0] = commamnd[cmdIndex];//0x11;
						TxMessage2.Data[1] = commamnd[cmdIndex+1]; 	//rudder 高位
						TxMessage2.Data[2] = commamnd[cmdIndex+2]; 	//rudder 低位
						TxMessage2.Data[3] = commamnd[cmdIndex+3];  	//propeller 高位
						TxMessage2.Data[4] = commamnd[cmdIndex+4];  	//propeller 低位
						//校验
						TxMessage2.Data[5] = TxMessage2.Data[1]^TxMessage2.Data[2]^TxMessage2.Data[3]^TxMessage2.Data[4];	
						CAN_TX(&TxMessage2);
						cmdIndex += 5;
						break;
					case 2://警灯,MC70门开关
						switch(commamnd[cmdIndex+1])
						{
							case 33:
								ALERT_LED_RESET;
								break;
							case 17:
								ALERT_LED_SET;
								break;
							case 65:
								break;
							case 129:
								break;
						}				
						
						//TxMessage2.StdId = 0x12;
						//TxMessage2.DLC = 2;
						//TxMessage2.Data[0] = 0x02;
						//TxMessage2.Data[1] = commamnd[cmdIndex+1]; 	//警灯,MC70门开关
						//CAN_TX(&TxMessage2);

						cmdIndex += 2;
						break;
					case 3://警报
						switch(commamnd[cmdIndex+1])
						{
							case 33:
								SPEAKER_RESET;
								break;
							case 17:
								SPEAKER_SET;
								break;
						}
						cmdIndex += 2;
						break;
					case 4://摄像头，发动机
						{
							if (commamnd[cmdIndex+1]==17)
							{
								CAM_PWR_SET;
							}
							else if (commamnd[cmdIndex+1]==33) 
							{
								CAM_PWR_RESET;
							}
							//else //20150313,命令通过CAN发给发动机控制板
							{
								TxMessage2.StdId = 0x12;
								TxMessage2.DLC = 3;
								TxMessage2.Data[0] = 0x08;
								TxMessage2.Data[1] = commamnd[cmdIndex+1]; 	//
								//校验
								TxMessage2.Data[2] = TxMessage2.Data[0]^TxMessage2.Data[1];	
								CAN_TX(&TxMessage2);
							}
							cmdIndex += 2;
							break;
						}
					case 5://阀
						switch(commamnd[cmdIndex+1])
						{
							//To close all the valves
							case 100:
								VALVE12_1SET;
								VALVE12_2RESET;
								VALVE34_1SET;
								VALVE34_2RESET;
								SetSampleValve(100);
								sampleValue = 0;//没有采样
								break;
							//To open first bottle
							case 101:
								VALVE12_1SET;
								VALVE12_2SET;
								VALVE34_1SET;
								VALVE34_2RESET;
								sampleValue = 1;//采样
								break;
							//To open second bottle				
							case 102:
								VALVE12_1RESET;
								VALVE12_2SET;
								VALVE34_1SET;
								VALVE34_2RESET;
								sampleValue = 1;//采样
								break;
							//To open third bottle
							case 103:
								VALVE12_1SET;
								VALVE12_2RESET;
								VALVE34_1SET;
								VALVE34_2SET;
								sampleValue = 1;//采样
								break;
							//To open fourth bottle
							case 104:
								VALVE12_1SET;
								VALVE12_2RESET;
								VALVE34_1RESET;
								VALVE34_2SET;
								sampleValue = 1;//采样
								break;

							case 105:
					      SetSampleValve(105);
								sampleValue = 1;//采样
								break;

							case 106:
								SetSampleValve(106);
								sampleValue = 1;//采样
								break;

							case 107:
								SetSampleValve(107);
								sampleValue = 1;//采样
								break;

							case 108:
								SetSampleValve(108);
								sampleValue = 1;//采样
								break;

							case 110:
								//Then turn off L293D to reduce the heat
								VALVE12_1RESET;
								VALVE12_2RESET;
								VALVE34_1RESET;
								VALVE34_2RESET;
								SetSampleValve(110);
								sampleValue = 1;//采样
								break;
							default:
								sampleValue = 1;//采样
								if (commamnd[cmdIndex+1] >=210 && commamnd[cmdIndex+1] <= 216)
								{
									TxMessage2.StdId = 0x14;//发给大采样箱板
									TxMessage2.DLC = 2;
									TxMessage2.Data[0] = 0x10;
									TxMessage2.Data[1] = commamnd[cmdIndex+1]; 	
									CAN_TX(&TxMessage2);
								}	
								else if (commamnd[cmdIndex+1] >= 220 && commamnd[cmdIndex+1] <= 230)
								{
									TxMessage2.StdId = 0x14;//发给大采样箱板
									TxMessage2.DLC = 2;
									TxMessage2.Data[0] = 0x10;
									TxMessage2.Data[1] = commamnd[cmdIndex+1]; 	
									CAN_TX(&TxMessage2);
								}
								break;								
						}
						cmdIndex += 2;
						break;
					case 6:                              //泵	
						//对于监测泵和采样泵分开的，需要这里作判断
						//对于监测和采样采用同一个泵的，这里的判断要去掉
						if(sampleValue == 1)
						{		  
							TxMessage2.StdId = 0x12;
							TxMessage2.DLC = 2;
							TxMessage2.Data[0] = 0x09;
							TxMessage2.Data[1] = commamnd[cmdIndex+1]; 	
							CAN_TX(&TxMessage2);
						
						}
						if(commamnd[cmdIndex+1] == 33)	 //如果33泵停止工作...
						{
						 	 SetSamplePump(33);
						
						}
						sampleValue = 2;	             //自动清0;
						cmdIndex += 2;
						break;
					case 7:                              //收放杆
						switch(commamnd[cmdIndex+1])
						{
							//stop 33; pull 17; push 65
							case 33:
								SendSamplePipeLenCmd(33); // 分层抽样需要给控制板发停止命令
								ROD_DOWN_RESET;
								ROD_UP_RESET;
								break;
							case 17:
								SendSamplePipeLenCmd(17); // 分层抽样需要给控制板发回收管命令
								ROD_UP_SET;
								ROD_DOWN_RESET;
								//rod_down = 0;
								break;
							case 65:
								SendSamplePipeLenCmd(65); // 分层抽样需要给控制板发放管命令
								ROD_DOWN_SET;
								ROD_UP_RESET;
								//rod_down = 1;
								break;
						}
						cmdIndex += 2;
						break;
					case 8://监测设备抽水泵
						if(commamnd[cmdIndex+1] != 0x00)
						{
							GPRS_PWR_RESET;
						}
						else if(commamnd[cmdIndex+1] == 0x00)
						{
							GPRS_PWR_SET;
						}

						/*
						TxMessage2.StdId = 0x15;
						TxMessage2.DLC = 4;
						TxMessage2.Data[0] = 0x10;
						if (Command[cmdIndex+1] & BIT7 == 0)//监测设备1关闭
						{
							TxMessage2.Data[1] = 33;
						}
						else //监测设备1开启 
						{
							TxMessage2.Data[1] = 17;
						}
						if (Command[cmdIndex+1] & BIT6 == 0)//监测设备2关闭
						{
							TxMessage2.Data[2] = 33;
						}
						else //监测设备2开启 
						{
							TxMessage2.Data[2] = 17;
						}
						if (Command[cmdIndex+1] & BIT5 == 0)//监测设备3关闭
						{
							TxMessage2.Data[3] = 33;
						}
						else //监测设备3开启 
						{
							TxMessage2.Data[3] = 17;
						} 
						CAN_TX(&TxMessage2);
						*/

						cmdIndex += 2;
						break;
					case 9://GPRS供电
						switch(commamnd[cmdIndex+1])
						{
							case 17:
								GPRS_PWR_SET;
								break;
							case 33:
								GPRS_PWR_RESET;
								break;
						}
						cmdIndex += 2;
						break;
					case 0x0A:
						ExecuteValve(commamnd[cmdIndex+1]);
						cmdIndex += 2;
						break;
					case 251://
						Arm9Started = TRUE;
					default:
					  //
						cmdIndex += 2;
						break;
				}
			}				
	}		
	else 
	{
			//DriverLED();
	}
}

