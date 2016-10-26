#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "navigation_box_led.h"
#include "navigation_box.h"
#include "system.h"

#if NAVIGATION_BOX
	
	
#define TEST_REPORT_IN_UART 1
#define NAVIGATION_BOX_CAN_ID 0x11
	
systick_time_t report_t;
systick_time_t gps_delay_t;
systick_time_t key_dect_t;
CompassTypeDef compass_data;
GPSTypeDef gps_data;
Uart_t Uart1 ;
Uart_t Uart2 ;
cmdcoder_t encoder;
volatile int key_count=0;
volatile char _navigation_need_recali = 0;


CanTxMsg TxMessage1;
char tempstr[10];
double GPSLat, GPSLatDec1, GPSLatDec2, GPSLatDec2Len; 
double GPSLon, GPSLonDec1, GPSLonDec2, GPSLonDec2Len; 
double GPSSpeed;
DtoBsTypeDef LatBytes;
DtoBsTypeDef LonBytes;
u16 Speed16;
u8 SpeedBytes[2];
bool already_send_time = FALSE;
DtoBsTypeDef DoubleToBytes(double data){ 
    unsigned char *pdata = (unsigned char*)&data; 
    DtoBsTypeDef byteArray;
	int ii;
    for (ii = 0; ii < 8; ++ii) 
        byteArray.bArray[ii] = *pdata++; 
    return byteArray; 
}
double BytesToDouble(DtoBsTypeDef data){ 
    double a = 0; 
    u8 ii; 
    void *pf; 
    unsigned char* px = data.bArray; 
    pf = &a; 
    for(ii=0;ii<8;ii++) 
    { 
        *((unsigned char *)pf+ii)=*(px+ii); 
    } 
    return a; 
}
void tempsubstr(char* barray, u8 start, u8 end){
	int ii;
	for(ii = 0; ii<10; ii++){
		tempstr[ii] = ' ';
	}
	for(ii = start; ii<end; ii++){
		tempstr[ii-start] = barray[ii];
	}	
}

