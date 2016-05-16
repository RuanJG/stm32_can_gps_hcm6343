#include <stm32f10x.h>
#include "global.h"													   

bool I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
bool I2C_WaitAck(void);
void I2C_SendByte(u8 SendByte);
u8 I2C_ReceiveByte(void);

bool HMC5843_Configuration(void) 
{
	//Read ID
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x3C);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0x0A);
	I2C_WaitAck(); 
	I2C_Stop(); 
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x3D);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	GlobalVariable.TempCompass.X = I2C_ReceiveByte();
	I2C_Ack();		   
	GlobalVariable.TempCompass.Y = I2C_ReceiveByte();
	I2C_Ack();
	GlobalVariable.TempCompass.Z = I2C_ReceiveByte();
	I2C_NoAck();
	I2C_Stop();
	if((GlobalVariable.TempCompass.X != 0x48) 
		| (GlobalVariable.TempCompass.Y != 0x34)	 
		| (GlobalVariable.TempCompass.Z != 0x33))
	{
		return FALSE;
	}
	//Configuration
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x3C);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0x00);
	I2C_WaitAck(); 	  
	I2C_SendByte(0x18);		//DataOutputRate = 50Hz
	I2C_WaitAck(); 
	I2C_SendByte(0x20);		//Gain = 1300/milli-gauss
	I2C_WaitAck(); 
	I2C_SendByte(0x00);		//ContinuousMode
	I2C_WaitAck(); 
	I2C_Stop(); 

	return TRUE; 
} 


bool HMC5843_Read(void) 
{
	
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x3C);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0x09);
	I2C_WaitAck(); 
	I2C_Stop(); 
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x3D);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}						
	if(I2C_ReceiveByte() & 0x1)	//DataReady
	{		   
#if 1
		if (!I2C_Start())
		{
			return FALSE; 
		}
		I2C_SendByte(0x3C);
		if (!I2C_WaitAck()) 
		{ 
			I2C_Stop();  
			return FALSE; 
		} 
		I2C_SendByte(0x03);
		I2C_WaitAck(); 
		I2C_Stop(); 
		if (!I2C_Start())
		{
			return FALSE; 
		}
		I2C_SendByte(0x3D);
		if (!I2C_WaitAck()) 
		{ 
			I2C_Stop();  
			return FALSE; 
		}	
#endif					
		GlobalVariable.TempCompass.X = I2C_ReceiveByte();							 
		I2C_Ack();		   
		GlobalVariable.TempCompass.X = (GlobalVariable.TempCompass.X<<8)+ I2C_ReceiveByte();
		I2C_Ack();		   														
		GlobalVariable.TempCompass.Y = I2C_ReceiveByte();							 
		I2C_Ack();		   
		GlobalVariable.TempCompass.Y = (GlobalVariable.TempCompass.Y<<8)+ I2C_ReceiveByte();
		I2C_Ack();		   
		GlobalVariable.TempCompass.Z = I2C_ReceiveByte();							 
		I2C_Ack();		   
		GlobalVariable.TempCompass.Z = (GlobalVariable.TempCompass.Z<<8)+ I2C_ReceiveByte();

		GlobalVariable.Compass = GlobalVariable.TempCompass;
		//if(I2C_ReceiveByte() & 0x2)
		{
		}	
		I2C_NoAck();
		I2C_Stop();
		return TRUE;
	}
	else
	{
		I2C_NoAck();
		I2C_Stop();
		return FALSE;
	}	  
}
//End of File
