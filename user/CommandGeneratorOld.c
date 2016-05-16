#include "stm32f10x.h"
#include "global.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	


void PutUART(char ch);	  
void PutBytes (const char *buffer, int len);

/**
 * @brief  USART1 send  to Arm9 information (navigation GPS, Compass, Water temp, chip temp, Battery)
 *			It is called around 10 times a second, in the while loop of main function
 *
 * @param  None
 * @return None
 *
 */
void reportToARM9(void)
{
		u8 i;
		u8 buf;
		u8 length = 0;
		//Generate the header
		PutUART(0);
		PutUART('B');
		PutUART(1);
		PutUART(124);
		length+=4;

		//Latitude 01
		PutUART(1);	   
		for(i = 0; i<8; i++)
		{
			PutUART (GlobalVariable.GPS.Latitude[i]);
		}						
		length+=9;
	   	//Longitude 02
		PutUART(2);
		for(i = 0; i<8; i++)
		{
			PutUART (GlobalVariable.GPS.Longitude[i]);
		}
		length+=9;
	
	   	//Speed 03
		PutUART(3);	
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.GPS.Speed[i]);
		}	
		length+=3;

		//time 04
		PutUART(4);	
		for(i = 0; i<3; i++)
		{
			PutUART (GlobalVariable.GPS.Time[i]);
		}	
		length+=4;	   	
		
		//date 05
		PutUART(5);	
		for(i = 0; i<3; i++)
		{
			PutUART (GlobalVariable.GPS.Date[i]);
		}	
		length+=4;

		//Power Life 06
		PutUART(6);
		if (GlobalVariable.Voltage >= 1680){
			buf = 100;
		}else if (GlobalVariable.Voltage >= 1632){
			buf = 90;
		}else if (GlobalVariable.Voltage >= 1600){
			buf = 80;
		}else if (GlobalVariable.Voltage >= 1572){
			buf = 70;
		}else if (GlobalVariable.Voltage >= 1548){
			buf = 60;
		}else if (GlobalVariable.Voltage >= 1528){
			buf = 50;
		}else if (GlobalVariable.Voltage >= 1516){
			buf = 40;
		}else if (GlobalVariable.Voltage >= 1508){
			buf = 30;
		}else if (GlobalVariable.Voltage >= 1492){
		 	buf = 20;
		}else if (GlobalVariable.Voltage >= 1480){
			buf = 15;
		}else if (GlobalVariable.Voltage >= 1472){
			buf = 10;
		}else if (GlobalVariable.Voltage >= 1400){
			buf = 5;
		}else{
			buf = 0;
		}
		PutUART(buf);
		length+=2;
	
		//Compass pitch 10
		PutUART(10);
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.Compass.pitch[i]);
		}
		length+=3;	
	
		//Compass roll 11
		PutUART(11);
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.Compass.roll[i]);
		}	
		length+=3;
	
		//Compass yaw 12
		PutUART(12);
		for(i = 0; i<2; i++)
		{
			PutUART (GlobalVariable.Compass.heading[i]);
		}	
		length+=3;
	
		//Left US
		PutUART(17);
		PutUART (GlobalVariable.UltraSonic[0] / 256);
		PutUART (GlobalVariable.UltraSonic[0] % 256);
		length+=3;

		//Right US
		PutUART(18);
		PutUART (GlobalVariable.UltraSonic[1] / 256);
		PutUART (GlobalVariable.UltraSonic[1] % 256);	
		length+=3;

		// internal temperature 19
		PutUART(19);
		PutUART (GlobalVariable.tempInternalByte);
		length+=2;	

		// water leakage status
		PutUART(21);
		if (GlobalVariable.WaterLeakage >= 3900)
		{
			PutUART(33);
		}
		else
		{
			PutUART(65);
		}
		length+=2;

		// water temperature 81
		PutUART(81);
		for(i = 0; i<4; i++)
		{
			PutUART (GlobalVariable.tempWaterBytes[i]);
		}	
		length+=5;
	
		//for (i = 0; i<7; ++i)	   //原来是10
		for(i=0; i<PARA_LIST_LEN; i++)
		{
			if (GlobalVariable.ParaList[i])
			{
				PutUART(i + PARA_BASE_INDEX);  
				//这个顺序，2143，这样才能被C#成功解读。
				PutUART(GlobalVariable.ParaListData[i*4+1]);
				PutUART(GlobalVariable.ParaListData[i*4]);
				PutUART(GlobalVariable.ParaListData[i*4+3]);
				PutUART(GlobalVariable.ParaListData[i*4+2]);
				length+=5;
			}
		}
		PutUART(length);
		PutUART(13);
		PutUART(10);
}