void navi_report()
{
		int j,i;
	
		TxMessage1.StdId = 0x10;	//0x10������CAN��ַ
		TxMessage1.DLC = 8;
		TxMessage1.Data[0] = 0x11;
		if(gps_data.Location == 1)
		{
			// prepare data : GPS latitude
			// string to double

			  for(j = 0; j<20; j++)
				{
					if(gps_data.Latitude[j]==0)
					{
								break;
					}
			  } //j find null								   
			tempsubstr(gps_data.Latitude, 1, 3);
			GPSLat = atof(tempstr);
			tempsubstr(gps_data.Latitude, 3, j);
			GPSLat += atof(tempstr)/60;
		 	if(gps_data.Latitude[0]=='S')   //'S'
			{
				GPSLat = -GPSLat;
			}
			// double to byte array
			LatBytes = DoubleToBytes(GPSLat);

			// prepare data : GPS longitude
			// string to double
			for(j = 0; j<20; j++)
			{
				if(gps_data.Longitude[j]==0)
				{
					break;
				}
			} //j find null								   
			tempsubstr(gps_data.Longitude, 1, 4);
			GPSLon = atof(tempstr);
			tempsubstr(gps_data.Longitude, 4, j);
			GPSLon += atof(tempstr)/60;
		 	if(gps_data.Longitude[0]=='W') //'W'
			{
				GPSLon = -GPSLon;
			}
		 	// double to byte array
			LonBytes = DoubleToBytes(GPSLon);

			// prepare data : GPS speed
			// GPS speed
			Speed16 = (u16)((int)(atof(gps_data.Speed)*10*0.514));		// m/s
			SpeedBytes[1] = Speed16&0xFF;		    //LSB
			SpeedBytes[0] = (Speed16>>8)&0xFF;	//MSB

		
			// Frame 1: GPS Latitude ��γ�ȣ���ǰ6��byte
			TxMessage1.Data[1] = 0x00;
			for(i = 0; i<6; i++)
			{
				TxMessage1.Data[i+2] = LatBytes.bArray[i];
			}
			//CAN_TX (&TxMessage1);
			Can1_Send(TxMessage1.StdId , TxMessage1.Data, TxMessage1.DLC);
			#if TEST_REPORT_IN_UART
			cmdcoder_send_bytes(&encoder, TxMessage1.Data , TxMessage1.DLC);
			#endif
			// Frame 2: GPS Latitude ��γ�ȣ� �ĺ�2��byte, Longitude �����ȣ� ��ǰ4��byte
			TxMessage1.Data[1] = 0x01;
			for(i = 0; i<2; i++)
			{
				TxMessage1.Data[i+2] = LatBytes.bArray[i+6];
			}
			for(i = 0; i<4; i++)
			{
				TxMessage1.Data[i+4] = LonBytes.bArray[i];
			}
			Can1_Send(TxMessage1.StdId , TxMessage1.Data, TxMessage1.DLC);		
			#if TEST_REPORT_IN_UART
			cmdcoder_send_bytes(&encoder, TxMessage1.Data , TxMessage1.DLC);
			#endif
		
			// Frame 3: GPS Longitude �����ȣ� �ĺ�4��byte, GPS speed 2 bytes
			TxMessage1.Data[1] = 0x02;
			for(i = 0; i<4; i++)
			{
				TxMessage1.Data[i+2] = LonBytes.bArray[i+4];
			}
			TxMessage1.Data[6] = SpeedBytes[0];
			TxMessage1.Data[7] = SpeedBytes[1];
			Can1_Send(TxMessage1.StdId , TxMessage1.Data, TxMessage1.DLC);
			#if TEST_REPORT_IN_UART
			cmdcoder_send_bytes(&encoder, TxMessage1.Data , TxMessage1.DLC);
			#endif
			/*** end transmission of data : GPS longitude***/				
			// Frame 5: UTCʱ��,���� from GPS
			if (!already_send_time)//���û����ʱ�䣬�ͷ���ʱ�䣬ÿ��GPS�ҵ��źţ�ֻ����һ��ʱ�䡣�����;ARM9��������
			//����Ǳ�����⡣
			{
				TxMessage1.Data[1] = 0x04;
				TxMessage1.Data[2]=(((u8)gps_data.Time[0])-0x30)*10 + ((u8)gps_data.Time[1])-0x30;
				TxMessage1.Data[3]=(((u8)gps_data.Time[2])-0x30)*10 + ((u8)gps_data.Time[3])-0x30;
				TxMessage1.Data[4]=(((u8)gps_data.Time[4])-0x30)*10 + ((u8)gps_data.Time[5])-0x30;	
				TxMessage1.Data[5]=(((u8)gps_data.Date[4])-0x30)*10 + ((u8)gps_data.Date[5])-0x30;	
				TxMessage1.Data[6]=(((u8)gps_data.Date[2])-0x30)*10 + ((u8)gps_data.Date[3])-0x30;
				TxMessage1.Data[7]=(((u8)gps_data.Date[0])-0x30)*10 + ((u8)gps_data.Date[1])-0x30;
				Can1_Send(TxMessage1.StdId , TxMessage1.Data, TxMessage1.DLC);
				#if TEST_REPORT_IN_UART
				cmdcoder_send_bytes(&encoder, TxMessage1.Data , TxMessage1.DLC);
				#endif
				//��־  �ѷ���
				already_send_time = TRUE;
			}		
		}
		// Frame 4: ���ĳ���
		TxMessage1.Data[1] = 0x03;
		for(i = 0; i<2; i++)
		{
			TxMessage1.Data[2+i] = compass_data.headingBytes[i];		
			TxMessage1.Data[4+i] = compass_data.pitchBytes[i];		
			TxMessage1.Data[6+i] = compass_data.rollBytes[i];		
		}
		Can1_Send(TxMessage1.StdId , TxMessage1.Data, TxMessage1.DLC);	
		#if TEST_REPORT_IN_UART
		cmdcoder_send_bytes(&encoder, TxMessage1.Data , TxMessage1.DLC);
		#endif	
	
}


