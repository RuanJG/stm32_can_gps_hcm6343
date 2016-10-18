/*-------------------------------------------------------------------------
�������ƣ����߿�������������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150615 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFSESSION_H
#define __RFSESSION_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "base64.h"
#include "SetUART.h"
#include "stdbool.h"
#include "keyboard_app.h"


/* Exported define ------------------------------------------------------------*/
#define VALVE_QUANTITY	5					//��������

//#define RF_TRANSMIT_TIMES		20						//20��Ϊ200���룬����ģ��ÿ200���봫��һ������

#define RF_TRANSMIT_TIMES_TOLER		5				//���͹�������������⿪����Ҫ��֤ң������200���뷢��һ��	

#define MAX_SPEAKER_SEND_COUNT 3					//��������ʹ���
#define MAX_ALERT_SEND_COUNT 3						//��������ʹ���

#define BOATDATATIMEOUTVALUE	60					//�����ݳ�ʱʱ�䣬����Ϊ��λ
#define LINKTIMEOUT		5										//����Ϊ��λ

#if defined QC_LOOP_TEST
#define RF_TRANSMIT_TIMES		50						//30��Ϊ300���룬����ģ��ÿ300���봫��һ������
#define QC_TEST_TIME		100						//�ʼ����ʱ����
#define QC_TEST_STATUS	10						//��������ָ�����࣬��10�֣��������£�

#else

#define RF_TRANSMIT_TIMES		30						//30��Ϊ300���룬����ģ��ÿ300���봫��һ������

#endif                                      //1����ǰ����2���Һ��ˣ�3����ǰ����4������ˣ�5��1��ƿ����500ml��6��2��ƿ������7��3��ƿ������8��4��ƿ������9��������10������


//USV������Ϣ����
typedef enum _RFSessionReceivedStates
{
        RFSessionReceivedByteIdle,

        RFSessionReceivedByte00,
        RFSessionReceivedByte42,
        RFSessionReceivedByte43,
        RFSessionReceivedByte53,

        RFSessionReceivedByte4201,//�Զ�ģʽ�����ݣ�0x00, 0x42, 0x01, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
        RFSessionReceivedByte4234,//�ֶ�ģʽ�����ݣ�0x00, 0x42, 0x34, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
        RFSessionReceivedByte426E,//����USVSetting��0x00, 0x42, 0x6E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. �䳤��Ϣ���������ֽ�, ��󳤶�70
        RFSessionReceivedByte42FB,//���ؾ���      ��0x00, 0x42, 0xFB, 0x7C, 0x01, 0xB9, 0x07, 0x0D, 0x0A
        RFSessionReceivedByte42F9,//GPS����       : 0x00, 0x42, 0xF9, 0x7C, 0x01, 0xB7, 0x07, 0x0D, 0x0A

        RFSessionReceivedByte4300,//�ֶ�����������: 0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, LEN, 0D, 0A; �ֶ����������ϣ�0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, 0D, 0A
        RFSessionReceivedByte4301,//·�����������0x00, 0x43, 0x01, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
        RFSessionReceivedByte4303,//�л����Զ�ģʽ: 0x00, 0x43, 0x03, 0x7C, 0xC2, 0x0D, 0x0A
        RFSessionReceivedByte4304,//�л����ֶ�ģʽ: 0x00, 0x43, 0x04, 0x7C, 0xC3, 0x0D, 0x0A
        RFSessionReceivedByte4332,//��ͣ����      : 0x00, 0x43, 0x32, 0x7C, 0xC1, 0x0D, 0x0A
        RFSessionReceivedByte4307,//ȡ������      : 0x00, 0x43, 0x07, 0x7C, 0xC6, 0x0D, 0x0A
        RFSessionReceivedByte4341,//������        : 0x00, 0x43, 0x41, 0x7C, 0x00, 0x0D, 0x0A
        RFSessionReceivedByte4343,//����          : 0x00, 0x43, 0x43, 0x7C, 0x41, 0x43, 0x0D, 0x0A; ¼��ʼ: 0x00, 0x43, 0x43, 0x7C, 0x51, 0x53, 0x0D, 0x0A; ¼��ֹͣ: 0x00, 0x43, 0x43, 0x7C, 0x61, 0x63, 0x0D, 0x0A;
        RFSessionReceivedByte434E,//����USVSetting��0x00, 0x43, 0x4E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. �䳤��Ϣ���������ֽ�, ��󳤶�70
        RFSessionReceivedByte4352,//�ָ�����      : 0x00, 0x43, 0x52, 0x7C, 0x64, 0x0D, 0x0A
        RFSessionReceivedByte4353,//��ͣ����      : 0x00, 0x43, 0x53, 0x7C, 0x65, 0x0D, 0x0A
        RFSessionReceivedByte4361,//�ؾ���        : 0x00, 0x43, 0x61, 0x7C, 0x20, 0x0D, 0x0A
        RFSessionReceivedByte436E,//��USVSetting  : 0x00, 0x43, 0x6E, 0x7C, 0x0D, 0x0A
        RFSessionReceivedByte4374,//����·�������ã�0x00, 0x43, 0x74, 0x7C, ..., CS1, CS2, LEN, 0D, 0A

        RFSessionReceivedByte5343,//�ֶ�����ֹͣ  ��0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
        RFSessionReceivedByte5346,//�ֶ��������  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
        RFSessionReceivedByte5353,//�ֶ�����      : 0x00, 0x53, 0x53, 0x7C, n, d(5), m(0), v, w, 0x0D, 0x0A 

        RFSessionReceivedByteFA,
        //RFSessionReceivedByteFB,
        RFSessionReceivedByteFC,
        RFSessionReceivedByteFD,

        RFSessionReceivedByteFAFA,
        //RFSessionReceivedByteFBFB,
        RFSessionReceivedByteFCFC,
        RFSessionReceivedByteFDFD,

}RFSessionReceivedStates;

