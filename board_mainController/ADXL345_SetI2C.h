#ifndef _MAINBOX_ADXL345_SETI2C
#define _MAINBOX_ADXL345_SETI2C

#include <stm32f10x.h>	

//End of File
//#define I2C_Speed              100000 
#define I2C1_SLAVE_ADDRESS7    0xA0 
#define I2C_PageSize           256 

#define SCL_H         GPIOB->BSRR = GPIO_Pin_1 
#define SCL_L         GPIOB->BRR  = GPIO_Pin_1  
    
#define SDA_H         GPIOB->BSRR = GPIO_Pin_2 
#define SDA_L         GPIOB->BRR  = GPIO_Pin_2 

#define SCL_read      GPIOB->IDR  & GPIO_Pin_1 
#define SDA_read      GPIOB->IDR  & GPIO_Pin_2 


void I2C_delay(void) 
{ 
   u8 i=150; 
   while(i)  
   {  
     i--;  
   }  
} 

bool I2C_Start(void) 
{ 
	SDA_H; 
	SCL_H; 
	I2C_delay(); 
	if(!SDA_read)
	{
		return FALSE; //SDA线为低电平则总线忙,退出 
	}
	SDA_L; 
	I2C_delay(); 
	if(SDA_read) 
	{
		return FALSE; //SDA线为高电平则总线出错,退出 
	}
	SDA_L; 
	I2C_delay(); 
	return TRUE; 
} 

void I2C_Stop(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_L; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
} 

void I2C_Ack(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_L; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
} 

void I2C_NoAck(void) 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
} 

bool I2C_WaitAck(void)   //返回为:=1有ACK,=0无ACK 
{ 
	SCL_L; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	if(SDA_read) 
	{ 
		SCL_L; 
		return FALSE; 
	} 
	SCL_L; 
	return TRUE; 
} 

void I2C_SendByte(u8 SendByte) //数据从高位到低位// 
{ 
	u8 i=8; 
	while(i--) 
	{ 
		SCL_L; 
		I2C_delay(); 
		if(SendByte&0x80)
		{ 
			SDA_H;
		}   
		else  
		{
			SDA_L;
		}
		SendByte<<=1; 
		I2C_delay(); 
		SCL_H; 
		I2C_delay(); 
	} 
	SCL_L; 
} 

u8 I2C_ReceiveByte(void)  //数据从高位到低位// 
{  
	u8 i=8; 
	u8 ReceiveByte=0; 

	SDA_H; 
	while(i--) 
	{ 
		ReceiveByte<<=1;       
		SCL_L; 
		I2C_delay(); 
		SCL_H; 
		I2C_delay(); 
		if(SDA_read) 
		{ 
			ReceiveByte|=0x01; 
		} 
	} 
	SCL_L; 
	return ReceiveByte; 
} 



#endif
//End of File
