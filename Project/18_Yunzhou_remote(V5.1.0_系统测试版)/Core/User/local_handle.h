/*-------------------------------------------------------------------------
�������ƣ�ң�������ش�����س���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150612 ������    5.0.0		�½���
																		
					
					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOCAL_HANDLE_H
#define __LOCAL_HANDLE_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"


/* Exported define ------------------------------------------------------------*/
#define LOCAL_PARAMETER_QUANTITY 7				//ң�����������ò�����������ģ�飩
#define WATER_SAMPLE_PARAMETER	5					//��ˮ��������

#define USV_Boat_Type_SF20 			0
#define USV_Boat_Type_SS20 			1
//#define USV_Boat_Type_SM20 			2
#define USV_Boat_Type_ESM20 			2

#define USV_Boat_Type_SS30 			3
#define USV_Boat_Type_ESM30 		4
//#define USV_Boat_Type_SSH30 		5
#define USV_Boat_Type_ES30 			5
#define USV_Boat_Type_ESMH30 		6

#define USV_Boat_Type_MC70 			7
#define USV_Boat_Type_MM70 			8
#define USV_Boat_Type_MS70 			9
#define USV_Boat_Type_ME70 			10
#define USV_Boat_Type_MMH70 		11

#define USV_Boat_Type_MC120 		12
#define USV_Boat_Type_ME120 		13
#define USV_Boat_Type_MS120 		14
#define USV_Boat_Type_MM120 		15
#define USV_Boat_Type_ME300 		16

#define USV_Boat_Type_LE2000 		17
#define USV_Boat_Type_LE3000 		18
//#define USV_Boat_Type_LP3000 		16
//#define USV_Boat_Type_LC4000 		17
//#define USV_Boat_Type_LG4000 		18


#define MANUAL_CONTROL_MODE_MONO 	0				//����ģʽ
#define MANUAL_CONTROL_MODE_DUAL 	1				//˫��ģʽ


//��ˮ��ز���
#define MaxBottleNumber4 4
#define MaxBottleNumber8 8
#define MaxBottleNumber10 10
#define MaxBottleNumber12 12
//#define MaxBottleNumber20 20
#define MaxVolume 20
#define MaxDepth 5
#define MinDepth 5
#define MinVolume 1
#define MinBottleNumber 1


/* Exported variables ---------------------------------------------------------*/
extern uint32_t local_para_conf[LOCAL_PARAMETER_QUANTITY];					//�洢ң�����������ò�����������ģ�飩
/*------------------------------------------------------------------------------------------------
������			����										����											ȡֵ��Χ
0						LCD_Backlight						�������ȵȼ�							0 ~ 15				��ֵԽ����ĻԽ��
1						Control_Mode						����ģʽ									0 ~ 1					0Ϊ����ģʽ��1Ϊ˫��ģʽ
2						USV_Series							����ϵ��									0 ~ 18				0Ϊ 	SF(J)20 			20������
																																						1Ϊ		SS(J)20				20������
																																						2Ϊ		SM(J)20				20��⴬
																																						
																																						3Ϊ		SS(J)30				30������
																																						4Ϊ		ESM(J)30			30������⴬
																																						5Ϊ		SSH(J)30			30�ֲ������
																																						6Ϊ		ESMH(J)30			30�ֲ������⴬
																																						
																																						7Ϊ		MS70					70������
																																						8Ϊ		MM70					70������⴬
																																						9Ϊ		ME(J)70				70���⴬
																																						10Ϊ	MMH 70				70�ֲ������⴬
																																						
																																						11Ϊ	MC120					120���ബ
																																						12Ϊ	ME120					120���⴬
																																						
																																						13Ϊ	ME300					300���⴬���캽��
																																						
																																						14Ϊ	LE2000				2000���ͧ
																																						15Ϊ	LE3000				3000���ͧ
																																						16Ϊ	LP3000				3000Ѳ��ͧ
																																						17Ϊ	LC4000				4000����ͧ
																																						18Ϊ	LG4000				4000����ͧ
																																						
������			����										����											ȡֵ��Χ
3						Rudder_Mid-point				�����λ									147 ~ 165
4						debug_Mode							����ģʽ									0Ϊoff��1Ϊon
5						sound_modify						��������									0Ϊoff��1 ~ 10Ϊ�������Σ�1Ϊ�����
6						language								��������									0ΪӢ�1Ϊ����
------------------------------------------------------------------------------------------------*/
extern uint32_t local_para_conf_buf[LOCAL_PARAMETER_QUANTITY];							//�����޸Ļ���

extern uint32_t local_para_conf_min[LOCAL_PARAMETER_QUANTITY];					//������Сֵ
extern uint32_t local_para_conf_max[LOCAL_PARAMETER_QUANTITY];					//�������ֵ


extern uint32_t water_sample_conf[WATER_SAMPLE_PARAMETER];					//��ˮ��������
/*------------------------------------------------------------------------------------------------
������			����										����											ȡֵ��Χ
0						Bottle_Number						�趨����ƿ��							С�ʹ�Ϊ1 ~ 4
																															���ʹ�Ϊ1 ~ 8
                                                              ���ʹ�Ϊ1 ~ 20
1						Sample_Depth						�������									Ĭ��Ϊ5����0.5��
2						Sample_Mode							����ģʽ									ĿǰһֱΪ0
3						Sample_Volume						��������									1 ~ 40����λΪ�ٺ���
4						Wash_Mode								��ϴģʽ									0Ϊ����ϴ��1Ϊ��ϴ
------------------------------------------------------------------------------------------------*/
extern uint32_t water_sample_conf_buf[WATER_SAMPLE_PARAMETER];					//��ˮ�������û���

extern uint32_t water_sample_conf_min[WATER_SAMPLE_PARAMETER];					//��ˮ����������Сֵ
extern uint32_t water_sample_conf_max[WATER_SAMPLE_PARAMETER];					//��ˮ����������Сֵ


/* Exported function prototypes -----------------------------------------------*/
char * GetBoatTypeText(uint8_t boat_type);

#endif
