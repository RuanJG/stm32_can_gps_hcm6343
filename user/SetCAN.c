/******************************************************************************
 * @file:    SetCAN.c
 * @purpose: functions related to CAN transmission
 * @version: V1.00
 * @date:    11. Jul 2011
 *----------------------------------------------------------------------------
 ******************************************************************************/
#include "stm32f10x.h"
#include "global.h"
#include "fifo_void.h"

void USB_LP_CAN1_RX0_IRQHandler (void);
__weak void CAN_Interrupt (void);

CanRxMsg RxMessage;
Can_Error_CallBack_t can_err_cb;
int msg_rx_recoveryed = 0;

#define CAN_MB_NONE_VAILD 0xff
u8 TransmitMailbox = CAN_MB_NONE_VAILD;

#define MSG_TX_COUNT 100 // max count is 0xfe
CanTxMsg TxMessageArray[MSG_TX_COUNT];

#define MSG_COUNT 100 // max count is 0xfe
CanRxMsg RxMessageArray[MSG_COUNT];

void rx_put_cb(int id, void* data){
	RxMessageArray[id] = *(CanRxMsg*)data;
}
void rx_get_cb(int id, void * dst){
	*(CanRxMsg*)dst = RxMessageArray[id];
}
void tx_put_cb(int id, void* data){
	TxMessageArray[id] = *(CanTxMsg*)data;
}
void tx_get_cb(int id, void * dst){
	*(CanTxMsg*)dst = TxMessageArray[id];
}
fifo_void_t tx_msg_fifo;//={.enable = 0,.head=0,.tail=0,};
fifo_void_t rx_msg_fifo;//={.enable = 0,.head=0,.tail=0,};


// functio definition
u8 Can_Configuration(u8 FilterNumber, u16 ID, u16 ID_Mask,Can_Error_CallBack_t err_cb)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	u8 Init_state;
	
	fifo_void_init (&rx_msg_fifo,MSG_COUNT, rx_put_cb, rx_get_cb);
	fifo_void_init (&tx_msg_fifo,MSG_COUNT, tx_put_cb, tx_get_cb);
	
	can_err_cb = err_cb;

	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStructure.CAN_Prescaler = 4;//2
	Init_state = CAN_Init(CAN1, &CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=FilterNumber;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=(ID << 5);
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(ID_Mask << 5);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig (CAN1, CAN_IT_FMP0, ENABLE);	
	CAN_ITConfig (CAN1, CAN_IT_FMP1, ENABLE);
  
	return Init_state;
}
u8 Can_Send(CanTxMsg* TxMessage)
{
	u8 res = 0;
	
	TxMessage->RTR = CAN_RTR_DATA;
	TxMessage->IDE = CAN_ID_STD;
	if( 0 < fifo_void_free(&tx_msg_fifo) ){
		fifo_void_put( &tx_msg_fifo, TxMessage);
		res = 1;
	}else{
		can_err_cb(CAN_ERROR_MSG_TX_OVERFLOW ,1);
		res = 0;
	}
	return res;
}

void _can_send_tx_fifo_msg()
{
	CanTxMsg tmsg;
	
	if( 0 < fifo_void_avail(&tx_msg_fifo) ){
		fifo_void_get(&tx_msg_fifo, &tmsg);
		//tmsg.RTR = CAN_RTR_DATA;
		//tmsg.IDE = CAN_ID_STD;
		TransmitMailbox=CAN_Transmit(CAN1, &tmsg);
	}
}
void Can_event()
{
	uint8_t res;
	
	if( msg_rx_recoveryed > 0 ){ 
		can_err_cb(CAN_ERROR_MSG_RX_OVERFLOW,msg_rx_recoveryed);
		msg_rx_recoveryed = 0;
	}
	
	if( TransmitMailbox == CAN_MB_NONE_VAILD ){
		_can_send_tx_fifo_msg(); //no send data, so check and send 
	}else if( TransmitMailbox == CAN_NO_MB ){
		can_err_cb(CAN_ERROR_MSG_TX_FAILED ,2);
		//and do send next package again
		_can_send_tx_fifo_msg();
	}else if( CAN_TransmitStatus(CAN1, TransmitMailbox) == CANTXPENDING ){
		;// do nothing for this event
	}else if ( CAN_TransmitStatus(CAN1, TransmitMailbox) == CANTXOK ){
		//check next tx package and send
		_can_send_tx_fifo_msg();
	}else if ( CAN_TransmitStatus(CAN1, TransmitMailbox) == CANTXFAILED ){
		can_err_cb(CAN_ERROR_MSG_TX_FAILED ,1);
		//and do send next package again
		_can_send_tx_fifo_msg();
	}
}

/**
 * @brief  CAN receive message from Navigation board, decode and store information
 *
 * @param  None
 * @return None
 *
 */
void CAN_Interrupt (void)
{

	if( 0 >= fifo_void_free( &rx_msg_fifo ) ){
		msg_rx_recoveryed ++;
	}else{
		CAN_Receive	(CAN1, 0, &RxMessage);
		fifo_void_put(&rx_msg_fifo,&RxMessage);
	}
}
void USB_LP_CAN1_RX0_IRQHandler (void)
{
	CAN_Interrupt ();
	CAN_ClearITPendingBit (CAN1, CAN_IT_FMP0); 
} 

int Can_Get_CanRxMsg(CanRxMsg *msg)
{
	return fifo_void_get(&rx_msg_fifo, msg);
}

//End of File
