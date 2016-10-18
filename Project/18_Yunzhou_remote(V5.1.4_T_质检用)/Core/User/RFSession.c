/*-------------------------------------------------------------------------
工程名称：无线控制命令发送与接收
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150615 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "RFSession.h"


/* Private variables ---------------------------------------------------------*/
static u8 sendBuffer[480];													//无线命令发送缓存
static u8 sendBuffer_encoded[640];									//发送缓存base64编码格式
static int sendBuffer_index;												//发送缓存指针
static int sendBuffer_encoded_length;								//发送缓存编码指针


/* Exported variables ---------------------------------------------------------*/
u8 NeedSendSpeaker = 0;													//警报声发送次数
u8 NeedSendAlertLED = 0;												//警报灯发送次数
u8 NeedSendSampleCount = 0;											//采样发送次数
u8 NeedSendSampleFinishedCount = 0;							//采样完成发送次数
u8 NeedSendAutoModeCommandCount = 0;						//发送自动模式命令次数
u8 NeedSendValveCount = 0;

uint8_t connect_ok = 0;							//0表示连接没成功，1表示连接成功

bool EngineStarted = FALSE;			//打火允许
bool Speaker;										//警报
bool Alert;											//警灯	
bool ValveStatus[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};											//阀门状态
bool ValveStatusUsed[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};									//阀门使用状态

u8 receivedManualMessageCount = 0;

uint16_t RF_transmit_timer = 0;						//数传模块定时传输计时器

uint8_t BoatDataTimeoutCount = 0;					//船数据超时计时器，秒计时

BoatDataTypeDef BoatData;									//USV通用回传数据
USVSetting receivedUSVSetting;						//USV设置信息
GlobalVariableTypeDef GlobalVariable;			//全局变量
SampleInfoTypeDef SampleInfo;							//采样相关变量

static RFCommandReceivedStates rfcommandReceivedState;
static RFSessionReceivedStates receivedState;
static RFSessionExpectStates expectState;

static int receivedBufCount = 0;
static u8 receivedBuf[1024];
static int decodedBufferCount;
static u8 decodedBuffer[1600];

static u8 bottleNumberLocal = 0;


#if defined QC_LOOP_TEST
uint16_t QC_Test_Timer = 0;				//质检测试用计数器
uint8_t QC_Test_Flag = 1;					//质检测试用命令顺序
uint8_t QC_Sample_Flag = 0;				//采样1次标记

uint16_t QC_Motor_Timer = 0;				//质检测试电机用计数器
uint8_t QC_Motor_Flag = 1;					//质检测试电机用命令顺序
#endif


/* Private function -----------------------------------------------*/
/*发送相关命令---------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
	函数名：GenerateTakeSampleCommandBase64
	功  能：采样命令发送，base64编码格式
	参  数：encoded 		布尔变量			判断是否需要base64编码
	返回值：
-------------------------------------------------------------------------*/
void GenerateTakeSampleCommandBase64(bool encoded)
{
#if defined QC_LOOP_TEST
	if(QC_Sample_Flag == 1)
	{
		QC_Sample_Flag = 0;
		
		//质检采样动作
		switch(QC_Test_Flag)
		{
			case 5:
				water_sample_conf[0] = 1;
				water_sample_conf[3] = 5;
				NeedSendSampleCount = 3;
				break;
			
			case 6:
				water_sample_conf[0] = 2;
				water_sample_conf[3] = 5;
				NeedSendSampleCount = 3;	
				break;

			case 7:
				water_sample_conf[0] = 3;
				water_sample_conf[3] = 5;
				NeedSendSampleCount = 3;
				break;

			case 8:
				water_sample_conf[0] = 4;
				water_sample_conf[3] = 5;
				NeedSendSampleCount = 3;
				break;

			default:
				break;
		}		
	}
#endif	

	if(NeedSendSampleCount > 0){
		sendBuffer_index = 0;
		
		sendBuffer[sendBuffer_index++] = 0;
		sendBuffer[sendBuffer_index++] = 0x53;//83; //S
		sendBuffer[sendBuffer_index++] = 0x53;//83; //S
		sendBuffer[sendBuffer_index++] = 0x7C;//124;
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[0];						//采样瓶号
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[1];						//采样深度
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[2];						//采样模式
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[3];						//采样容量
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[4];						//清洗模式
		sendBuffer[sendBuffer_index++] = (uint8_t) (83 + 83 + 124 + water_sample_conf[0] + water_sample_conf[1] + water_sample_conf[2] + water_sample_conf[3] + water_sample_conf[4]);		//checksum
		
		//PutString("\r\n");
		if(encoded == TRUE){
			sendBuffer_encoded_length = base64_encode((char* )sendBuffer_encoded, (const char*) sendBuffer, sendBuffer_index);
			PutUART(0xFA);
			PutUART(0xFA);
			transmit_data(sendBuffer_encoded, sendBuffer_encoded_length);
			PutUART(0xFE);
			PutUART(0xFE);
		}
		else {
			sendBuffer[sendBuffer_index++] = 0x0D;
			sendBuffer[sendBuffer_index++] = 0x0A;
			transmit_data(sendBuffer, sendBuffer_index);
		}
		
		NeedSendSampleCount--;		
	}	
}


/*-------------------------------------------------------------------------
	函数名：SendSampleFinished
	功  能：采样完成发送，base64编码格式
	参  数：encoded 		布尔变量			判断是否需要base64编码
	返回值：
-------------------------------------------------------------------------*/
void SendSampleFinished(bool encoded)
{
	int i;
	u8 modeBuffer[30];
	u8 modBufferEncoded[40];
	int modeBufferIndex;
	int modBufferEncodedLength;
		
	//重置
	modeBufferIndex = 0;
	
	modeBuffer[modeBufferIndex++] = 0x00;
	modeBuffer[modeBufferIndex++] = 0x53;
	modeBuffer[modeBufferIndex++] = 0x46;
	modeBuffer[modeBufferIndex++] = (uint8_t) water_sample_conf[0];						//采样瓶号
	modeBuffer[modeBufferIndex++] = 0x7C;
	sendBuffer[sendBuffer_index++] = (uint8_t) (0x00 + 0x53 + 0x46 + 0x7C + water_sample_conf[0]);
	
	if(encoded == TRUE)
	{
		modBufferEncodedLength = base64_encode((char* )modBufferEncoded, (const char*) modeBuffer, modeBufferIndex);

			PutUART(0xFA);
			PutUART(0xFA);
			transmit_data(modBufferEncoded, modBufferEncodedLength);
			PutUART(0xFE);
			PutUART(0xFE);
	}
	else 
	{
		modeBuffer[modeBufferIndex++] = 0x0D;
		modeBuffer[modeBufferIndex++] = 0x0A;

		transmit_data(modeBuffer, modeBufferIndex);
	}
}


/*-------------------------------------------------------------------------
	函数名：GenerateAlarmCommandBase64
	功  能：警报警灯发送，base64编码格式
	参  数：encoded 		布尔变量			判断是否需要base64编码
	返回值：
-------------------------------------------------------------------------*/
void GenerateAlarmCommandBase64(bool encoded)
{
	u8 tempbyte;
	u16 checksum = 0;

#if defined QC_LOOP_TEST	
	//质检警报警灯动作
	switch(QC_Test_Flag)
	{
		case 9:
			NeedSendAlertLED = 1;
			GlobalVariable.Alert = 1;
			GlobalVariable.Speaker = 0;
			break;
		
		case 10:
			NeedSendSpeaker = 1;
			GlobalVariable.Speaker = 1;
			GlobalVariable.Alert = 0;
			break;
		
		case 1:
			GlobalVariable.Alert = 0;
			GlobalVariable.Speaker = 0;
			NeedSendSpeaker = 1;
			NeedSendAlertLED = 1;
			break;
		
		default:
			break;
	}
#endif
	
	
	if(NeedSendAlertLED > 0 || NeedSendSpeaker > 0)
	{
		sendBuffer_index = 0;
		
		sendBuffer[sendBuffer_index++] = 0;
		sendBuffer[sendBuffer_index++] = 0x43;//(67); //C
		sendBuffer[sendBuffer_index++] = 0x0;//PutUART(0);
		sendBuffer[sendBuffer_index++] = 0x7C;//PutUART(124);
		
		if (NeedSendAlertLED > 0)
		{
			sendBuffer[sendBuffer_index++] = 0x02;//PutUART(2);
			checksum += 2;
			//船型判断警报警灯解析
			if (local_para_conf[2] != USV_Boat_Type_ME300)
			{
				//开警报警灯
#if defined QC_LOOP_TEST
				if (GlobalVariable.Alert)
#else
				if (ALARM_Button)
#endif
				{
					tempbyte  = 17;
				}
				else 
				{
					tempbyte  = 33;
				}			
			}
			else 
			{
				//打火限制在5秒内，先将alert拨动到警报状态，然后拨动采样杆即可打火
				if (EngineStarted)
				{
					tempbyte  = 17;
				}
				else 
				{
					tempbyte  = 33;
				}
			}
			
//			USART_SendData(USART2, tempbyte);
			
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte );
			checksum += tempbyte ;
			NeedSendAlertLED --;
		}
		
		if (NeedSendSpeaker > 0)
		{
			sendBuffer[sendBuffer_index++] = 0x03;//PutUART(3);
			checksum += 3;

#if defined QC_LOOP_TEST			
			if (GlobalVariable.Speaker)
#else
			if (ALARM_Button)
#endif
			{
				tempbyte  = 17;
			}
			else 
			{
				tempbyte = 33;
			}
			
//			USART_SendData(USART2, tempbyte + 1);
			
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			NeedSendSpeaker --;
		}

		sendBuffer[sendBuffer_index++] = checksum / 256;//PutUART((char)(checksum / 256));
		sendBuffer[sendBuffer_index++] = checksum % 256;//PutUART((char)(checksum % 256));
		
		if(encoded == TRUE){
			sendBuffer_encoded_length = base64_encode((char* )sendBuffer_encoded, (const char*) sendBuffer, sendBuffer_index);
			PutUART(0xFA);
			PutUART(0xFA);
			transmit_data(sendBuffer_encoded, sendBuffer_encoded_length);
			PutUART(0xFE);
			PutUART(0xFE);
		}
		else {
			sendBuffer[sendBuffer_index++] = 0x0D;
			sendBuffer[sendBuffer_index++] = 0x0A;
			transmit_data(sendBuffer, sendBuffer_index);
		}
	}
}


/*-------------------------------------------------------------------------
	函数名：SendCommandAutoMode
	功  能：发送自动模式命令，base64编码格式
	参  数：encoded 		布尔变量			判断是否需要base64编码
	返回值：
-------------------------------------------------------------------------*/
void SendCommandAutoMode(bool encoded)
{
	int i;
	u8 modeBuffer[30];
	u8 modBufferEncoded[40];
	int modeBufferIndex;
	int modBufferEncodedLength;
	
		//重置
	modeBufferIndex = 0;
	
	modeBuffer[modeBufferIndex++] = 0x00;		//PutUART(0);
	modeBuffer[modeBufferIndex++] = 0x43;		//PutUART(67); //C
	modeBuffer[modeBufferIndex++] = 0x03;		//PutUART(3);
	modeBuffer[modeBufferIndex++] = 0x7C;		//PutUART(124); //C
	modeBuffer[modeBufferIndex++] = 0xC2;		//PutUART(194);
	
	if(encoded)
	{
		modBufferEncodedLength = base64_encode((char* )modBufferEncoded, (const char*) modeBuffer, modeBufferIndex);

		for (i = 0; i < 2; ++i)
		{
			PutUART(0xFA);
			PutUART(0xFA);
			transmit_data(modBufferEncoded, modBufferEncodedLength);
			PutUART(0xFE);
			PutUART(0xFE);
		}
	}
	else 
	{
		modeBuffer[modeBufferIndex++] = 0x0D;
		modeBuffer[modeBufferIndex++] = 0x0A;
		//for (i = 0; i < 5; ++i)
		for (i = 0; i < 2; ++i)
		{
			transmit_data(modeBuffer, modeBufferIndex);
		}
	}
}


