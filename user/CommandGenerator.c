#include "stm32f10x.h"
#include "global.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdio.h>	

#include "base64\base64.h"

void PutUART(char ch);	  
void PutBytes (const char *buffer, int len);

static u8 buffer[300];
static u8 buffer_encoded[400];
static u8 buffer_index;

void ReportToARM9Base64(void)
{
	u8 i;
	u8 voltage;
	u8 length = 0;
	u8 length_encoded;
	
	buffer_index = 0;
	
	//Generate the header
	buffer[buffer_index++]=0x0;//PutUART(0);
	buffer[buffer_index++]=0x42;//PutUART('B');
	buffer[buffer_index++]=0x01;//PutUART(1);
	buffer[buffer_index++]=0x7C;//PutUART(124);
	length+=4;

	//Latitude 01
	buffer[buffer_index++]=0x01;//PutUART(1);	   
	for(i = 0; i<8; i++)
	{
			buffer[buffer_index++]=GlobalVariable.GPS.Latitude[i];
	}						
	length+=9;
	
	//Longitude 02
	buffer[buffer_index++]=0x02;//PutUART(2);
	for(i = 0; i<8; i++)
	{
			buffer[buffer_index++]=GlobalVariable.GPS.Longitude[i];
	}
	length+=9;
	
 	//Speed 03
	buffer[buffer_index++]=0x03;//PutUART(3);	
	for(i = 0; i<2; i++)
	{
		buffer[buffer_index++]=GlobalVariable.GPS.Speed[i];
	}	
	length+=3;

	//time 04
	buffer[buffer_index++]=0x04;//PutUART(4);	
	for(i = 0; i<3; i++)
	{
			buffer[buffer_index++]=GlobalVariable.GPS.Time[i];
	}	
	length+=4;	   	
		
	//date 05
	buffer[buffer_index++]=0x05;//PutUART(5);	
	for(i = 0; i<3; i++)
	{
			buffer[buffer_index++]=GlobalVariable.GPS.Date[i];
	}	
	length+=4;

	//Power Life 06
	buffer[buffer_index++]=0x06;//PutUART(6);
	if (GlobalVariable.Voltage >= 1680){
			voltage = 100;
	}else if (GlobalVariable.Voltage >= 1632){
			voltage = 90;
	}else if (GlobalVariable.Voltage >= 1600){
		voltage = 80;
	}else if (GlobalVariable.Voltage >= 1572){
			voltage = 70;
	}else if (GlobalVariable.Voltage >= 1548){
			voltage = 60;
	}else if (GlobalVariable.Voltage >= 1528){
			voltage = 50;
	}else if (GlobalVariable.Voltage >= 1516){
			voltage = 40;
	}else if (GlobalVariable.Voltage >= 1508){
			voltage = 30;
	}else if (GlobalVariable.Voltage >= 1492){
		 	voltage = 20;
	}else if (GlobalVariable.Voltage >= 1480){
			voltage = 15;
	}else if (GlobalVariable.Voltage >= 1472){
			voltage = 10;
	}else if (GlobalVariable.Voltage >= 1400){
			voltage = 5;
	}else{
			voltage = 0;
	}
	buffer[buffer_index++]=voltage;
	length+=2;
	
	//Compass pitch 10
	buffer[buffer_index++]=0x0A;//PutUART(10);
	for(i = 0; i<2; i++)
	{
			buffer[buffer_index++]=GlobalVariable.Compass.pitch[i];
	}
	length+=3;	
	
	//Compass roll 11
	buffer[buffer_index++]=0x0B;//PutUART(11);
	for(i = 0; i<2; i++)
	{
			buffer[buffer_index++]=GlobalVariable.Compass.roll[i];
	}	
	length+=3;
	
	//Compass yaw 12
	buffer[buffer_index++]=0x0C;//PutUART(12);
	for(i = 0; i<2; i++)
	{
			buffer[buffer_index++]=GlobalVariable.Compass.heading[i];
	}	
	length+=3;
	
	//Left US
	buffer[buffer_index++]=0x11;//PutUART(17);
	buffer[buffer_index++]=GlobalVariable.UltraSonic[0] / 256;
	buffer[buffer_index++]=GlobalVariable.UltraSonic[0] % 256;
	length+=3;

	//Right US
	buffer[buffer_index++]=0x12;//PutUART(18);
	buffer[buffer_index++]=GlobalVariable.UltraSonic[1] / 256;
	buffer[buffer_index++]=GlobalVariable.UltraSonic[1] % 256;	
	length+=3;
	
	

	

	// internal temperature 19
	buffer[buffer_index++]=0x13;//PutUART(19);
	buffer[buffer_index++]=GlobalVariable.tempInternalByte;
	length+=2;	

	// water leakage status
	buffer[buffer_index++]=0x15;//PutUART(21);
	if (GlobalVariable.WaterLeakage >= 3900)
	{
		buffer[buffer_index++]=0x21;//PutUART(33);
	}
	else
	{
		buffer[buffer_index++]=0x41;//PutUART(65);
	}
	length+=2;

	// water temperature 81
	buffer[buffer_index++]=0x51;//PutUART(81);
	for(i = 0; i<4; i++)
	{
		buffer[buffer_index++]=GlobalVariable.tempWaterBytes[i];
	}	
	length+=5;
	
	#if 1 //add by ruan
	buffer[buffer_index++]=102;//pipe deep;
	for(i = 0; i<4; i++)
	{
		buffer[buffer_index++]=GlobalVariable.SamplePipeLen[i];
	}
	length+=5;
	
	if( GlobalVariable.SamplePipeAck != 0){
		buffer[buffer_index++]=103;//pipe cmd ack;
		buffer[buffer_index++]=GlobalVariable.SamplePipeAck;
		length+=2;
		GlobalVariable.SamplePipeAck = 0;
	}
	#endif
	
	//ADD BY ARPAN 2015.3.24
	//ADD BD_Latitude
	buffer[buffer_index++]=0x64;//PutUART(100);
	for(i = 0; i<8; i++)
	{
		buffer[buffer_index++]=GlobalVariable.BD.BD_Latitude[i];
	}	
	length+=9;
	
	buffer[buffer_index++]=0x65;//PutUART(101);
	for(i = 0; i<8; i++)
	{
		buffer[buffer_index++]=GlobalVariable.BD.BD_Longitude[i];
	}	
	length+=9;


	//for (i = 0; i<7; ++i)	   //原来是10
	for(i=0; i<PARA_LIST_LEN; i++)
	{
		if (GlobalVariable.ParaList[i])
		{
			buffer[buffer_index++]=i + PARA_BASE_INDEX;  
			/*//MODBUS中的浮点格式是低半字在前，高半字在后，高字节在前，低字节在后。
				All data values are returned in IEEE Floating Point Format (4 bytes). 
				Each data value is returned in two 16 bit words with the low word being transmitted first. 
				Within each word, the high byte is transmitted first. 
				Each byte is transmitted Most Significant Bit first.
				For example: The floating point value 1.56 = 0x3FC7AE14 would be transmitted as(1032):
				0xAE 0x14 0x3F 0xC7
			*/
			//这个顺序，1032->0123，这样才能被C#成功解读。
			buffer[buffer_index++]=GlobalVariable.ParaListData[i*4+1];
			buffer[buffer_index++]=GlobalVariable.ParaListData[i*4];
			buffer[buffer_index++]=GlobalVariable.ParaListData[i*4+3];
			buffer[buffer_index++]=GlobalVariable.ParaListData[i*4+2];
			length+=5;
		}
	}
	buffer[buffer_index++]=length;
	//PutUART(13);
	//PutUART(10);
	
	length_encoded = base64_encode((char* )buffer_encoded, (const char*) buffer, length);
	PutUART(0xFA);
	PutUART(0xFA);
	PutBytes((char *)buffer_encoded, length_encoded);
	PutUART(0xFE);
	PutUART(0xFE);
}