//433����ģ���������
typedef enum _RFCommandReceivedStates
{
        ReceivedRFCommandIdle,

        ReceivedRFCommandByte5A,
        ReceivedRFCommandByte5A5A,
        ReceivedRFCommandByte5A5A00,
        ReceivedRFCommandByte5A5A0000,
        ReceivedRFCommandByte5A5A00005A,
        ReceivedRFCommandByte5A5A00005A00,
        ReceivedRFCommandByte5A5A00005A0001,
        ReceivedRFCommandByte5A5A00005A0002,
        ReceivedRFCommandByte5A5A00005A0003,
        ReceivedRFCommandByte5A5A00005A0004,
        ReceivedRFCommandByte5A5A00005A0005,
        ReceivedRFCommandByte5A5A00005A0006,
        ReceivedRFCommandByte5A5A00005A000E,
        ReceivedRFCommandByte5A5A00005A0015,
}RFCommandReceivedStates;

//�������
typedef enum _RFSessionExpectStates
{
        RFSessionExpectIdle,
        RFSessionExpectByte0D,
        RFSessionExpectByte0A,

        RFSessionExpectByteFE,
        RFSessionExpectByteFEFE,
}RFSessionExpectStates;

//USVͨ������
typedef struct {
	u8 Latitude[8];
	u8 Longitude[8];
	u8 PreviousLatitude[8];
	u8 PreviousLongitude[8];
	u8 PreviousPitch[2];
	u8 PreviousRoll[2];
	u16 Speed;
	float PreviousSpeed;
	
	double DoubleLatitude;
	double DoubleLongitude;

	
	//u8 BoatType;
	
	u16 Pitch;
	u16 Roll;
	u16 Yaw;
	u8 Mode;
	u16 Status;
	u16 PreviousStatus;
	u8 BatLife;
	u8 PreviousBatLife;
	u16 Ultra1;
	u16 Ultra2;
	u8 InternalTemp;
	u8 PreviousInternalTemp;
	u32 ErrorMessage;
	float WaterTemp;
	float PreviousWaterTemp;
	u8 PreviousBottle;
	u8 Bottle;
	float Depth;
}BoatDataTypeDef;

//USV������Ϣ
typedef struct _USVSetting{
	float P;
	float I;
	float D;
	float SumUpperLimit_forI;
	u8 RudderMiddle;
	float RANGE;
	u8 BoxType;
	u8 BoatType;
	u16 MissionID;
	u16 Interval;
	u16 Interval2;
	u8 ServerIP[4];
	u16	ServerPort;
	u8 BoatID[14];
	u8 VideoIP[4];
	u8 Channel;
	u16 PumpSpeed;
}USVSetting;