/*-------------------------------------------------------------------------
	函数名：GetValveValue
	功  能：获取阀门配置信息
	参  数：
	返回值：阀门状态	无符号字符型		低5位对应5个阀门
-------------------------------------------------------------------------*/
u8 GetValveValue(void)
{
	int i;
	u8 value = 0;
	//u8 debugbuffer[8];
		
	for(i=0; i < 5; i++)
	{
		if(ValveStatusUsed[i])
		{
			value |= 0x1 << i;
		}
	}

	return value;
}


/*-------------------------------------------------------------------------
	函数名：save_Valve_parameter
	功  能：保存阀门状态
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void save_Valve_parameter(void)
{
	uint8_t i;
	
	for(i = 0; i < VALVE_QUANTITY; i++)
	{
		ValveStatusUsed[i] = ValveStatus[i];
	}
	
	NeedSendValveCount = 10;			//发送10次
	
	return;
}


/*-------------------------------------------------------------------------
	函数名：load_Valve_parameter
	功  能：读取阀门状态
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void load_Valve_parameter(void)
{
	uint8_t i;
	
	for(i = 0; i < VALVE_QUANTITY; i++)
	{
		ValveStatus[i] = ValveStatusUsed[i];
	}
	
	return;
}


/*-------------------------------------------------------------------------
	函数名：GenerateValveCommand
	功  能：发送阀门命令，base64编码格式
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void GenerateValveCommand(void)
{
	if(NeedSendValveCount > 0)
	{
		u8 value = GetValveValue();
		
		sendBuffer_index = 0;
		sendBuffer[sendBuffer_index++]= 0x02;
		sendBuffer[sendBuffer_index++] = value;
		sendBuffer[sendBuffer_index++] = 0x02 ^ value;
		sendBuffer_encoded_length = base64_encode((char* )sendBuffer_encoded, (const char*) sendBuffer, sendBuffer_index);
		PutUART(0xFA);
		PutUART(0xFA);
		transmit_data(sendBuffer_encoded, sendBuffer_encoded_length);
		PutUART(0xFE);
		PutUART(0xFE);
		
		NeedSendValveCount--;
	}
}


/*-------------------------------------------------------------------------
	函数名：StopEngineTrigger
	功  能：停止发动机打火
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void StopEngineTrigger(void)
{
			if(EngineStarted != FALSE)
			{
//				EngineTimerCount=0;
				EngineStarted = FALSE;
				NeedSendAlertLED = MAX_ALERT_SEND_COUNT;
				NeedSendSpeaker = MAX_SPEAKER_SEND_COUNT;
				//显示发动机状态
//				Show_Engine_Status();
			}
}


/*-------------------------------------------------------------------------
	函数名：GenerateEngineStartCommandBase64
	功  能：发送引擎录像点火命令，base64编码格式
	参  数：encoded 		布尔变量			判断是否需要base64编码
	返回值：
-------------------------------------------------------------------------*/
void GenerateEngineStartCommandBase64(bool encoded)
{
	if (NeedSendAlertLED > 0)
	{
		sendBuffer[0] = 0;
		sendBuffer[1] = 0x43; 
		sendBuffer[2] = 0x43; 
		sendBuffer[3] = 0x7C; 
			
		if (EngineStarted)
		{
			sendBuffer[4] = 0x51;
		}
		else 
		{
			sendBuffer[4] = 0x61;
			
		}
		
		sendBuffer[5] = sendBuffer[0] + sendBuffer[1] + sendBuffer[2] + sendBuffer[3] + sendBuffer[4];						//checksum
		NeedSendAlertLED --;

		if(encoded == TRUE)
		{
			sendBuffer_encoded_length = base64_encode((char* )sendBuffer_encoded, (const char*) sendBuffer, 6);
			PutUART(0xFA);
			PutUART(0xFA);
			transmit_data(sendBuffer_encoded, sendBuffer_encoded_length);
			PutUART(0xFE);
			PutUART(0xFE);
		}
		else {
			sendBuffer[6] = 0x0D;
			sendBuffer[7] = 0x0A;
			transmit_data(sendBuffer, 8);
		}
	}
}


/*-------------------------------------------------------------------------
	函数名：GenerateEngineStopCommandBase64
	功  能：发送引擎供电熄火命令，base64编码格式
	参  数：encoded 		布尔变量			判断是否需要base64编码
	返回值：
-------------------------------------------------------------------------*/
void GenerateEngineStopCommandBase64(bool encoded)
{
	if (NeedSendSpeaker > 0)
	{
		sendBuffer[0] = 0;
		sendBuffer[1] = 0x43; 
		sendBuffer[2] = 0x43; 
		sendBuffer[3] = 0x7C; 

		if (!EngineStarted)
		{
			sendBuffer[4] = 0x11;
		}
		else 
		{
			sendBuffer[4] = 0x21;
		}

		sendBuffer[5] = sendBuffer[0] + sendBuffer[1] + sendBuffer[2] + sendBuffer[3] + sendBuffer[4];
		NeedSendSpeaker --;

		if(encoded == TRUE)
		{
			sendBuffer_encoded_length = base64_encode((char* )sendBuffer_encoded, (const char*) sendBuffer, 6);
			PutUART(0xFA);
			PutUART(0xFA);
			transmit_data(sendBuffer_encoded, sendBuffer_encoded_length);
			PutUART(0xFE);
			PutUART(0xFE);
		}
		else {
			sendBuffer[6] = 0x0D;
			sendBuffer[7] = 0x0A;
			transmit_data(sendBuffer, 8);
		}
	}
}


