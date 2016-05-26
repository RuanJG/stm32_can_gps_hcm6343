/******************************************************************************
 * @file:    SetCAN.c
 * @purpose: functions related to CAN transmission
 * @version: V1.00
 * @date:    11. Jul 2011
 *----------------------------------------------------------------------------
 ******************************************************************************/
#include "stm32f10x.h"
#include "global.h"			 

// function declaration
u8 CAN_Configuration(u8 FilterNumber, u16 ID, u16 ID_Mask);
u8 CAN_TX(CanTxMsg* TxMessage);
void USB_LP_CAN1_RX0_IRQHandler (void);
__weak void CAN_Interrupt (void);
void PutUART2(char ch);
void PutUART3(char ch);


extern u8 can_data_updating;

//variable declaration

CanRxMsg RxMessage;
//extern bool RxEndFlag;
extern u8 rod_down;
extern GlobalVariableTypeDef GlobalVariable;
#define ALERT_LED_SET	GPIOA->BSRR = GPIO_Pin_5
// functio definition
u8 CAN_Configuration(u8 FilterNumber, u16 ID, u16 ID_Mask)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	u8 Init_state;

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
u8 CAN_TX(CanTxMsg* TxMessage)
{
	u8 i=0;
	u8 TransmitMailbox;
					
	TxMessage->RTR = CAN_RTR_DATA;
	TxMessage->IDE = CAN_ID_STD;
	TransmitMailbox=CAN_Transmit(CAN1, TxMessage);

	while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
	return CAN_TransmitStatus(CAN1, TransmitMailbox);
}

