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

uint8_t connect_ok = 0;							//0��ʾ����û�ɹ���1��ʾ���ӳɹ�

bool EngineStarted = FALSE;			//�������
bool Speaker;										//����
bool Alert;											//����	
bool ValveStatus[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};											//����״̬
bool ValveStatusUsed[VALVE_QUANTITY] = {FALSE, FALSE, FALSE, FALSE, FALSE};									//����ʹ��״̬

u8 receivedManualMessageCount = 0;

uint16_t RF_transmit_timer = 0;						//����ģ�鶨ʱ�����ʱ��

uint8_t BoatDataTimeoutCount = 0;					//�����ݳ�ʱ��ʱ�������ʱ

BoatDataTypeDef BoatData;									//USVͨ�ûش�����
USVSetting receivedUSVSetting;						//USV������Ϣ
GlobalVariableTypeDef GlobalVariable;			//ȫ�ֱ���
SampleInfoTypeDef SampleInfo;							//������ر���

static RFCommandReceivedStates rfcommandReceivedState;
static RFSessionReceivedStates receivedState;
static RFSessionExpectStates expectState;

static int receivedBufCount = 0;
static u8 receivedBuf[1024];
static int decodedBufferCount;
static u8 decodedBuffer[1600];

static u8 bottleNumberLocal = 0;


#if defined QC_LOOP_TEST
uint16_t QC_Test_Timer = 0;				//�ʼ�����ü�����
uint8_t QC_Test_Flag = 1;					//�ʼ����������˳��
uint8_t QC_Sample_Flag = 0;				//����1�α��

uint16_t QC_Motor_Timer = 0;				//�ʼ���Ե���ü�����
uint8_t QC_Motor_Flag = 1;					//�ʼ���Ե��������˳��
#endif


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
#if defined QC_LOOP_TEST
	if(QC_Sample_Flag == 1)
	{
		QC_Sample_Flag = 0;
		
		//�ʼ��������
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

#if defined QC_LOOP_TEST	
	//�ʼ쾯�����ƶ���
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
			//�����жϾ������ƽ���
			if (local_para_conf[2] != USV_Boat_Type_ME300)
			{
				//����������
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
	��������load_Valve_parameter
	��  �ܣ���ȡ����״̬
	��  ����
	����ֵ��
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
				NeedSendSpeaker = MAX_SPEAKER_SEND_COUNT;
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
	
	if ((MODE_Button == CONTROL_MODE_AUTO) && ((interface_index == DEBUG_MAIN_INTERFACE_INDEX) || (interface_index == MAIN_GRAPHIC_INDEX)))	//���Զ�ģʽ�����ֶ������£��Żᷢ�͡�
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
//			if(gatepwm < 3000)
//			{
//				gatepwm = 3000;
//			}
			if(gatepwm > 4000)
			{
				gatepwm = 4000;
			}
			//���ſ��� 1000-2000, ��ťֵ��Χ3000-4000;
			gatespeed = gatepwm;
			gatepwm = gatepwm - 2000;
//			gatespeed = 2000;//ֹͣ			
						
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

#if defined QC_LOOP_TEST			
		//�ʼ�����ƽ�����
		switch(QC_Motor_Flag)
		{
			case 1:				//��ǰ��
				RightSpeedCommand = 4090;
				RightRudderCommand = 4090;
				break;
			
			case 2:				//�Һ���
				RightSpeedCommand = 0;
				RightRudderCommand = 0;
				break;			
			
			case 3:				//��ǰ��
				RightSpeedCommand = 0;
				RightRudderCommand = 4090;
				break;

			case 4:				//�����
				RightSpeedCommand = 4090;
				RightRudderCommand = 0;
				break;								
			
			default:
//				RightSpeedCommand = 1500;
//				RightRudderCommand = 2000;
				break;				
		}		
#endif
		
		
		//���
		tmpint = RightRudderCommand;
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
void OnCommandDataReceived(char c)
{
	ParseRFComamndByte(c);
}


/*-------------------------------------------------------------------------
	��������ParseRFComamndByte
	��  �ܣ�����RFģ�������
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void ParseRFComamndByte(u8 b)
{
	//�ȴ���ڻ�����
	rfcommandReceivedBuf[rfcommandReceivedBufCount++] = b;

	//���ݽ���״̬����
	switch (rfcommandReceivedState)
	{
	case ReceivedRFCommandIdle:
		if (b == 0x5A)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A;
		}
		else
		{//����
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A:
		if (b == 0x5A)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A;
		}
		else
		{//����
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A:
		if (b == 0x00)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00;
		}
		else
		{//����
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A00:
		if (b == 0x00)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A0000;
		}
		else
		{//����
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A0000:
		if (b == 0x5A)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A;
		}
		else
		{//����
			OnRFComamndTextParseError();
		}
		break;
	case ReceivedRFCommandByte5A5A00005A:
		if (b == 0x00)
		{
			rfcommandReceivedState = ReceivedRFCommandByte5A5A00005A00;
		}
		else
		{//����
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
		default://����
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
	��������OnRFComamndTextParseError
	��  �ܣ�
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void OnRFComamndTextParseError(void)
{
   //����RFCommand, ��ô������ͨ��USV��Ϣ
   ParseRFSessionText(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount);

   ResetRFCommandReceivedState();
}


/*-------------------------------------------------------------------------
	��������ResetRFCommandReceivedState
	��  �ܣ�
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void ResetRFCommandReceivedState(void)
{
   rfcommandReceivedBufCount = 0;
   rfcommandReceivedState = ReceivedRFCommandIdle;
   //rfcommandExpectState = RFComamndExpectStates.Idle;
}


/*-------------------------------------------------------------------------
	��������CountRFModuleCheckSum
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnRFComamndTextParseOk
	��  �ܣ�433�������ý���
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void OnRFComamndTextParseOk(void)
{
	switch (rfcommandReceivedState)
	{
	//���ڲ����ʺ�У��
	case ReceivedRFCommandByte5A5A00005A0001:
	case ReceivedRFCommandByte5A5A00005A0002:
		break;
   //RF������
  case ReceivedRFCommandByte5A5A00005A0003:
	case ReceivedRFCommandByte5A5A00005A0004:
		break;
	//RF�ŵ�
	case ReceivedRFCommandByte5A5A00005A0005:
	case ReceivedRFCommandByte5A5A00005A0006:
		break;
	//RF�ŵ���RF�����ʣ����ڲ����ʣ�����У��
	case ReceivedRFCommandByte5A5A00005A000E:
	case ReceivedRFCommandByte5A5A00005A0015:
		break;
	default:
		break;
	}

  //ToDo: ����RFCommand��Ϣ
	ProcessRFModuleMessage(rfcommandReceivedBuf, 0, rfcommandReceivedBufCount - 2);

	//����
	ResetRFCommandReceivedState();
}


/*-------------------------------------------------------------------------
	��������ProcessRFModuleMessage
	��  �ܣ�433����ģ��������մ���
	��  ����
	����ֵ��
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
//			//����RF����
//			//session.Status = SessionStatus.ConnectedRF;
//			//�ȱ������
//			switch (operationCode)
//			{
//			case 0x01://Ӧ��:���ô��ڲ���
//			case 0x02://Ӧ��:��ȡ���ڲ���
//				//ProcessRFModuleSerialParameter(allBytes[8], allBytes[9]);
//				RFSessionPortBaudRateCode = allBytes[8];
//				RFSessionPortParity = allBytes[9];
//				//log.Debug("���ڲ���: ������ " + RFDataService.PortBaudRate + ", У�� " + RFDataService.PortParity);
//				break;
//			case 0x03://Ӧ��:���ÿ��в���
//			case 0x04://Ӧ��:��ȡ���в���
//				//ProcessRFModuleAirParameter(allBytes[8], allBytes[9]);
//				RFSessionAirBaudRateCode = allBytes[8];
//				//log.Debug("���в���: ������ " + RFDataService.AirBaudRate);
//				break;
//			case 0x05://Ӧ��:�����ŵ�����
//				//ProcessRFModuleChannelParameter(allBytes[8], allBytes[9]);
//				RFSessionAirChannel = allBytes[8];
//				if(RFSessionAirChannel != GlobalVariable.RFSessionAirChannel)
//				{
//					GlobalVariable.RFSessionAirChannel = RFSessionAirChannel;
//					ShowRFChannel();
//				}
//				//log.Debug("���в���: �ŵ� " + RFDataService.AirChannel);
//				//��ȡUSV
//				//RFSessionReadUSVSetting();
//				break;
//			case 0x06://Ӧ��:��ȡ�ŵ�����
//				//ProcessRFModuleChannelParameter(allBytes[8], allBytes[9]);
//				RFSessionAirChannel = allBytes[8];
//				break;
//			case 0x15://Ӧ�𣺶�����
//			case 0x0E://Ӧ��д����
//				RFSessionAirChannel = allBytes[8];
//				RFSessionAirBaudRateCode = allBytes[9];
//				RFSessionPortBaudRateCode = allBytes[10];
//				RFSessionPortParity = allBytes[11];
//				break;
//			default:
//				//log.Error("ProcessRFModuleMessage: unknown message:\r\n" + RFDataService.DumpBytes(allText));
//				//���أ��������¼�
//				return;
//			}
//			//�󴥷��¼�
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
	��������ParseRFSessionText
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������ParseRFSessionByte
	��  �ܣ�
	��  ����
	����ֵ��
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
		case RFSessionReceivedByte42FB://ˮ��������������\x00\x42\xFB\x7C
		case RFSessionReceivedByte42F9://ˮ����������ҵ�GPS�ź�\x00\x42\xF9\x7C
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
	��������OnRFSessionParseError
	��  �ܣ������յ���Ϣ���������
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void OnRFSessionParseError(void)
{
	//log.Error("ERROR RF MSG: " + RFDataService.DumpBytes(receivedBuf, 0, receivedBufCount));
	ResetReceivedState();
}


/*-------------------------------------------------------------------------
	��������OnExpectByteFE
	��  �ܣ�
	��  ����
	����ֵ��
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
			//����
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
	��������IsValidBase64Byte
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnReceivedByteFD
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnReceivedByteFC
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnReceivedByteFA
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnRFSessionExpectByte
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnRFSessionExpect0A
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnRFSessionExpect0D
	��  �ܣ�
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void OnRFSessionExpect0D(u8 b)
{
	receivedBuf[receivedBufCount++] = b;
	switch (receivedState)
	{
	//�̶�7�ֽ���Ϣ, û�г����ֽ�
	case RFSessionReceivedByte4303://�л����Զ�ģʽ: 0x00, 0x43, 0x03, 0x7C, 0xC2, 0x0D, 0x0A
	case RFSessionReceivedByte4304://�л����ֶ�ģʽ: 0x00, 0x43, 0x04, 0x7C, 0xC3, 0x0D, 0x0A
	case RFSessionReceivedByte4307://ȡ������      : 0x00, 0x43, 0x07, 0x7C, 0xC6, 0x0D, 0x0A
	case RFSessionReceivedByte4332://��ͣ����      : 0x00, 0x43, 0x32, 0x7C, 0xC1, 0x0D, 0x0A
	case RFSessionReceivedByte4341://������        : 0x00, 0x43, 0x41, 0x7C, 0x00, 0x0D, 0x0A
	case RFSessionReceivedByte4352://�ָ�����      : 0x00, 0x43, 0x52, 0x7C, 0x64, 0x0D, 0x0A
	case RFSessionReceivedByte4353://��ͣ����      : 0x00, 0x43, 0x53, 0x7C, 0x65, 0x0D, 0x0A
	case RFSessionReceivedByte4361://�ؾ���        : 0x00, 0x43, 0x61, 0x7C, 0x20, 0x0D, 0x0A
		if (receivedBufCount >= 7)
		{
			if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
			{
				OnRFSessionParseOk();
			}
			else
			{//����
				OnRFSessionParseError();
			}
		}
		break;
	//�̶�8�ֽ���Ϣ, û�г����ֽ�
	//����    : 0x00, 0x43, 0x43, 0x7C, 0x41, 0x43, 0x0D, 0x0A;
	//¼��ʼ: 0x00, 0x43, 0x43, 0x7C, 0x51, 0x53, 0x0D, 0x0A;
	//¼��ֹͣ: 0x00, 0x43, 0x43, 0x7C, 0x61, 0x63, 0x0D, 0x0A;
	case RFSessionReceivedByte4343:
		if (receivedBufCount >= 8)
		{
			if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
			{
				OnRFSessionParseOk();
			}
			else
			{//����
				OnRFSessionParseError();
			}
		}
		break;
	//�̶�6��7�ֽ���Ϣ, û�г����ֽڣ� 0x7C��Ҫ����0x0D����Ϣ
	case RFSessionReceivedByte436E://��USVSetting  : 0x00, 0x43, 0x6E, 0x7C, 0x0D, 0x0A
	case RFSessionReceivedByte5343://�ֶ�����ֹͣ  ��0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
	case RFSessionReceivedByte5346://�ֶ��������  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
		if (b == 0x0D)
		{
			expectState = RFSessionExpectByte0A;
		}
		else
		{ //����
			OnRFSessionParseError();
		}
		break;
		//�̶�11�ֽ���Ϣ, û�г����ֽ�
		case RFSessionReceivedByte5353://�ֶ�����      : 0x00, 0x53, 0x53, 0x7C, n, d, m, v, w, xx, 0x0D, 0x0A; 0x00, 0x53, 0x53, 0x7C, n, d, m, v, w, CS1, CS2, 0x0D, 0x0A 
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
						//����
						OnRFSessionParseError();
					}
				}
			}
			break;
	//�䳤��Ϣ���������ֽ�, ��\r\n����79
	case RFSessionReceivedByte426E://����USVSetting��0x00, 0x42, 0x6E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A
	case RFSessionReceivedByte434E://����USVSetting��0x00, 0x43, 0x4E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A
		//if (receivedBufCount > 79)
		if (receivedBufCount > 82)
		{//������Ϣ����, ����
			OnRFSessionParseError();
		}
		if (receivedBufCount > 74 && b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]))
		{//���\r\n��β�����ҳ����ֽڷ���
			//expectState = RFSessionExpectStates.ExpectByte0A;
			//if (BoatDataService.IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
			{//У��OK
				OnRFSessionParseOk();
			}
			else
			{//У���
				OnRFSessionParseError();
			}
		}
		break;
	//�̶�����9�ֽ���Ϣ���޳����ֽ�
	case RFSessionReceivedByte42FB://���ؾ���      ��0x00, 0x42, 0xFB, 0x7C, 0x01, 0xB9, 0x07, 0x0D, 0x0A
	case RFSessionReceivedByte42F9://GPS����       : 0x00, 0x42, 0xF9, 0x7C, 0x01, 0xB7, 0x07, 0x0D, 0x0A
		if (receivedBufCount >= 9)
		{
			if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
			{
				OnRFSessionParseOk();
			}
			else
			{//������Ϣ����, ����
				OnRFSessionParseError();
			}
		}
		break;
	//�䳤64-161�ֽ���Ϣ����2�ֽ�У���1�ֽڳ��ȣ�
	case RFSessionReceivedByte4201://�Զ�ģʽ�����ݣ�0x00, 0x42, 0x01, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
		if (receivedBufCount > 200)
		{//������Ϣ����, ����
				OnRFSessionParseError();
		}
		else
		{
			if (receivedBufCount > 58 && b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]) && receivedBuf[receivedBufCount - 6] == 0x23)
			{//���\r\n��β�����ҳ����ֽڷ���
				//expectState = RFSessionExpectStates.ExpectByte0A;
				//if (BoatDataService.IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
				if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
				{//У��OK
					OnRFSessionParseOk();
				}
				else
				{//У���
					OnRFSessionParseError();
				}
			}
		}
		break;
	//�䳤36�ֽ���Ϣ����2�ֽ�У���1�ֽڳ���
	case RFSessionReceivedByte4234://�ֶ�ģʽ�����ݣ�0x00, 0x42, 0x34, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
		//if (receivedBufCount > 36)
		//if (receivedBufCount > 49)//�������¶ȣ���ҡ����ҡ������
		if (receivedBufCount > 64)
		{//������Ϣ����, ����
			OnRFSessionParseError();
		}
		else
		{
			if (receivedBufCount >= 25 && b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]) && receivedBuf[receivedBufCount - 6] == 0x23)
			{//���\r\n��β�����ҳ����ֽڷ���
				//expectState = RFSessionExpectStates.ExpectByte0A;
				//if (BoatDataService.IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
				if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
				{//У��OK
					OnRFSessionParseOk();
				}
				else
				{//У���
					OnRFSessionParseError();
				}
			}
		}
		break;
	//�䳤��Ϣ����2�ֽ�У���1�ֽڳ���
	case RFSessionReceivedByte4301://·�����������0x00, 0x43, 0x01, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
	case RFSessionReceivedByte4374://����·�������ã�0x00, 0x43, 0x74, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
		if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D && ((receivedBufCount - 2) % 256 == receivedBuf[receivedBufCount - 3]))
		{//���\r\n��β�����ҳ����ֽڷ���
			//if (IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			if (IsBoatDataMessageChecksumOk(receivedBuf, 0, receivedBufCount - 2))
			{//У��OK
				OnRFSessionParseOk();
			}
			else
			{//У���
				OnRFSessionParseError();
			}
		}
		if (receivedBufCount >= sizeof(receivedBuf))
		{//���ȴ�
			OnRFSessionParseError();
		}
		break;
	//�䳤��Ϣ����2�ֽ�У�飬û�г����ֽ�
	case RFSessionReceivedByte4300://�ֶ���������  : ��0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, LEN, 0D, 0A; �ϣ�0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, 0D, 0A
		if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
		{//���\r\n��β�����ҳ����ֽڷ���
			//if (IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			{//У��OK
				OnRFSessionParseOk();
			}
			//else
			{//У���
			//    OnRFSessionParseError();
			}
		}
		if (receivedBufCount > 32)
		{//���ȴ�
			OnRFSessionParseError();
		}
		break;
	default:
		if (b == 0x0A && receivedBuf[receivedBufCount - 2] == 0x0D)
		{//���\r\n��β�����ҳ����ֽڷ���
			//if (IsBoatDataMessageChecksumOk(receivedBuf, receivedBufCount - 5))
			{//У��OK
				OnRFSessionParseOk();
			}
			//else
			{//У���
			//    OnRFSessionParseError();
			}
		}
		if (receivedBufCount > 32)
		{//���ȴ�
			OnRFSessionParseError();
		}
		break;
	}
}


/*-------------------------------------------------------------------------
	��������RFSessionCheckSum
	��  �ܣ�����һ����Ϣ��CheckSum
	��  ����data						byte������ʽ��Ϣ
					startindex			��ʼ����
					len							���㳤��

	����ֵ���������CheckSum
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
	��������IsBoatDataMessageChecksumOk
	��  �ܣ�
	��  ����len������CS1��CS2�� LEN�ļ���
	����ֵ��
-------------------------------------------------------------------------*/
bool IsBoatDataMessageChecksumOk(u8* encode_alldata, int startindex, int len)
{
	int endindex = startindex + len;
	u16 countValue = RFSessionCheckSum(encode_alldata, startindex, len - 3);//CS1��CS2�� LEN
	u16 receivedValue = (u16)(encode_alldata[endindex - 3] * 256 + encode_alldata[endindex - 2]);

	if (countValue != receivedValue)
	{
		//log.Warn("BoatDataMessageChecksum Error: C " + countValue.ToString("X4") + "R " + receivedValue.ToString("X4") + ", " + RFDataService.DumpBytes(encode_alldata, startindex, len));
	}

	return countValue == receivedValue;
}


/*-------------------------------------------------------------------------
	��������ProcessBoatDataMessage
	��  �ܣ�
	��  ����
	����ֵ��
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
					if (startindex + 8 > endindex) //��ֹһ����Ϣֻ����һ����
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
					if (startindex + 8 > endindex) //��ֹһ����Ϣֻ����һ����
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
					if (startindex + 2 > endindex) //��ֹһ����Ϣֻ����һ����
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
					if (startindex + 1 > endindex) //��ֹһ����Ϣֻ����һ����
						return;
	        BoatData.BatLife =  bytearray[startindex + 1];
					//����
					//ShowBattery(BoatData.BatLife);
					
	        startindex += 1 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
		 case 7:
				startindex += 1 + 1;
         break;
	   case 10:    // pitch 2 byte
					if (startindex + 2 > endindex) //��ֹһ����Ϣֻ����һ����
						return;
	        BoatData.Pitch = (u16)((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]); 
	        startindex += 2 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	   case 11:    //roll 2 byte
					if (startindex + 2 > endindex) //��ֹһ����Ϣֻ����һ����
						return;
					BoatData.Roll = (u16)((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]); 
	        startindex += 2 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	   case 12:    //yaw 2 byte
	        //float tempfloat = (float)(((Int16)(bytearray[startindex + 1] * 256 + (int)bytearray[startindex + 2])) / 10.0f);                    
					if (startindex + 2 > endindex) //��ֹһ����Ϣֻ����һ����
						return;
					BoatData.Yaw = (u16)((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]);                        
	        startindex += 2 + 1;
					//if (startindex >= endindex)
					//	return;
	        break;
	    case 14:
	    {
					if (startindex + 1 > endindex) //��ֹһ����Ϣֻ����һ����
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
	        if (startindex + 2 > endindex) //��ֹһ����Ϣֻ����һ����
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
						if (startindex + 1 > endindex) //��ֹһ����Ϣֻ����һ����
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
			case 43://0x2b, ��ǰ���ڲ�ˮ��ƿ��
			{
					if (startindex + 1 > endindex) //��ֹһ����Ϣֻ����һ����
						return;
					BoatData.Bottle = bytearray[startindex + 1];	

					if(bottleNumberLocal != BoatData.Bottle && BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)
					{
//						LCD_ClearArea(2, 55, 108, 175);
//						LCD_WR_L_HanZi_String(30, 79 , (u8*)zheng zai cai yang);	//���ڲ���
//						LCD_WR_L_HanZi_String(27, 120 , (u8*)zhi_2 );
						if (BoatData.Bottle < 10)
						{
//							LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Bottle + 48);
//							LCD_WR_L_HanZi_String(91, 120 , (u8*)hao ping );
						}
						//10-20��ƿ��
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
					if (startindex + 4 > endindex) //��ֹһ����Ϣֻ����һ����
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
			case 91://���
			{
					if (startindex + 4 > endindex) //��ֹһ����Ϣֻ����һ����
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
	��������BytesToFloat
	��  �ܣ���4���ֽ�����ת��Ϊ�ַ���
	��  ����data		�ֽ�����
	����ֵ��������
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
	��������OnRFSessionParseOk
	��  �ܣ��ɹ��յ���Ϣ��������Ϣ 
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void OnRFSessionParseOk(void)
{
	//TIM_Cmd(TIM3, DISABLE); //�رճ�ʱ�����ж�3//20140730
	//���㳬ʱ����
	BoatDataTimeoutCount = 0;
	connect_ok = 1;
	//GlobalVariable.IsNeedDisplayBoatData = 1;
	//��ʾRF״̬
//	ShowRFChannel();
	
	switch (receivedState)
	{
		case RFSessionReceivedByte4201://�Զ�ģʽ�����ݣ�0x00, 0x42, 0x01, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
			//�յ��Զ�ģʽ����
			ProcessBoatDataMessage(receivedBuf, 0, receivedBufCount - 2);
			//��ʾ
//			ShowAutoModeBoatData();
			//��ʾ����
//			ShowBoatBatteryLife();
			receivedManualMessageCount = 0;
			break;
		case RFSessionReceivedByte4234://�ֶ�ģʽ�����ݣ�0x00, 0x42, 0x34, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
			//�յ��ֶ�ģʽ����
			ProcessBoatDataMessage(receivedBuf, 0, receivedBufCount - 2);
			
//			if(GlobalVariable.CurrentWindow == WIN_Manual)
//			{
//				ShowManualModeBoatData();
//			}
			//��ʾ����
//			ShowBoatBatteryLife();
			if(++receivedManualMessageCount > 100)
			{
				receivedManualMessageCount = 100;
			}
			break;
		case RFSessionReceivedByte426E://����USVSetting��0x00, 0x42, 0x6E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. �䳤��Ϣ���������ֽ�, ��󳤶�70
			//USVMessageReceivedStatusText = DateTime.Now + ": �յ�USVSetting";
			ProcessUSVSettingMessage(receivedBuf, 0, receivedBufCount - 5);
			break;
		case RFSessionReceivedByteFAFA:
			//USVMessageReceivedStatusText = DateTime.Now + ": �յ�USV��������";
			//u8 *decodedBytes = Base64.Decode(receivedBuf, 2, receivedBufCount - 4);
			//decodedBufferCount = base64_decode((char *)decodedBuffer, (char *)receivedBuf + 2, receivedBufCount - 4);
			decodedBufferCount = fnBase64Decode((char *)decodedBuffer, (char *)receivedBuf + 2, receivedBufCount - 4);
			//
			ProcessBoatDataMessageNew(decodedBuffer, 0, decodedBufferCount);
			//����
			//ResetReceivedState();
			//�������Ĵ���
			//ParseRFSessionText(decodedBytes, 0, decodedBytes.Length);
			//��ʾ
//			if (GlobalVariable.CurrentWindow == WIN_Auto)
//			{
//				ShowAutoModeBoatData();
//			}
//			else if(GlobalVariable.CurrentWindow == WIN_Manual)
//			{
//				ShowManualModeBoatData();
//			}
			//��ʾ����
//			ShowBoatBatteryLife();
			break;
		//case ReceivedByteFBFB:
		case RFSessionReceivedByteFCFC:
		case RFSessionReceivedByteFDFD:
			break;

		case RFSessionReceivedByte5353://�ֶ�����      : 0x00, 0x53, 0x53, 0x7C, n, d(5), m(0), v, w, 0x0D, 0x0A 
			//RFDataService.IsSampling = true;
			break;
		case RFSessionReceivedByte5343://�ֶ�����ֹͣ  ��0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
		case RFSessionReceivedByte5346://�ֶ��������  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
			//SampleInfo.IsBottleTaken[receivedBuf[3]-1] = TRUE;
			SampleInfo.IsBottleTaking[receivedBuf[3]-1] = FALSE;
			GlobalVariable.IsManualSampling = FALSE;
			//IsSampling = false;
//			if(GlobalVariable.CurrentWindow == WIN_Manual)
//			{
//					ShowSampling();
//			}
			break;

		case RFSessionReceivedByte42FB://���ؾ���      ��0x00, 0x42, 0xFB, 0x7C, 0x01, 0xB9, 0x07, 0x0D, 0x0A
		case RFSessionReceivedByte42F9://GPS����       : 0x00, 0x42, 0xF9, 0x7C, 0x01, 0xB7, 0x07, 0x0D, 0x0A

		case RFSessionReceivedByte4300://�ֶ���������  : 0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
		case RFSessionReceivedByte4301://·�����������0x00, 0x43, 0x01, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
		case RFSessionReceivedByte4303://�л����Զ�ģʽ: 0x00, 0x43, 0x03, 0x7C, 0xC2, 0x0D, 0x0A
		case RFSessionReceivedByte4304://�л����ֶ�ģʽ: 0x00, 0x43, 0x04, 0x7C, 0xC3, 0x0D, 0x0A
		case RFSessionReceivedByte4332://��ͣ����      : 0x00, 0x43, 0x32, 0x7C, 0xC1, 0x0D, 0x0A
		case RFSessionReceivedByte4307://ȡ������      : 0x00, 0x43, 0x07, 0x7C, 0xC6, 0x0D, 0x0A
		case RFSessionReceivedByte4341://������        : 0x00, 0x43, 0x41, 0x7C, 0x00, 0x0D, 0x0A
		case RFSessionReceivedByte4343://����          : 0x00, 0x43, 0x43, 0x7C, 0x41, 0x43, 0x0D, 0x0A; ¼��ʼ: 0x00, 0x43, 0x43, 0x7C, 0x51, 0x53, 0x0D, 0x0A; ¼��ֹͣ: 0x00, 0x43, 0x43, 0x7C, 0x61, 0x63, 0x0D, 0x0A;
		case RFSessionReceivedByte434E://����USVSetting��0x00, 0x43, 0x4E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. �䳤��Ϣ���������ֽ�, ��󳤶�70
		case RFSessionReceivedByte4352://�ָ�����      : 0x00, 0x43, 0x52, 0x7C, 0x64, 0x0D, 0x0A
		case RFSessionReceivedByte4353://��ͣ����      : 0x00, 0x43, 0x53, 0x7C, 0x65, 0x0D, 0x0A
		case RFSessionReceivedByte4361://�ؾ���        : 0x00, 0x43, 0x61, 0x7C, 0x20, 0x0D, 0x0A
		case RFSessionReceivedByte436E://��USVSetting  : 0x00, 0x43, 0x6E, 0x7C, 0x0D, 0x0A
		case RFSessionReceivedByte4374://����·�������ã�0x00, 0x43, 0x74, 0x7C, ..., CS1, CS2, LEN, 0D, 0A

		default:
			break;
	}
	
	//TIM_Cmd(TIM3, ENABLE);//�򿪳�ʱ�����ж�3//20140730 
}


/*-------------------------------------------------------------------------
	��������ProcessBoatDataMessageNew
	��  �ܣ�
	��  ����
	����ֵ��
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
	{//��Ļ�վ����ARM9��
		return;
	}
	if (header[0] != 0x00 || header[1] != 0x42)
	{
		if (header[0] == 0x00 && header[1] == 0x53)
		{
			//log.Debug("�ֶ�������ʼ");
			switch(header[2])
			{
				case 0x53:
					//�ֶ�������ʼ
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
					//�ֶ���������
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
					//��־�����ͷ����������ô�ֹͣ���Ͳ������������ݱ���
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
	case 0x01://�Զ�ģʽ
		//RFDataService.USVMessageReceivedStatusText = DateTime.Now + ": �յ��Զ�ģʽ����";
		ProcessBoatDataMessage(databytes, startindex, len);
		receivedManualMessageCount = 0;
		break;
	case 0x34://�ֶ�ģʽ
		//RFDataService.USVMessageReceivedStatusText = DateTime.Now + ": �յ��ֶ�ģʽ����";
		ProcessBoatDataMessage(databytes, startindex, len);
		if(++receivedManualMessageCount > 100)
		{
			receivedManualMessageCount = 100;
		}
		break;
	case 0x6E://USVSetting
		//RFDataService.USVMessageReceivedStatusText = DateTime.Now + ": �յ�USVSetting����";
		ProcessUSVSettingMessage(databytes, startindex, len-3);
		break;
	case 0xFB://�������
	case 0xF9://GSP�ҵ�
	default:
		break;
	}
}


/*-------------------------------------------------------------------------
	��������ProcessUSVSettingMessage
	��  �ܣ�
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void ProcessUSVSettingMessage(u8* bytearray, int offset, int len)
{
	int startindex = offset;
	int arraylength = offset + len;
	int i;

	while (startindex+1 < arraylength) //����case��8������count��ֻ��С�ڵ���8
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
				//����
//				ConfigurationSave();
				//��ʾ 
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
	��������OnRFSessionExpect7C
	��  �ܣ�
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void OnRFSessionExpect7C(u8 b)
 {
	receivedBuf[receivedBufCount++] = b;
	switch (receivedState)
	{
	//7C֮ǰ�����ݵ�
	case RFSessionReceivedByte5343://�ֶ�����ֹͣ  ��0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
	case RFSessionReceivedByte5346://�ֶ��������  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
		if (receivedBufCount >= 5)
		{
			if (b == 0x7C)
			{
				expectState = RFSessionExpectByte0D;
			}
			else
			{//����
				OnRFSessionParseError();
			}
		}
		break;
	//����7C��
	default:
		if (b == 0x7C)
		{
			expectState = RFSessionExpectByte0D;
		}
		else
		{//����
			OnRFSessionParseError();
		}
		break;
	}
}


/*-------------------------------------------------------------------------
	��������OnRFSessionReceivedStatesIdle
	��  �ܣ�
	��  ����
	����ֵ��
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
	��������OnRFSessionReceivedStatesByte00
	��  �ܣ�
	��  ����
	����ֵ��
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
		//����
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	��������OnRFSessionReceivedStatesByte42
	��  �ܣ�
	��  ����
	����ֵ��
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
		//����
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	��������OnRFSessionReceivedStatesByte43
	��  �ܣ�
	��  ����
	����ֵ��
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
		//����
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	��������OnRFSessionReceivedStatesByte53
	��  �ܣ�
	��  ����
	����ֵ��
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
		//����
		OnRFSessionParseError();
		break;
	}
}


/*-------------------------------------------------------------------------
	��������ResetReceivedState
	��  �ܣ�
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void ResetReceivedState(void)
{
	receivedBufCount = 0;
	receivedState = RFSessionReceivedByteIdle;
	expectState = RFSessionExpectIdle;
}


///*-------------------------------------------------------------------------
//	��������UpdateScreen
//	��  �ܣ�
//	��  ����
//	����ֵ��
//-------------------------------------------------------------------------*/
//void UpdateScreen(void)
//{
//	bool moving = FALSE;
//	bool sampling = FALSE;
//	char textBuffer[16];
//	
//	if (GlobalVariable.ControlMode != CONTROL_MODE_AUTO)
//		return;
//	//------------------���������ݵ���ʾ----------------------
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
//					LCD_WR_L_HanZi_String(30, 79, wei she zhi dao);	   //δ���õ�
//					LCD_WR_L_HanZi_String(45, 120 , hang ren wu); 	   //������
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
//					LCD_WR_L_HanZi_String(30, 79 , dao hang ren wu);   //��������
//					LCD_WR_L_HanZi_String(45, 120 , yi jie shu); 	   //�ѽ���
//				}
//				else
//				{
//					LCD_WR_S_String(5, 79, "Mission");
//					LCD_WR_S_String(5, 120 , "Teminated");
//				}
//				break;
//			}
//			case 65534://������
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				sampling = TRUE;
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//				{
//					LCD_WR_L_HanZi_String(30, 79 , zheng zai cai yang);	//���ڲ���
//					LCD_WR_L_HanZi_String(27, 120 , zhi_2 );
//					//10��ƿ������
//					if (BoatData.Bottle < 10)
//					{
//						LCD_WR_L_Char(57, 120, 0 + 48);
//						LCD_WR_L_Char(74, 120, BoatData.Bottle + 48);
//						LCD_WR_L_HanZi_String(91, 120 , hao ping );
//					}
//					//10-20��ƿ��
//					else if (BoatData.Bottle < 20 && BoatData.Bottle >= 10)
//					{
//						LCD_WR_L_Char(57, 120, 1 + 48);
//						LCD_WR_L_Char(74, 120, BoatData.Bottle - 10 + 48);
//						LCD_WR_L_HanZi_String(91, 120 , hao ping );
//					}
//				}
//				else {
//					sprintf(textBuffer, "Bottle %d" , BoatData.Bottle);
//					LCD_WR_S_String(5, 79 , "Sampling");	//���ڲ���
//					LCD_WR_S_String(5, 120 , textBuffer);
//				}
//				break;
//			}
////			case 65533://���߼����
////			{
////				LCD_ClearArea(30, 79, 82, 120);
////				LCD_WR_L_HanZi_String(30, 79 , zheng zai jian ce);	//
////				//LCD_WR_L_HanZi_String(30, 120 , zhi ); 	//��
////				break;
////			}
//			case 65532://�����ѱ�ȡ��
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//					LCD_WR_L_HanZi_String(30, 98 , dao hang qu xiao);	//����ȡ��
//				else 
//					LCD_WR_S_String(30, 98 , "Canceled");	//����ȡ��
//				break;
//			}
//			case 65531://�����ѱ���ͣ
//			{
//				LCD_ClearArea(2, 55, 108, 175);
//				if(GlobalVariable.Language == LANGUAGE_CHINESE)
//					LCD_WR_L_HanZi_String(30, 98 , dao hang zan ting);	//������ͣ 
//				else
//					LCD_WR_S_String(30, 98 , "Paused");	//������ͣ 
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
//							LCD_WR_L_HanZi_String(30, 79 , zheng zai dao hang);	 //���ڵ���
//							LCD_WR_L_HanZi_String(27, 120 , zhi_2 ); 	//��
//							if(BoatData.Status < 10)
//						{
//								LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status + 48); //�ֿ������16��ʼ��0
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
//						LCD_WR_L_HanZi_String(30, 79 , zheng zai jian_1 ce); //���ڼ��
//						LCD_WR_L_HanZi_String(27, 120 , zhi_2); //��
//						if(BoatData.Status - 128 * 256 < 10)
//						{
//							LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status - 128 * 256 + 48); //�ֿ������16��ʼ��0
//							LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status - 128 * 256 >= 10 && BoatData.Status - 128 * 256 <= 99)
//						{
//							LCD_WR_L_Char(57, 120, (BoatData.Status - 128 * 256) / 10 + 48);
//							LCD_WR_L_Char(74, 120, (BoatData.Status - 128 * 256) % 10 + 48);
//							LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)//99�ŵ�����
//						{
//							LCD_WR_L_Char(57, 120, ( BoatData.Status - 128 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(74, 120, ( (BoatData.Status - 128 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(91, 120, ( BoatData.Status - 128 * 256 ) % 10 + 48);
//							LCD_WR_L_HanZi_String(108, 120 , hao dian );	
//						}
//					}
//					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //�������
//					{
//						//LCD_WR_L_HanZi_String(2, 55 , zheng zai); //����
//						LCD_WR_L_HanZi_String(2, 55 , cai yang); //����
//						//LCD_WR_L_HanZi_String(65, 56 , maohao); 
//						LCD_WR_L_HanZi_String(2, 89 , jian_1 ce); //���
//						
//						if(BoatData.Status - 192 * 256 < 10)
//						{
//						 //	LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(60, 89, BoatData.Status - 192 * 256 + 48); //�ֿ������16��ʼ��0
//							LCD_WR_L_HanZi_String(75, 89 , hao dian );
//						}
//						else if(BoatData.Status - 192 * 256 >= 10 && BoatData.Status - 192 * 256 <= 99)
//						{
//								LCD_WR_L_Char(60, 89, (BoatData.Status - 192 * 256) / 10 + 48);
//							LCD_WR_L_Char(75, 89, (BoatData.Status - 192 * 256) % 10 + 48);
//							LCD_WR_L_HanZi_String(90, 89 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)  //99�ŵ�����
//						{
//							LCD_WR_L_Char(60, 89, ( BoatData.Status - 192 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(75, 89, ( (BoatData.Status - 192 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(90, 89, ( BoatData.Status - 192 * 256 ) % 10 + 48);
//							LCD_WR_L_HanZi_String(105, 89 , hao dian );	
//						}
//					
//						LCD_WR_L_HanZi_String(2, 123 , cai yang zhi_2); //������
//						//LCD_WR_L_Char(89, 123, BoatData.Bottle + 48);
//						if (BoatData.Bottle < 10)
//						{
//								LCD_WR_L_Char(57, 123, 0 + 48);
//								LCD_WR_L_Char(74, 123, BoatData.Bottle + 48);
//						}
//						//10-20��ƿ��
//						else if (BoatData.Bottle < 20 && BoatData.Bottle >= 10)
//						{
//								LCD_WR_L_Char(57, 123, 1 + 48);
//								LCD_WR_L_Char(74, 123, BoatData.Bottle - 10 + 48);
//						}
//						LCD_WR_L_HanZi_String(108, 123 , hao ping );	 //��ƿ
//					}
//				}
//				else
//				{
//					if (BoatData.Status >= 1 && BoatData.Status <= 255)
//					{
//						LCD_WR_S_String(30, 79 , "Moving to");	 //���ڵ���
//						if(BoatData.Status < 10)
//						{
//								LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status + 48); //�ֿ������16��ʼ��0
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
//						LCD_WR_S_String(30, 79 , "Monitoring"); //���ڼ��
//						if(BoatData.Status - 128 * 256 < 10)
//						{
//							LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(74, 120, BoatData.Status - 128 * 256 + 48); //�ֿ������16��ʼ��0
//							//LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if(BoatData.Status - 128 * 256 >= 10 && BoatData.Status - 128 * 256 <= 99)
//						{
//							LCD_WR_L_Char(57, 120, (BoatData.Status - 128 * 256) / 10 + 48);
//							LCD_WR_L_Char(74, 120, (BoatData.Status - 128 * 256) % 10 + 48);
//							//LCD_WR_L_HanZi_String(91, 120 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)//99�ŵ�����
//						{
//							LCD_WR_L_Char(57, 120, ( BoatData.Status - 128 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(74, 120, ( (BoatData.Status - 128 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(91, 120, ( BoatData.Status - 128 * 256 ) % 10 + 48);
//							//LCD_WR_L_HanZi_String(108, 120 , hao dian );	
//						}
//					}
//					else if (BoatData.Status >= 49152 && BoatData.Status <= 49152 + 255)   //�������
//					{
//						LCD_WR_S_String(2, 55 , "Sampling"); //����
//						//LCD_WR_L_HanZi_String(65, 56 , maohao); 
//						LCD_WR_S_String(2, 89 , "Monitoring"); //���
//						
//						if(BoatData.Status - 192 * 256 < 10)
//						{
//						 //	LCD_WR_L_Char(57, 120, 0 + 48);
//							LCD_WR_L_Char(60, 89, BoatData.Status - 192 * 256 + 48); //�ֿ������16��ʼ��0
//							//LCD_WR_L_HanZi_String(75, 89 , hao dian );
//						}
//						else if(BoatData.Status - 192 * 256 >= 10 && BoatData.Status - 192 * 256 <= 99)
//						{
//								LCD_WR_L_Char(60, 89, (BoatData.Status - 192 * 256) / 10 + 48);
//							LCD_WR_L_Char(75, 89, (BoatData.Status - 192 * 256) % 10 + 48);
//							//LCD_WR_L_HanZi_String(90, 89 , hao dian );
//						}
//						else if (BoatData.Status >= 100 && BoatData.Status <= 255)  //99�ŵ�����
//						{
//							LCD_WR_L_Char(60, 89, ( BoatData.Status - 192 * 256 ) / 100 + 48);
//							LCD_WR_L_Char(75, 89, ( (BoatData.Status - 192 * 256) % 100 ) / 10 + 48);
//							LCD_WR_L_Char(90, 89, ( BoatData.Status - 192 * 256 ) % 10 + 48);
//							//LCD_WR_L_HanZi_String(105, 89 , hao dian );	
//						}
//					
//						LCD_WR_S_String(2, 123 , "Sampling"); //������
//						//LCD_WR_L_Char(89, 123, BoatData.Bottle + 48);
//						if (BoatData.Bottle < 10)
//						{
//								LCD_WR_L_Char(57, 123, 0 + 48);
//								LCD_WR_L_Char(74, 123, BoatData.Bottle + 48);
//						}
//						//10-20��ƿ��
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
//	//�ոձ任��״̬����Ҫ��ʾ��λ��֮��Ͳ��и��µ�λ��
//	//if (BoatData.PreviousStatus != BoatData.Status)//��һ�ν�������
//	{
//		//LCD_ClearArea(185,137, 31, 130);
//		LCD_DrawCircle(260,141,2);
//		LCD_WR_S_String(265,137,"C");
//	}
//}

/*�����������===============================================================================================================*/
