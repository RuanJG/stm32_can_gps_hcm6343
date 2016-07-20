#ifndef _GPIO_I2C_H
#define _GPIO_I2C_H

#include <stm32f10x.h>	
/******************************************
函数名称：define
功    能：定义所用端口PIN1和PIN2
*******************************************/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_1 	 //将PIN1拉高
#define SCL_L         GPIOB->BRR  = GPIO_Pin_1   //将PIN1拉低
    
#define SDA_H         GPIOB->BSRR = GPIO_Pin_2 	 //将PIN2拉高
#define SDA_L         GPIOB->BRR  = GPIO_Pin_2 	 //将PIN2拉低

#define SCL_read      GPIOB->IDR  & GPIO_Pin_1 	 //读取PIN1数据
#define SDA_read      GPIOB->IDR  & GPIO_Pin_2 	 //读取PIN2数据

/******************************************
函数名称：I2C_Delay()
功    能：拉高拉低总线后的short delay
*******************************************/
static void I2C_Delay(void) 
{ 
   volatile u8 i=25; 
   while(i)  
   {  
     i--;  
   }  
} 
/******************************************
函数名称：I2C_Start()
功    能：启动I2C总线传输
说    明：A HIGH to LOW transition on the SDA line while SCL is
HIGH is one such unique case. This situation indicates a START condition.
*******************************************/
static bool I2C_Start(void) 
{ 
	SDA_H; 			  //release SDA线
	SCL_H; 			  //拉高SCL线
	I2C_Delay(); 
	if(!SDA_read)
	{
		return FALSE; //SDA线为低电平则总线忙,退出 
	}
	SDA_L; 			  //拉低SDA线，发出start signal
	I2C_Delay(); 
	if(SDA_read) 
	{
		return FALSE; //SDA线为高电平则总线出错,退出 
	}
	SCL_L; 			  //保持SCL线处于低电平
	I2C_Delay(); 
	return TRUE; 
} 
/******************************************
函数名称：I2C_Stop()
功    能：结束I2C总线传输
说    明：A LOW to HIGH transition on the SDA line while SCL is
HIGH defines a STOP condition
*******************************************/
static void I2C_Stop(void) 
{ 
	SCL_L; 			  
	I2C_Delay(); 
	SDA_L; 
	I2C_Delay(); 
	SCL_H; 			  //SCL线由低变高
	I2C_Delay(); 
	SDA_H; 			  //在SCL线高时，拉高SDA线，产生Stop信号
	I2C_Delay(); 
} 
/******************************************
函数名称：I2C_Ack()
功    能：通知SLAVE，I2C已收到数据
说    明：I2C协议中要求master每收到一个byte都要向slave发出ACK信号，
ACK信号就是将SDA线拉低
*******************************************/
static void I2C_Ack(void) 
{ 
	u16 i;
	SCL_L; 
	I2C_Delay(); 
	SDA_L; 			  //在SCL线低时，拉低SDA线，产生ACK信号
	I2C_Delay(); 
	SCL_H; 
	I2C_Delay(); 
	SCL_L; 			  //保持SCL线处于低电平
	I2C_Delay(); 
	for(i=0; i<200; i++)	//每次ACK后，都要给slave一定的反应时间，否则读数会出错
	{
		I2C_Delay();
	}
} 
/******************************************
函数名称：I2C_NoAck()
功    能：通知SLAVE，数据接收结束，不用再发数据啦
说    明：I2C协议中要求master接收到最后一个byte时向slave发出NOACK信号，
表示数据已经全部收到，不用再发数据。NOACK信号就是将SDA线拉高
*******************************************/
static void I2C_NoAck(void) 
{ 
	SCL_L; 
	I2C_Delay(); 
	SDA_H; 			   //在SCL线低时，拉高SDA线，产生NOACK信号
	I2C_Delay(); 
	SCL_H; 
	I2C_Delay(); 
	SCL_L; 			   //保持SCL线处于低电平
	I2C_Delay(); 
} 
/******************************************
函数名称：I2C_WaitAck()
功    能：等待SLAVE收到数据后的确认信号
说    明：I2C协议中要求SLAVE每收到一个byte的数据，都要通知MASTER.
信号就是发送给MASTER一个low的信号，表示收到数据
*******************************************/
static bool I2C_WaitAck(void)   
{ 
	u16 i;
	SCL_L; 
	I2C_Delay(); 
	SDA_H; 				//Master release SDA线，交由SLAVE掌控
	I2C_Delay(); 
	SCL_H; 
	I2C_Delay(); 
	if(SDA_read) 		//如果SDA线为HIGH,表示SLAVE没有ACK
	{ 
		SCL_L; 
		return FALSE; 
	} 
	SCL_L; 				//保持SCL线处于低电平
	for(i=0; i<200; i++) 	//每次收到ACK后，都要给Master一定的反应时间，否则发送数据会出错
	{
		I2C_Delay();
	}
	return TRUE; 
} 
/******************************************
函数名称：I2C_SendByte()
功    能：Master向Slave发送一个byte的数据
说    明：数据是从高位到低位bit by bit传送
*******************************************/
static void I2C_SendByte(u8 SendByte) 
{ 
	u8 i=8; 
	while(i--) 
	{ 
		SCL_L; 					 //拉低SCL线，可改变SDA数据线
		I2C_Delay(); 
		if(SendByte&0x80)		 //检测高位bit是不是HIGH
		{ 
			SDA_H;				 //将SDA线拉高，表示数据为HIGH
			I2C_Delay(); 
		}   
		else  
		{
			SDA_L;				 //将SDA线拉低，表示数据为LOW
			I2C_Delay(); 
		}
		SendByte<<=1; 			 //移动一个bit,准备传送下一个bit
		I2C_Delay(); 
		SCL_H; 					 //将SCL拉高，结束一个cycle
		I2C_Delay(); 
	} 
	SCL_L; 						 //保持SCL线处于低电平
} 
/******************************************
函数名称：I2C_ReceiveByte()
功    能：Master从Slave接收一个byte的数据
说    明：数据是从高位到低位bit by bit传送
*******************************************/
static u8 I2C_ReceiveByte(void)  
{  
	u8 i=8; 
	u8 ReceiveByte=0; 

	SDA_H; 						  //Master release SDA线，交由SLAVE主管
	while(i--) 
	{ 
		ReceiveByte<<=1;       	  //移动一个bit,准备接收下一个bit
		SCL_L; 					  //拉低SCL线，允许SLAVE改变SDA线
		I2C_Delay(); 
		SCL_H; 					  //将SCL拉高，结束一个cycle
		I2C_Delay(); 
		if(SDA_read) 			  //读取SDA线上数据
		{ 
			ReceiveByte|=0x01; 	  //数据为HIGH
		} 
	} 
	SCL_L; 						  //保持SCL线处于低电平
	return ReceiveByte; 
} 

#endif
//End of File