void USB_LP_CAN1_RX0_IRQHandler (void)
{
	CAN_Interrupt ();
	CAN_ClearITPendingBit (CAN1, CAN_IT_FMP0); 
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
	int tt = 0;
	register int i;
	CAN_Receive	(CAN1, 0, &RxMessage);
	
	//for(i=0;i<RxMessage.DLC; i++)
	//{
	//	PutUART2(RxMessage.Data[i]);
	//}
	
	if(RxMessage.Data[0] == 0x11) 
	{ 	
		switch(RxMessage.Data[1]){
			case 0x00:
				can_data_updating = 0;
				for(i=0; i<6; i++){
					GlobalVariable.GPS.Latitude[i] = RxMessage.Data[2+i];							
				}
				break;
			case 0x01:
				can_data_updating = 1;
				for(i=0; i<2; i++){
				GlobalVariable.GPS.Latitude[i+6]=RxMessage.Data[2+i];							
				}
				for(i=0; i<4; i++){
					GlobalVariable.GPS.Longitude[i]=RxMessage.Data[4+i];							
				}

				break;
			case 0x02:
				can_data_updating = 2;
				for(i=0; i<4; i++){
					GlobalVariable.GPS.Longitude[i+4]=RxMessage.Data[2+i];							
				}
				GlobalVariable.GPS.Speed[0]=RxMessage.Data[6];	
				GlobalVariable.GPS.Speed[1]=RxMessage.Data[7];	 
				can_data_updating = 0;
				break;
			case 0x03:
				can_data_updating = 3;
				GlobalVariable.Compass.heading[0]=RxMessage.Data[2];	
				GlobalVariable.Compass.heading[1]=RxMessage.Data[3];
				/***********************
				GlobalVariable.Compass.pitch[0]=RxMessage.Data[4];	
				GlobalVariable.Compass.pitch[1]=RxMessage.Data[5];
				GlobalVariable.Compass.roll[0]=RxMessage.Data[6];	
				GlobalVariable.Compass.roll[1]=RxMessage.Data[7];
			*********************/
				can_data_updating = 4;
			
			/*
				switch(usv_protocol_version)
				{
					case USV_PROTOCOL_VERSION_1:
						reportToARM9();
						break;
					case USV_PROTOCOL_VERSION_2:
						ReportToARM9Base64();
						break;
					default:
						break;
				}
			*/
				break;
			case 0x04:
				GlobalVariable.GPS.Time[0]=RxMessage.Data[2];	
				GlobalVariable.GPS.Time[1]=RxMessage.Data[3];
				GlobalVariable.GPS.Time[2]=RxMessage.Data[4];
				GlobalVariable.GPS.Date[0]=RxMessage.Data[5];	
				GlobalVariable.GPS.Date[1]=RxMessage.Data[6];
				GlobalVariable.GPS.Date[2]=RxMessage.Data[7];
				break;
			default:
				break;
		}// end of case
	}// end of data from 0x11
	
	// add by arpan 2015.3.24
	// ADD BEIDOU
	else if(RxMessage.Data[0] == 0x30)
	{
		//PutUART3('3');
		switch(RxMessage.Data[1]){
			case 0x00:
				//can_data_updating = 0;
				for(i=0; i<6; i++){
					GlobalVariable.BD.BD_Latitude[i] = RxMessage.Data[2+i];							
				}
				break;
			case 0x01:
				//can_data_updating = 1;
				for(i=0; i<2; i++){
					
				GlobalVariable.BD.BD_Latitude[i+6]=RxMessage.Data[2+i];							
				}
				for(i=0; i<4; i++){
					GlobalVariable.BD.BD_Longitude[i]=RxMessage.Data[4+i];							
				}

				break;
			case 0x02:
				//can_data_updating = 2;
				for(i=0; i<4; i++){
					GlobalVariable.BD.BD_Longitude[i+4]=RxMessage.Data[2+i];							
				}
				//GlobalVariable.GPS.Speed[0]=RxMessage.Data[6];	
				//GlobalVariable.GPS.Speed[1]=RxMessage.Data[7];	 
				//DEBUG
				for(tt = 0 ; tt <8 ;tt++)
				{
					//PutUART3(GlobalVariable.BD.BD_Latitude[tt]);
				}
				
				for(tt = 0 ; tt <8 ;tt++)
				{
					//PutUART3(GlobalVariable.BD.BD_Longitude[tt]);
				}
				
				//can_data_updating = 0;
				
				break;
				/********
			case 0x03:
				can_data_updating = 3;
				GlobalVariable.Compass.heading[0]=RxMessage.Data[2];	
				GlobalVariable.Compass.heading[1]=RxMessage.Data[3];
				GlobalVariable.Compass.pitch[0]=RxMessage.Data[4];	
				GlobalVariable.Compass.pitch[1]=RxMessage.Data[5];
				GlobalVariable.Compass.roll[0]=RxMessage.Data[6];	
				GlobalVariable.Compass.roll[1]=RxMessage.Data[7];
				can_data_updating = 4;
				********/
			

				break;
			default:
				break;
		}// end of case		
	}
	
	//add by arpan 2015.3.25
	//姿态信息
	else if(RxMessage.Data[0] == 0x31)
	{
		switch(RxMessage.Data[1]){
			case 0x0A:	//横摇
				for(i=0; i<2; i++){
					GlobalVariable.Compass.pitch[i] = RxMessage.Data[i+2];							
				}
				break;
				
			case 0x0B:	//纵摇
				for(i=0; i<2; i++){
					GlobalVariable.Compass.roll[i] = RxMessage.Data[i+2];							
				}
				break;
				
			default:
				break;
			}
	}
	
	else if(RxMessage.Data[0] == 0x15)  
	{
		i = RxMessage.Data[1]-PARA_BASE_INDEX;	   //更改之前是82
		//if (i >= 0 && i < 7)	  //原来是10
		if(i>=0 && i<PARA_LIST_LEN)
		{
			GlobalVariable.ParaList[i] = TRUE;
			GlobalVariable.ParaListData[i*4] = RxMessage.Data[2];
			GlobalVariable.ParaListData[i*4+1] = RxMessage.Data[3];
			GlobalVariable.ParaListData[i*4+2] = RxMessage.Data[4];
			GlobalVariable.ParaListData[i*4+3] = RxMessage.Data[5];
		}
		

		///////////////////////////
		/*if(RxMessage.Data[1]==64)
		{
			
			PutUART3(GlobalVariable.ParaListData[i*4]);
			PutUART3(GlobalVariable.ParaListData[i*4+1]);
			PutUART3(GlobalVariable.ParaListData[i*4+2]);
			PutUART3(GlobalVariable.ParaListData[i*4+3]);
		}*/
	}
	
	else if( RxMessage.Data[0] == 0x40 )  //分层抽样的深度反馈
	{
		for( i=0; i< 4; i++){
			GlobalVariable.SamplePipeLen[i]= RxMessage.Data[1+i]; // len is a int type , maybe -676943807mm ~ 676943807mm
		}
		//GlobalVariable.SamplePipeLen = RxMessage.Data[1] | (RxMessage.Data[2]<<8) | (RxMessage.Data[3]<<16) | (RxMessage.Data[4]<<24);
		//i= RxMessage.Data[1] | (RxMessage.Data[2]<<8) | (RxMessage.Data[3]<<16) | (RxMessage.Data[4]<<24);

	}
	
	
}

//End of File
