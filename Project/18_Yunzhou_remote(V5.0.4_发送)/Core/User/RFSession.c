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

bool EngineStarted = FALSE;			//打火允许
bool Speaker;										//警报
bool Alert;											//警灯	
bool ValveStatus[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};											//阀门状态
bool ValveStatusUsed[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};									//阀门使用状态

u8 receivedManualMessageCount = 0;

uint16_t RF_transmit_timer = 0;						//数传模块定时传输计时器

//static RFCommandReceivedStates rfcommandReceivedState;


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
				if (ALARM_Button)
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

			if (ALARM_Button)
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

		if (Speaker)
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
	
	if ((MODE_Button == CONTROL_MODE_AUTO) && ((interface_index == DEBUG_MAIN_INTERFACE_INDEX) || (interface_index == MAIN_INTERFACE_INDEX)))	//非自动模式和在手动窗口下，才会发送。
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
			if(gatepwm < 3000)
			{
				gatepwm = 3000;
			}
			if(gatepwm > 4000)
			{
				gatepwm = 4000;
			}
			//油门开度 1000-2000, 旋钮值范围3000-4000;
			gatepwm = gatepwm - 2000;
			gatespeed = 2000;//停止
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
//void OnCommandDataReceived(char c)
//{
//	ParseRFComamndByte(c);
//}


///*-------------------------------------------------------------------------
//	函数名：ParseRFComamndByte
//	功  能：分析RF模块的命令
//	参  数：
//	返回值：
//-------------------------------------------------------------------------*/
//void ParseRFComamndByte(u8 b)
//{
//	//先存放在缓冲中
//	rfcommandReceivedBuf[rfcommandReceivedBufCount++] = b;

//	//根据接收状态处理
//	switch (rfcommandReceivedState)
//	{
//	case ReceivedRFCommandIdle:
//		if (b == 0x5A)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A;
//		}
//		else
//		{//错误
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A:
//		if (b == 0x5A)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A;
//		}
//		else
//		{//错误
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A:
//		if (b == 0x00)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00;
//		}
//		else
//		{//错误
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A00:
//		if (b == 0x00)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A0000;
//		}
//		else
//		{//错误
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A0000:
//		if (b == 0x5A)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A;
//		}
//		else
//		{//错误
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A00005A:
//		if (b == 0x00)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A00;
//		}
//		else
//		{//错误
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A00005A00:
//		switch (b)
//		{
//		case 0x01:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0001;
//			break;
//		case 0x02:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0002;
//			break;
//		case 0x03:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0003;
//			break;
//		case 0x04:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0004;
//			break;
//		case 0x05:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0005;
//			break;
//		case 0x06:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0006;
//			break;
//		case 0x0E:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A000E;
//			break;
//		case 0x15:
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A0015;
//			break;
//		default://错误
//			OnRFComamndTextParseError();
//			break;
//		}
//		break;
//	case ReceivedRFCommandByte5A5A00005A0001:
//	case ReceivedRFCommandByte5A5A00005A0002:
//	case ReceivedRFCommandByte5A5A00005A0003:
//	case ReceivedRFCommandByte5A5A00005A0004:
//	case ReceivedRFCommandByte5A5A00005A0005:
//	case ReceivedRFCommandByte5A5A00005A0006:
//		if ((rfcommandReceivedBufCount == 8 && b != 0x02) || (rfcommandReceivedBufCount == 12 && b != 0x0D) || (rfcommandReceivedBufCount == 13 && b != 0x0A))
//		{
//			OnRFComamndTextParseError();
//		}
//		if (b == 0x0A && rfcommandReceivedBuf[rfcommandReceivedBufCount - 2] == 0x0D && rfcommandReceivedBufCount < 13)
//		{
//			OnRFComamndTextParseError();
//		}
//		if (rfcommandReceivedBufCount == 13)
//		{
//			int checksumReceived = rfcommandReceivedBuf[rfcommandReceivedBufCount - 3];
//			int checksumCalculate = CountRFModuleCheckSum(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount - 3);
//			
//			if (checksumReceived == checksumCalculate)
//			{
//				OnRFComamndTextParseOk();
//			}
//			else
//			{
//				OnRFComamndTextParseError();
//			}
//		}
//		break;
//	case ReceivedRFCommandByte5A5A00005A000E:
//	case ReceivedRFCommandByte5A5A00005A0015:
//		if ((rfcommandReceivedBufCount == 8 && b != 0x04) || (rfcommandReceivedBufCount == 14 && b != 0x0D) || (rfcommandReceivedBufCount == 15 && b != 0x0A))
//		{
//			OnRFComamndTextParseError();
//		}
//		if (rfcommandReceivedBufCount == 15)
//		{
//			int checksumReceived = rfcommandReceivedBuf[rfcommandReceivedBufCount - 3];
//			int checksumCalculate = CountRFModuleCheckSum(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount - 3);
//                            
//			if (checksumReceived == checksumCalculate)
//			{
//				OnRFComamndTextParseOk();
//			}
//			else
//			{
//				OnRFComamndTextParseError();
//			}
//		}
//		break;
//	default:
//		OnRFComamndTextParseError();
//		break;
//	}
//}

