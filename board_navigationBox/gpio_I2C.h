#ifndef _GPIO_I2C_H
#define _GPIO_I2C_H

#include <stm32f10x.h>	
/******************************************
�������ƣ�define
��    �ܣ��������ö˿�PIN1��PIN2
*******************************************/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_1 	 //��PIN1����
#define SCL_L         GPIOB->BRR  = GPIO_Pin_1   //��PIN1����
    
#define SDA_H         GPIOB->BSRR = GPIO_Pin_2 	 //��PIN2����
#define SDA_L         GPIOB->BRR  = GPIO_Pin_2 	 //��PIN2����

#define SCL_read      GPIOB->IDR  & GPIO_Pin_1 	 //��ȡPIN1����
#define SDA_read      GPIOB->IDR  & GPIO_Pin_2 	 //��ȡPIN2����

/******************************************
�������ƣ�I2C_Delay()
��    �ܣ������������ߺ��short delay
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
�������ƣ�I2C_Start()
��    �ܣ�����I2C���ߴ���
˵    ����A HIGH to LOW transition on the SDA line while SCL is
HIGH is one such unique case. This situation indicates a START condition.
*******************************************/
static bool I2C_Start(void) 
{ 
	SDA_H; 			  //release SDA��
	SCL_H; 			  //����SCL��
	I2C_Delay(); 
	if(!SDA_read)
	{
		return FALSE; //SDA��Ϊ�͵�ƽ������æ,�˳� 
	}
	SDA_L; 			  //����SDA�ߣ�����start signal
	I2C_Delay(); 
	if(SDA_read) 
	{
		return FALSE; //SDA��Ϊ�ߵ�ƽ�����߳���,�˳� 
	}
	SCL_L; 			  //����SCL�ߴ��ڵ͵�ƽ
	I2C_Delay(); 
	return TRUE; 
} 
/******************************************
�������ƣ�I2C_Stop()
��    �ܣ�����I2C���ߴ���
˵    ����A LOW to HIGH transition on the SDA line while SCL is
HIGH defines a STOP condition
*******************************************/
static void I2C_Stop(void) 
{ 
	SCL_L; 			  
	I2C_Delay(); 
	SDA_L; 
	I2C_Delay(); 
	SCL_H; 			  //SCL���ɵͱ��
	I2C_Delay(); 
	SDA_H; 			  //��SCL�߸�ʱ������SDA�ߣ�����Stop�ź�
	I2C_Delay(); 
} 
/******************************************
�������ƣ�I2C_Ack()
��    �ܣ�֪ͨSLAVE��I2C���յ�����
˵    ����I2CЭ����Ҫ��masterÿ�յ�һ��byte��Ҫ��slave����ACK�źţ�
ACK�źž��ǽ�SDA������
*******************************************/
static void I2C_Ack(void) 
{ 
	u16 i;
	SCL_L; 
	I2C_Delay(); 
	SDA_L; 			  //��SCL�ߵ�ʱ������SDA�ߣ�����ACK�ź�
	I2C_Delay(); 
	SCL_H; 
	I2C_Delay(); 
	SCL_L; 			  //����SCL�ߴ��ڵ͵�ƽ
	I2C_Delay(); 
	for(i=0; i<200; i++)	//ÿ��ACK�󣬶�Ҫ��slaveһ���ķ�Ӧʱ�䣬������������
	{
		I2C_Delay();
	}
} 
/******************************************
�������ƣ�I2C_NoAck()
��    �ܣ�֪ͨSLAVE�����ݽ��ս����������ٷ�������
˵    ����I2CЭ����Ҫ��master���յ����һ��byteʱ��slave����NOACK�źţ�
��ʾ�����Ѿ�ȫ���յ��������ٷ����ݡ�NOACK�źž��ǽ�SDA������
*******************************************/
static void I2C_NoAck(void) 
{ 
	SCL_L; 
	I2C_Delay(); 
	SDA_H; 			   //��SCL�ߵ�ʱ������SDA�ߣ�����NOACK�ź�
	I2C_Delay(); 
	SCL_H; 
	I2C_Delay(); 
	SCL_L; 			   //����SCL�ߴ��ڵ͵�ƽ
	I2C_Delay(); 
} 
/******************************************
�������ƣ�I2C_WaitAck()
��    �ܣ��ȴ�SLAVE�յ����ݺ��ȷ���ź�
˵    ����I2CЭ����Ҫ��SLAVEÿ�յ�һ��byte�����ݣ���Ҫ֪ͨMASTER.
�źž��Ƿ��͸�MASTERһ��low���źţ���ʾ�յ�����
*******************************************/
static bool I2C_WaitAck(void)   
{ 
	u16 i;
	SCL_L; 
	I2C_Delay(); 
	SDA_H; 				//Master release SDA�ߣ�����SLAVE�ƿ�
	I2C_Delay(); 
	SCL_H; 
	I2C_Delay(); 
	if(SDA_read) 		//���SDA��ΪHIGH,��ʾSLAVEû��ACK
	{ 
		SCL_L; 
		return FALSE; 
	} 
	SCL_L; 				//����SCL�ߴ��ڵ͵�ƽ
	for(i=0; i<200; i++) 	//ÿ���յ�ACK�󣬶�Ҫ��Masterһ���ķ�Ӧʱ�䣬���������ݻ����
	{
		I2C_Delay();
	}
	return TRUE; 
} 
/******************************************
�������ƣ�I2C_SendByte()
��    �ܣ�Master��Slave����һ��byte������
˵    ���������ǴӸ�λ����λbit by bit����
*******************************************/
static void I2C_SendByte(u8 SendByte) 
{ 
	u8 i=8; 
	while(i--) 
	{ 
		SCL_L; 					 //����SCL�ߣ��ɸı�SDA������
		I2C_Delay(); 
		if(SendByte&0x80)		 //����λbit�ǲ���HIGH
		{ 
			SDA_H;				 //��SDA�����ߣ���ʾ����ΪHIGH
			I2C_Delay(); 
		}   
		else  
		{
			SDA_L;				 //��SDA�����ͣ���ʾ����ΪLOW
			I2C_Delay(); 
		}
		SendByte<<=1; 			 //�ƶ�һ��bit,׼��������һ��bit
		I2C_Delay(); 
		SCL_H; 					 //��SCL���ߣ�����һ��cycle
		I2C_Delay(); 
	} 
	SCL_L; 						 //����SCL�ߴ��ڵ͵�ƽ
} 
/******************************************
�������ƣ�I2C_ReceiveByte()
��    �ܣ�Master��Slave����һ��byte������
˵    ���������ǴӸ�λ����λbit by bit����
*******************************************/
static u8 I2C_ReceiveByte(void)  
{  
	u8 i=8; 
	u8 ReceiveByte=0; 

	SDA_H; 						  //Master release SDA�ߣ�����SLAVE����
	while(i--) 
	{ 
		ReceiveByte<<=1;       	  //�ƶ�һ��bit,׼��������һ��bit
		SCL_L; 					  //����SCL�ߣ�����SLAVE�ı�SDA��
		I2C_Delay(); 
		SCL_H; 					  //��SCL���ߣ�����һ��cycle
		I2C_Delay(); 
		if(SDA_read) 			  //��ȡSDA��������
		{ 
			ReceiveByte|=0x01; 	  //����ΪHIGH
		} 
	} 
	SCL_L; 						  //����SCL�ߴ��ڵ͵�ƽ
	return ReceiveByte; 
} 

#endif
//End of File
