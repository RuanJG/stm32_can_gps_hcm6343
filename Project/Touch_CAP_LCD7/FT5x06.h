#ifndef __FT5x06_H_
#define __FT5x06_H_

#include "stm32f4xx.h"
#include "delay.h"
#include "CT_I2C.h"

#define CT_CMD_WR	   	0X70	//写数据命令 0110 000 0
#define CT_CMD_RD   	0X71	//读数据命令

#define CT_MAX_TOUCH    5		//电容触摸屏最大支持的点数

//电容触摸屏控制器
typedef struct
{
	u8   (*init)(void);			//初始化触摸屏控制器
	void (*scan)(void);			//扫描触摸屏 	 
	u16 x[CT_MAX_TOUCH];		//触摸X坐标
	u16 y[CT_MAX_TOUCH];		//触摸Y坐标
 	u8	ppr[CT_MAX_TOUCH];		//触摸点的压力	 
	u8 tpsta;				      	// 按下点的个数

				   	    	  
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