//��һ��ң��ȫ�ֱ���
typedef struct {
	u8 CurrentWindow;
	u8 CurrentDecision;
	u8 ControlMode;
	bool Speaker;//����
	bool Alert;//����
	bool EngineStarted;
	bool IsRFChannelSetting;
	bool IsRFSessionSending;
	

	//begin: configuration saved in flash
	u8 ConfigVersion; //configuration offset:0
  u8 ProtocolVersion;//configuration offset:1
	u8 ControllerType;//configuration offset:2
	u8 Sound;//configuration offset:3
	u8 ColorPlan;//configuration offset:4
	u8 RudderMiddle;//configuration offset:5
	u8 SpeedMiddle;//configuration offset:6
	
//	USV_Boat_Types BoatType;//configuration offset:8 //u8 BoatType; //1 for ss30, 2 for es30, 3 for ms70, 4 for mm70, 5 for platform30, 6 for platform70,
	u8 RFSessionPortBaudRateCode;//configuration offset:9,//Baud rate . 02-----1200��03----2400��04----4800��05---9600��06--19200
	u8 RFSessionPortParity;//configuration offset:10, //verity: 00----None��01-----Even��02------Odd
	u8 RFSessionAirBaudRateCode;//configuration offset:11,//RF Baud rate : 02-----1200��03----2400��04----4800��05---9600��06��19200��07--38400
	//01-1st channel (428.0028), 02��2nd channel (429.0012), 03��3rd channel (433.3020),
  //04��4th channel (433.9164), 05��5th channel (433.1176), 06��6th channel (433.6706)
  //07��7th channel (433.8286), 08��8th channel (434.5308)
	u8 RFSessionAirChannel;//configuration offset:12
	u8 ManualControlMode;
	u8 Language;
	u8 DebugSwitch;
	//end
	
	bool IsManualSampling;
	u8 Hour;
	u8 Minute;
	u8 Second;
	bool IsCountDownStopped;
	//u8 BoatDataChange;		      //luhaisen����  ÿ�δ�������ǰһ�����һ�β�ͬʱ,ң����������ʾ�͸���һ��,�Խ�ԼCPU��Դ..
	//u8 IsNeedDisplayBoatData;	  //luhaisen����  ��ң��������,����û�п���,�˲������,ң��������ʾ���ĵ���
	//u8 JudgeBoatData;             //luhaisen����, ��ң�����ʹ�������,ң��������ʾ���ĵ���,��ֻ��ͻȻ�ر�,��
																//�������,8S����ң������ʾ����Ϊ0,�ڴ�luhaisenһ������3������.
	u8 BoatDataTimeoutCount;
	//bool BoatDataTimeout;
}GlobalVariableTypeDef;

//��һ��ң�ش���
//typedef enum _USV_Boat_Types{
//	USV_Boat_Type_SS30 		= 1,
//	USV_Boat_Type_ES30 		= 2,
//	USV_Boat_Type_ESM30	 	= 3,
//	USV_Boat_Type_MS70 		= 4,
//	USV_Boat_Type_ME70 		= 5,
//	USV_Boat_Type_MM70	 	= 6,
//	USV_Boat_Type_MC70 		= 7,
//	USV_Boat_Type_Surf20 	= 8,
//	USV_Boat_Type_SS20		= 9,
//	USV_Boat_Type_ES20		= 10,
//	USV_Boat_Type_ESM20		= 11,
//	USV_Boat_Type_MS120   = 12,
//	USV_Boat_Type_ME120   = 13,
//	USV_Boat_Type_MM120   = 14,
//	USV_Boat_Type_MC120   = 15,
//	USV_Boat_Type_ME300 	= 16,
//}USV_Boat_Types;

//������ر���
typedef struct {
	u8 BottleNumber;//����ƿ��
	u8 Depth;//�������
	u8 Mode;//����ģʽ
	u8 Volume;//�ٺ���
	u8 WashMode;//��ϴģʽ
	//bool IsBottleTaken[10];
	bool IsBottleTaking[12];
	u8 VolumeTaken[12];
}SampleInfoTypeDef;


