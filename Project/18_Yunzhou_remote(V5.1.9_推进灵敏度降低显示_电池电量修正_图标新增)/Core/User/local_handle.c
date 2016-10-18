/*-------------------------------------------------------------------------
�������ƣ�ң�������ش�����س���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150612 ������    5.0.0		�½���
																		
					
					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "local_handle.h"


/* Exported variables ---------------------------------------------------------*/
uint32_t local_para_conf[LOCAL_PARAMETER_QUANTITY] = {1, 0, 0, 150, 0, 0, 1, 2};					//�洢ң�����������ò�����������ģ�飩
/*------------------------------------------------------------------------------------------------
������			����										����											ȡֵ��Χ
0						LCD_Backlight						�������ȵȼ�							0 ~ 2					��ֵԽ����ĻԽ��
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
7						Protocol								ͨ��Э��									1ΪV1��2ΪV2��3ΪV3
------------------------------------------------------------------------------------------------*/
uint32_t local_para_conf_buf[LOCAL_PARAMETER_QUANTITY];					//�����޸Ļ���

uint32_t local_para_conf_min[LOCAL_PARAMETER_QUANTITY] = {0, 0, 0, 147, 0, 0, 0, 2};						//������Сֵ
uint32_t local_para_conf_max[LOCAL_PARAMETER_QUANTITY] = {2, 1, 18, 165, 1, 1, 1, 2};						//�������ֵ


uint32_t water_sample_conf[WATER_SAMPLE_PARAMETER] = {1, 5, 0, 5, 1};					//��ˮ��������
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
uint32_t water_sample_conf_buf[WATER_SAMPLE_PARAMETER] = {1, 5, 0, 5, 1};					//��ˮ�������û���

uint32_t water_sample_conf_min[WATER_SAMPLE_PARAMETER] = {1, 5, 0, 1, 0};					//��ˮ����������Сֵ
uint32_t water_sample_conf_max[WATER_SAMPLE_PARAMETER] = {12, 5, 0, 20, 1};					//��ˮ����������Сֵ


/* Exported function prototypes -----------------------------------------------*/
/*-------------------------------------------------------------------------
	��������GetBoatTypeText
	��  �ܣ���ȡ�����ַ���
	��  ����boat_type 		���ͺ�
	����ֵ�������ַ���
-------------------------------------------------------------------------*/
char * GetBoatTypeText(uint8_t boat_type)
{
	switch(boat_type)
	{
		case USV_Boat_Type_SF20:
			return (char *)"Surf20";
		case USV_Boat_Type_SS20:
			return (char *)"SS20";
		case USV_Boat_Type_ESM20:
			return (char *)"ESM20";		
		
		case 	USV_Boat_Type_SS30:
			return (char *)"SS30";
		case USV_Boat_Type_ESM30:
			return (char *)"ESM30";		  //PL30
		case 	USV_Boat_Type_ES30:
			return (char *)"ES30";
		case 	USV_Boat_Type_ESMH30:
			return (char *)"ESMH30";

		case USV_Boat_Type_MC70:
			return (char *)"MC70";
		case USV_Boat_Type_MM70:
			return (char *)"MM70";				
		case USV_Boat_Type_MS70:
			return (char *)"MS70";
		case USV_Boat_Type_ME70:
			return (char *)"ME70";
		case USV_Boat_Type_MMH70:
			return (char *)"MMH70";
		
		case USV_Boat_Type_MC120:
			return (char *)"MC120";		
		case USV_Boat_Type_ME120:
			return (char *)"ME120";
		case USV_Boat_Type_MS120:
			return (char *)"MS120";		
		case USV_Boat_Type_MM120:
			return (char *)"MM120";	
		
		case USV_Boat_Type_ME300:
			return (char *)"ME300";	
		case USV_Boat_Type_LE2000:
			return (char *)"LE2000";
		case USV_Boat_Type_LE3000:
			return (char *)"LE3000";		
		
//		case USV_Boat_Type_Surf20:
//		case USV_Boat_Type_ES20:
//			return (char *)"ES20";

		default:
			return (char *)"USV";
	}
}


/*-------------------------------------------------------------------------
	��������Get_On_Off
	��  �ܣ���ȡ�����ַ���
	��  ����on_off 		���ز���
	����ֵ�������ַ���
-------------------------------------------------------------------------*/
char * Get_On_Off(uint8_t on_off)
{
	if(on_off)
	{
		return "ON";
	}
	
	else
	{
		return "OFF";
	}
}


