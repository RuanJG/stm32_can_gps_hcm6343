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
#define LOCAL_PARAMETER_QUANTITY 5				//ң�����������ò�����������ģ�飩


/* Exported variables ---------------------------------------------------------*/
extern uint32_t local_para_conf[LOCAL_PARAMETER_QUANTITY];					//�洢ң�����������ò�����������ģ�飩
/*------------------------------------------------------------------------------------------------
������			����										����											ȡֵ��Χ
0						LCD_Backlight						�������ȵȼ�							0 ~ 9					��ֵԽ����ĻԽ��
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
------------------------------------------------------------------------------------------------*/
extern uint32_t local_para_conf_buf[LOCAL_PARAMETER_QUANTITY];							//�����޸Ļ���


/* Const define ------------------------------------------------------------*/
extern uint32_t local_para_conf_min[LOCAL_PARAMETER_QUANTITY];					//������Сֵ
extern uint32_t local_para_conf_max[LOCAL_PARAMETER_QUANTITY];					//�������ֵ


#endif