/*-------------------------------------------------------------------------
工程名称：无线控制命令发送与接收
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150615 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
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
#define VALVE_QUANTITY	5					//阀门总数

//#define RF_TRANSMIT_TIMES		20						//20即为200毫秒，数传模块每200毫秒传输一次命令

#define RF_TRANSMIT_TIMES_TOLER		5				//发送公差，由于其他额外开销，要保证遥控数据200毫秒发送一次	

#define MAX_SPEAKER_SEND_COUNT 3					//警报最大发送次数
#define MAX_ALERT_SEND_COUNT 3						//警灯最大发送次数

#define BOATDATATIMEOUTVALUE	60					//船数据超时时间，以秒为单位
#define LINKTIMEOUT		5										//以秒为单位

#if defined QC_LOOP_TEST
#define RF_TRANSMIT_TIMES		50						//30即为300毫秒，数传模块每300毫秒传输一次命令
#define QC_TEST_TIME		100						//质检测试时间间隔
#define QC_TEST_STATUS	10						//测试序列指令种类，共10种，分述如下：

#else

#define RF_TRANSMIT_TIMES		30						//30即为300毫秒，数传模块每300毫秒传输一次命令

#endif                                      //1、左前进；2、右后退；3、右前进；4、左后退；5、1号瓶采用500ml；6、2号瓶采样；7、3号瓶采样；8、4号瓶采样；9、警报；10、警灯


//USV接收信息解析
typedef enum _RFSessionReceivedStates
{
        RFSessionReceivedByteIdle,

        RFSessionReceivedByte00,
        RFSessionReceivedByte42,
        RFSessionReceivedByte43,
        RFSessionReceivedByte53,

        RFSessionReceivedByte4201,//自动模式船数据：0x00, 0x42, 0x01, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
        RFSessionReceivedByte4234,//手动模式船数据：0x00, 0x42, 0x34, 0x7C, ..., 0x23, CS1, CS2, LEN, 0D, 0A
        RFSessionReceivedByte426E,//返回USVSetting：0x00, 0x42, 0x6E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. 变长消息，含长度字节, 最大长度70
        RFSessionReceivedByte42FB,//主控就绪      ：0x00, 0x42, 0xFB, 0x7C, 0x01, 0xB9, 0x07, 0x0D, 0x0A
        RFSessionReceivedByte42F9,//GPS就绪       : 0x00, 0x42, 0xF9, 0x7C, 0x01, 0xB7, 0x07, 0x0D, 0x0A

        RFSessionReceivedByte4300,//手动控制命令新: 0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, LEN, 0D, 0A; 手动控制命令老：0x00, 0x43, 0x00, 0x7C, ..., CS1, CS2, 0D, 0A
        RFSessionReceivedByte4301,//路径点设置命令：0x00, 0x43, 0x01, 0x7C, ..., CS1, CS2, LEN, 0D, 0A
        RFSessionReceivedByte4303,//切换到自动模式: 0x00, 0x43, 0x03, 0x7C, 0xC2, 0x0D, 0x0A
        RFSessionReceivedByte4304,//切换到手动模式: 0x00, 0x43, 0x04, 0x7C, 0xC3, 0x0D, 0x0A
        RFSessionReceivedByte4332,//暂停发送      : 0x00, 0x43, 0x32, 0x7C, 0xC1, 0x0D, 0x0A
        RFSessionReceivedByte4307,//取消任务      : 0x00, 0x43, 0x07, 0x7C, 0xC6, 0x0D, 0x0A
        RFSessionReceivedByte4341,//开警报        : 0x00, 0x43, 0x41, 0x7C, 0x00, 0x0D, 0x0A
        RFSessionReceivedByte4343,//拍照          : 0x00, 0x43, 0x43, 0x7C, 0x41, 0x43, 0x0D, 0x0A; 录像开始: 0x00, 0x43, 0x43, 0x7C, 0x51, 0x53, 0x0D, 0x0A; 录像停止: 0x00, 0x43, 0x43, 0x7C, 0x61, 0x63, 0x0D, 0x0A;
        RFSessionReceivedByte434E,//设置USVSetting：0x00, 0x43, 0x4E, 0x7C, ...., CS1, CS2, LEN, 0x0D, 0x0A. 变长消息，含长度字节, 最大长度70
        RFSessionReceivedByte4352,//恢复任务      : 0x00, 0x43, 0x52, 0x7C, 0x64, 0x0D, 0x0A
        RFSessionReceivedByte4353,//暂停任务      : 0x00, 0x43, 0x53, 0x7C, 0x65, 0x0D, 0x0A
        RFSessionReceivedByte4361,//关警报        : 0x00, 0x43, 0x61, 0x7C, 0x20, 0x0D, 0x0A
        RFSessionReceivedByte436E,//读USVSetting  : 0x00, 0x43, 0x6E, 0x7C, 0x0D, 0x0A
        RFSessionReceivedByte4374,//测试路径点设置：0x00, 0x43, 0x74, 0x7C, ..., CS1, CS2, LEN, 0D, 0A

        RFSessionReceivedByte5343,//手动采样停止  ：0x00, 0x53, 0x43, n, 0x7C, 0x0D, 0x0A
        RFSessionReceivedByte5346,//手动采样完成  : 0x00, 0x53, 0x46, n, 0x7C, 0x0D, 0x0A
        RFSessionReceivedByte5353,//手动采样      : 0x00, 0x53, 0x53, 0x7C, n, d(5), m(0), v, w, 0x0D, 0x0A 

        RFSessionReceivedByteFA,
        //RFSessionReceivedByteFB,
        RFSessionReceivedByteFC,
        RFSessionReceivedByteFD,

        RFSessionReceivedByteFAFA,
        //RFSessionReceivedByteFBFB,
        RFSessionReceivedByteFCFC,
        RFSessionReceivedByteFDFD,

}RFSessionReceivedStates;

//433数传模块命令解析
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

//结束标记
typedef enum _RFSessionExpectStates
{
        RFSessionExpectIdle,
        RFSessionExpectByte0D,
        RFSessionExpectByte0A,

        RFSessionExpectByteFE,
        RFSessionExpectByteFEFE,
}RFSessionExpectStates;

//USV通用数据
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

//USV设置信息
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

//第一代遥控全局变量
typedef struct {
	u8 CurrentWindow;
	u8 CurrentDecision;
	u8 ControlMode;
	bool Speaker;//警报
	bool Alert;//警灯
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
	u8 RFSessionPortBaudRateCode;//configuration offset:9,//Baud rate . 02-----1200，03----2400，04----4800，05---9600，06--19200
	u8 RFSessionPortParity;//configuration offset:10, //verity: 00----None，01-----Even，02------Odd
	u8 RFSessionAirBaudRateCode;//configuration offset:11,//RF Baud rate : 02-----1200，03----2400，04----4800，05---9600，06—19200，07--38400
	//01-1st channel (428.0028), 02—2nd channel (429.0012), 03—3rd channel (433.3020),
  //04—4th channel (433.9164), 05—5th channel (433.1176), 06—6th channel (433.6706)
  //07—7th channel (433.8286), 08—8th channel (434.5308)
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
	//u8 BoatDataChange;		      //luhaisen增加  每次串口数据前一次与后一次不同时,遥控器电量显示就更新一次,以节约CPU资源..
	//u8 IsNeedDisplayBoatData;	  //luhaisen增加  若遥控器开启,而船没有开启,此参数标记,遥控器不显示船的电量
	//u8 JudgeBoatData;             //luhaisen增加, 若遥控器和船均开启,遥控器有显示船的电量,若只船突然关闭,此
																//参数标记,8S后让遥控器显示电量为0,在此luhaisen一共定义3个变量.
	u8 BoatDataTimeoutCount;
	//bool BoatDataTimeout;
}GlobalVariableTypeDef;

//第一代遥控船型
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

//采样相关变量
typedef struct {
	u8 BottleNumber;//采样瓶号
	u8 Depth;//采样深度
	u8 Mode;//采样模式
	u8 Volume;//百毫升
	u8 WashMode;//清洗模式
	//bool IsBottleTaken[10];
	bool IsBottleTaking[12];
	u8 VolumeTaken[12];
}SampleInfoTypeDef;


/* Exported variables ---------------------------------------------------------*/
extern u8 NeedSendSpeaker;													//警报声发送次数
extern u8 NeedSendAlertLED ;												//警报灯发送次数
extern u8 NeedSendSampleCount;											//采样发送次数
extern u8 NeedSendSampleFinishedCount;							//采样完成发送次数
extern u8 NeedSendAutoModeCommandCount;						//发送自动模式命令次数
extern u8 NeedSendValveCount;

extern uint8_t connect_ok;							//0表示连接没成功，1表示连接成功

extern bool EngineStarted;			//打火允许
extern bool Speaker;										//警报
extern bool Alert;											//警灯	
extern bool ValveStatus[5];											//阀门状态
extern bool ValveStatusUsed[5];									//阀门使用状态

extern u8 receivedManualMessageCount;

extern uint16_t RF_transmit_timer;						//数传模块定时传输计时器

extern uint8_t BoatDataTimeoutCount;					//船数据超时计时器，秒计时

extern BoatDataTypeDef BoatData;									//USV通用回传数据
extern USVSetting receivedUSVSetting;						//USV设置信息
extern GlobalVariableTypeDef GlobalVariable;			//全局变量
extern SampleInfoTypeDef SampleInfo;							//采样相关变量


#if defined QC_LOOP_TEST
extern uint16_t QC_Test_Timer;				//质检测试用计数器
extern uint8_t QC_Test_Flag;					//质检测试用命令顺序
extern uint8_t QC_Sample_Flag;			

extern uint16_t QC_Motor_Timer;				//质检测试电机用计数器
extern uint8_t QC_Motor_Flag;					//质检测试电机用命令顺序
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
