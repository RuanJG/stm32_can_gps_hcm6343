#include "CT_I2C.h"
#include "delay.h"

void CT_I2C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(CT_SDA_CLK | CT_SCL_CLK, ENABLE);
	
	/* Configure PF6 PF7 PF8 PF9 in output pushpull mode */	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = CT_SDA_GPIO_PIN;
	GPIO_Init(CT_SDA_GPIO_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = CT_SCL_GPIO_PIN;
	GPIO_Init(CT_SCL_GPIO_PORT, &GPIO_InitStructure);	
	
}

void CT_SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(CT_SDA_CLK , ENABLE);
	/* Configure PF6 PF7 PF8 PF9 in output pushpull mode */	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = CT_SDA_GPIO_PIN;
	GPIO_Init(CT_SDA_GPIO_PORT, &GPIO_InitStructure);	
}


void CT_SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(CT_SDA_CLK , ENABLE);
	/* Configure PF6 PF7 PF8 PF9 in output pushpull mode */	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = CT_SDA_GPIO_PIN;
	GPIO_Init(CT_SDA_GPIO_PORT, &GPIO_InitStructure);	
}


//产生I2C起始信号
void CT_I2C_Start(void)
{
	CT_SDA_OUT();     //sda线输出
	SET_SDA();	  	  
	SET_CLK();
	delay_us(5);
 	CLR_SDA();//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	CLR_CLK();//钳住I2C总线，准备发送或接收数据 
}	  

//产生I2C停止信号
void CT_I2C_Stop(void)
{
	CT_SDA_OUT();//sda线输出
	SET_CLK();
	CLR_SDA();//STOP:when CLK is high DATA change form low to high
 	delay_us(5);
	SET_CLK();
	SET_SDA();//发送I2C总线结束信号
	delay_us(5);							   	
}


//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 CT_I2C_Wait_Ack(void)
{
	u16 ucErrTime=0;
	//CT_SDA_OUT();     //SDA设置为输入  
	//SET_SDA();   delay_us(1);	
	CT_SDA_IN(); 
	SET_CLK();delay_us(1);	
	while(CT_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			CT_I2C_Stop();
			return 1;
		}
	}
	CLR_CLK();//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void CT_I2C_Ack(void)
{
	CLR_CLK();
	CT_SDA_OUT();
	CLR_SDA();
	delay_us(5);
	SET_CLK();
	delay_us(5);
	CLR_CLK();
}
//不产生ACK应答		    
void CT_I2C_NAck(void)
{
	CLR_CLK();
	CT_SDA_OUT();
	SET_SDA();
	delay_us(5);
	SET_CLK();
	delay_us(5);
	CLR_CLK();
}					 				     
//I2C发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void CT_I2C_Send_Byte(u8 txd)
{                        
    u8 t;   
	CT_SDA_OUT(); 	    
    CLR_CLK();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
//        CT_I2C_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
		SET_SDA();
		else	
		CLR_SDA();
		
		txd<<=1; 	      
		SET_CLK();
		delay_us(1);
		CLR_CLK();	
		delay_us(1);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 CT_I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
 	CT_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
    CLR_CLK(); 	    	   
		delay_us(1);
		SET_CLK();
    receive<<=1;
		delay_us(1);
    if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_I2C_NAck();//发送nACK
	else CT_I2C_Ack(); //发送ACK   
	
 	return receive;
}
/*******************************************************************************
* Function Name  : CT_TEST
* Description    : Capacitance screen test
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/	
void CT_TEST(void)
{
	u8 	ctout;			
	u8 i=0,t=0;
	
}



