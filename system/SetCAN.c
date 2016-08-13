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
u8 Can1_Configuration_mask(u8 FilterNumber, u16 ID, u16 ID_Mask)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	u8 Init_state;
	
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
	CAN_FilterInitStructure.CAN_FilterIdHigh=(ID << 5);
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=(ID_Mask << 5);
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	// irq config 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;							 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;							 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;							 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	CAN_ITConfig (CAN1, CAN_IT_FMP0, ENABLE);	
	CAN_ITConfig (CAN1, CAN_IT_FMP1, ENABLE);
	CAN_ITConfig (CAN1, CAN_IT_TME, DISABLE);
	CAN1->TSR |= CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
	CAN_ITConfig (CAN1, CAN_IT_TME, ENABLE);
  
	return Init_state;
}

u8 Can1_Configuration(u16 ID)
{
	return Can1_Configuration_mask(0, ID, 0x1ff);
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

void Can1_Send(uint8_t id, uint8_t *data, int len)
{
	CanTxMsg TxMsg;
	int lest_len,i;
	uint8_t * point;
	
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_STD;
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
		}else if( status == CANTXFAILED ){
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




/*
#include "sysdef.h"

#define MAX_MAIL_NUM  3
//CAN????:0=?? 1=????
#define CAN_DEBUG     0
//CAN?????:0=250kbps,1=500kbps,2=1Mbps
#define CAN1_BPS      0

unsigned char can1_addr = 0;

unsigned short  Can1_Tx_Count =0;
unsigned short  Can1_Rx_Count =0;

unsigned short  Can1_Send_Delay =0;

unsigned char   Can1_Send_Buf[10]={0xeb,0x90,0x01,0x55,0xAA};
unsigned char   Can1_Recv_Buf[10]={0};
extern int  angle_num ;
extern unsigned int  angle_data ;

static  u8 CAN_msg_num[MAX_MAIL_NUM];   // ??????

void CAN1_Config_init(void)
{
     CAN_InitTypeDef        CAN_InitStructure;
     CAN_FilterInitTypeDef  CAN_FilterInitStructure;


     CAN_DeInit(CAN1);
     CAN_StructInit(&CAN_InitStructure);
  
     // CAN cell init  //36MHz 500Kbps
     CAN_InitStructure.CAN_TTCM=DISABLE;//??????????
     CAN_InitStructure.CAN_ABOM=DISABLE;//???CAN_MCR????INRQ????1???0?,??????
                                        //?128?11???????,???????
     CAN_InitStructure.CAN_AWUM=DISABLE;//????????CAN_MCR????SLEEP?,?????
     CAN_InitStructure.CAN_NART=DISABLE;//CAN??????1?,?????????(??/???????)
     CAN_InitStructure.CAN_RFLM=DISABLE;//???????FIFO????,????FIFO??????,????????????????
     CAN_InitStructure.CAN_TXFP=DISABLE;//???FIFO?????????????
      
#if CAN_DEBUG
     CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;
#else
     CAN_InitStructure.CAN_Mode= CAN_Mode_Normal; 
#endif
     //?????

     if(CAN1_BPS ==  0)
     {
        CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;//????????1?????
        CAN_InitStructure.CAN_BS1=CAN_BS1_12tq;//???1?9?????
        CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;//???2?8?????
        CAN_InitStructure.CAN_Prescaler= 9;//36M/(1+12+3)/9= 250kbps
                                            //36M/(1+5+2)/9 = 500kbps
                                           //36M(1+2+1)/9 = 1M
     }
     else if(CAN1_BPS == 1)
     {
        CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;//????????1?????
        CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;//???1?9?????
        CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;//???2?8?????
        CAN_InitStructure.CAN_Prescaler= 9;//36M/(1+12+3)/9= 250kbps
        
     }
     else
     {
        CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;//????????1?????
        CAN_InitStructure.CAN_BS1=CAN_BS1_2tq;//???1?9?????
        CAN_InitStructure.CAN_BS2=CAN_BS2_1tq;//???2?8?????
        CAN_InitStructure.CAN_Prescaler= 9;//36M/(1+12+3)/9= 250kbps
         
     }
     CAN_Init(CAN1,&CAN_InitStructure); 
                                         
     // CAN filter init 
     CAN_FilterInitStructure.CAN_FilterNumber=0;  //???????????0
     CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;//????????????????????????
     CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;;//????????1?32???? 
     CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;//??????????(32?????????,16????????)
     CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;;//??????????(32?????????,16????????)
     CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//????????????????????(32?????????,16????????)
     CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;//????????????????????(32?????????,16????????)
     CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;;//?????????FIFO0 
     CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//?????
     CAN_FilterInit(&CAN_FilterInitStructure);

}

// 
//       BaudRate = 1 / NominalBitTime
//       NominalBitTime = 1tq + tBS1 + tBS2
//       tq = (BRP[9:0] + 1) x tPCLK
//       tPCLK = CAN's clock = APB1's clock
//   1Mbps ???,???????6tq???,BS1=5, BS2=2
//   500kbps ???,???????8tq???,BS1=7, BS2=3
//   250kbps ???,???????14tq???,BS1=13, BS2=2
//   125k, 100k, 50k, 20k, 10k ??????? 250K ??
// 

void  CAN1_Com_init(void)
{
      GPIO_InitTypeDef GPIO_InitStruct;
      NVIC_InitTypeDef NVIC_InitStructure;
      
      // Enable CAN RX0 interrupt IRQ channel 
      NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
       
      NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
       
      NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);
       
       
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
      //Can Rx
      GPIO_InitStruct.GPIO_Pin  =  GPIO_Pin_11;
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStruct.GPIO_Mode =  GPIO_Mode_IPU;
      GPIO_Init(GPIOA,&GPIO_InitStruct);
      
      //Can Tx
      GPIO_InitStruct.GPIO_Pin  =  GPIO_Pin_12;
      GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStruct.GPIO_Mode =  GPIO_Mode_AF_PP;
      GPIO_Init(GPIOA,&GPIO_InitStruct);
       
      CAN1_Config_init();
       
      CAN_ITConfig(CAN1,CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE);  // fifo0??
      CAN_ITConfig(CAN1,CAN_IT_FMP1 | CAN_IT_FF1 | CAN_IT_FOV1, ENABLE);  // fifo1??
      CAN_ITConfig(CAN1,CAN_IT_TME, DISABLE);                // ????
      CAN_ITConfig(CAN1,CAN_IT_EWG | CAN_IT_EPV | CAN_IT_BOF | CAN_IT_LEC | CAN_IT_ERR | CAN_IT_WKU | CAN_IT_SLK, ENABLE);  // ERR??

      // CAN?????
      memset(CAN_msg_num,0,MAX_MAIL_NUM);   
}


int CAN1_Tx_msg(CanTxMsg TxMessage)
{
    u8 TransmitMailbox = 0;
  
    TransmitMailbox = CAN_Transmit(CAN1,&TxMessage);
    if(CAN_NO_MB == TransmitMailbox)
    {
        //????
        return 0;
    }
    else
    {         
        CAN_msg_num[TransmitMailbox] = 1;
       
    }
    CAN_ITConfig(CAN1,CAN_IT_TME, ENABLE);
     
    return 1;
}


u16 angle=0,angle_h=0,angle_l=0;
extern unsigned char angle_dir;
int CAN1_Tx_data(void)
{
    CanTxMsg TxMessage;
    u8 TransmitMailbox = 0,i=0;
    
    // transmit 
    TxMessage.StdId=0x6f1;//???????
    TxMessage.ExtId=0x1234;//???????
    TxMessage.RTR=CAN_RTR_DATA;//???????????
    TxMessage.IDE=CAN_ID_STD;//??????????
    TxMessage.DLC=6; //????
     
    angle=angle_data/10;
    if(angle>9999) angle=9999;
    angle_h=angle/100;
    angle_h=angle_h/10*16+angle_h%10;
    angle_l=angle%100;
    angle_l=angle_l/10*16+angle_l%10;
    Can1_Send_Buf[0]=angle_num;  //??
    Can1_Send_Buf[1]=angle_h; //???
    Can1_Send_Buf[2]=angle_l;
    Can1_Send_Buf[3]=0;
    Can1_Send_Buf[4]=0;
    Can1_Send_Buf[5]=angle_dir;
     
    for(i=0;i < TxMessage.DLC;i++)
    {
       TxMessage.Data[i] = Can1_Send_Buf[i];
    }
     
    TransmitMailbox = CAN_Transmit(CAN1,&TxMessage);
    if(CAN_NO_MB == TransmitMailbox)
    {
        //????,?????
        return 0;
    }
    else
    {         
        CAN_msg_num[TransmitMailbox] = 1;   
    }
    CAN_ITConfig(CAN1,CAN_IT_TME, ENABLE);
     
    Can1_Tx_Count++;
    if(Can1_Tx_Count > 10000)
       Can1_Tx_Count =0;
     
    Can1_Send_Delay =200;
    return 1;
}

//????
void  CAN1_Rx_Data(CanRxMsg RxMessage)
{     
      u8  i =0;
      if((RxMessage.StdId==0x6f1) && (RxMessage.IDE==CAN_ID_STD) && ((RxMessage.Data[1]|RxMessage.Data[0]<<8)==0xEB90))
      {
           
          for(i=0;i < RxMessage.DLC;i++)
          {
              Can1_Recv_Buf[i] =  RxMessage.Data[i];
          }
         
          Can1_Rx_Count++;
          if(Can1_Rx_Count > 10000)
             Can1_Rx_Count =0;
      }
}
//???????
void CAN1_Send(void)
{
     if(CAN_msg_num[0])
     {
        if(CAN_GetITStatus(CAN1,CAN_IT_RQCP0))
        {
            CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP0);
            CAN_ITConfig(CAN1,CAN_IT_TME, DISABLE);
            CAN_msg_num[0] = 0;
        }
     }
     if(CAN_msg_num[1])
     {
        if(CAN_GetITStatus(CAN1,CAN_IT_RQCP1))
        {
            CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP1);
            CAN_ITConfig(CAN1,CAN_IT_TME, DISABLE);
            CAN_msg_num[1] = 0;
        }
     }  
    if(CAN_msg_num[2])
    {
        if(CAN_GetITStatus(CAN1,CAN_IT_RQCP2))
        {
            CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP2);
            CAN_ITConfig(CAN1,CAN_IT_TME, DISABLE);
            CAN_msg_num[2] = 0;
        }
    }
}
//??????
void CAN1_Recv(unsigned char num)
{
     CanRxMsg RxMessage;
     switch(num)
     {
     case 0:
          if(CAN_GetITStatus(CAN1,CAN_IT_FF0))
          {
             CAN_ClearITPendingBit(CAN1,CAN_IT_FF0);
          }
          else if(CAN_GetITStatus(CAN1,CAN_IT_FOV0))
          {
             CAN_ClearITPendingBit(CAN1,CAN_IT_FOV0);
          }
          else
          {
             CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);
             //????:
             CAN1_Rx_Data(RxMessage);
          } 
          break;
     case 1:
          if(CAN_GetITStatus(CAN1,CAN_IT_FF1))
          {
             CAN_ClearITPendingBit(CAN1,CAN_IT_FF1);
          }
          else if(CAN_GetITStatus(CAN1,CAN_IT_FOV1))
          {
             CAN_ClearITPendingBit(CAN1,CAN_IT_FOV1);
          }
          else
          {
             CAN_Receive(CAN1,CAN_FIFO1, &RxMessage);
            //????
             CAN1_Rx_Data(RxMessage);
          } 
          break;
     } 
}

void  CAN1_Main(unsigned char flg )
{
      if(flg)
      {
        if(Can1_Send_Delay == 0)
           CAN1_Tx_data();
      }
}


*/