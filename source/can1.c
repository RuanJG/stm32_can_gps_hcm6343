#include "stm32f10x_conf.h"
#include "main_config.h"





u8 Can1_Configuration_mask(u8 FilterNumber, u16 ID, uint32_t id_type,  u16 ID_Mask , uint8_t sjw ,uint8_t bs1, uint8_t bs2, uint8_t prescale )
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	u8 Init_state,id_offset;
	

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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	
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
	CAN_ITConfig (CAN1, CAN_IT_FMP1, DISABLE);
	CAN_ITConfig (CAN1, CAN_IT_TME, DISABLE);
	
	//CAN1->TSR |= CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
	//CAN_ITConfig (CAN1, CAN_IT_TME, ENABLE);
  
	return Init_state;
}



u8 Can1_Send_Message(CanTxMsg* TxMessage)
{
	volatile u8 i=0;
	u8 TransmitMailbox;
					
	TransmitMailbox=CAN_Transmit(CAN1, TxMessage);

	while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
	return CAN_TransmitStatus(CAN1, TransmitMailbox);
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


/**
 * @brief  CAN receive message from Navigation board, decode and store information
 *
 * @param  None
 * @return None
 *
 */
void CAN_Interrupt (void)
{
	CanRxMsg RxMessage;
	
	
	
	CAN_Receive	(CAN1, 0, &RxMessage);
	
}




void USB_LP_CAN1_RX0_IRQHandler (void)
{
	CAN_Interrupt ();
	CAN_ClearITPendingBit (CAN1, CAN_IT_FMP0); 
} 