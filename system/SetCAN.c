/******************************************************************************
 * @file:    SetCAN.c
 * @purpose: functions related to CAN transmission
 * @version: V1.00
 * @date:    11. Jul 2011
 *----------------------------------------------------------------------------
 ******************************************************************************/
#include "stm32f10x.h"
#include "system.h"
#include "fifo_void.h"

void USB_LP_CAN1_RX0_IRQHandler (void);
__weak void CAN_Interrupt (void);

CanRxMsg RxMessage;
CanTxMsg TxMessage;

// resend , when send message failed , try resend CAN_RESEND_MAX_COUNT times, if still failed , send next message
// this resend need to turn off auto retranslation --> CAN_NART = ENABLE
volatile int resend_count = 0;
#define CAN_RESEND_MAX_COUNT 10

volatile char in_send_loop = 0;

// record the mailbox which last message send with
#define CAN_MB_NONE_VAILD 0xff
u8 TransmitMailbox = CAN_MB_NONE_VAILD;

//the buff for tx and rx message 
#define MSG_TX_COUNT 100 // max count is 0xfe
CanTxMsg TxMessageArray[MSG_TX_COUNT];
#define MSG_COUNT 100 // max count is 0xfe
CanRxMsg RxMessageArray[MSG_COUNT];

//the fifos for tx rx,  and the fcuntion for fifos
void can_rx_put_cb(int id, void* data){
	RxMessageArray[id] = *(CanRxMsg*)data;
}
void can_rx_get_cb(int id, void * dst){
	*(CanRxMsg*)dst = RxMessageArray[id];
}
void can_tx_put_cb(int id, void* data){
	TxMessageArray[id] = *(CanTxMsg*)data;
}
void can_tx_get_cb(int id, void * dst){
	*(CanTxMsg*)dst = TxMessageArray[id];
}
fifo_void_t can1_tx_msg_fifo;//={.enable = 0,.head=0,.tail=0,};
fifo_void_t can1_rx_msg_fifo;//={.enable = 0,.head=0,.tail=0,};







// functio definition
u8 Can1_Configuration_mask(u8 FilterNumber, u16 ID, uint32_t id_type,  u16 ID_Mask , uint8_t sjw ,uint8_t bs1, uint8_t bs2, uint8_t prescale )
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	u8 Init_state,id_offset;
	
	// software config
	fifo_void_init (&can1_rx_msg_fifo,MSG_COUNT, can_rx_put_cb, can_rx_get_cb);
	fifo_void_init (&can1_tx_msg_fifo,MSG_COUNT, can_tx_put_cb, can_tx_get_cb);
	resend_count = 0;
	in_send_loop = 0;
	TransmitMailbox = CAN_MB_NONE_VAILD;

	
	// clock config 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = ENABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = ENABLE; //DISABLE
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_LoopBack;//CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = sjw;
	CAN_InitStructure.CAN_BS1 = bs1;
	CAN_InitStructure.CAN_BS2 = bs2;
	CAN_InitStructure.CAN_Prescaler = prescale;//2

	Init_state = CAN_Init(CAN1, &CAN_InitStructure);
	if( Init_state  == 0 ){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, DISABLE);
		system_Error_Callback(ERROR_CAN1_INIT_TYPE, 1);
		return Init_state;
	}

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=FilterNumber;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	if( id_type == CAN_ID_EXT)
	{
		CAN_FilterInitStructure.CAN_FilterIdHigh=0;//(ID << 5);
		CAN_FilterInitStructure.CAN_FilterIdLow=ID<<3;//0x0000;
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0;//(ID_Mask << 5);
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=(ID_Mask << 3);//0x0000;
	}else{
		CAN_FilterInitStructure.CAN_FilterIdHigh=(ID << 5);
		CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(ID_Mask << 5);
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	}
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	// irq config 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = CUSTOM_CAN1_IRQ_PREPRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = CUSTOM_CAN1_IRQ_SUBPRIORITY;
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;							 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;							 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;							 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig (CAN1, CAN_IT_FMP0, ENABLE);	
	CAN_ITConfig (CAN1, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig (CAN1, CAN_IT_TME, DISABLE);
	CAN1->TSR |= CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
	CAN_ITConfig (CAN1, CAN_IT_TME, ENABLE);
  
	return Init_state;
}

