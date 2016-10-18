/*-------------------------------------------------------------------------
�������ƣ����߿�������������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150615 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "RFSession.h"


/* Private variables ---------------------------------------------------------*/
static u8 sendBuffer[480];													//��������ͻ���
static u8 sendBuffer_encoded[640];									//���ͻ���base64�����ʽ
static int sendBuffer_index;												//���ͻ���ָ��
static int sendBuffer_encoded_length;								//���ͻ������ָ��


/* Exported variables ---------------------------------------------------------*/
u8 NeedSendSpeaker = 0;													//���������ʹ���
u8 NeedSendAlertLED = 0;												//�����Ʒ��ʹ���
u8 NeedSendSampleCount = 0;											//�������ʹ���
u8 NeedSendSampleFinishedCount = 0;							//������ɷ��ʹ���
u8 NeedSendAutoModeCommandCount = 0;						//�����Զ�ģʽ�������
u8 NeedSendValveCount = 0;

bool EngineStarted = FALSE;			//�������
bool Speaker;										//����
bool Alert;											//����	
bool ValveStatus[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};											//����״̬
bool ValveStatusUsed[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};									//����ʹ��״̬

u8 receivedManualMessageCount = 0;

uint16_t RF_transmit_timer = 0;						//����ģ�鶨ʱ�����ʱ��

//static RFCommandReceivedStates rfcommandReceivedState;


