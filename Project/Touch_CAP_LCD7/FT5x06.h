#ifndef __FT5x06_H_
#define __FT5x06_H_

#include "stm32f4xx.h"
#include "delay.h"
#include "CT_I2C.h"

#define CT_CMD_WR	   	0X70	//д�������� 0110 000 0
#define CT_CMD_RD   	0X71	//����������

#define CT_MAX_TOUCH    5		//���ݴ��������֧�ֵĵ���

//���ݴ�����������
typedef struct
{
	u8   (*init)(void);			//��ʼ��������������
	void (*scan)(void);			//ɨ�败���� 	 
	u16 x[CT_MAX_TOUCH];		//����X����
	u16 y[CT_MAX_TOUCH];		//����Y����
 	u8	ppr[CT_MAX_TOUCH];		//�������ѹ��	 
	u8 tpsta;				      	// ���µ�ĸ���

				   	    	  
}_m_ctp_dev;

extern _m_ctp_dev ctp_dev;

void TP_INT_Config(void);
void EXTI4_IRQHandler(void);
u8 FT5x06_WR_Reg(u8 reg,u8 *buf,u8 len);
void FT5x06_RD_Reg(u8 reg,u8 *buf,u8 len);
u8 FT5x06_Init(void);
void FT5x06_Scan(void);
void FT5x06_Test(void);
#endif /*__FT5x06_H_*/































