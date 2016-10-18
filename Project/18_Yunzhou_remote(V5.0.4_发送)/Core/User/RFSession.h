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

#define RF_TRANSMIT_TIMES		20						//20��Ϊ200���룬����ģ��ÿ200���봫��һ������
#define RF_TRANSMIT_TIMES_TOLER		5				//���͹�������������⿪����Ҫ��֤ң������200���뷢��һ��	

#define MAX_SPEAKER_SEND_COUNT 3					//��������ʹ���
#define MAX_ALERT_SEND_COUNT 3						//��������ʹ���


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


/* Exported variables ---------------------------------------------------------*/
extern u8 NeedSendSpeaker;													//���������ʹ���
extern u8 NeedSendAlertLED ;												//�����Ʒ��ʹ���
extern u8 NeedSendSampleCount;											//�������ʹ���
extern u8 NeedSendSampleFinishedCount;							//������ɷ��ʹ���
extern u8 NeedSendAutoModeCommandCount;						//�����Զ�ģʽ�������
extern u8 NeedSendValveCount;

extern bool EngineStarted;			//�������
extern bool Speaker;										//����
extern bool Alert;											//����	
extern bool ValveStatus[5];											//����״̬
extern bool ValveStatusUsed[5];									//����ʹ��״̬

extern u8 receivedManualMessageCount;

extern uint16_t RF_transmit_timer;						//����ģ�鶨ʱ�����ʱ��


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