u8 Can1_Configuration_withRate(u16 ID, uint32_t id_type, uint8_t sjw ,uint8_t bs1, uint8_t bs2, uint8_t prescale )
{
	/*
	#if 0 // 1M
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStructure.CAN_Prescaler = 4;//2
#else // 500k
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 9;//2
#endif
	*/
	return Can1_Configuration_mask(0,  ID, id_type, 0x1ff , sjw, bs1, bs2, prescale);
}
u8 Can1_Configuration(u16 ID )
{
	/*
	#if 0 // 1M
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStructure.CAN_Prescaler = 4;//2
#else // 500k
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 9;//2
#endif
	*/
	//defalut 1M
	return Can1_Configuration_mask(0, ID, CAN_ID_STD, 0x1ff , CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq, 4);
}


u8 Can1_Configuration_with_mask(u16 ID , u16 mask)
{
	/*
	#if 0 // 1M
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
	CAN_InitStructure.CAN_Prescaler = 4;//2
#else // 500k
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 9;//2
#endif
	*/
	//defalut 1M
	return Can1_Configuration_mask(0, ID, CAN_ID_STD, mask , CAN_SJW_1tq, CAN_BS1_3tq, CAN_BS2_5tq, 4);
}


void Can1_Send_Message(CanTxMsg* TxMessage)
{
	//TxMessage->RTR = CAN_RTR_DATA;
	//TxMessage->IDE = CAN_ID_STD;
	
	// close irq 
	CAN_ITConfig (CAN1, CAN_IT_TME, DISABLE);
	  
	if( in_send_loop == 0 ){
	// fifo_void is empty in irq , so send it direct
		in_send_loop = 1;
		CAN1->TSR |= CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
		TransmitMailbox=CAN_Transmit(CAN1, TxMessage);
	}else{
	// can is busy , fill to fifo_void
		#if 1 // wait old message  send in fifo_void 
		if( fifo_void_free(&can1_tx_msg_fifo) <= 0 ){
			system_Error_Callback(ERROR_CAN1_TX_FIFO_OVERFLOW_TYPE,1);
			CAN_ITConfig (CAN1, CAN_IT_TME, ENABLE);
			while( fifo_void_free(&can1_tx_msg_fifo) <= 0 );
			CAN_ITConfig (CAN1, CAN_IT_TME, DISABLE);
		}
		fifo_void_put( &can1_tx_msg_fifo, TxMessage);
		#else // recovery old message
		fifo_void_recovery_put( &can1_tx_msg_fifo, TxMessage);
		#endif
	}
	//open irq
	CAN_ITConfig (CAN1, CAN_IT_TME, ENABLE);
}

void Can1_Send_Ext(uint8_t id, uint8_t *data, int len, uint32_t id_type, uint32_t frame_type)
{
	CanTxMsg TxMsg;
	int lest_len,i;
	uint8_t * point;
	
	TxMsg.RTR =  frame_type;//CAN_RTR_DATA; //CAN_RTR_REMOTE
	TxMsg.IDE = id_type;  // CAN_ID_STD CAN_ID_EXT
	if( TxMsg.IDE == CAN_ID_EXT ){
		TxMsg.ExtId = id;
	}else{
		TxMsg.StdId = id;
	}
	
	lest_len = len;
	point = data;
	while( lest_len > 0 ){
		TxMsg.DLC = lest_len<8 ? lest_len:8;
		lest_len -= TxMsg.DLC;
		for( i = 0; i< TxMsg.DLC ; i++){
			TxMsg.Data[i] = *point;
			point++;
		}
		 Can1_Send_Message(&TxMsg);
	}
}
void Can1_Send(uint8_t id, uint8_t *data, int len)
{
	CanTxMsg TxMsg;
	int lest_len,i;
	uint8_t * point;
	
	TxMsg.RTR =  CAN_RTR_DATA;//CAN_RTR_DATA; //CAN_RTR_REMOTE
	TxMsg.IDE = CAN_ID_STD;  // CAN_ID_STD CAN_ID_EXT
	TxMsg.StdId = id;
	
	lest_len = len;
	point = data;
	while( lest_len > 0 ){
		TxMsg.DLC = lest_len<8 ? lest_len:8;
		lest_len -= TxMsg.DLC;
		for( i = 0; i< TxMsg.DLC ; i++){
			TxMsg.Data[i] = *point;
			point++;
		}
		 Can1_Send_Message(&TxMsg);
	}
}

