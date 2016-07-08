#include <stm32f10x.h>
#include "system.h"													   

bool I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NoAck(void);
bool I2C_WaitAck(void);
void I2C_SendByte(u8 SendByte);
u8 I2C_ReceiveByte(void);

bool ADXL345_Configuration (s8 OffsetX, s8 OffsetY, s8 OffsetZ)
{
	//Read ID
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0xa6);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0x0);
	I2C_WaitAck(); 
	I2C_Stop(); 
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0xa7);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	//GlobalVariable.Acc.X = I2C_ReceiveByte();
	I2C_NoAck();
	I2C_Stop();
	//if(GlobalVariable.Acc.X != 0xe5)
	//{
	//	return FALSE;
	//}
	//Configuration
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0xa6);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0x1e);
	I2C_WaitAck(); 	  
	I2C_SendByte(OffsetX);
	I2C_WaitAck(); 
	I2C_SendByte(OffsetY);
	I2C_WaitAck(); 
	I2C_SendByte(OffsetZ);
	I2C_WaitAck(); 
	I2C_Stop();  
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0xa6);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0x2c);
	I2C_WaitAck(); 	  
	I2C_SendByte(0x1a);
	I2C_WaitAck(); 
	I2C_SendByte(0x8);
	I2C_WaitAck(); 
	I2C_Stop();    

	return TRUE; 
}

bool ADXL345_Read(void) 
{
	
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0xa6);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0x30);
	I2C_WaitAck(); 
	I2C_Stop(); 
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0xa7);
	if (!I2C_WaitAck()) 
	{ 
		I2C_Stop();  
		return FALSE; 
	}						
	if(I2C_ReceiveByte() & 0x80)	//DataReady
	{		   
#if 1
		if (!I2C_Start())
		{
			return FALSE; 
		}
		I2C_SendByte(0xa6);
		if (!I2C_WaitAck()) 
		{ 
			I2C_Stop();  
			return FALSE; 
		} 
		I2C_SendByte(0x32);
		I2C_WaitAck(); 
		I2C_Stop(); 
		if (!I2C_Start())
		{
			return FALSE; 
		}
		I2C_SendByte(0xa7);
		if (!I2C_WaitAck()) 
		{ 
			I2C_Stop();  
			return FALSE; 
		}	
#endif					
		/*
		GlobalVariable.Acc.X = I2C_ReceiveByte();							 
		I2C_Ack();		   
		GlobalVariable.Acc.X = (I2C_ReceiveByte()<<8)+ GlobalVariable.Acc.X;
		I2C_Ack();		   														
		GlobalVariable.Acc.Y = I2C_ReceiveByte();							 
		I2C_Ack();		   
		GlobalVariable.Acc.Y = (I2C_ReceiveByte()<<8)+ GlobalVariable.Acc.Y;
		I2C_Ack();		   
		GlobalVariable.Acc.Z = I2C_ReceiveByte();							 
		I2C_Ack();		   
		GlobalVariable.Acc.Z = (I2C_ReceiveByte()<<8)+ GlobalVariable.Acc.Z;
		//if(I2C_ReceiveByte() & 0x2)
		{
		}	
		I2C_NoAck();
		I2C_Stop();
		*/
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