int encodeCallback ( unsigned char c )
{
	Uart_PutChar(&Uart1,c);
	return 1;
}





void main_setup()
{
	SetupPllClock(HSE_CLOCK_6MHZ);
	Navi_GPIO_Configuration ();
	Can1_Configuration (NAVIGATION_BOX_CAN_ID);	//0x11CAN��ַ
	Uart_Configuration (&Uart1, USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	//Uart_Configuration (&Uart3, USART3, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	
	Iap_Configure(&Uart1);
	Nbl_Led_Configuration();
	HMC6343_Configuration();
	gps_config(&Uart2, &gps_data);
	
	cmdcoder_init(&encoder, NAVIGATION_BOX_CMDCODER_ID, encodeCallback);
	//time_t init 
	systick_time_start(&report_t,50);//REPORT_STATUS_MS);
	systick_time_start(&gps_delay_t,5);//REPORT_STATUS_MS);
	systick_time_start(&key_dect_t,40);
	
	//system error 
	//system_error = system_error_get();
}





// if use iap , you can setup this function for your deinit
void main_deinit()
{
	// TODO something that import for extern ic or machine befor reset
}



unsigned char _cmd_buffer[8];
unsigned char _index = 0;

void main_loop()
{
	CanRxMsg can_msg;

	
	if( check_systick_time(&gps_delay_t) ){
		gps_event();
	}
	if( check_systick_time(&report_t) ){
		if( HMC6343_Read(&compass_data) ){
			if( key_count == 0)
				Nbl_Led_toggle(COMPASS_LED_ID);
		}else{
			Nbl_Led_off(COMPASS_LED_ID);
			//TODO reoprt error , if can
		}
		navi_report();
	}
	
	
	//check cammand from can interface
	if( 0 < Can1_Get_CanRxMsg(&can_msg) )
	{
		if( can_msg.StdId == NAVIGATION_BOX_CAN_ID )
		{
			//send "cali" for calibration cmd
			if( can_msg.DLC==4 && can_msg.Data[0] == 'c' && can_msg.Data[1] == 'a' && can_msg.Data[2]=='l' && can_msg.Data[3]=='i')
			{
				_navigation_need_recali = 1;
			}
			
		}
	}		

#if TEST_REPORT_IN_UART
	//check cammand from uart interface
	if( 0 < Uart_GetChar(&Uart1, &_cmd_buffer[_index]) )
	{
		if( _cmd_buffer[_index] == '\n' )
		{
			//check 'cali\n' cmd
			if( _index >= 4 && _cmd_buffer[_index-4]=='c' && _cmd_buffer[_index-3]=='a' && _cmd_buffer[_index-2]=='l' && _cmd_buffer[_index-1]=='i')
				_navigation_need_recali = 1;
			
			_index=0;
			
		}else{
			_index++;
			if( _index >= 8 ) _index=0;
		}
		
	}
#endif
	
	if( check_systick_time(&key_dect_t) ){
		if(GPIO_ReadInputDataBit(KEY_GPIO_BANK ,KEY_GPIO_PIN) ==0){
					key_count++;
					Nbl_Led_toggle(COMPASS_LED_ID);
					if( key_count >= 100 ) // more than 4s
					{
						key_count = 0;
						_navigation_need_recali= 1;
					}
		}else{
			key_count = 0;
		}
	}
	
	
	if( _navigation_need_recali == 1 )
	{
		Nbl_Led_on(COMPASS_LED_ID);
		if(!HMC6343_Calibrate() )
		{
			//TODO 
		}
		Nbl_Led_toggle(COMPASS_LED_ID);
									
		_navigation_need_recali = 0;
	}
	

}





#endif 






//End of file
