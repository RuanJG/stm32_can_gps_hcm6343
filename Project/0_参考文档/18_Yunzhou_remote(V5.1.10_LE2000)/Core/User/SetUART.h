/*-------------------------------------------------------------------------
�������ƣ�ң����ƽ̨�������������ĵ�
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150526 ������    5.0.0		�½���
																		USART1 ��Ϊ915ģ��ͨ�ſ�
																		USART2 ��Ϊ���Կ�
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
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
#define USART1_RX_BUFFER_SIZE			450			//���ֽ�Ϊ��λ
#define USART1_TX_BUFFER_SIZE			450			//�԰��Ϊ��λ
#define GOOD_REF									0x5A		//����ͳ�ƽ�����ȷ�ֽڵĲο������ֽ�


/* Public variables ---------------------------------------------------------*/
extern uint8_t USART1_RX_Buffer[USART1_RX_BUFFER_SIZE];						//USART1 ���ݽ��ջ�����
extern uint16_t receive_pointer;																	//���ջ�����ָ��
extern uint32_t receive_counter;																	//�����ֽ�����
extern uint32_t receive_good_count;																//�ڲ���ģʽ��ͳ�ƽ�����ȷ���ֽ���

extern uint8_t USART1_TX_Buffer[USART1_TX_BUFFER_SIZE];						//USART1 ���ݷ��ͻ�����
extern uint16_t transmit_pointer;																	//���ͻ�����ָ��

extern int rfcommandReceivedBufCount;							//�������ݻ�����
extern u8 rfcommandReceivedBuf[100];							//32		


/* Exported function prototypes -----------------------------------------------*/
void mCOMInit(void);
void transmit_data(uint8_t *send_buf, uint16_t send_len);
void PutUART(char ch);


#endif
