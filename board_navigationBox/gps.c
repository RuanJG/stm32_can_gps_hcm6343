#include "navigation_box.h"
#include "navigation_box_led.h"

//TimeSource:GPS, MeasurementPeriod:1000ms, MeasurementFrquency:1Hz,NavigationRate:1cyc, NavigationFrequency:1Hz
char SetCfgRate1hz[14] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xE8, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x39};
//TimeSource:GPS, MeasurementPeriod:500ms, MeasurementFrquency:2Hz,NavigationRate:1cyc, NavigationFrequency:2Hz
char SetCfgRate2hz[14] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xF4, 0x01, 0x01, 0x00, 0x01, 0x00, 0x0B, 0x77};
//TimeSource:GPS, MeasurementPeriod:200ms, MeasurementFrquency:5Hz,NavigationRate:1cyc, NavigationFrequency:5Hz
char SetCfgRate5hz[14] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A};
char PollCfgRate[7] = {0xB5, 0x62, 0x06, 0x08, 0x00, 0x0E, 0x22};

char CfgPort[] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0xB5};

char Close_GGA[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01, 0x00, 0x24};
char Close_GLL[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01, 0x01, 0x2b};
char Close_GSA[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01, 0x02, 0x32};
char Close_GSV[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01, 0x03, 0x39};
char Close_VTG[16] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x01, 0x05, 0x47};
char Open_RMC[16] =  {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x09, 0x54};

char CfgCfgSave[21] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1D, 0xAB };
	
unsigned char GPS_Command_PTR=0;
unsigned char GPS_Command_Buffer[82];

	

Uart_t * gps_uart;

void send_commad(char *buffer, int length)
{
	char i;
	for(i = 0; i < length; i ++)
	{
		Uart_PutChar(gps_uart,buffer[i]);
		delay_us(20000);
  }
}
void gps_config(Uart_t* uart)
{
	gps_uart = uart;
	//send_commad(PollCfgRate, sizeof(PollCfgRate));
	//send_commad(PollCfgRate, 7);
	//send_commad(SetCfgRate5hz, sizeof(SetCfgRate5hz));
	//send_commad(SetCfgRate5hz, 14);
	 
	//设置为ubox和NMEA协议
	send_commad(CfgPort, sizeof(CfgPort));
	//打开RMC,关闭其它
  send_commad(Open_RMC, sizeof(Open_RMC));//send_commad(Open_RMC, 16);
  send_commad(Close_GLL, sizeof(Close_GLL));//send_commad(Close_GLL, 16);
  send_commad(Close_GSA, sizeof(Close_GSA));//send_commad(Close_GSA, 16);
  send_commad(Close_GSV, sizeof(Close_GSV));//send_commad(Close_GSV, 16);
  send_commad(Close_VTG, sizeof(Close_VTG));//send_commad(Close_VTG, 16);
  send_commad(Close_GGA, sizeof(Close_GGA));//send_commad(Close_GGA, 16);
	//设置为5Hz
	send_commad(SetCfgRate5hz, sizeof(SetCfgRate5hz));
	//设置为1Hz
	//send_commad(SetCfgRate1hz, sizeof(SetCfgRate1hz));
	
	//保存
	send_commad(CfgCfgSave, sizeof(CfgCfgSave));
	 
	//GPS_Configured = 1;
}




bool GPS_Analyze (void)
{
	if(GPS_Command_PTR == 0xff)		//CommandValid
	{
		u8 ptr=0, i;
		while (GPS_Command_Buffer[ptr++] != ',');
		//Time
		for(i = 0; GPS_Command_Buffer[ptr] != ','; )
		{ 
			gps_data.Time[i] = GPS_Command_Buffer[ptr];
			ptr ++;
			i ++; 
		}
		gps_data.Time[i+1] = '\0';
		ptr ++;
		if(GPS_Command_Buffer[ptr] == 'A')
		{
			gps_data.Location = 1;//TRUE
		}
		else
		{
			gps_data.Location = 0;//FLASE
		}
		ptr += 2;
		//Latitude									  
		for(i = 1; GPS_Command_Buffer[ptr] != ','; )
		{ 
			gps_data.Latitude[i] = GPS_Command_Buffer[ptr];	
			ptr ++;
			i ++; 
		}								   
		ptr ++;								  
		gps_data.Latitude[0] = GPS_Command_Buffer[ptr];
		gps_data.Latitude[i] = '\0'; 				   
		ptr += 2;			
		//Longitude									  
		for(i = 1; GPS_Command_Buffer[ptr] != ','; )
		{ 
			gps_data.Longitude[i] = GPS_Command_Buffer[ptr];	
			ptr ++;
			i ++; 
		}								   
		ptr ++;								  
		gps_data.Longitude[0] = GPS_Command_Buffer[ptr];
		gps_data.Longitude[i] = '\0';	 				   
		ptr += 2;			
		//Speed									  
		for(i = 0; GPS_Command_Buffer[ptr] != ','; )
		{ 
			gps_data.Speed[i] = GPS_Command_Buffer[ptr];	
			ptr ++;
			i ++; 
		}		
		gps_data.Speed[i] = '\0';	 										   
		ptr ++;	
		//Course									  
		for(i = 0; GPS_Command_Buffer[ptr] != ','; )
		{ 
			gps_data.Course[i] = GPS_Command_Buffer[ptr];	
			ptr ++;
			i ++; 
		}	
		gps_data.Course[i] = '\0';	 							   
		ptr ++;	
		//Date									  
		for(i = 0; GPS_Command_Buffer[ptr] != ','; )
		{ 
			gps_data.Date[i] = GPS_Command_Buffer[ptr];	
			ptr ++;
			i ++; 
		}		
		gps_data.Date[i] = '\0';	 						   
		ptr ++;				
		GPS_Command_PTR = 0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


int gps_parse(unsigned char c)
{
		if(GPS_Command_PTR != 0xff)
		{
			if((c == '$') && (!GPS_Command_PTR))
			{
				GPS_Command_Buffer[GPS_Command_PTR] = c;
				GPS_Command_PTR ++;
				//GPS_Command_Buffer[GPS_Command_PTR] = 0;
			}
			else if(GPS_Command_PTR)
			{
				GPS_Command_Buffer[GPS_Command_PTR] = c;
				if(GPS_Command_PTR == 5)		//CommandFilter
				{
					if((GPS_Command_Buffer[3]== 'R')
						&& (GPS_Command_Buffer[4]== 'M')  
						&& (GPS_Command_Buffer[5]== 'C'))
					{
					}
					else
					{
						GPS_Command_PTR = 0;
						return 0;
					}
				}
				GPS_Command_PTR ++;
				GPS_Command_Buffer[GPS_Command_PTR] = 0; 
				if(c == '\n')
				{
					GPS_Command_Buffer[GPS_Command_PTR] = c;
					//CAN_SendText(GPS_Command_Buffer, GPS_Command_PTR);
					
					if (GPS_Command_PTR <= 60)
					{
						GPS_Command_PTR = 0;
						gps_data.Location = 0;
					}
					else 
					{
						GPS_Command_PTR = 0xff;	 
						//20140107改为GPS分析后发送
						if(GPS_Analyze () == TRUE)
						{
							Nbl_Led_toggle(GPS_LED_ID);
							return 1;
						}else{
							Nbl_Led_off(GPS_LED_ID);
						}
					}
				}
			}	 
		}	
	return 0;
}


void gps_event()
{
	unsigned char byte;
	int count=0;
	
	for( count=0; count< 256; count ++){
		if( Uart_GetChar(gps_uart, &byte) == 0 ){
			return;
		}
		gps_parse(byte);
	}
}
