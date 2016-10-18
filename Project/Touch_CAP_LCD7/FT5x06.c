

#include "main.h"
#include "32f429_lcd.h"
#include "usart.h"
#include "FT5x06.h"


_m_ctp_dev ctp_dev=
{
	FT5x06_Init,
	FT5x06_Scan,
	0,
	0,
	0,
	0,
};	 
_m_ctp_dev ctp_dev_p;

u8 buf[34],num;
u16 CT_Color_table[]={Green,Blue,Red,White,Yellow};
/******************************************************************************
* Function Name  : TP_INT_Config
* Description    : Capacitive touch screen configuration
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void TP_INT_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB1PeriphClockCmd(CT_INT_CLK, ENABLE);
  
  /***********************GPIO Configuration***********************/
  GPIO_InitStructure.GPIO_Pin = CT_INT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CT_INT_GPIO_PORT, &GPIO_InitStructure);
    
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource4);
  EXTI_InitStructure.EXTI_Line = CT_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);  

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : Touch screen interrupt
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{  
  if(EXTI_GetITStatus(CT_INT_EXTI_LINE)!= RESET )
  {			

		
		ctp_dev.tpsta|=0X80;//标记有有效触摸		
		FT5x06_RD_Reg(0x2,&num,1);
		printf("num:%x\r\n",num);
		if(num<=5 && num>0)
		{
			num=num&0x07;
			FT5x06_RD_Reg(0x0,buf,num*6+3);
		}
		else
			ctp_dev.tpsta=0;
		
    EXTI_ClearITPendingBit(CT_INT_EXTI_LINE);//Clears the EXTI's line pending bits.
    EXTI_ClearFlag(CT_INT_EXTI_LINE);			
  }  
}


/*******************************************************************************
* Function Name  : FT5x06_WR_Reg
* Description    : Writes to the selected GT811 register.
* Input          : - reg: address of the selected register.
*                  - buf: Need to write the BUF pointer.
*                  - len: The length of the array
* Output         : None
* Return         : ret
                   -0  succeed
									 -1  error
* Attention		 : None
*******************************************************************************/
u8 FT5x06_WR_Reg(u8 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_I2C_Start();	
	
 	CT_I2C_Send_Byte(CT_CMD_WR);   //Slaver Addr
	CT_I2C_Wait_Ack();
	
	CT_I2C_Send_Byte(reg);   	     //Data Addr
	CT_I2C_Wait_Ack(); 	 										  		   
 
	for(i=0;i<len;i++)
	{	   
    	CT_I2C_Send_Byte(buf[i]);  
		ret=CT_I2C_Wait_Ack();
		if(ret)break;  
	}
    CT_I2C_Stop();					
	return ret; 
}

/*******************************************************************************
* Function Name  : FT5x06_RD_Reg
* Description    : Writes to the selected GT811 register.
* Input          : - reg: address of the selected register.
*                  - buf: Need to read the BUF pointer.
*                  - len: The length of the array
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/		  
void FT5x06_RD_Reg(u8 reg,u8 *buf,u8 len)
{
	u8 i;
 	CT_I2C_Start();	
 	CT_I2C_Send_Byte(CT_CMD_WR);  
	CT_I2C_Wait_Ack();
 	CT_I2C_Send_Byte(reg);   
	CT_I2C_Wait_Ack(); 	 										  		   
  CT_I2C_Stop(); 
  
 	CT_I2C_Start();  	 	   
	CT_I2C_Send_Byte(CT_CMD_RD);     
	CT_I2C_Wait_Ack();		
	
	for(i=0;i<len;i++)
	{	   
    	buf[i]=CT_I2C_Read_Byte(i==(len-1)?0:1); 
	} 
    CT_I2C_Stop();   
}

/*******************************************************************************
* Function Name  : FT5x06_Init
* Description    : GT811 initialization
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/
u8 FT5x06_Init(void)
{
 	u8 version=0x0;
	u8 temp;
	
 	CT_I2C_Init();								//I2C initialization
	TP_INT_Config();
	
	FT5x06_WR_Reg(0x0,&version,1);
	delay_ms(1);
// 	while(1)
// 	{
// 		FT5x06_RD_Reg(0x2,data,8);
// 		printf("%d, %d, %d, %d, %d, %d, %d, %d\r\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
// 		delay_ms(500);
// 	}
	return temp; 
}

void FT5x06_Scan(void)
{

	u8 i;
	if((ctp_dev.tpsta&0X80)==0)return;
	

	
	ctp_dev.tpsta=buf[2]&0X07;	

	ctp_dev.x[0]=(((u16)(buf[3]&0x0f)<<8)+buf[4]);	//Touch point 1 coordinates
	ctp_dev.y[0]=480-(((u16)(buf[5]&0x0f)<<8)+buf[6]);	 // 
	ctp_dev.ppr[0]=buf[3]&0xf0;	
	
	ctp_dev.x[1]=(((u16)(buf[9]&0x0f)<<8)+buf[10]);	//Touch point 2 coordinates
	ctp_dev.y[1]=480-(((u16)(buf[11]&0x0f)<<8)+buf[12]);	  
	ctp_dev.ppr[1]=buf[9]&0xf0;
	
	ctp_dev.x[2]=(((u16)(buf[15]&0x0f)<<8)+buf[16]);//Touch point 3 coordinates
	ctp_dev.y[2]=480-(((u16)(buf[17]&0x0f)<<8)+buf[18]);	  
	ctp_dev.ppr[2]=buf[15]&0xf0;		
	
	ctp_dev.x[3]=(((u16)(buf[21]&0x0f)<<8)+buf[22]);//Touch point 4 coordinates
	ctp_dev.y[3]=480-(((u16)(buf[23]&0x0f)<<8)+buf[24]);	  
	ctp_dev.ppr[3]=buf[21]&0xf0;
	
	ctp_dev.x[4]=(((u16)(buf[27]&0x0f)<<8)+buf[28]);//Touch point 5 coordinates
	ctp_dev.y[4]=480-(((u16)(buf[29]&0x0f)<<8)+buf[30]);	  
	ctp_dev.ppr[4]=buf[27]&0xf0;
	
	for(i=0;i<num;i++)
	{
		ctp_dev.x[i]=800-ctp_dev.x[i];
	}		
	
}

void LCD_Display_xy(uint16_t x_vlue, uint16_t y_vlue, uint16_t i)  //
{
	u8 j=0;
	LCD_SetFont(&Font8x8);
	
	LCD_DisplayStringLine(LINE(7+i), (uint8_t*)"x:    y:  "); 

	j=8*(7+i);
	LCD_DisplayChar(j,16,0x30+x_vlue%1000/100);
	LCD_DisplayChar(j,24,0x30+x_vlue%100/10);
	LCD_DisplayChar(j,32,0x30+x_vlue%10);	
	
	LCD_DisplayChar(j,56+8,0x30+y_vlue%1000/100);
	LCD_DisplayChar(j,56+16,0x30+y_vlue%100/10);
	LCD_DisplayChar(j,56+24,0x30+y_vlue%10);	
}

/*******************************************************************************
* Function Name  : FT5x06_Test
* Description    : Test FT5x06 I2C
* Input          : None
* Output         : None
* Return         : None
* Attention		   : None
*******************************************************************************/

