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
�������ƣ�HMC6343_Configuration()
��    �ܣ�����I2C���ߴ��䲢����HMC6343���ݸ���Ƶ��
˵    ����configurationֻҪ����һ�μ��ɣ���measurement rate��5Hz�ĳ�10Hz
*******************************************/
bool HMC6343_Configuration(void)
{
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x32);	 		 //����WRITEָ��
	if (!I2C_WaitAck()) 		 //�ȴ�ACK(),���û��ACK��˵��������󣬽�������ͨѶ
	{ 
		I2C_Stop();  
		return FALSE; 
	} 
	I2C_SendByte(0xF1);			 //WRITE to EEPROM command
	I2C_WaitAck();				 //�ȴ�ACK()
	I2C_SendByte(0x05);			 //EEPROM location 0x05 OP_Mode2
	I2C_WaitAck();				 //�ȴ�ACK()
	I2C_SendByte(0x02);			 //��mode�ĳ�0x02,measurement rate���10Hz
	I2C_WaitAck();				 //�ȴ�ACK()
	Delay_Nms(2); 				 //Delay 2ms
	I2C_Stop();					 //���óɹ���stop I2C
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
	I2C_SendByte(0x32);	 		 //����WRITEָ��
	if (!I2C_WaitAck()) 		 //�ȴ�ACK(),���û��ACK��˵��������󣬽�������ͨѶ
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
�������ƣ�HMC6343_Read()
��    �ܣ���ȡHMC6343������
˵    ������������6��byte��ɵģ�ÿ����byte��һ������
*******************************************/
bool HMC6343_Read(void)
{
	if (!I2C_Start())
	{
		return FALSE; 
	}
	I2C_SendByte(0x32);	 		  //����WRITEָ��
	if (!I2C_WaitAck()) 		  //�ȴ�ACK(),���û��ACK��˵��������󣬽�������ͨѶ
	{ 
		I2C_Stop();  
		return FALSE; 
	} 

	I2C_SendByte(0x50); 		  //����command, read heading data��6 bytes
	I2C_WaitAck();				  //�ȴ�ACK()
	if (!I2C_Start())			  //restart I2C,����ܹؼ���ÿ����д֮�䶼Ҫrestart
	{
		return FALSE; 
	}

	I2C_SendByte(0x33);			  //����Readָ��
	if (!I2C_WaitAck()) 		  //�ȴ�ACK(),���û��ACK��˵��������󣬽�������ͨѶ
	{ 
		I2C_Stop();  
		return FALSE; 
	}
			
	
 	heading_msb = I2C_ReceiveByte();	  //��ȡ��һ��byte
	I2C_Ack();							  //ACK Slave

 	heading_lsb = I2C_ReceiveByte();	  //��ȡ�ڶ���byte
	I2C_Ack();							  //ACK Slave

	pitch_msb = I2C_ReceiveByte();		  //��ȡ������byte
	I2C_Ack();							  //ACK Slave
						  
	pitch_lsb = I2C_ReceiveByte();		  //��ȡ���ĸ�byte
	I2C_Ack();							  //ACK Slave
  						   
	roll_msb = I2C_ReceiveByte();		  //��ȡ�����byte
	I2C_Ack();							  //ACK Slave
 						    
	roll_lsb = I2C_ReceiveByte();		  //��ȡ���һ��byte
	I2C_NoAck();						  //���������һ��byte����NoAck��������������
	I2C_Stop();							  //��������ͨ��

	compass_data.heading = (heading_msb<< 8) + heading_lsb;	  //Heading ���ݣ�0~3600��
	compass_data.pitch = (pitch_msb << 8) + pitch_lsb;		  //Pitch ���ݣ�-1800~+1800��compass facing water 
	compass_data.roll = (roll_msb << 8) + roll_lsb;			  //roll ���ݣ�-1800~+1800��

	compass_data.headingBytes[0] = heading_msb;
	compass_data.headingBytes[1] = heading_lsb;
	compass_data.pitchBytes[0] = pitch_msb;
	compass_data.pitchBytes[1] = pitch_lsb;
	compass_data.rollBytes[0] = roll_msb;
	compass_data.rollBytes[1] = roll_lsb;

	return TRUE;
			  
}

