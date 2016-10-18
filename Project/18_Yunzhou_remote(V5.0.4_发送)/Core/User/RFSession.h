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

#define RF_TRANSMIT_TIMES		20						//20即为200毫秒，数传模块每200毫秒传输一次命令
#define RF_TRANSMIT_TIMES_TOLER		5				//发送公差，由于其他额外开销，要保证遥控数据200毫秒发送一次	

#define MAX_SPEAKER_SEND_COUNT 3					//警报最大发送次数
#define MAX_ALERT_SEND_COUNT 3						//警灯最大发送次数


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


/* Exported variables ---------------------------------------------------------*/
extern u8 NeedSendSpeaker;													//警报声发送次数
extern u8 NeedSendAlertLED ;												//警报灯发送次数
extern u8 NeedSendSampleCount;											//采样发送次数
extern u8 NeedSendSampleFinishedCount;							//采样完成发送次数
extern u8 NeedSendAutoModeCommandCount;						//发送自动模式命令次数
extern u8 NeedSendValveCount;

extern bool EngineStarted;			//打火允许
extern bool Speaker;										//警报
extern bool Alert;											//警灯	
extern bool ValveStatus[5];											//阀门状态
extern bool ValveStatusUsed[5];									//阀门使用状态

extern u8 receivedManualMessageCount;

extern uint16_t RF_transmit_timer;						//数传模块定时传输计时器


/* Private function -----------------------------------------------*/
void GenerateTakeSampleCommandBase64(bool encoded);
void SendSampleFinished(bool encoded);
void GenerateAlarmCommandBase64(bool encoded);
void SendCommandAutoMode(bool encoded);
u8 GetValveValue(void);
void save_Valve_parameter(void);
void GenerateValveCommand(void);
void StopEngineTrigger(void);
void GenerateEngineStartCommandBase64(bool encoded);
void GenerateEngineStopCommandBase64(bool encoded);
void GenerateCommandBase64(bool encoded);

void OnCommandDataReceived(char c);
void ParseRFComamndByte(u8 b);
void ParseRFSessionText(u8* rfcommRecvedBuf, int offset, int count);

#endif
