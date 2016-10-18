/*-------------------------------------------------------------------------
工程名称：遥控器本地处理相关程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150612 赵铭章    5.0.0		新建立
																		
					
					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOCAL_HANDLE_H
#define __LOCAL_HANDLE_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"


/* Exported define ------------------------------------------------------------*/
#define LOCAL_PARAMETER_QUANTITY 7				//遥控器本地配置参数（除数传模块）
#define WATER_SAMPLE_PARAMETER	5					//采水参数配置

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


#define MANUAL_CONTROL_MODE_MONO 	0				//单推模式
#define MANUAL_CONTROL_MODE_DUAL 	1				//双推模式


//采水相关参数
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
extern uint32_t local_para_conf[LOCAL_PARAMETER_QUANTITY];					//存储遥控器本地配置参数（除数传模块）
/*------------------------------------------------------------------------------------------------
索引号			名称										描述											取值范围
0						LCD_Backlight						背光亮度等级							0 ~ 15				数值越大屏幕越暗
1						Control_Mode						控制模式									0 ~ 1					0为单推模式，1为双推模式
2						USV_Series							船型系列									0 ~ 18				0为 	SF(J)20 			20测量船
																																						1为		SS(J)20				20采样船
																																						2为		SM(J)20				20监测船
																																						
																																						3为		SS(J)30				30采样船
																																						4为		ESM(J)30			30采样监测船
																																						5为		SSH(J)30			30分层采样船
																																						6为		ESMH(J)30			30分层采样监测船
																																						
																																						7为		MS70					70采样船
																																						8为		MM70					70采样监测船
																																						9为		ME(J)70				70勘测船
																																						10为	MMH 70				70分层采样监测船
																																						
																																						11为	MC120					120保洁船
																																						12为	ME120					120勘测船
																																						
																																						13为	ME300					300勘测船，领航者
																																						
																																						14为	LE2000				2000测绘艇
																																						15为	LE3000				3000测绘艇
																																						16为	LP3000				3000巡航艇
																																						17为	LC4000				4000破障艇
																																						18为	LG4000				4000安防艇
																																						
索引号			名称										描述											取值范围
3						Rudder_Mid-point				舵机中位									147 ~ 165
4						debug_Mode							调试模式									0为off，1为on
5						sound_modify						声音开关									0为off，1 ~ 10为声音档次，1为最大声
6						language								语言设置									0为英语，1为中文
------------------------------------------------------------------------------------------------*/
extern uint32_t local_para_conf_buf[LOCAL_PARAMETER_QUANTITY];							//参数修改缓存

extern uint32_t local_para_conf_min[LOCAL_PARAMETER_QUANTITY];					//定义最小值
extern uint32_t local_para_conf_max[LOCAL_PARAMETER_QUANTITY];					//定义最大值


extern uint32_t water_sample_conf[WATER_SAMPLE_PARAMETER];					//采水参数配置
/*------------------------------------------------------------------------------------------------
索引号			名称										描述											取值范围
0						Bottle_Number						设定采样瓶号							小型船为1 ~ 4
																															中型船为1 ~ 8
                                                              大型船为1 ~ 20
1						Sample_Depth						采样深度									默认为5，即0.5米
2						Sample_Mode							采样模式									目前一直为0
3						Sample_Volume						采样容量									1 ~ 40，单位为百毫升
4						Wash_Mode								清洗模式									0为不清洗，1为清洗
------------------------------------------------------------------------------------------------*/
extern uint32_t water_sample_conf_buf[WATER_SAMPLE_PARAMETER];					//采水参数配置缓存

extern uint32_t water_sample_conf_min[WATER_SAMPLE_PARAMETER];					//采水参数配置最小值
extern uint32_t water_sample_conf_max[WATER_SAMPLE_PARAMETER];					//采水参数配置最小值


/* Exported function prototypes -----------------------------------------------*/
char * GetBoatTypeText(uint8_t boat_type);

#endif