/* Private function -----------------------------------------------*/
/*�����������---------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
	��������GenerateTakeSampleCommandBase64
	��  �ܣ���������ͣ�base64�����ʽ
	��  ����encoded 		��������			�ж��Ƿ���Ҫbase64����
	����ֵ��
-------------------------------------------------------------------------*/
void GenerateTakeSampleCommandBase64(bool encoded)
{
	if(NeedSendSampleCount > 0){
		sendBuffer_index = 0;
		
		sendBuffer[sendBuffer_index++] = 0;
		sendBuffer[sendBuffer_index++] = 0x53;//83; //S
		sendBuffer[sendBuffer_index++] = 0x53;//83; //S
		sendBuffer[sendBuffer_index++] = 0x7C;//124;
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[0];						//����ƿ��
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[1];						//�������
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[2];						//����ģʽ
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[3];						//��������
		sendBuffer[sendBuffer_index++] = (uint8_t) water_sample_conf[4];						//��ϴģʽ
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
	��������SendSampleFinished
	��  �ܣ�������ɷ��ͣ�base64�����ʽ
	��  ����encoded 		��������			�ж��Ƿ���Ҫbase64����
	����ֵ��
-------------------------------------------------------------------------*/
void SendSampleFinished(bool encoded)
{
	int i;
	u8 modeBuffer[30];
	u8 modBufferEncoded[40];
	int modeBufferIndex;
	int modBufferEncodedLength;
		
	//����
	modeBufferIndex = 0;
	
	modeBuffer[modeBufferIndex++] = 0x00;
	modeBuffer[modeBufferIndex++] = 0x53;
	modeBuffer[modeBufferIndex++] = 0x46;
	modeBuffer[modeBufferIndex++] = (uint8_t) water_sample_conf[0];						//����ƿ��
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
	��������GenerateAlarmCommandBase64
	��  �ܣ��������Ʒ��ͣ�base64�����ʽ
	��  ����encoded 		��������			�ж��Ƿ���Ҫbase64����
	����ֵ��
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
			//�����жϾ������ƽ���
			if (local_para_conf[2] != USV_Boat_Type_ME300)
			{
				//����������
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
				//���������5���ڣ��Ƚ�alert����������״̬��Ȼ�󲦶������˼��ɴ��
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
	��������SendCommandAutoMode
	��  �ܣ������Զ�ģʽ���base64�����ʽ
	��  ����encoded 		��������			�ж��Ƿ���Ҫbase64����
	����ֵ��
-------------------------------------------------------------------------*/
void SendCommandAutoMode(bool encoded)
{
	int i;
	u8 modeBuffer[30];
	u8 modBufferEncoded[40];
	int modeBufferIndex;
	int modBufferEncodedLength;
	
		//����
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
	��������GetValveValue
	��  �ܣ���ȡ����������Ϣ
	��  ����
	����ֵ������״̬	�޷����ַ���		��5λ��Ӧ5������
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
	��������save_Valve_parameter
	��  �ܣ����淧��״̬
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void save_Valve_parameter(void)
{
	uint8_t i;
	
	for(i = 0; i < VALVE_QUANTITY; i++)
	{
		ValveStatusUsed[i] = ValveStatus[i];
	}
	
	NeedSendValveCount = 10;			//����10��
	
	return;
}


/*-------------------------------------------------------------------------
	��������GenerateValveCommand
	��  �ܣ����ͷ������base64�����ʽ
	��  ����
	����ֵ��
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
	��������StopEngineTrigger
	��  �ܣ�ֹͣ���������
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void StopEngineTrigger(void)
{
			if(EngineStarted != FALSE)
			{
//				EngineTimerCount=0;
				EngineStarted = FALSE;
				NeedSendAlertLED = MAX_ALERT_SEND_COUNT;
				//��ʾ������״̬
//				Show_Engine_Status();
			}
}


/*-------------------------------------------------------------------------
	��������GenerateEngineStartCommandBase64
	��  �ܣ���������¼�������base64�����ʽ
	��  ����encoded 		��������			�ж��Ƿ���Ҫbase64����
	����ֵ��
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
	��������GenerateEngineStopCommandBase64
	��  �ܣ��������湩��Ϩ�����base64�����ʽ
	��  ����encoded 		��������			�ж��Ƿ���Ҫbase64����
	����ֵ��
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
	��������GenerateCommandBase64
	��  �ܣ���ʱ�������base64�����ʽ
	��  ����encoded 		��������			�ж��Ƿ���Ҫbase64����
	����ֵ��
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
	
	if ((MODE_Button == CONTROL_MODE_AUTO) && ((interface_index == DEBUG_MAIN_INTERFACE_INDEX) || (interface_index == MAIN_INTERFACE_INDEX)))	//���Զ�ģʽ�����ֶ������£��Żᷢ�͡�
	//if (GlobalVariable.ControlMode == CONTROL_MODE_AUTO)//���Զ�ģʽ���Żᷢ�͡�
	//if (GlobalVariable.ControlMode == CONTROL_MODE_AUTO || GlobalVariable.IsRFChannelSetting != FALSE)//���Զ�ģʽ�ͷǸ����ŵ�ʱ���Żᷢ�͡�
	{
			return;
	}
	
	if (local_para_conf[2] == USV_Boat_Type_ME300)
	{
		GenerateEngineStartCommandBase64(TRUE);
		GenerateEngineStopCommandBase64(TRUE);
	}
	
//	//�������ڷ��ͱ�־
//	GlobalVariable.IsRFSessionSending = TRUE;
	
	if(NeedSendSampleFinishedCount > 0)
	{
		SendSampleFinished(TRUE);
		NeedSendSampleFinishedCount = 0;
	}
	
	GenerateTakeSampleCommandBase64(TRUE);
	
	GenerateAlarmCommandBase64(TRUE);

	//����
	sendBuffer_index = 0;

	sendBuffer[sendBuffer_index++] = 0;
	sendBuffer[sendBuffer_index++] = 0x43;//(67); //C
	sendBuffer[sendBuffer_index++] = 0x0;//PutUART(0);
	sendBuffer[sendBuffer_index++] = 0x7C;//PutUART(124);


	//-----------------------------20131227�¶���ƽ���ʼ------------------------
	//MC70�ú��ſ��ƣ�ME300�ͻ������ſ���
	switch(local_para_conf[2])
	{
		case USV_Boat_Type_MC120:
		case USV_Boat_Type_MC70:
			switch(ALARM_Button)
			{
				case 0:
					//GlobalVariable.Alert = TRUE;
					//GlobalVariable.Speaker = TRUE;
					gatepwm = 1000;//����
					break;
				case 2:
					//GlobalVariable.Alert = TRUE;
					//GlobalVariable.Speaker = FALSE;
					gatepwm = 1000;//����
					break;
				case 1:
				default:
					//GlobalVariable.Alert = FALSE;
					//GlobalVariable.Speaker = FALSE;
					gatepwm = 2000;//����
					break;
			}
			gatespeed = KnobValue;//��ťֵ
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
			//���ſ��� 1000-2000, ��ťֵ��Χ3000-4000;
			gatepwm = gatepwm - 2000;
			gatespeed = 2000;//ֹͣ
			break;
		default:
			gatepwm = 1500;//��λ
			gatespeed = 2000;//ֹͣ
			break;
	}
	
	//MC70��ˮ�õ���Ϳ����ſ��ƣ�ME300�ͻ������ſ���
	switch(local_para_conf[2])
	{
		case USV_Boat_Type_MC120:
		case USV_Boat_Type_MC70:
			sendBuffer[sendBuffer_index++] = 0x12;//PutUART(0x12);
			checksum += 0x12;
			//���pwm
			tmpint = gatepwm;
			tempbyte = ((tmpint>>7)&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			tempbyte = (tmpint&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			//�ƽ�H��
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
			//���pwm
			//��������
			tmpint = gatepwm;
			//��������
			//tmpint = 3000-gatepwm;
			tempbyte = ((tmpint>>7)&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			tempbyte = (tmpint&0x7F)|0x80;
			sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
			checksum += tempbyte;
			//�ƽ�H��
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
	{//��ҡ�˿���ģʽ
		/////////////////////////////�����ƽ����/////////////////
		sendBuffer[sendBuffer_index++] = 0x11;//PutUART(0x11);
		checksum += 0x11;
		//���
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

		//�ƽ�
		tmpint = RightSpeedCommand;
		tempbyte = ((tmpint>>7)&0x7F)|0x80;
		sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
		checksum += tempbyte;
		tempbyte = (tmpint&0x7F)|0x80;
		sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
		checksum += tempbyte;
	}
	else 
	{//˫ҡ��ģʽ
		if (local_para_conf[1] != MANUAL_CONTROL_MODE_DUAL)						//����ģʽ
		{//��ҡ�˿������ţ���ҡ�˿��ƶ��
				/////////////////////////////�����ƽ����/////////////////
				sendBuffer[sendBuffer_index++] = 0x11;//PutUART(0x11);
				checksum += 0x11;
				//��ǣ� ������ҡ�˵�ֵ
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

				//�ƽ�, ������ҡ�˵�ֵ
				tmpint = LeftSpeedCommand;
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
		}
		else{//��ҡ�˿����ƽ�����ҡ�˿������ƽ�																				˫��ģʽ
				//////////////////////////////��ҡ��/////////////////
				sendBuffer[sendBuffer_index++] = 0x13;//PutUART(0x13);
				checksum += 0x13;
				//���
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

				//�ƽ�
				tmpint = RightSpeedCommand;
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				
				//////////////////////////////��ҡ��/////////////////
				sendBuffer[sendBuffer_index++] = 0x14;//PutUART(0x14);
				checksum += 0x14;
				//���
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

				//�ƽ�
				tmpint = LeftSpeedCommand;
				tempbyte = ((tmpint>>7)&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
				tempbyte = (tmpint&0x7F)|0x80;
				sendBuffer[sendBuffer_index++] = tempbyte;//PutUART(tempbyte);
				checksum += tempbyte;
		}
	}
	//-----------------------------20131227�¶���ƽ�����------------------------

	//���������
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

//		//�������ڷ��ͱ�־
//	GlobalVariable.IsRFSessionSending = FALSE;
}
/*�����������===============================================================================================================*/


/*�����������---------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
	��������OnCommandDataReceived
	��  �ܣ���Ϣ���մ�����
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
//void OnCommandDataReceived(char c)
//{
//	ParseRFComamndByte(c);
//}


///*-------------------------------------------------------------------------
//	��������ParseRFComamndByte
//	��  �ܣ�����RFģ�������
//	��  ����
//	����ֵ��
//-------------------------------------------------------------------------*/
//void ParseRFComamndByte(u8 b)
//{
//	//�ȴ���ڻ�����
//	rfcommandReceivedBuf[rfcommandReceivedBufCount++] = b;

//	//���ݽ���״̬����
//	switch (rfcommandReceivedState)
//	{
//	case ReceivedRFCommandIdle:
//		if (b == 0x5A)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A;
//		}
//		else
//		{//����
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A:
//		if (b == 0x5A)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A;
//		}
//		else
//		{//����
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A:
//		if (b == 0x00)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00;
//		}
//		else
//		{//����
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A00:
//		if (b == 0x00)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A0000;
//		}
//		else
//		{//����
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A0000:
//		if (b == 0x5A)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A;
//		}
//		else
//		{//����
//			OnRFComamndTextParseError();
//		}
//		break;
//	case ReceivedRFCommandByte5A5A00005A:
//		if (b == 0x00)
//		{
//			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A00;
//		}
//		else
//		{//����
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
//		default://����
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
//	��������ParseRFSessionText
//	��  �ܣ�
//	��  ����rfcommRecvedBuf 		��������
//					offset							����ƫ����
//					count								��������
//	����ֵ��
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
//	��������ParseRFSessionByte
//	��  �ܣ�
//	��  ����b 		��������
//	����ֵ��
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
//	case RFSessionReceivedByte42FB://ˮ��������������\x00\x42\xFB\x7C
//	case RFSessionReceivedByte42F9://ˮ����������ҵ�GPS�ź�\x00\x42\xF9\x7C
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

/*�����������===============================================================================================================*/