void FT5x06_Test(void)
{
	u8 i=0,p=0,j=0;
	uint16_t TextColor,BackColor;

	
	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
	LCD_GetColors(&TextColor,&BackColor);
	LCD_Clear(BackColor);
	LCD_SetFont(&Font16x24);
	LCD_DisplayStringLine(LINE(1), (uint8_t*)"         Capacitive touch screen test"); 
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	LCD_DrawLine(2, 50, 800, LCD_DIR_HORIZONTAL);	
	while(1)
	{
		ctp_dev.tpsta=0;
		ctp_dev.scan();
	if(ctp_dev.tpsta&0X07)	
	{			LCD_SetColors(BackColor,BackColor);
				//LCD_DrawFullRect(0,55,800,480);   //clear
				for(i=0;i<(ctp_dev.tpsta&0X07);i++)
				{
					//if(ctp_dev.y[i]<75 || ctp_dev.x[i]<20 || ctp_dev.x[i]>780)continue;
					if(ctp_dev.y[i]<75)ctp_dev.y[i]=75;
					if(ctp_dev.y[i]>460)ctp_dev.y[i]=460;
					if(ctp_dev.x[i]<20)ctp_dev.x[i]=20;
					if(ctp_dev.x[i]>780)ctp_dev.x[i]=780;
					
					LCD_GetColors(&TextColor,&BackColor);
					LCD_SetColors(BackColor,BackColor);
					LCD_DrawFullCircle(ctp_dev_p.x[i],ctp_dev_p.y[i],20);
					LCD_DrawLine(ctp_dev_p.x[i], 55, 470, LCD_DIR_VERTICAL);
					LCD_DrawLine(5, ctp_dev_p.y[i], 790, LCD_DIR_HORIZONTAL);	
					
					LCD_SetColors(CT_Color_table[i],BackColor);
		      LCD_Display_xy(ctp_dev.x[i],ctp_dev.y[i],i);
					LCD_DrawFullCircle(ctp_dev.x[i],ctp_dev.y[i],20);
					LCD_DrawLine(ctp_dev.x[i], 55, 470, LCD_DIR_VERTICAL);
					LCD_DrawLine(5, ctp_dev.y[i], 790, LCD_DIR_HORIZONTAL);	
					
					ctp_dev_p.x[i]=ctp_dev.x[i];
					ctp_dev_p.y[i]=ctp_dev.y[i];
				}
				for(j=i;j<p;j++)
				{
					LCD_GetColors(&TextColor,&BackColor);
					LCD_SetColors(BackColor,BackColor);
					LCD_DisplayStringLine(LINE(7+j), (uint8_t*)"            "); 
					LCD_DrawFullCircle(ctp_dev_p.x[j],ctp_dev_p.y[j],20);
					LCD_DrawLine(ctp_dev_p.x[j], 55, 470, LCD_DIR_VERTICAL);
					LCD_DrawLine(5, ctp_dev_p.y[j], 790, LCD_DIR_HORIZONTAL);						
				}
				p=ctp_dev.tpsta&0X07;					

	}

	
}

}






