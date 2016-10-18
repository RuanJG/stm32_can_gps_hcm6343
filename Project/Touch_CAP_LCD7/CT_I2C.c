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


//����I2C��ʼ�ź�
void CT_I2C_Start(void)
{
	CT_SDA_OUT();     //sda�����
	SET_SDA();	  	  
	SET_CLK();
	delay_us(5);
 	CLR_SDA();//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	CLR_CLK();//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  

//����I2Cֹͣ�ź�
void CT_I2C_Stop(void)
{
	CT_SDA_OUT();//sda�����
	SET_CLK();
	CLR_SDA();//STOP:when CLK is high DATA change form low to high
 	delay_us(5);
	SET_CLK();
	SET_SDA();//����I2C���߽����ź�
	delay_us(5);							   	
}


//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 CT_I2C_Wait_Ack(void)
{
	u16 ucErrTime=0;
	//CT_SDA_OUT();     //SDA����Ϊ����  
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
	CLR_CLK();//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
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
//������ACKӦ��		    
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
//I2C����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void CT_I2C_Send_Byte(u8 txd)
{                        
    u8 t;   
	CT_SDA_OUT(); 	    
    CLR_CLK();//����ʱ�ӿ�ʼ���ݴ���
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
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 CT_I2C_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
 	CT_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
    CLR_CLK(); 	    	   
		delay_us(1);
		SET_CLK();
    receive<<=1;
		delay_us(1);
    if(CT_READ_SDA)receive++;   
	}	  				 
	if (!ack)CT_I2C_NAck();//����nACK
	else CT_I2C_Ack(); //����ACK   
	
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



