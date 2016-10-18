/*-------------------------------------------------------------------------
工程名称：Xtend900 数传模块相关驱动程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150608 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
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
#define PARAMETER_QUANTITY	8						//共读取8个参数，ATHP,ID,MY,DT,MK,PL,AP,TP


/* Exported variables ---------------------------------------------------------*/
extern uint32_t Xtend_900_para[PARAMETER_QUANTITY];
extern uint32_t Xtend_900_para_buf[PARAMETER_QUANTITY];					//参数修改缓存
/*------------------------------------------------------------------------------------------------
索引号			名称										描述											取值范围
0 					Hopping_Channel					数传模块信道 (HP) 				0 ~ 9
1 					Modem_VID								数传模块VID	(ID)					0x11 ~ 0x7fff
2 					Source_Address					本地地址		(MY)					0 ~ 0xffff
3						Destination_Address			目的地址		(DT)					0 ~ 0xffff   0xffff为广播地址
4						Address_Mask						掩码地址		(MK)					0 ~ 0xffff
5						TX_Power_Level					发射功率		(PL)					0 ~ 4    0为1mW，1为10mW，2为100mW，3为500mW，4为1W
6						API_Enable							API模式			(AP)					0 ~ 2		0为禁止API，1为不带escaped的API模式，2为带escaped的API模式
7						Board_Temperature				数传模块当前温度 (TP)			0 ~ 0x7f	        
------------------------------------------------------------------------------------------------*/
extern uint8_t Xtend_900_para_char[PARAMETER_QUANTITY][4];				//参数缓存16进制 ASCII码形式


/* Const define ------------------------------------------------------------*/
extern uint32_t Xtend_900_para_min[PARAMETER_QUANTITY];														//定义最小值
extern uint32_t Xtend_900_para_max[PARAMETER_QUANTITY];				//定义最大值


/* Exported function prototypes -----------------------------------------------*/
void Xtend_900_Task(void * pvParameters);
void enter_AT_Command(void);
void send_AT_inquiry(void);
void handle_parameter_message(void);
void RF_Parameter_Transform(void);
void save_RF_parameter(void);


#endif