///*-------------------------------------------------------------------------
//	函数名：ParseRFSessionText
//	功  能：
//	参  数：rfcommRecvedBuf 		缓冲数组
//					offset							处理偏移量
//					count								处理数量
//	返回值：
//-------------------------------------------------------------------------*/
//void ParseRFSessionText(u8* rfcommRecvedBuf, int offset, int count)
//{
//	int startIndex = offset;
//	int endIndex = offset + count;
//	
//	for(;startIndex < endIndex; startIndex++)
//	{
//		ParseRFSessionByte(rfcommRecvedBuf[startIndex]);
//	}
//}


///*-------------------------------------------------------------------------
//	函数名：ParseRFSessionByte
//	功  能：
//	参  数：b 		处理数组
//	返回值：
//-------------------------------------------------------------------------*/
//void ParseRFSessionByte(u8 b)
//{
//	switch (receivedState)
//	{
//	case RFSessionReceivedByteIdle:
//		OnRFSessionReceivedStatesIdle(b);
//		break;
//	case RFSessionReceivedByte00:
//		OnRFSessionReceivedStatesByte00(b);
//		break;
//	case RFSessionReceivedByte42:
//		OnRFSessionReceivedStatesByte42(b);
//		break;
//	case RFSessionReceivedByte43:
//		OnRFSessionReceivedStatesByte43(b);
//		break;
//	case RFSessionReceivedByte53:
//		OnRFSessionReceivedStatesByte53(b);
//		break;
//	case RFSessionReceivedByte4201:
//	case RFSessionReceivedByte4234:
//	case RFSessionReceivedByte42FB://水面机器人启动完毕\x00\x42\xFB\x7C
//	case RFSessionReceivedByte42F9://水面机器人已找到GPS信号\x00\x42\xF9\x7C
//	case RFSessionReceivedByte426E:
//	case RFSessionReceivedByte4300:
//	case RFSessionReceivedByte4301:
//	case RFSessionReceivedByte4303:
//	case RFSessionReceivedByte4304:
//	case RFSessionReceivedByte4332:
//	case RFSessionReceivedByte4307:
//	case RFSessionReceivedByte4341:
//	case RFSessionReceivedByte4343:
//	case RFSessionReceivedByte434E:
//	case RFSessionReceivedByte4352:
//	case RFSessionReceivedByte4353:
//	case RFSessionReceivedByte4361:
//	case RFSessionReceivedByte436E:
//	case RFSessionReceivedByte4374:
//	case RFSessionReceivedByte5343:
//	case RFSessionReceivedByte5346:
//	case RFSessionReceivedByte5353:
//		OnRFSessionExpectByte(b);
//		break;

//	case RFSessionReceivedByteFA:
//		OnReceivedByteFA(b);
//		break;
//	//case RFSessionReceivedStates.ReceivedByteFB:
//	//    OnReceivedByteFB(b);
//	//    break;
//	case RFSessionReceivedByteFC:
//		OnReceivedByteFC(b);
//		break;
//	case RFSessionReceivedByteFD:
//		OnReceivedByteFD(b);
//		break;

//	case RFSessionReceivedByteFAFA:
//	//case ReceivedByteFBFB:
//	case RFSessionReceivedByteFCFC:
//	case RFSessionReceivedByteFDFD:
//		OnExpectByteFE(b);
//		break;
//	default:
//		OnRFSessionParseError();
//		break;
//	}
//}

/*接收相关命令===============================================================================================================*/
