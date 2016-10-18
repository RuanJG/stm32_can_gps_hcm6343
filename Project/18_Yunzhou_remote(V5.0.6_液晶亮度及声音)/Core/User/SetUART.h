/*-------------------------------------------------------------------------
工程名称：遥控器平台串口驱动程序文档
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150526 赵铭章    5.0.0		新建立
																		USART1 作为915模块通信口
																		USART2 作为调试口
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SETUART_H
#define __SETUART_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include "global_includes.h"
#include "Xtend_900.h"


/* Private define ------------------------------------------------------------*/
#define USART1_DR_Address    (&(USART1->DR)) 
#define USART1_RX_BUFFER_SIZE			450			//以字节为单位
#define USART1_TX_BUFFER_SIZE			450			//以半节为单位
#define GOOD_REF									0x5A		//用作统计接收正确字节的参考发送字节


/* Public variables ---------------------------------------------------------*/
extern uint8_t USART1_RX_Buffer[USART1_RX_BUFFER_SIZE];						//USART1 数据接收缓冲区
extern uint16_t receive_pointer;																	//接收缓冲区指针
extern uint32_t receive_counter;																	//接收字节总数
extern uint32_t receive_good_count;																//于测试模式下统计接收正确的字节数

extern uint8_t USART1_TX_Buffer[USART1_TX_BUFFER_SIZE];						//USART1 数据发送缓冲区
extern uint16_t transmit_pointer;																	//发送缓冲区指针

extern int rfcommandReceivedBufCount;							//接收数据缓冲区
extern u8 rfcommandReceivedBuf[100];							//32		


/* Exported function prototypes -----------------------------------------------*/
void mCOMInit(void);
void transmit_data(uint8_t *send_buf, uint16_t send_len);
void PutUART(char ch);


#endif