/*-------------------------------------------------------------------------
	函数名：GenerateCommandBase64
	功  能：定时发送命令，base64编码格式
	参  数：encoded 		布尔变量			判断是否需要base64编码
	返回值：
-------------------------------------------------------------------------*/
void GenerateCommandBase64(bool encoded)
{	
	float percent;
	int tempvalue;
	u16 gatepwm;
	u16 gatespeed;
	u16 checksum = 0;
	u16 tmpint;
	u8 tempbyte;
	
	//GenerateValveCommand();
	
	if(NeedSendAutoModeCommandCount > 0 || (MODE_Button == CONTROL_MODE_AUTO && receivedManualMessageCount > 2))
	{
		SendCommandAutoMode(TRUE);
		
		NeedSendAutoModeCommandCount = 0;
		receivedManualMessageCount = 0;
		
//		USART_SendData(USART2, 0x99);
	}
	
	if ((MODE_Button == CONTROL_MODE_AUTO) && ((interface_index == DEBUG_MAIN_INTERFACE_INDEX) || (interface_index == MAIN_GRAPHIC_INDEX)))	//非自动模式和在手动窗口下，才会发送。
	//if (GlobalVariable.ControlMode == CONTROL_MODE_AUTO)//非自动模式，才会发送。
	//if (GlobalVariable.ControlMode == CONTROL_MODE_AUTO || GlobalVariable.IsRFChannelSetting != FALSE)//非自动模式和非更改信道时，才会发送。
	{
			return;
	}
	
	if (local_para_conf[2] == USV_Boat_Type_ME300)
	{
		GenerateEngineStartCommandBase64(TRUE);
		GenerateEngineStopCommandBase64(TRUE);
	}
	
//	//设置正在发送标志
//	GlobalVariable.IsRFSessionSending = TRUE;
	
	if(NeedSendSampleFinishedCount > 0)
	{
		SendSampleFinished(TRUE);
		NeedSendSampleFinishedCount = 0;
	}
	
	GenerateTakeSampleCommandBase64(TRUE);
	
	GenerateAlarmCommandBase64(TRUE);

	//重置
	sendBuffer_index = 0;

	sendBuffer[sendBuffer_index++] = 0;
	sendBuffer[sendBuffer_index++] = 0x43;//(67); //C
	sendBuffer[sendBuffer_index++] = 0x0;//PutUART(0);
	sendBuffer[sendBuffer_index++] = 0x7C;//PutUART(124);


	//-----------------------------20131227新舵角推进开始------------------------
	//MC70泵和门控制，ME300油机的油门控制
	switch(local_para_conf[2])
	{
		case USV_Boat_Type_MC120:
		case USV_Boat_Type_MC70:
			switch(ALARM_Button)
			{
				case 0:
					//GlobalVariable.Alert = TRUE;
					//GlobalVariable.Speaker = TRUE;
					gatepwm = 1000;//开门
					break;
				case 2:
					//GlobalVariable.Alert = TRUE;
					//GlobalVariable.Speaker = FALSE;
					gatepwm = 1000;//开门
					break;
				case 1:
				default:
					//GlobalVariable.Alert = FALSE;
					//GlobalVariable.Speaker = FALSE;
					gatepwm = 2000;//关门
					break;
			}
			gatespeed = KnobValue;//旋钮值
			break;
		case USV_Boat_Type_ME300:
			gatepwm = KnobValue;
//			if(gatepwm < 3000)
//			{
//				gatepwm = 3000;
//			}
			if(gatepwm > 4000)
			{
				gatepwm = 4000;
			}
			//油门开度 1000-2000, 旋钮值范围3000-4000;
			gatespeed = gatepwm;
			gatepwm = gatepwm - 2000;
//			gatespeed = 2000;//停止			
						
			break;
		default:
			gatepwm = 1500;//中位
			gatespeed = 2000;//停止
			break;
	}
	
	//MC70喷水泵电机和开关门控制，ME300油机的油门控制
	switch(local_para_conf[2])
	{
		case USV_Boat_Type_MC120:
		case USV_Boat_Type_MC70:
			sendBuffer[sendBuffer_index++] = 0x12;//PutUART(0x12);
			checksum += 0x12;
			//舵角pwm
			tmpint = gatepwm;
			tempbyte = ((tmpint>>7)&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			tempbyte = (tmpint&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			//推进H桥
			tmpint = gatespeed;
			tempbyte = ((tmpint>>7)&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			tempbyte = (tmpint&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			break;
		case USV_Boat_Type_ME300:
			sendBuffer[sendBuffer_index++] = 0x12;//PutUART(0x12);
			checksum += 0x12;
			//舵角pwm
			//正向油门
			tmpint = gatepwm;
			//反向油门
			//tmpint = 3000-gatepwm;
			tempbyte = ((tmpint>>7)&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			tempbyte = (tmpint&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			//推进H桥
			tmpint = gatespeed;
			tempbyte = ((tmpint>>7)&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			tempbyte = (tmpint&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			break;
		default:
			break;
	}

	//if(Switches.SwitchValue[1] == SWITCH_UP)
	if(MODE_Button != CONTROL_MODE_MANUAL2)
	{//单摇杆控制模式
		/////////////////////////////左右推进电机/////////////////
		sendBuffer[sendBuffer_index++] = 0x11;//PutUART(0x11);
		checksum += 0x11;
		//舵角
		//if (GlobalVariable.BoatType == USV_Boat_Type_MS70)
		//{
		//	tmpint = GlobalVariable.RudderMiddle*10 - ADC.RightRudderCommand + GlobalVariable.RudderMiddle*10;
		//}
		//else

#if defined QC_LOOP_TEST			
		//质检测试推进动作
		switch(QC_Motor_Flag)
		{
			case 1:				//左前进
				RightSpeedCommand = 4090;
				RightRudderCommand = 4090;
				break;
			
			case 2:				//右后退
				RightSpeedCommand = 0;
				RightRudderCommand = 0;
				break;			
			
			case 3:				//右前进
				RightSpeedCommand = 0;
				RightRudderCommand = 4090;
				break;

			case 4:				//左后退
				RightSpeedCommand = 4090;
				RightRudderCommand = 0;
				break;								
			
			default:
//				RightSpeedCommand = 1500;
//				RightRudderCommand = 2000;
				break;				
		}		
#endif
		
		
		//舵机
		tmpint = RightRudderCommand;
		tempbyte = ((tmpint>>7)&0x7F)|0x80;
		sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
		checksum += tempbyte;
		tempbyte = (tmpint&0x7F)|0x80;
		sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
		checksum += tempbyte;

		//推进
		tmpint = RightSpeedCommand;
		tempbyte = ((tmpint>>7)&0x7F)|0x80;
		sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
		checksum += tempbyte;
		tempbyte = (tmpint&0x7F)|0x80;
		sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
		checksum += tempbyte;
	}
	else 
	{//双摇杆模式
		if (local_para_conf[1] != MANUAL_CONTROL_MODE_DUAL)						//单推模式
		{//左摇杆控制油门，右摇杆控制舵机
				/////////////////////////////左右推进电机/////////////////
				sendBuffer[sendBuffer_index++] = 0x11;//PutUART(0x11);
				checksum += 0x11;
				//舵角， 采用右摇杆的值
				//if (GlobalVariable.BoatType == USV_Boat_Type_MS70)
				//{
				//	tmpint = GlobalVariable.RudderMiddle*10 - ADC.RightRudderCommand + GlobalVariable.RudderMiddle*10;
				//}
				//else
				{
					tmpint = RightRudderCommand;
				}
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;

				//推进, 采用左摇杆的值
				tmpint = LeftSpeedCommand;
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
		}
		else{//左摇杆控左推进，右摇杆控制右推进																				双推模式
				//////////////////////////////右摇杆/////////////////
				sendBuffer[sendBuffer_index++] = 0x13;//PutUART(0x13);
				checksum += 0x13;
				//舵角
				//if (GlobalVariable.BoatType == USV_Boat_Type_MS70)
				//{
				//	tmpint = GlobalVariable.RudderMiddle*10 - ADC.RightRudderCommand + GlobalVariable.RudderMiddle*10;
				//}
				//else
				{
					tmpint = RightRudderCommand;
				}
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;

				//推进
				tmpint = RightSpeedCommand;
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				
				//////////////////////////////左摇杆/////////////////
				sendBuffer[sendBuffer_index++] = 0x14;//PutUART(0x14);
				checksum += 0x14;
				//舵角
				//if (GlobalVariable.BoatType == USV_Boat_Type_MS70)
				//{
				//	tmpint = GlobalVariable.RudderMiddle*10 - ADC.LeftRudderCommand + GlobalVariable.RudderMiddle*10;
				//}
				//else
				{
					tmpint = LeftRudderCommand;
				}
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;

				//推进
				tmpint = LeftSpeedCommand;
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
		}
	}
	//-----------------------------20131227新舵角推进结束------------------------

	//阀门命令发送
	if(NeedSendValveCount > 0)
	{
		sendBuffer[sendBuffer_index++]=0x0A;
		checksum += 0x0A;
		tempbyte = GetValveValue();
		sendBuffer[sendBuffer_index++]=tempbyte;
		checksum += tempbyte;
		
		NeedSendValveCount--;
	}

	sendBuffer[sendBuffer_index++] = checksum / 256;//PutUART((char)(checksum / 256));
	sendBuffer[sendBuffer_index++] = checksum % 256;//PutUART((char)(checksum % 256));
	
	if(encoded == TRUE){
			sendBuffer_encoded_length = base64_encode((char* )sendBuffer_encoded, (const char*) sendBuffer, sendBuffer_index);
			PutUART(0xFA);
			PutUART(0xFA);
			transmit_data(sendBuffer_encoded, sendBuffer_encoded_length);
			PutUART(0xFE);
			PutUART(0xFE);
	}
	else {	
			sendBuffer[sendBuffer_index++] = 0x0D;
			sendBuffer[sendBuffer_index++] = 0x0A;
			transmit_data(sendBuffer, sendBuffer_index);
			//PutString("\r\n");
	}

//		//设置正在发送标志
//	GlobalVariable.IsRFSessionSending = FALSE;
}
/*发送相关命令===============================================================================================================*/


/*接收相关命令---------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
	函数名：OnCommandDataReceived
	功  能：信息接收处理函数
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnCommandDataReceived(char c)
{
	ParseRFComamndByte(c);
}


/*-------------------------------------------------------------------------
	函数名：ParseRFComamndByte
	功  能：分析RF模块的命令
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ParseRFComamndByte(u8 b)
{
	//先存放在缓冲中
	rfcommandReceivedBuf[rfcommandReceivedBufCount++] = b;

	//根据接收状态处理
	switch (rfcommandReceivedState)
	{
	case ReceivedRFCommandIdle:
		if (b == 0x5A)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A;
		}
		else
		{//错误
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A:
		if (b == 0x5A)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A;
		}
		else
		{//错误
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A:
		if (b == 0x00)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00;
		}
		else
		{//错误
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A00:
		if (b == 0x00)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A0000;
		}
		else
		{//错误
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A0000:
		if (b == 0x5A)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A;
		}
		else
		{//错误
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A00005A:
		if (b == 0x00)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A00;
		}
		else
		{//错误
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A00005A00:
		switch (b)
		{
		case 0x01:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0001;
			break;
		case 0x02:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0002;
			break;
		case 0x03:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0003;
			break;
		case 0x04:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0004;
			break;
		case 0x05:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0005;
			break;
		case 0x06:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0006;
			break;
		case 0x0E:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A000E;
			break;
		case 0x15:
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0015;
			break;
		default://错误
			OnRFComamndTextParseError();
			break;
		}
		break;
	case ReceivedRFCommandByte5A5A00005A0001:
	case ReceivedRFCommandByte5A5A00005A0002:
	case ReceivedRFCommandByte5A5A00005A0003:
	case ReceivedRFCommandByte5A5A00005A0004:
	case ReceivedRFCommandByte5A5A00005A0005:
	case ReceivedRFCommandByte5A5A00005A0006:
		if ((rfcommandReceivedBufCount == 8 && b != 0x02) || (rfcommandReceivedBufCount == 12 && b != 0x0D) || (rfcommandReceivedBufCount == 13 && b != 0x0A))
		{
			OnRFComamndTextParseError();
		}
		if (b == 0x0A && rfcommandReceivedBuf[rfcommandReceivedBufCount - 2] == 0x0D && rfcommandReceivedBufCount < 13)
		{
			OnRFComamndTextParseError();
		}
		if (rfcommandReceivedBufCount == 13)
		{
			int checksumReceived = rfcommandReceivedBuf[rfcommandReceivedBufCount - 3];
			int checksumCalculate = CountRFModuleCheckSum(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount - 3);
			
			if (checksumReceived == checksumCalculate)
			{
				OnRFComamndTextParseOk();
			}
			else
			{
				OnRFComamndTextParseError();
			}
		}
		break;
	case ReceivedRFCommandByte5A5A00005A000E:
	case ReceivedRFCommandByte5A5A00005A0015:
		if ((rfcommandReceivedBufCount == 8 && b != 0x04) || (rfcommandReceivedBufCount == 14 && b != 0x0D) || (rfcommandReceivedBufCount == 15 && b != 0x0A))
		{
			OnRFComamndTextParseError();
		}
		if (rfcommandReceivedBufCount == 15)
		{
			int checksumReceived = rfcommandReceivedBuf[rfcommandReceivedBufCount - 3];
			int checksumCalculate = CountRFModuleCheckSum(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount - 3);
                            
			if (checksumReceived == checksumCalculate)
			{
				OnRFComamndTextParseOk();
			}
			else
			{
				OnRFComamndTextParseError();
			}
		}
		break;
	default:
		OnRFComamndTextParseError();
		break;
	}
}

/*-------------------------------------------------------------------------
	函数名：OnRFComamndTextParseError
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFComamndTextParseError(void)
{
   //不是RFCommand, 那么就是普通的USV消息
   ParseRFSessionText(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount);

   ResetRFCommandReceivedState();
}


/*-------------------------------------------------------------------------
	函数名：ResetRFCommandReceivedState
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ResetRFCommandReceivedState(void)
{
   rfcommandReceivedBufCount = 0;
   rfcommandReceivedState = ReceivedRFCommandIdle;
   //rfcommandExpectState = RFComamndExpectStates.Idle;
}


/*-------------------------------------------------------------------------
	函数名：CountRFModuleCheckSum
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
int CountRFModuleCheckSum(u8* bytes, int offset, int count)
{
	int checksum = 0;
	int i;
	
	for (i = offset; i < offset + count; i++)
	{
		checksum += bytes[i];
	}
	return (checksum & 0xff);
}


/*-------------------------------------------------------------------------
	函数名：OnRFComamndTextParseOk
	功  能：433参数配置解析
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFComamndTextParseOk(void)
{
	switch (rfcommandReceivedState)
	{
	//串口波特率和校验
	case ReceivedRFCommandByte5A5A00005A0001:
	case ReceivedRFCommandByte5A5A00005A0002:
		break;
   //RF波特率
  case ReceivedRFCommandByte5A5A00005A0003:
	case ReceivedRFCommandByte5A5A00005A0004:
		break;
	//RF信道
	case ReceivedRFCommandByte5A5A00005A0005:
	case ReceivedRFCommandByte5A5A00005A0006:
		break;
	//RF信道，RF波特率，串口波特率，串口校验
	case ReceivedRFCommandByte5A5A00005A000E:
	case ReceivedRFCommandByte5A5A00005A0015:
		break;
	default:
		break;
	}

  //ToDo: 处理RFCommand消息
	ProcessRFModuleMessage(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount - 2);

	//重置
	ResetRFCommandReceivedState();
}


/*-------------------------------------------------------------------------
	函数名：ProcessRFModuleMessage
	功  能：433数传模块命令接收处理
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ProcessRFModuleMessage(u8* allBytes, int offset, int count)
{
//	int operationCode = (allBytes[offset + 5] << 8) + allBytes[offset + 6];
//	u8 len = allBytes[offset + 7];

//	if (count < 10)
//		return;

//	if (IsRFModuleCommandLengthOk(allBytes, offset, count))
//	{
//		if (IsRFModuleCheckSumOk(allBytes, offset, count))
//		{
//			//设置RF连接
//			//session.Status = SessionStatus.ConnectedRF;
//			//先保存参数
//			switch (operationCode)
//			{
//			case 0x01://应答:设置串口参数
//			case 0x02://应答:读取串口参数
//				//ProcessRFModuleSerialParameter(allBytes[8], allBytes[9]);
//				RFSessionPortBaudRateCode = allBytes[8];
//				RFSessionPortParity = allBytes[9];
//				//log.Debug("串口参数: 波特率 " + RFDataService.PortBaudRate + ", 校验 " + RFDataService.PortParity);
//				break;
//			case 0x03://应答:设置空中参数
//			case 0x04://应答:读取空中参数
//				//ProcessRFModuleAirParameter(allBytes[8], allBytes[9]);
//				RFSessionAirBaudRateCode = allBytes[8];
//				//log.Debug("空中参数: 波特率 " + RFDataService.AirBaudRate);
//				break;
//			case 0x05://应答:设置信道参数
//				//ProcessRFModuleChannelParameter(allBytes[8], allBytes[9]);
//				RFSessionAirChannel = allBytes[8];
//				if(RFSessionAirChannel != GlobalVariable.RFSessionAirChannel)
//				{
//					GlobalVariable.RFSessionAirChannel = RFSessionAirChannel;
//					ShowRFChannel();
//				}
//				//log.Debug("空中参数: 信道 " + RFDataService.AirChannel);
//				//读取USV
//				//RFSessionReadUSVSetting();
//				break;
//			case 0x06://应答:读取信道参数
//				//ProcessRFModuleChannelParameter(allBytes[8], allBytes[9]);
//				RFSessionAirChannel = allBytes[8];
//				break;
//			case 0x15://应答：读参数
//			case 0x0E://应答：写参数
//				RFSessionAirChannel = allBytes[8];
//				RFSessionAirBaudRateCode = allBytes[9];
//				RFSessionPortBaudRateCode = allBytes[10];
//				RFSessionPortParity = allBytes[11];
//				break;
//			default:
//				//log.Error("ProcessRFModuleMessage: unknown message:\r\n" + RFDataService.DumpBytes(allText));
//				//返回，不触发事件
//				return;
//			}
//			//后触发事件
//			//RFDataService.FireRFCommandReceived(operationCode, allBytes[8], allBytes[9]);
//		}
//		else
//		{
//			//log.Error("ProcessRFModuleMessage: invalid cs message:\r\n" + RFDataService.DumpBytes(allText));
//		}
//	}
//	else
//	{
//		//log.Error("ProcessRFModuleMessage: invalid len message:\r\n" + RFDataService.DumpBytes(allText));
//	}
}


/*-------------------------------------------------------------------------
	函数名：ParseRFSessionText
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ParseRFSessionText(u8* rfcommRecvedBuf, int offset, int count)
{
	int startIndex = offset;
	int endIndex = offset + count;
	
	for(;startIndex < endIndex; startIndex++)
	{
		ParseRFSessionByte(rfcommRecvedBuf[startIndex]);
	}
}


/*-------------------------------------------------------------------------
	函数名：ParseRFSessionByte
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ParseRFSessionByte(u8 b)
{
	switch (receivedState)
	{
		case RFSessionReceivedByteIdle:
			OnRFSessionReceivedStatesIdle(b);
			break;
		case RFSessionReceivedByte00:
			OnRFSessionReceivedStatesByte00(b);
			break;
		case RFSessionReceivedByte42:
			OnRFSessionReceivedStatesByte42(b);
			break;
		case RFSessionReceivedByte43:
			OnRFSessionReceivedStatesByte43(b);
			break;
		case RFSessionReceivedByte53:
			OnRFSessionReceivedStatesByte53(b);
			break;
		case RFSessionReceivedByte4201:
		case RFSessionReceivedByte4234:
		case RFSessionReceivedByte42FB://水面机器人启动完毕\x00\x42\xFB\x7C
		case RFSessionReceivedByte42F9://水面机器人已找到GPS信号\x00\x42\xF9\x7C
		case RFSessionReceivedByte426E:
		case RFSessionReceivedByte4300:
		case RFSessionReceivedByte4301:
		case RFSessionReceivedByte4303:
		case RFSessionReceivedByte4304:
		case RFSessionReceivedByte4332:
		case RFSessionReceivedByte4307:
		case RFSessionReceivedByte4341:
		case RFSessionReceivedByte4343:
		case RFSessionReceivedByte434E:
		case RFSessionReceivedByte4352:
		case RFSessionReceivedByte4353:
		case RFSessionReceivedByte4361:
		case RFSessionReceivedByte436E:
		case RFSessionReceivedByte4374:
		case RFSessionReceivedByte5343:
		case RFSessionReceivedByte5346:
		case RFSessionReceivedByte5353:
			OnRFSessionExpectByte(b);
			break;

		case RFSessionReceivedByteFA:
			OnReceivedByteFA(b);
			break;
		//case RFSessionReceivedStates.ReceivedByteFB:
		//    OnReceivedByteFB(b);
		//    break;
		case RFSessionReceivedByteFC:
			OnReceivedByteFC(b);
			break;
		case RFSessionReceivedByteFD:
			OnReceivedByteFD(b);
			break;

		case RFSessionReceivedByteFAFA:
		//case ReceivedByteFBFB:
		case RFSessionReceivedByteFCFC:
		case RFSessionReceivedByteFDFD:
			OnExpectByteFE(b);
			break;
		default:
			OnRFSessionParseError();
			break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionParseError
	功  能：错误收到消息，处理错误
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionParseError(void)
{
	//log.Error("ERROR RF MSG: " + RFDataService.DumpBytes(receivedBuf, 0, receivedBufCount));
	ResetReceivedState();
}


/*-------------------------------------------------------------------------
	函数名：OnExpectByteFE
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnExpectByteFE(u8 c)
{
	receivedBuf[receivedBufCount++] = c;
	switch (expectState)
	{
	case RFSessionExpectByteFE:
		switch (c)
		{
		case 0xFE:
			expectState = RFSessionExpectByteFEFE;
			break;
		case 0xFA:
		//case 0xFB:
		case 0xFC:
		case 0xFD:
			//重置
			ResetReceivedState();
			OnRFSessionReceivedStatesIdle(c);
			break;
		default:
			if(IsValidBase64Byte(c) != TRUE)
			{
				ResetReceivedState();
			}
			break;
		}

		if (receivedBufCount >= sizeof(receivedBuf))
		{
			OnRFSessionParseError();
		}
		break;
	case RFSessionExpectByteFEFE:
		if (c != 0xFE)
		{
			OnRFSessionParseError();
		}
		else
		{
			OnRFSessionParseOk();
		}
		break;
	default:
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：IsValidBase64Byte
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
bool IsValidBase64Byte(u8 c)
{
	if((c >= 'A') && (c <= 'Z'))  
	{   
		return TRUE;  
	}
	else if((c >= 'a') && (c <= 'z'))  
	{   
		return TRUE;  
	}else if((c >= '0') && (c <= '9'))  
	{   
		return TRUE;  
	}else if(c == '+')  
	{   
		return TRUE;  
	}else if(c == '/')  
	{   
		return TRUE;  
	}else if(c == '=')  
	{   
		return TRUE;  
	}  
	return FALSE;  
}


/*-------------------------------------------------------------------------
	函数名：OnReceivedByteFD
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnReceivedByteFD(u8 c)
{
	receivedBuf[receivedBufCount++] = c;
	switch (c)
	{
	case 0xFD:
		receivedState = RFSessionReceivedByteFDFD;
		expectState = RFSessionExpectByteFE;
		break;
	default:
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnReceivedByteFC
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnReceivedByteFC(u8 c)
{
	receivedBuf[receivedBufCount++] = c;
	switch (c)
	{
	case 0xFC:
		receivedState = RFSessionReceivedByteFCFC;
		expectState = RFSessionExpectByteFE;
		break;
	default:
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnReceivedByteFA
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnReceivedByteFA(u8 c)
{
	receivedBuf[receivedBufCount++] = c;
	switch (c)
	{
	case 0xFA:
		receivedState = RFSessionReceivedByteFAFA;
		expectState = RFSessionExpectByteFE;
		break;
	default:
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionExpectByte
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionExpectByte(u8 b)
{
	switch (expectState)
	{
	case RFSessionExpectIdle:
		OnRFSessionExpect7C(b);
		break;
	case RFSessionExpectByte0D:
		OnRFSessionExpect0D(b);
		break;
	case RFSessionExpectByte0A:
		OnRFSessionExpect0A(b);
		break;
	default:
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionExpect0A
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionExpect0A(u8 b)
{
	receivedBuf[receivedBufCount++] = b;
            
	if (b != 0x0A && receivedBuf[receivedBufCount - 2] != 0x0D)
	{
		OnRFSessionParseError();
	}
	else
	{
		OnRFSessionParseOk();
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionExpect0D
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionExpect0D(u8 b)
{
	receivedBuf[receivedBufCount++] = b;
	switch (receivedState)
	{
	//固定7字节消息, 没有长度字节
	case RFSessionReceivedByte4303://切换到自动模式: 0x00, 0x43, 0x03, 0x7C, 0xC2, 0x0D, 0x0A
	case RFSessionReceivedByte4304://切换到手动模式: 0x00, 0x43, 0x04, 0x7C, 0xC3, 0x0D, 0x0A
	case RFSessionReceivedByte4307://取消任务      : 0x00, 0x43, 0x07, 0x7C, 0xC6, 0x0D, 0x0A
	case RFSessionReceivedByte4332://暂停发送      : 0x00, 0x43, 0x32, 0x7C, 0xC1, 0x0D, 0x0A
	case RFSessionReceivedByte4341://开警报        : 0x00, 0x43, 0x41, 0x7C, 0x00, 0x0D, 0x0A
	case RFSessionReceivedByte4352://恢复任务      : 0x00, 0x43, 0x52, 0x7C, 0x64, 0x0D, 0x0A
	case RFSessionReceivedByte4353://暂停任务      : 0x00, 0x43, 0x53, 0x7C, 0x65, 0x0D, 0x0A
	case RFSessionReceivedByte4361://关警报        : 0x00, 0x43, 0x61, 0x7C, 0x20, 0x0D, 0x0A
		if (receivedBufCount >= 7)
		{
			if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
			{
				OnRFSessionParseOk();
			}
			else
			{//错误
				OnRFSessionParseError();
			}
		}
		break;
	//固定8字节消息, 没有长度字节
	//拍照    : 0x00, 0x43, 0x43, 0x7C, 0x41, 0x43, 0x0D, 0x0A;
	//录像开始: 0x00, 0x43, 0x43, 0x7C, 0x51, 0x53, 0x0D, 0x0A;
	//录像停止: 0x00, 0x43, 0x43, 0x7C, 0x61, 0x63, 0x0D, 0x0A;
	case RFSessionReceivedByte4343:
		if (receivedBufCount >= 8)
		{
			if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
			{
				OnRFSessionParseOk();
			}
			else
			{//错误
				OnRFSessionParseError();
			}
		}
		break;
	//固定6或7字节消息, 没有长度字节， 0x7C后要紧跟0x0D的消息
	case RFSessionReceivedByte436E://读USVSetting  : 0x00, 0x43, 0x6E, 0x7C, 0x0D, 0x0A
	case RFSessionReceivedByte5343://手动采样停止  ：0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
	case RFSessionReceivedByte5346://手动采样完成  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
		if (b == 0x0D)
		{
			expectState = RFSessionExpectByte0A;
		}
		else
		{ //错误
			OnRFSessionParseError();
		}
		break;
		//固定11字节消息, 没有长度字节
		case RFSessionReceivedByte5353://手动采样      : 0x00, 0x53, 0x53, 0x7C, n, d, m, v, w, xx, 0x0D, 0x0A; 0x00, 0x53, 0x53, 0x7C, n, d, m, v, w, CS1, CS2, 0x0D, 0x0A 
			//if (receivedBufCount >= 13)//11, 12, 13
			{
				if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
				{
					OnRFSessionParseOk();
				}
				else
				{
					if (receivedBufCount >= 13)
					{
						//错误
						OnRFSessionParseError();
					}
				}
			}
			break;
	//变长消息，含长度字节, 含\r\n长度79
	case RFSessionReceivedByte426E://返回USVSetting：0x00, 0x42, 0x6E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A
	case RFSessionReceivedByte434E://设置USVSetting：0x00, 0x43, 0x4E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A
		//if (receivedBufCount > 79)
		if (receivedBufCount > 82)
		{//超过消息长度, 错误
			OnRFSessionParseError();
		}
		if (receivedBufCount > 74 && b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]))
		{//如果\r\n结尾，而且长度字节符合
			//expectState = RFSessionExpectStates.ExpectByte0A;
			//if (BoatDataService.IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
			{//校验OK
				OnRFSessionParseOk();
			}
			else
			{//校验错
				OnRFSessionParseError();
			}
		}
		break;
	//固定长度9字节消息，无长度字节
	case RFSessionReceivedByte42FB://主控就绪      ：0x00, 0x42, 0xFB, 0x7C, 0x01, 0xB9, 0x07, 0x0D, 0x0A
	case RFSessionReceivedByte42F9://GPS就绪       : 0x00, 0x42, 0xF9, 0x7C, 0x01, 0xB7, 0x07, 0x0D, 0x0A
		if (receivedBufCount >= 9)
		{
			if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
			{
				OnRFSessionParseOk();
			}
			else
			{//超过消息长度, 错误
				OnRFSessionParseError();
			}
		}
		break;
	//变长64-161字节消息，含2字节校验和1字节长度，
	case RFSessionReceivedByte4201://自动模式船数据：0x00, 0x42, 0x01, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
		if (receivedBufCount > 200)
		{//超过消息长度, 错误
				OnRFSessionParseError();
		}
		else
		{
			if (receivedBufCount > 58 && b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]) && receivedBuf[receivedBufCount - 6] == 0x23)
			{//如果\r\n结尾，而且长度字节符合
				//expectState = RFSessionExpectStates.ExpectByte0A;
				//if (BoatDataService.IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
				if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
				{//校验OK
					OnRFSessionParseOk();
				}
				else
				{//校验错
					OnRFSessionParseError();
				}
			}
		}
		break;
	//变长36字节消息，含2字节校验和1字节长度
	case RFSessionReceivedByte4234://手动模式船数据：0x00, 0x42, 0x34, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
		//if (receivedBufCount > 36)
		//if (receivedBufCount > 49)//增加了温度，横摇，纵摇的数据
		if (receivedBufCount > 64)
		{//超过消息长度, 错误
			OnRFSessionParseError();
		}
		else
		{
			if (receivedBufCount >= 25 && b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]) && receivedBuf[receivedBufCount - 6] == 0x23)
			{//如果\r\n结尾，而且长度字节符合
				//expectState = RFSessionExpectStates.ExpectByte0A;
				//if (BoatDataService.IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
				if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
				{//校验OK
					OnRFSessionParseOk();
				}
				else
				{//校验错
					OnRFSessionParseError();
				}
			}
		}
		break;
	//变长消息，含2字节校验和1字节长度
	case RFSessionReceivedByte4301://路径点设置命令：0x00, 0x43, 0x01, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
	case RFSessionReceivedByte4374://测试路径点设置：0x00, 0x43, 0x74, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
		if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]))
		{//如果\r\n结尾，而且长度字节符合
			//if (IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
			{//校验OK
				OnRFSessionParseOk();
			}
			else
			{//校验错
				OnRFSessionParseError();
			}
		}
		if (receivedBufCount >= sizeof(receivedBuf))
		{//长度错
			OnRFSessionParseError();
		}
		break;
	//变长消息，含2字节校验，没有长度字节
	case RFSessionReceivedByte4300://手动控制命令  : 新0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, LEN, 0D, 0A; 老：0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, 0D, 0A
		if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
		{//如果\r\n结尾，而且长度字节符合
			//if (IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			{//校验OK
				OnRFSessionParseOk();
			}
			//else
			{//校验错
			//    OnRFSessionParseError();
			}
		}
		if (receivedBufCount > 32)
		{//长度错
			OnRFSessionParseError();
		}
		break;
	default:
		if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
		{//如果\r\n结尾，而且长度字节符合
			//if (IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			{//校验OK
				OnRFSessionParseOk();
			}
			//else
			{//校验错
			//    OnRFSessionParseError();
			}
		}
		if (receivedBufCount > 32)
		{//长度错
			OnRFSessionParseError();
		}
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：RFSessionCheckSum
	功  能：计算一串信息的CheckSum
	参  数：data						byte数组形式信息
					startindex			开始索引
					len							计算长度

	返回值：计算出的CheckSum
-------------------------------------------------------------------------*/
u16 RFSessionCheckSum(u8* data, int startindex, int len)
{
	int endindex = startindex + len;
	u16 result = 0;
	int i;

	for (i = startindex; i < endindex; ++i)
	{
		result += data[i];
	}
	return result;
}


/*-------------------------------------------------------------------------
	函数名：IsBoatDataMessageChecksumOk
	功  能：
	参  数：len，包含CS1，CS2， LEN的计数
	返回值：
-------------------------------------------------------------------------*/
bool IsBoatDataMessageChecksumOk(u8* encode_alldata, int startindex, int len)
{
	int endindex = startindex + len;
	u16 countValue = RFSessionCheckSum(encode_alldata, startindex, len - 3);//CS1，CS2， LEN
	u16 receivedValue = (u16)(encode_alldata[endindex - 3] * 256 + encode_alldata[endindex - 2]);

	if (countValue != receivedValue)
	{
		//log.Warn("BoatDataMessageChecksum Error: C " + countValue.ToString("X4") + "R " + receivedValue.ToString("X4") + ", " + RFDataService.DumpBytes(encode_alldata, startindex, len));
	}

	return countValue == receivedValue;
}


/*-------------------------------------------------------------------------
	函数名：ProcessBoatDataMessage
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ProcessBoatDataMessage(u8* bytearray, int startindex, int len)
{
  //int count = 0;
  int endindex = startindex + len;
	float depth = 0;
	
	//if (IsBoatDataMessageChecksumOk(bytearray, startindex, len))
	{
		startindex += 4;
	
   if (len <= 4)
   		return;		
	 
   //while (count <= 20)
	 while(startindex < endindex)
   {
			switch (bytearray[startindex])
			{
			case 1:
	    {
					if (startindex + 8 > endindex) //防止一条消息只收了一部分
						return;
	        BoatData.Latitude[0] = bytearray[startindex+1];
					BoatData.Latitude[1] = bytearray[startindex+2];
					BoatData.Latitude[2] = bytearray[startindex+3];
					BoatData.Latitude[3] = bytearray[startindex+4];
					BoatData.Latitude[4] = bytearray[startindex+5];
					BoatData.Latitude[5] = bytearray[startindex+6];
					BoatData.Latitude[6] = bytearray[startindex+7];
					BoatData.Latitude[7] = bytearray[startindex+8];
					
					BoatData.DoubleLatitude = *(double*)BoatData.Latitude;
					
					//result.GpsData.Latitude = BitConverter.ToDouble(bytearray, startindex + 1);
						startindex += 8 + 1;
					//if (startindex >= endindex)
					//	return;
	         break;
	    }
	    //GPS Longitude
	    case 2:
	    {
					if (startindex + 8 > endindex) //防止一条消息只收了一部分
						return;
	        BoatData.Longitude[0] = bytearray[startindex+1];
					BoatData.Longitude[1] = bytearray[startindex+2];
					BoatData.Longitude[2] = bytearray[startindex+3];
					BoatData.Longitude[3] = bytearray[startindex+4];
					BoatData.Longitude[4] = bytearray[startindex+5];
					BoatData.Longitude[5] = bytearray[startindex+6];
					BoatData.Longitude[6] = bytearray[startindex+7];
					BoatData.Longitude[7] = bytearray[startindex+8];
					
					BoatData.DoubleLongitude = *(double*)BoatData.Longitude;
					
	        startindex += 8 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	    }
	    case 3:
	    {
					if (startindex + 2 > endindex) //防止一条消息只收了一部分
						return;
					BoatData.Speed = ((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]);
	                
//				LCD_ClearArea(100,100,31,60);
//				sprintf((u8*)TxBuffer2,"%d", bytearray[startindex + 1]);
//				LCD_WR_S_String(100,100,TxBuffer2);
//				LCD_ClearArea(100,150,31,60);
//				sprintf((u8*)TxBuffer2,"%d", bytearray[startindex + 2]);
//				LCD_WR_S_String(100,150,TxBuffer2);
					startindex += 2 + 1;	
					//if (startindex >= endindex)
					//	return;
	        break;
	    }
	    case 4:      //GPS time 3 byte
//	      result.USVTimeData.Hour = bytearray[startindex + 1];
//	      result.USVTimeData.Minute = bytearray[startindex + 2];
//	      result.USVTimeData.Second = bytearray[startindex + 3];
	        startindex += 3 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	    case 5:      //GPS date 3 byte
//	      result.USVDateData.Year = bytearray[startindex + 1];
//	      result.USVDateData.Month = bytearray[startindex + 2];
//	      result.USVDateData.Day = bytearray[startindex + 3];
	        startindex += 3 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	   case 6:      //remaining battery 1 byte
					if (startindex + 1 > endindex) //防止一条消息只收了一部分
						return;
	        BoatData.BatLife =  bytearray[startindex + 1];
					//测试
					//ShowBattery(BoatData.BatLife);
					
	        startindex += 1 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
		 case 7:
				startindex += 1 + 1;
         break;
	   case 10:    // pitch 2 byte
					if (startindex + 2 > endindex) //防止一条消息只收了一部分
						return;
	        BoatData.Pitch = (u16)((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]); 
	        startindex += 2 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	   case 11:    //roll 2 byte
					if (startindex + 2 > endindex) //防止一条消息只收了一部分
						return;
					BoatData.Roll = (u16)((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]); 
	        startindex += 2 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	   case 12:    //yaw 2 byte
	        //float tempfloat = (float)(((Int16)(bytearray[startindex + 1] * 256 + (int)bytearray[startindex + 2])) / 10.0f);                    
					if (startindex + 2 > endindex) //防止一条消息只收了一部分
						return;
					BoatData.Yaw = (u16)((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]);                        
	        startindex += 2 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	    case 14:
	    {
					if (startindex + 1 > endindex) //防止一条消息只收了一部分
						return;
	        if (bytearray[startindex + 1] == 65)
					{
						BoatData.Mode = 'A';
					}
	        //result.RobotInfoData.CurrentMode = Mode.Auto;
	        else if (bytearray[startindex + 1] == 77)
					{
						BoatData.Mode = 'M';
					}
					//result.RobotInfoData.CurrentMode = Mode.ManualRF;
	        startindex += 1 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
			}
			case 15:
			{
	        if (startindex + 2 > endindex) //防止一条消息只收了一部分
						return;
					BoatData.Status = (u32)((u32)bytearray[startindex + 1] * 256 + (u32)bytearray[startindex + 2]);
	        startindex += 2 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	    }
			case 17:
	    {
						//result.UltraSonicData.Sensor1Distance = ((int)(bytearray[startindex + 1]) * 256) + ((int)bytearray[startindex + 2]);
						startindex += 2 + 1;
						//if (startindex >= len)
						//	return;
						break;
			}
	    case 18:
	    {
						//result.UltraSonicData.Sensor2Distance = ((int)(bytearray[startindex + 1]) * 256) + ((int)bytearray[startindex + 2]);
						startindex += 2 + 1;
						//if (startindex >= endindex)
						//  return;
	                break;
	    }
			case 19:
			{
						//board temperature 1byte
		        //result.RobotInfoData.Temperature = (int)(bytearray[startindex + 1]) - 40;
						if (startindex + 1 > endindex) //防止一条消息只收了一部分
							return;
						BoatData.InternalTemp = (bytearray[startindex + 1]) - 40;
		        startindex += 1 + 1;
						//if (startindex >= endindex)
						//	return;
		        break;
			}
	    case 20:
	    {
	           //result.RobotInfoData.ErrorMessage = (BoatError)BitConverter.ToInt32(bytearray, startindex + 1);
	           startindex += 1 + 4;
						//if (startindex >= endindex)
						//	return;
	          break;
	    }
			case 22:
			{
						startindex += 1 + 2;
						//if (startindex >= endindex)
						//	return;
	          break;
			}
			case 43://0x2b, 当前正在采水的瓶号
			{
					if (startindex + 1 > endindex) //防止一条消息只收了一部分
						return;
					BoatData.Bottle = bytearray[startindex + 1];	

					if(bottleNumberLocal != BoatData.Bottle && BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)
					{
//						LCD_ClearArea(2, 55, 108, 175);
//						LCD_WR_L_HanZi_String(30, 79 , (u8*)zheng zai cai yang);	//正在采样
//						LCD_WR_L_HanZi_String(27, 120 , (u8*)zhi_2 );
						if (BoatData.Bottle < 10)
						{
//							LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Bottle + 48);
//							LCD_WR_L_HanZi_String(91, 120 , (u8*)hao ping );
						}
						//10-20个瓶子
						else if (BoatData.Bottle < 20 && BoatData.Bottle >= 10)
						{
//							LCD_WR_L_Char(57, 120, 1 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Bottle - 10 + 48);
//							LCD_WR_L_HanZi_String(91, 120 , (u8*)hao ping );
						}
					}
					bottleNumberLocal = BoatData.Bottle;	 

					startindex += 1 + 1;
					//if (startindex >= endindex)
					//	return;
					break;
			}
	    case 81:
	    {
					if (startindex + 4 > endindex) //防止一条消息只收了一部分
						return;
	        //result.WaterParaData.Temperature = (float)Math.Round(BitConverter.ToSingle(bytearray, startindex + 1), 2);
	        BoatData.WaterTemp = BytesToFloat(&(bytearray[startindex + 1]));
					startindex += 4 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	    }
			case 82:
			{
					startindex += 1 + 4;
					//if (startindex >= endindex)
					//	return;
         break;
			}
			case 83:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 84:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 85:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 86:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 87:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 88:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 89:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 90:
			{
				startindex += 1 + 4;
				//if (startindex >= endindex)
				//	return;
        break;
			}
			case 91://深度
			{
					if (startindex + 4 > endindex) //防止一条消息只收了一部分
						return;
	        //result.WaterParaData.Depth = (float)Math.Round(BitConverter.ToSingle(bytearray, startindex + 1), 2);
	        depth = BytesToFloat(&(bytearray[startindex + 1]));
					startindex += 4 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
			}

	    case 35:
	    {
				startindex ++;break;
	       //return;
	    }
	    default:
				startindex ++;break;
				//return;
				//break;
	    }
	    //count++;
   }
	}
	//else
	{
		//log.Error("RF Module ProcessData: CheckSum error");
	}
	BoatData.Depth = depth;
}


/*-------------------------------------------------------------------------
	函数名：BytesToFloat
	功  能：将4个字节数据转换为字符数
	参  数：data		字节数组
	返回值：浮点数
-------------------------------------------------------------------------*/
float BytesToFloat(u8* data)
{ 
    float a = 0; 
    u8 ii; 
    void *pf; 
    u8* px = data; 
    pf = &a; 
    for(ii=0;ii<4;ii++) 
    { 
        *((u8 *)pf+ii)=*(px+ii); 
    } 
    return a; 
} 


/*-------------------------------------------------------------------------
	函数名：OnRFSessionParseOk
	功  能：成功收到消息，处理消息 
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionParseOk(void)
{
	//TIM_Cmd(TIM3, DISABLE); //关闭超时计数中断3//20140730
	//清零超时计数
	BoatDataTimeoutCount = 0;
	connect_ok = 1;
	//GlobalVariable.IsNeedDisplayBoatData = 1;
	//显示RF状态
//	ShowRFChannel();
	
	switch (receivedState)
	{
		case RFSessionReceivedByte4201://自动模式船数据：0x00, 0x42, 0x01, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
			//收到自动模式数据
			ProcessBoatDataMessage(receivedBuf, 0, receivedBufCount - 2);
			//显示
//			ShowAutoModeBoatData();
			//显示电量
//			ShowBoatBatteryLife();
			receivedManualMessageCount = 0;
			break;
		case RFSessionReceivedByte4234://手动模式船数据：0x00, 0x42, 0x34, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
			//收到手动模式数据
			ProcessBoatDataMessage(receivedBuf, 0, receivedBufCount - 2);
			
//			if(GlobalVariable.CurrentWindow == WIN_Manual)
//			{
//				ShowManualModeBoatData();
//			}
			//显示电量
//			ShowBoatBatteryLife();
			if(++receivedManualMessageCount > 100)
			{
				receivedManualMessageCount = 100;
			}
			break;
		case RFSessionReceivedByte426E://返回USVSetting：0x00, 0x42, 0x6E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. 变长消息，含长度字节, 最大长度70
			//USVMessageReceivedStatusText = DateTime.Now + ": 收到USVSetting";
			ProcessUSVSettingMessage(receivedBuf, 0, receivedBufCount - 5);
			break;
		case RFSessionReceivedByteFAFA:
			//USVMessageReceivedStatusText = DateTime.Now + ": 收到USV加密数据";
			//u8 *decodedBytes = Base64.Decode(receivedBuf, 2, receivedBufCount - 4);
			//decodedBufferCount = base64_decode((char *)decodedBuffer, (char *)receivedBuf + 2, receivedBufCount - 4);
			decodedBufferCount = fnBase64Decode((char *)decodedBuffer, (char *)receivedBuf + 2, receivedBufCount - 4);
			//
			ProcessBoatDataMessageNew(decodedBuffer, 0, decodedBufferCount);
			//重置
			//ResetReceivedState();
			//调用明文处理
			//ParseRFSessionText(decodedBytes, 0, decodedBytes.Length);
			//显示
//			if (GlobalVariable.CurrentWindow == WIN_Auto)
//			{
//				ShowAutoModeBoatData();
//			}
//			else if(GlobalVariable.CurrentWindow == WIN_Manual)
//			{
//				ShowManualModeBoatData();
//			}
			//显示电量
//			ShowBoatBatteryLife();
			break;
		//case ReceivedByteFBFB:
		case RFSessionReceivedByteFCFC:
		case RFSessionReceivedByteFDFD:
			break;

		case RFSessionReceivedByte5353://手动采样      : 0x00, 0x53, 0x53, 0x7C, n, d(5), m(0), v, w, 0x0D, 0x0A 
			//RFDataService.IsSampling = true;
			break;
		case RFSessionReceivedByte5343://手动采样停止  ：0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
		case RFSessionReceivedByte5346://手动采样完成  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
			//SampleInfo.IsBottleTaken[receivedBuf[3]-1] = TRUE;
			SampleInfo.IsBottleTaking[receivedBuf[3]-1] = FALSE;
			GlobalVariable.IsManualSampling = FALSE;
			//IsSampling = false;
//			if(GlobalVariable.CurrentWindow == WIN_Manual)
//			{
//					ShowSampling();
//			}
			break;

		case RFSessionReceivedByte42FB://主控就绪      ：0x00, 0x42, 0xFB, 0x7C, 0x01, 0xB9, 0x07, 0x0D, 0x0A
		case RFSessionReceivedByte42F9://GPS就绪       : 0x00, 0x42, 0xF9, 0x7C, 0x01, 0xB7, 0x07, 0x0D, 0x0A

		case RFSessionReceivedByte4300://手动控制命令  : 0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
		case RFSessionReceivedByte4301://路径点设置命令：0x00, 0x43, 0x01, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
		case RFSessionReceivedByte4303://切换到自动模式: 0x00, 0x43, 0x03, 0x7C, 0xC2, 0x0D, 0x0A
		case RFSessionReceivedByte4304://切换到手动模式: 0x00, 0x43, 0x04, 0x7C, 0xC3, 0x0D, 0x0A
		case RFSessionReceivedByte4332://暂停发送      : 0x00, 0x43, 0x32, 0x7C, 0xC1, 0x0D, 0x0A
		case RFSessionReceivedByte4307://取消任务      : 0x00, 0x43, 0x07, 0x7C, 0xC6, 0x0D, 0x0A
		case RFSessionReceivedByte4341://开警报        : 0x00, 0x43, 0x41, 0x7C, 0x00, 0x0D, 0x0A
		case RFSessionReceivedByte4343://拍照          : 0x00, 0x43, 0x43, 0x7C, 0x41, 0x43, 0x0D, 0x0A; 录像开始: 0x00, 0x43, 0x43, 0x7C, 0x51, 0x53, 0x0D, 0x0A; 录像停止: 0x00, 0x43, 0x43, 0x7C, 0x61, 0x63, 0x0D, 0x0A;
		case RFSessionReceivedByte434E://设置USVSetting：0x00, 0x43, 0x4E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. 变长消息，含长度字节, 最大长度70
		case RFSessionReceivedByte4352://恢复任务      : 0x00, 0x43, 0x52, 0x7C, 0x64, 0x0D, 0x0A
		case RFSessionReceivedByte4353://暂停任务      : 0x00, 0x43, 0x53, 0x7C, 0x65, 0x0D, 0x0A
		case RFSessionReceivedByte4361://关警报        : 0x00, 0x43, 0x61, 0x7C, 0x20, 0x0D, 0x0A
		case RFSessionReceivedByte436E://读USVSetting  : 0x00, 0x43, 0x6E, 0x7C, 0x0D, 0x0A
		case RFSessionReceivedByte4374://测试路径点设置：0x00, 0x43, 0x74, 0x7C, ..., CS1, CS2, LEN, 0D, 0A

		default:
			break;
	}
	
	//TIM_Cmd(TIM3, ENABLE);//打开超时计数中断3//20140730 
}


/*-------------------------------------------------------------------------
	函数名：ProcessBoatDataMessageNew
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ProcessBoatDataMessageNew(u8* databytes, int startindex, int len)
{
	int endindex = startindex + len;
	u8 header[3];
	int bottle_number;
	
	//Navigator.Log.Debug("ProcessMessage:" + data);
	//log.Debug("RF RECV MSG:" + USV.Logging.LogService.DumpBytes(databytes));

	if (len < 4)
	{
		//log.Debug("RF ProcessMessage: message is too short");
		return;
	}


	header[0] = databytes[startindex + 0];
	header[1] = databytes[startindex + 1];
	header[2] = databytes[startindex + 2];

	if (header[0] == 0x00 && header[1] == 0x43)
	{//别的基站发给ARM9的
		return;
	}
	if (header[0] != 0x00 || header[1] != 0x42)
	{
		if (header[0] == 0x00 && header[1] == 0x53)
		{
			//log.Debug("手动采样开始");
			switch(header[2])
			{
				case 0x53:
					//手动采样开始
					GlobalVariable.IsManualSampling = TRUE;
//					//IsSampling = false;
//					//if (GlobalVariable.ControlMode != CONTROL_MODE_AUTO)
//					//{
//					//	LCD_Manual_Window();
//					//}					
//					if(GlobalVariable.CurrentWindow == WIN_Manual)
//					{
//						ShowSampling();
//					}
					break;
				case 0x46:
					//手动采样结束
					bottle_number = databytes[startindex+3]-1;
					//SampleInfo.IsBottleTaken[bottle_number] = TRUE;
					SampleInfo.IsBottleTaking[bottle_number] = FALSE;
					SampleInfo.VolumeTaken[bottle_number] += SampleInfo.Volume;
				  
					GlobalVariable.IsManualSampling = FALSE;
					//IsSampling = false;
					//if (GlobalVariable.ControlMode != CONTROL_MODE_AUTO)
					//{
						//LCD_Manual_Window();
					//}
//					if(GlobalVariable.CurrentWindow == WIN_Manual)
//					{
//						ShowSampling();
//					}
					//标志，发送反馈给船，让船停止发送采样结束的数据报文
					NeedSendSampleFinishedCount = 2;
					break;
				case 0x43:
					break;
				default:
					break;
			}
		}
		return;
	}

	switch (header[2])
	{
	case 0x01://自动模式
		//RFDataService.USVMessageReceivedStatusText = DateTime.Now + ": 收到自动模式数据";
		ProcessBoatDataMessage(databytes, startindex, len);
		receivedManualMessageCount = 0;
		break;
	case 0x34://手动模式
		//RFDataService.USVMessageReceivedStatusText = DateTime.Now + ": 收到手动模式数据";
		ProcessBoatDataMessage(databytes, startindex, len);
		if(++receivedManualMessageCount > 100)
		{
			receivedManualMessageCount = 100;
		}
		break;
	case 0x6E://USVSetting
		//RFDataService.USVMessageReceivedStatusText = DateTime.Now + ": 收到USVSetting数据";
		ProcessUSVSettingMessage(databytes, startindex, len-3);
		break;
	case 0xFB://启动完毕
	case 0xF9://GSP找到
	default:
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：ProcessUSVSettingMessage
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ProcessUSVSettingMessage(u8* bytearray, int offset, int len)
{
	int startindex = offset;
	int arraylength = offset + len;
	int i;

	while (startindex+1 < arraylength) //这里case到8，这里count就只能小于等于8
	{
		switch (bytearray[startindex])
		{
		case 0:
			receivedUSVSetting.P = BytesToFloat(bytearray + startindex + 1);
			startindex += 4 + 1;
			break;
		case 1:
			receivedUSVSetting.I = BytesToFloat(bytearray + startindex + 1);
			startindex += 4 + 1;
			break;
		case 2:
			receivedUSVSetting.D = BytesToFloat(bytearray +  startindex + 1);
			startindex += 4 + 1;
			break;
		case 3:
			receivedUSVSetting.SumUpperLimit_forI = BytesToFloat(bytearray +  startindex + 1);
			startindex += 4 + 1;
			break;
		case 4:
			receivedUSVSetting.RudderMiddle = bytearray[startindex + 1];
			startindex += 1 + 1;
			break;
		case 5:
			receivedUSVSetting.RANGE = BytesToFloat(bytearray +  startindex + 1);
			startindex += 4 + 1;
			break;
		case 6:
			receivedUSVSetting.BoxType = bytearray[startindex + 1];
			startindex += 1 + 1;
			break;
		case 7:
			receivedUSVSetting.BoatType = bytearray[startindex + 1];
			if(receivedUSVSetting.BoatType != local_para_conf[2])
			{
				local_para_conf[2] = receivedUSVSetting.BoatType;
				//保存
//				ConfigurationSave();
				//显示 
//				ShowBoatType();
			}
			startindex += 1 + 1;
			break;
		case 8:
			receivedUSVSetting.MissionID = (bytearray[startindex + 1] * 256 + bytearray[startindex + 2]);
			startindex += 2 + 1;
			break;
		case 9:
			receivedUSVSetting.Interval = (bytearray[startindex + 1] * 256 + bytearray[startindex + 2]);
			startindex += 2 + 1;
			break;
		case 10:
			receivedUSVSetting.Interval2 = (bytearray[startindex + 1] * 256 + bytearray[startindex + 2]);
			startindex += 2 + 1;
			break;
		case 11:
			receivedUSVSetting.ServerIP[0] = bytearray[startindex + 1];
			receivedUSVSetting.ServerIP[1] = bytearray[startindex + 2];
			receivedUSVSetting.ServerIP[2] = bytearray[startindex + 3];
			receivedUSVSetting.ServerIP[3] = bytearray[startindex + 4];
			startindex += 4 + 1;
			break;
		case 12:
			receivedUSVSetting.ServerPort = (bytearray[startindex + 1] * 256 + bytearray[startindex + 2]);
			startindex += 2 + 1;
			break;
		case 14:
			for(i=0;i<14;i++)
			{
				receivedUSVSetting.BoatID[i] = bytearray[startindex + i];
			}
			startindex += 14 + 1;
			break;
		case 15:
			receivedUSVSetting.VideoIP[0] = bytearray[startindex + 1];
			receivedUSVSetting.VideoIP[1] = bytearray[startindex + 2];
			receivedUSVSetting.VideoIP[2] = bytearray[startindex + 3];
			receivedUSVSetting.VideoIP[3] = bytearray[startindex + 4];
			startindex += 4 + 1;
			break;
		case 16:
			receivedUSVSetting.Channel = bytearray[startindex + 1];
			startindex += 1 + 1;
			break;
    case 17:
      receivedUSVSetting.PumpSpeed = (u16)(((bytearray[startindex + 1] & 0x7f) << 7) + (bytearray[startindex + 2] & 0x7f));
			startindex += 2 + 1;
      break;
		default:
			//log.Error("Unknown byte: " + bytearray[startindex].ToString("X2"));
			return;
		}
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionExpect7C
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionExpect7C(u8 b)
 {
	receivedBuf[receivedBufCount++] = b;
	switch (receivedState)
	{
	//7C之前有数据的
	case RFSessionReceivedByte5343://手动采样停止  ：0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
	case RFSessionReceivedByte5346://手动采样完成  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
		if (receivedBufCount >= 5)
		{
			if (b == 0x7C)
			{
				expectState = RFSessionExpectByte0D;
			}
			else
			{//错误
				OnRFSessionParseError();
			}
		}
		break;
	//紧跟7C的
	default:
		if (b == 0x7C)
		{
			expectState = RFSessionExpectByte0D;
		}
		else
		{//错误
			OnRFSessionParseError();
		}
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionReceivedStatesIdle
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionReceivedStatesIdle(u8 b)
{
	receivedBuf[receivedBufCount++] = b;
	switch (b)
	{
	case 0x00:
		//ResetReceivedState();
		receivedState = RFSessionReceivedByte00;
		break;
	case 0xFA:
		receivedState = RFSessionReceivedByteFA;
		break;
	//case 0xFB:
	//    receivedState = ReceivedByteFB;
	//    break;
	case 0xFC:
		receivedState = RFSessionReceivedByteFC;
		break;
	case 0xFD:
		receivedState = RFSessionReceivedByteFD;
		break;
	default:
		//log.Warn("Unknown RF byte: " + b.ToString("X2"));
		ResetReceivedState();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionReceivedStatesByte00
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionReceivedStatesByte00(u8 b)
{
	switch (b)
	{
	case 0x42:
		receivedState = RFSessionReceivedByte42;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x43:
		receivedState = RFSessionReceivedByte43;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x53:
		receivedState = RFSessionReceivedByte53;
		receivedBuf[receivedBufCount++] = b;
		break;
	default:
		receivedBuf[receivedBufCount++] = b;
		//错误
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionReceivedStatesByte42
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionReceivedStatesByte42(u8 b)
{
	switch (b)
	{
	case 0x01:
		receivedState = RFSessionReceivedByte4201;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x34:
		receivedState = RFSessionReceivedByte4234;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x6E:
		receivedState = RFSessionReceivedByte426E;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0xFB:
		receivedState = RFSessionReceivedByte42FB;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0xF9:
		receivedState = RFSessionReceivedByte42F9;
		receivedBuf[receivedBufCount++] = b;
		break;
	default:
		receivedBuf[receivedBufCount++] = b;
		//错误
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionReceivedStatesByte43
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionReceivedStatesByte43(u8 b)
{
	switch (b)
	{
	case 0x00:
		receivedState = RFSessionReceivedByte4300;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x01:
		receivedState = RFSessionReceivedByte4301;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x03:
		receivedState = RFSessionReceivedByte4303;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x04:
		receivedState = RFSessionReceivedByte4304;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x07:
		receivedState = RFSessionReceivedByte4307;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x32:
		receivedState = RFSessionReceivedByte4332;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x41:
		receivedState = RFSessionReceivedByte4341;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x43:
		receivedState = RFSessionReceivedByte4343;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x4E:
		receivedState = RFSessionReceivedByte434E;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x52:
		receivedState = RFSessionReceivedByte4352;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x53:
		receivedState = RFSessionReceivedByte4353;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x61:
		receivedState = RFSessionReceivedByte4361;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x6E:
		receivedState = RFSessionReceivedByte436E;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x74:
		receivedState = RFSessionReceivedByte4374;
		receivedBuf[receivedBufCount++] = b;
		break;
	default:
		receivedBuf[receivedBufCount++] = b;
		//错误
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：OnRFSessionReceivedStatesByte53
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void OnRFSessionReceivedStatesByte53(u8 b)
{
	switch (b)
	{
	case 0x43:
		receivedState = RFSessionReceivedByte5343;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x46:
		receivedState = RFSessionReceivedByte5346;
		receivedBuf[receivedBufCount++] = b;
		break;
	case 0x53:
		receivedState = RFSessionReceivedByte5353;
		receivedBuf[receivedBufCount++] = b;
		break;
	default:
		receivedBuf[receivedBufCount++] = b;
		//错误
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	函数名：ResetReceivedState
	功  能：
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void ResetReceivedState(void)
{
	receivedBufCount = 0;
	receivedState = RFSessionReceivedByteIdle;
	expectState = RFSessionExpectIdle;
}


///*-------------------------------------------------------------------------
//	函数名：UpdateScreen
//	功  能：
//	参  数：
//	返回值：
//-------------------------------------------------------------------------*/
//void UpdateScreen(void)
//{
//	bool moving = FALSE;
//	bool sampling = FALSE;
//	char textBuffer[16];
//	
//	if (GlobalVariable.ControlMode != CONTROL_MODE_AUTO)
//		return;
//	//------------------大区域内容的显示----------------------
//	//if (BoatData.PreviousStatus != BoatData.Status)
//	{
//		switch (BoatData.Status)
//		{
//			case 0:
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				moving = TRUE;
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//				{
//					LCD_WR_L_HanZi_String(30, 79, wei she zhi dao);	   //未设置导
//					LCD_WR_L_HanZi_String(45, 120 , hang ren wu); 	   //航任务
//				}
//				else
//				{
//					LCD_WR_S_String(5, 79, "No Mission");
//				}
//				break;
//			}
//			case 65535:	 //
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//				{
//					LCD_WR_L_HanZi_String(30, 79 , dao hang ren wu);   //导航任务
//					LCD_WR_L_HanZi_String(45, 120 , yi jie shu); 	   //已结束
//				}
//				else
//				{
//					LCD_WR_S_String(5, 79, "Mission");
//					LCD_WR_S_String(5, 120 , "Teminated");
//				}
//				break;
//			}
//			case 65534://采样中
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				sampling = TRUE;
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//				{
//					LCD_WR_L_HanZi_String(30, 79 , zheng zai cai yang);	//正在采样
//					LCD_WR_L_HanZi_String(27, 120 , zhi_2 );
//					//10个瓶子以内
//					if (BoatData.Bottle < 10)
//					{
//						LCD_WR_L_Char(57, 120, 0 + 48);
//						LCD_WR_L_Char(74, 120, BoatData.Bottle + 48);
//						LCD_WR_L_HanZi_String(91, 120 , hao ping );
//					}
//					//10-20个瓶子
//					else if (BoatData.Bottle < 20 && BoatData.Bottle >= 10)
//					{
//						LCD_WR_L_Char(57, 120, 1 + 48);
//						LCD_WR_L_Char(74, 120, BoatData.Bottle - 10 + 48);
//						LCD_WR_L_HanZi_String(91, 120 , hao ping );
//					}
//				}
//				else {
//					sprintf(textBuffer, "Bottle %d" , BoatData.Bottle);
//					LCD_WR_S_String(5, 79 , "Sampling");	//正在采样
//					LCD_WR_S_String(5, 120 , textBuffer);
//				}
//				break;
//			}
////			case 65533://在线监测中
////			{
////				LCD_ClearArea(30, 79, 82, 120);
////				LCD_WR_L_HanZi_String(30, 79 , zheng zai jian ce);	//
////				//LCD_WR_L_HanZi_String(30, 120 , zhi ); 	//至
////				break;
////			}
//			case 65532://导航已被取消
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//					LCD_WR_L_HanZi_String(30, 98 , dao hang qu xiao);	//导航取消
//				else 
//					LCD_WR_S_String(30, 98 , "Canceled");	//导航取消
//				break;
//			}
//			case 65531://导航已被暂停
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//					LCD_WR_L_HanZi_String(30, 98 , dao hang zan ting);	//导航暂停 
//				else
//					LCD_WR_S_String(30, 98 , "Paused");	//导航暂停 
//				break;		  
//			}
//			default:
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				moving = TRUE;
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//				{
//					if (BoatData.Status >= 1 && BoatData.Status <= 255)
//					{
//							LCD_WR_L_HanZi_String(30, 79 , zheng zai dao hang);	 //正在导航
//							LCD_WR_L_HanZi_String(27, 120 , zhi_2 ); 	//至
//							if(BoatData.Status < 10)
//						{
//								LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status + 48); //字库数组从16开始是0
//							LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status >= 10 && BoatData.Status <= 99)
//						{
//							LCD_WR_L_Char(57, 120, BoatData.Status / 10 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status % 10 + 48);
//							LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status >= 100 && BoatData.Status <= 255)
//						{
//							LCD_WR_L_Char(57, 120, BoatData.Status / 100 + 48);
//							LCD_WR_L_Char(74, 120, (BoatData.Status % 100 ) / 10 + 48);
//							LCD_WR_L_Char(91, 120, BoatData.Status % 10 + 48);
//							LCD_WR_L_HanZi_String(108, 120 , hao dian );
//						}
//					}
//					else if (BoatData.Status >= 32768 && BoatData.Status <= 32768 + 255)
//					{
//						LCD_WR_L_HanZi_String(30, 79 , zheng zai jian_1 ce); //正在监测
//						LCD_WR_L_HanZi_String(27, 120 , zhi_2); //至
//						if(BoatData.Status - 128 * 256 < 10)
//						{
//							LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status - 128 * 256 + 48); //字库数组从16开始是0
//							LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status - 128 * 256 >= 10 && BoatData.Status - 128 * 256 <= 99)
//						{
//							LCD_WR_L_Char(57, 120, (BoatData.Status - 128 * 256) / 10 + 48);
//							LCD_WR_L_Char(74, 120, (BoatData.Status - 128 * 256) % 10 + 48);
//							LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)//99号点以上
//						{
//							LCD_WR_L_Char(57, 120, ( BoatData.Status - 128 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(74, 120, ( (BoatData.Status - 128 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(91, 120, ( BoatData.Status - 128 * 256 ) % 10 + 48);
//							LCD_WR_L_HanZi_String(108, 120 , hao dian );	
//						}
//					}
//					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //采样监测
//					{
//						//LCD_WR_L_HanZi_String(2, 55 , zheng zai); //正在
//						LCD_WR_L_HanZi_String(2, 55 , cai yang); //采样
//						//LCD_WR_L_HanZi_String(65, 56 , maohao); 
//						LCD_WR_L_HanZi_String(2, 89 , jian_1 ce); //监测
//						
//						if(BoatData.Status - 192 * 256 < 10)
//						{
//						 //	LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(60, 89, BoatData.Status - 192 * 256 + 48); //字库数组从16开始是0
//							LCD_WR_L_HanZi_String(75, 89 , hao dian );
//						}
//						else if(BoatData.Status - 192 * 256 >= 10 && BoatData.Status - 192 * 256 <= 99)
//						{
//								LCD_WR_L_Char(60, 89, (BoatData.Status - 192 * 256) / 10 + 48);
//							LCD_WR_L_Char(75, 89, (BoatData.Status - 192 * 256) % 10 + 48);
//							LCD_WR_L_HanZi_String(90, 89 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)  //99号点以上
//						{
//							LCD_WR_L_Char(60, 89, ( BoatData.Status - 192 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(75, 89, ( (BoatData.Status - 192 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(90, 89, ( BoatData.Status - 192 * 256 ) % 10 + 48);
//							LCD_WR_L_HanZi_String(105, 89 , hao dian );	
//						}
//					
//						LCD_WR_L_HanZi_String(2, 123 , cai yang zhi_2); //采样至
//						//LCD_WR_L_Char(89, 123, BoatData.Bottle + 48);
//						if (BoatData.Bottle < 10)
//						{
//								LCD_WR_L_Char(57, 123, 0 + 48);
//								LCD_WR_L_Char(74, 123, BoatData.Bottle + 48);
//						}
//						//10-20个瓶子
//						else if (BoatData.Bottle < 20 && BoatData.Bottle >= 10)
//						{
//								LCD_WR_L_Char(57, 123, 1 + 48);
//								LCD_WR_L_Char(74, 123, BoatData.Bottle - 10 + 48);
//						}
//						LCD_WR_L_HanZi_String(108, 123 , hao ping );	 //号瓶
//					}
//				}
//				else
//				{
//					if (BoatData.Status >= 1 && BoatData.Status <= 255)
//					{
//						LCD_WR_S_String(30, 79 , "Moving to");	 //正在导航
//						if(BoatData.Status < 10)
//						{
//								LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status + 48); //字库数组从16开始是0
//							//LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status >= 10 && BoatData.Status <= 99)
//						{
//							LCD_WR_L_Char(57, 120, BoatData.Status / 10 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status % 10 + 48);
//							//LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status >= 100 && BoatData.Status <= 255)
//						{
//							LCD_WR_L_Char(57, 120, BoatData.Status / 100 + 48);
//							LCD_WR_L_Char(74, 120, (BoatData.Status % 100 ) / 10 + 48);
//							LCD_WR_L_Char(91, 120, BoatData.Status % 10 + 48);
//							//LCD_WR_L_HanZi_String(108, 120 , hao dian );
//						}
//					}
//					else if (BoatData.Status >= 32768 && BoatData.Status <= 32768 + 255)
//					{
//						LCD_WR_S_String(30, 79 , "Monitoring"); //正在监测
//						if(BoatData.Status - 128 * 256 < 10)
//						{
//							LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status - 128 * 256 + 48); //字库数组从16开始是0
//							//LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status - 128 * 256 >= 10 && BoatData.Status - 128 * 256 <= 99)
//						{
//							LCD_WR_L_Char(57, 120, (BoatData.Status - 128 * 256) / 10 + 48);
//							LCD_WR_L_Char(74, 120, (BoatData.Status - 128 * 256) % 10 + 48);
//							//LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)//99号点以上
//						{
//							LCD_WR_L_Char(57, 120, ( BoatData.Status - 128 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(74, 120, ( (BoatData.Status - 128 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(91, 120, ( BoatData.Status - 128 * 256 ) % 10 + 48);
//							//LCD_WR_L_HanZi_String(108, 120 , hao dian );	
//						}
//					}
//					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //采样监测
//					{
//						LCD_WR_S_String(2, 55 , "Sampling"); //采样
//						//LCD_WR_L_HanZi_String(65, 56 , maohao); 
//						LCD_WR_S_String(2, 89 , "Monitoring"); //监测
//						
//						if(BoatData.Status - 192 * 256 < 10)
//						{
//						 //	LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(60, 89, BoatData.Status - 192 * 256 + 48); //字库数组从16开始是0
//							//LCD_WR_L_HanZi_String(75, 89 , hao dian );
//						}
//						else if(BoatData.Status - 192 * 256 >= 10 && BoatData.Status - 192 * 256 <= 99)
//						{
//								LCD_WR_L_Char(60, 89, (BoatData.Status - 192 * 256) / 10 + 48);
//							LCD_WR_L_Char(75, 89, (BoatData.Status - 192 * 256) % 10 + 48);
//							//LCD_WR_L_HanZi_String(90, 89 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)  //99号点以上
//						{
//							LCD_WR_L_Char(60, 89, ( BoatData.Status - 192 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(75, 89, ( (BoatData.Status - 192 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(90, 89, ( BoatData.Status - 192 * 256 ) % 10 + 48);
//							//LCD_WR_L_HanZi_String(105, 89 , hao dian );	
//						}
//					
//						LCD_WR_S_String(2, 123 , "Sampling"); //采样至
//						//LCD_WR_L_Char(89, 123, BoatData.Bottle + 48);
//						if (BoatData.Bottle < 10)
//						{
//								LCD_WR_L_Char(57, 123, 0 + 48);
//								LCD_WR_L_Char(74, 123, BoatData.Bottle + 48);
//						}
//						//10-20个瓶子
//						else if (BoatData.Bottle < 20 && BoatData.Bottle >= 10)
//						{
//								LCD_WR_L_Char(57, 123, 1 + 48);
//								LCD_WR_L_Char(74, 123, BoatData.Bottle - 10 + 48);
//						}
//					}
//				}
//				break;
//			}
//		}
//	}
//	
//	ShowGPSData();

//	sprintf((u8*)textBuffer,"%4.3d", (int)(BoatData.WaterTemp*100));
//	//LCD_ClearArea(190,137, 31, 70);
//	LCD_WR_S_DecimalFromInt(190,137,textBuffer,2);
//	//刚刚变换的状态才需要显示单位，之后就不行更新单位了
//	//if (BoatData.PreviousStatus != BoatData.Status)//第一次进到这里
//	{
//		//LCD_ClearArea(185,137, 31, 130);
//		LCD_DrawCircle(260,141,2);
//		LCD_WR_S_String(265,137,"C");
//	}
//}

/*接收相关命令===============================================================================================================*/