int Can1_Get_CanRxMsg(CanRxMsg *msg)
{
	int res;
	CAN_ITConfig (CAN1, CAN_IT_FMP0, DISABLE);	
	CAN_ITConfig (CAN1, CAN_IT_FMP1, DISABLE);
	res = fifo_void_get(&can1_rx_msg_fifo, msg);
	CAN_ITConfig (CAN1, CAN_IT_FMP0, ENABLE);	
	CAN_ITConfig (CAN1, CAN_IT_FMP1, ENABLE);
	return res;
}





/**
 * CAN reciver interrupt 
 */

void USB_LP_CAN1_RX0_IRQHandler (void)
{
	do{
		if( 0 >= fifo_void_free( &can1_rx_msg_fifo ) ){
			system_Error_Callback(ERROR_CAN1_RX0_FIFO_OVERFLOW_TYPE,1);
		}
		CAN_Receive	(CAN1, CAN_FIFO0, &RxMessage);
		fifo_void_recovery_put(&can1_rx_msg_fifo,&RxMessage);
	}while(RESET != CAN_GetITStatus(CAN1, CAN_IT_FMP0));
	
}
void CAN1_RX1_IRQHandler()
{
	//error , no fix filte to fifo1
	system_Error_Callback(ERROR_CAN1_RX1_GET_DATA_TYPE,1);
	CAN_ITConfig (CAN1, CAN_IT_FMP1, DISABLE);
}


void USB_HP_CAN1_TX_IRQHandler (void)
{
	uint8_t status ;
	//volatile static int i=0,j=0,s=0;
	//s++;
	if( TransmitMailbox != CAN_MB_NONE_VAILD ){
		//check last translate status
		status = CAN_TransmitStatus(CAN1, TransmitMailbox);//TransmitMailbox);
		if( status == CANTXPENDING ){
			//TODO why this irq will happen, if no clean tsr , it will in here much time ( answer : can not close irq in this function)
			CAN1->TSR |= CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
			//i++;
			return;
		}else if( status == CANTXOK ){
			//j++;
			resend_count =0;
		}else if( status == CANTXFAILE  ){
			if( resend_count < CAN_RESEND_MAX_COUNT ){
				//resend
				CAN1->TSR |= CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
				TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
				resend_count++;
			}else{
				//ignore this failed
				system_Error_Callback(ERROR_CAN1_TX_RETRY_FAILED_TYPE,1);
				//send new packget
				resend_count=0;
			}
		}else{
			// should not to be here
			resend_count=0;
			system_Error_Callback(ERROR_CAN1_PROGRAM_NOFIX_TYPE,1);
		}
	}else{
		resend_count = 0;
	}
	
	//send new packget 
	if( resend_count == 0){
		CAN1->TSR |= CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
		if( 0 < fifo_void_get(&can1_tx_msg_fifo, &TxMessage) ){
			TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
		}else{
			//CAN_ITConfig (CAN1, CAN_IT_TME, DISABLE); //can not close irq here
			in_send_loop = 0;
		}
	}
}





//End of File


