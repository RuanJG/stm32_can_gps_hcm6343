/*-------------------------------------------------------------------------
�������ƣ�Xtend900 ����ģ�������������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150608 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __XTEND_900_H
#define __XTEND_900_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "keyboard_app.h"
#include "SetUART.h"
#include "UI_app.h"


/* Exported define ------------------------------------------------------------*/
#define PARAMETER_QUANTITY	8						//����ȡ8��������ATHP,ID,MY,DT,MK,PL,AP,TP


/* Exported variables ---------------------------------------------------------*/
extern uint32_t Xtend_900_para[PARAMETER_QUANTITY];
extern uint32_t Xtend_900_para_buf[PARAMETER_QUANTITY];					//�����޸Ļ���
/*------------------------------------------------------------------------------------------------
������			����										����											ȡֵ��Χ
0 					Hopping_Channel					����ģ���ŵ� (HP) 				0 ~ 9
1 					Modem_VID								����ģ��VID	(ID)					0x11 ~ 0x7fff
2 					Source_Address					���ص�ַ		(MY)					0 ~ 0xffff
3						Destination_Address			Ŀ�ĵ�ַ		(DT)					0 ~ 0xffff   0xffffΪ�㲥��ַ
4						Address_Mask						�����ַ		(MK)					0 ~ 0xffff
5						TX_Power_Level					���书��		(PL)					0 ~ 4    0Ϊ1mW��1Ϊ10mW��2Ϊ100mW��3Ϊ500mW��4Ϊ1W
6						API_Enable							APIģʽ			(AP)					0 ~ 2		0Ϊ��ֹAPI��1Ϊ����escaped��APIģʽ��2Ϊ��escaped��APIģʽ
7						Board_Temperature				����ģ�鵱ǰ�¶� (TP)			0 ~ 0x7f	        
------------------------------------------------------------------------------------------------*/
extern uint8_t Xtend_900_para_char[PARAMETER_QUANTITY][4];				//��������16���� ASCII����ʽ


/* Const define ------------------------------------------------------------*/
extern uint32_t Xtend_900_para_min[PARAMETER_QUANTITY];														//������Сֵ
extern uint32_t Xtend_900_para_max[PARAMETER_QUANTITY];				//�������ֵ


/* Exported function prototypes -----------------------------------------------*/
void Xtend_900_Task(void * pvParameters);
void enter_AT_Command(void);
void send_AT_inquiry(void);
void handle_parameter_message(void);
void RF_Parameter_Transform(void);
void save_RF_parameter(void);


#endif