/* Exported variables ---------------------------------------------------------*/
extern u8 NeedSendSpeaker;													//���������ʹ���
extern u8 NeedSendAlertLED ;												//�����Ʒ��ʹ���
extern u8 NeedSendSampleCount;											//�������ʹ���
extern u8 NeedSendSampleFinishedCount;							//������ɷ��ʹ���
extern u8 NeedSendAutoModeCommandCount;						//�����Զ�ģʽ�������
extern u8 NeedSendValveCount;

extern uint8_t connect_ok;							//0��ʾ����û�ɹ���1��ʾ���ӳɹ�

extern bool EngineStarted;			//�������
extern bool Speaker;										//����
extern bool Alert;											//����	
extern bool ValveStatus[5];											//����״̬
extern bool ValveStatusUsed[5];									//����ʹ��״̬

extern u8 receivedManualMessageCount;

extern uint16_t RF_transmit_timer;						//����ģ�鶨ʱ�����ʱ��

extern uint8_t BoatDataTimeoutCount;					//�����ݳ�ʱ��ʱ�������ʱ

extern BoatDataTypeDef BoatData;									//USVͨ�ûش�����
extern USVSetting receivedUSVSetting;						//USV������Ϣ
extern GlobalVariableTypeDef GlobalVariable;			//ȫ�ֱ���
extern SampleInfoTypeDef SampleInfo;							//������ر���


#if defined QC_LOOP_TEST
extern uint16_t QC_Test_Timer;				//�ʼ�����ü�����
extern uint8_t QC_Test_Flag;					//�ʼ����������˳��
extern uint8_t QC_Sample_Flag;			

extern uint16_t QC_Motor_Timer;				//�ʼ���Ե���ü�����
extern uint8_t QC_Motor_Flag;					//�ʼ���Ե��������˳��
#endif


/* Private function -----------------------------------------------*/
void GenerateTakeSampleCommandBase64(bool encoded);
void SendSampleFinished(bool encoded);
void GenerateAlarmCommandBase64(bool encoded);
void SendCommandAutoMode(bool encoded);
u8 GetValveValue(void);
void save_Valve_parameter(void);
void load_Valve_parameter(void);
void GenerateValveCommand(void);
void StopEngineTrigger(void);
void GenerateEngineStartCommandBase64(bool encoded);
void GenerateEngineStopCommandBase64(bool encoded);
void GenerateCommandBase64(bool encoded);

void OnCommandDataReceived(char c);
void OnRFComamndTextParseError(void);
void ResetRFCommandReceivedState(void);
int CountRFModuleCheckSum(u8* bytes, int offset, int count);
bool IsBoatDataMessageChecksumOk(u8* encode_alldata, int startindex, int len);
void OnRFComamndTextParseOk(void);
void ProcessRFModuleMessage(u8* allBytes, int offset, int count);
void ParseRFComamndByte(u8 b);
void ParseRFSessionText(u8* rfcommRecvedBuf, int offset, int count);
void ParseRFSessionByte(u8 b);
void OnRFSessionExpectByte(u8 b);
float BytesToFloat(u8* data);
void OnRFSessionExpect7C(u8 b);
void OnRFSessionExpect0D(u8 b);
void OnRFSessionExpect0A(u8 b);
void OnReceivedByteFA(u8 c);
void OnReceivedByteFC(u8 c);
void OnReceivedByteFD(u8 c);
void OnExpectByteFE(u8 c);
bool IsValidBase64Byte(u8 c);
void OnRFSessionParseError(void);
void OnRFSessionParseOk(void);
void OnRFSessionReceivedStatesIdle(u8 b);
void OnRFSessionReceivedStatesByte00(u8 b);
void OnRFSessionReceivedStatesByte42(u8 b);
void OnRFSessionReceivedStatesByte43(u8 b);
void OnRFSessionReceivedStatesByte53(u8 b);
void ResetReceivedState(void);
void ProcessUSVSettingMessage(u8* bytearray, int offset, int len);
void ProcessBoatDataMessage(u8* bytearray, int startindex, int len);
void ProcessBoatDataMessageNew(u8* databytes, int startindex, int len);

#endif
