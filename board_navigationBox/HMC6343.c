#include <stm32f10x.h>												   
#include "navigation_box.h"
#include "gpio_I2C.h"


static u16 heading_msb;
static u16 heading_lsb;
static u16 pitch_msb;
static u16 pitch_lsb;
static u16 roll_msb;
static u16 roll_lsb;

/******************************************
函数名称：HMC6343_Configuration()
功    能：启动I2C总线传输并设置HMC6343数据更新频率
说    明：configuration只要运行一次即可，将measurement rate由5Hz改成10Hz
*******************************************/
bool HMC6343_Configuration(void)
{
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x32);	 		 //发送WRITE指令
	if (!I2C_WaitAck()) 		 //等待ACK(),如果没有ACK，说明传输错误，结束本次通讯
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0xF1);			 //WRITE to EEPROM command
	I2C_WaitAck();				 //等待ACK()
	I2C_SendByte(0x05);			 //EEPROM location 0x05 OP_Mode2
	I2C_WaitAck();				 //等待ACK()
	I2C_SendByte(0x02);			 //将mode改成0x02,measurement rate变成10Hz
	I2C_WaitAck();				 //等待ACK()
	Delay_Nms(2); 				 //Delay 2ms
	I2C_Stop();					 //设置成功，stop I2C
	return TRUE;
}

bool HMC6343_Calibrate(void)
{
	int i;
#if 1
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x32);	 		 //发送WRITE指令
	if (!I2C_WaitAck()) 		 //等待ACK(),如果没有ACK，说明传输错误，结束本次通讯
	{ 
		I2C_Stop();  
		return FALSE; 
	}
	I2C_SendByte(0x71);			 //WRITE to EEPROM command
	I2C_WaitAck();
//	Delay_Ms(30000);
	for(i=0;i<1000; i++){//70s
		Delay_Nms(140); 
		i++;
	}
	I2C_SendByte(0x7E);			 //WRITE to EEPROM command
	I2C_WaitAck();
	I2C_Stop(); 
	
	return TRUE;
	
#else
		if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x32);
	I2C_WaitAck();
	
    /* enter cali */
	I2C_SendByte(0x70);
	//I2C_WaitAck();
	I2C_NoAck();						  
	I2C_Stop();
	
    Delay_Nms(10000);

	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x32);
	I2C_WaitAck();
	
    /* exit cali */
	I2C_SendByte(0x7F);
	I2C_NoAck();						  
	I2C_Stop();	

	return TRUE;
	
#endif
}
/******************************************
函数名称：HMC6343_Read()
功    能：读取HMC6343的数据
说    明：数据是由6个byte组成的，每两个byte是一组数据
*******************************************/
bool HMC6343_Read(void)
{
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x32);	 		  //发送WRITE指令
	if (!I2C_WaitAck()) 		  //等待ACK(),如果没有ACK，说明传输错误，结束本次通讯
	{ 
		I2C_Stop();  
		return FALSE; 
	} 

	I2C_SendByte(0x50); 		  //发送command, read heading data，6 bytes
	I2C_WaitAck();				  //等待ACK()
	if (!I2C_Start())			  //restart I2C,这个很关键，每个读写之间都要restart
	{
		return FALSE; 
	}

	I2C_SendByte(0x33);			  //发送Read指令
	if (!I2C_WaitAck()) 		  //等待ACK(),如果没有ACK，说明传输错误，结束本次通讯
	{ 
		I2C_Stop();  
		return FALSE; 
	}
			
	
 	heading_msb = I2C_ReceiveByte();	  //读取第一个byte
	I2C_Ack();							  //ACK Slave

 	heading_lsb = I2C_ReceiveByte();	  //读取第二个byte
	I2C_Ack();							  //ACK Slave

	pitch_msb = I2C_ReceiveByte();		  //读取第三个byte
	I2C_Ack();							  //ACK Slave
						  
	pitch_lsb = I2C_ReceiveByte();		  //读取第四个byte
	I2C_Ack();							  //ACK Slave
  						   
	roll_msb = I2C_ReceiveByte();		  //读取第五个byte
	I2C_Ack();							  //ACK Slave
 						    
	roll_lsb = I2C_ReceiveByte();		  //读取最后一个byte
	I2C_NoAck();						  //由于是最后一个byte，发NoAck，结束接收数据
	I2C_Stop();							  //结束本次通信

	compass_data.heading = (heading_msb<< 8) + heading_lsb;	  //Heading 数据（0~3600）
	compass_data.pitch = (pitch_msb << 8) + pitch_lsb;		  //Pitch 数据（-1800~+1800）compass facing water 
	compass_data.roll = (roll_msb << 8) + roll_lsb;			  //roll 数据（-1800~+1800）

	compass_data.headingBytes[0] = heading_msb;
	compass_data.headingBytes[1] = heading_lsb;
	compass_data.pitchBytes[0] = pitch_msb;
	compass_data.pitchBytes[1] = pitch_lsb;
	compass_data.rollBytes[0] = roll_msb;
	compass_data.rollBytes[1] = roll_lsb;

	return TRUE;
			  
}

