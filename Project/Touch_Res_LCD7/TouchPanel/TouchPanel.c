/*********************************************************************************************************
*
* File                : TouchPanel.c
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "TouchPanel/TouchPanel.h"
#include "TouchPanel/config.h"
#include "32f429_lcd.h"
#include "math.h"
#include "usart.h"
/* Private variables ---------------------------------------------------------*/
Matrix matrix={
 .An=0.2853127181968,
 .Bn=-0.006479341921389,	
 .Cn=-85.80441938016,
 .Dn=0.005553647235954,	
 .En=-0.1871552550425,
 .Fn=562.3708463642,	
 .Divider=-3376022,
} ;

Coordinate  display;
__IO u8 Line_Flag=0,line_cnt=0;

Coordinate ScreenSample[5];

Coordinate DisplaySample[5] = {
                                 { 80, 50 },   // +50
											           { 720,50},
                                 { 80,430},
                                 { 720, 430 },
											           { 400,240}

	                          };

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay Time
* Input          : - cnt: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 0xffff; j++ );
	}
} 
/*******************************************************************************
* Function Name  : ADS7843_SPI_Init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void ADS7843_SPI_Init(void) 
{ 
  SPI_InitTypeDef SPI_InitStruct;	

  RCC_APB1PeriphClockCmd(Open_RCC_SPI,ENABLE);	

  SPI_I2S_DeInit(Open_SPI);
  /* Open_SPI Config -------------------------------------------------------------*/ 
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master; 
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; 
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; 
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge; 
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; 
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; 
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB; 
  SPI_InitStruct.SPI_CRCPolynomial = 7; 

  SPI_Init(Open_SPI, &SPI_InitStruct);

  SPI_Cmd(Open_SPI, ENABLE); 
} 

/*******************************************************************************
* Function Name  : TP_Init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TP_Init(void) 
{ 

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(Open_SPI_SCK_GPIO_CLK | Open_SPI_MISO_GPIO_CLK | Open_SPI_MOSI_GPIO_CLK,ENABLE);

	RCC_AHB1PeriphClockCmd(Open_TP_CS_CLK | Open_TP_IRQ_CLK,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	Open_SPI_CLK_INIT(Open_RCC_SPI,ENABLE);

	GPIO_PinAFConfig(Open_SPI_SCK_GPIO_PORT,  Open_SPI_SCK_SOURCE,  Open_GPIO_AF_SPI);
	GPIO_PinAFConfig(Open_SPI_MISO_GPIO_PORT, Open_SPI_MISO_SOURCE, Open_GPIO_AF_SPI);
	GPIO_PinAFConfig(Open_SPI_MOSI_GPIO_PORT, Open_SPI_MOSI_SOURCE, Open_GPIO_AF_SPI);

	GPIO_InitStructure.GPIO_Pin = Open_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  
	GPIO_Init(Open_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = Open_SPI_MISO_PIN;
	GPIO_Init(Open_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = Open_SPI_MOSI_PIN;
	GPIO_Init(Open_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);


  /* TP_CS  */
  GPIO_InitStructure.GPIO_Pin = Open_TP_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(Open_TP_CS_PORT, &GPIO_InitStructure);

   /*4.3 inch TP_CS  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
    /*TP_IRQ */
    GPIO_InitStructure.GPIO_Pin = Open_TP_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(Open_TP_IRQ_PORT, &GPIO_InitStructure);	

  GPIO_SetBits(GPIOD,GPIO_Pin_5);
	GPIO_ResetBits(GPIOD,GPIO_Pin_4);
  TP_CS(1); 
  ADS7843_SPI_Init(); 
} 


void TP_IRQ_EXTI_Config(void)     
{
  GPIO_InitTypeDef   GPIO_InitStructure;  
  NVIC_InitTypeDef   NVIC_InitStructure;   
	EXTI_InitTypeDef   EXTI_InitStructure;
  
  RCC_AHB1PeriphClockCmd(Open_TP_IRQ_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);   

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
 	GPIO_InitStructure.GPIO_Pin = Open_TP_IRQ_PIN;   
  GPIO_Init(Open_TP_IRQ_PORT, &GPIO_InitStructure);    
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
 
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line3;//PB15做外部中断,下降沿触发,做为拍照按钮
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;     
  EXTI_Init(&EXTI_InitStructure);  

 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}


/*******************************************************************************
* Function Name  : DelayUS
* Description    : 
* Input          : - cnt:
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/

static void DelayUS(vu32 cnt)
{
  uint16_t i;
  for(i = 0;i<cnt;i++)
  {
     uint8_t us = 10;
     while (us--)
     {
       ;   
     }
  }
}


/*******************************************************************************
* Function Name  : WR_CMD
* Description    : 
* Input          : - cmd: 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void WR_CMD (uint8_t cmd)  
{ 
  /* Wait for SPI3 Tx buffer empty */ 
  while (SPI_I2S_GetFlagStatus(Open_SPI, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send SPI3 data */ 
  SPI_I2S_SendData(Open_SPI,cmd); 
  /* Wait for SPI3 data reception */ 
  while (SPI_I2S_GetFlagStatus(Open_SPI, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read Open_SPI received data */ 
  SPI_I2S_ReceiveData(Open_SPI); 
} 



/*******************************************************************************
* Function Name  : RD_AD
* Description    : 
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
static int RD_AD(void)  
{ 
  unsigned short buf,temp; 
  /* Wait for Open_SPI Tx buffer empty */ 
  while (SPI_I2S_GetFlagStatus(Open_SPI, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send Open_SPI data */ 
  SPI_I2S_SendData(Open_SPI,0x0000); 
  /* Wait for SPI3 data reception */ 
  while (SPI_I2S_GetFlagStatus(Open_SPI, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read Open_SPI received data */ 
  temp=SPI_I2S_ReceiveData(Open_SPI); 
  buf=temp<<8; 
  DelayUS(1); 
  while (SPI_I2S_GetFlagStatus(Open_SPI, SPI_I2S_FLAG_TXE) == RESET); 
  /* Send Open_SPI data */ 
  SPI_I2S_SendData(Open_SPI,0x0000); 
  /* Wait for Open_SPI data reception */ 
  while (SPI_I2S_GetFlagStatus(Open_SPI, SPI_I2S_FLAG_RXNE) == RESET); 
  /* Read Open_SPI received data */ 
  temp=SPI_I2S_ReceiveData(Open_SPI); 
  buf |= temp; 
  buf>>=3; 
  buf&=0xfff; 
  return buf; 
} 


/*******************************************************************************
* Function Name  : Read_X
* Description    : Read ADS7843 ADC X 
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
int Read_X(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHX); 
  DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;    
} 

/*******************************************************************************
* Function Name  : Read_Y
* Description    : Read ADS7843 ADC Y
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
int Read_Y(void)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(CHY); 
  DelayUS(1); 
  i=RD_AD(); 
  TP_CS(1); 
  return i;     
} 


/*******************************************************************************
* Function Name  : TP_GetAdXY
* Description    : Read ADS7843
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
void TP_GetAdXY(int *x,int *y)  
{ 
  int adx,ady; 
  adx=Read_X(); 
  DelayUS(1); 
  ady=Read_Y(); 
  *x=adx; 
  *y=ady; 
} 

/*******************************************************************************
* Function Name  : TP_DrawPoint
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos)
{
//   LCD_SetPoint(Xpos,Ypos,Blue);     /* Center point */
//   LCD_SetPoint(Xpos+1,Ypos,Blue);
//   LCD_SetPoint(Xpos,Ypos+1,Blue);
//   LCD_SetPoint(Xpos+1,Ypos+1,Blue);	
}	

/*******************************************************************************
* Function Name  : DrawCross
* Description    : 
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate 
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void DrawCross(uint16_t Xpos,uint16_t Ypos)
{
  LCD_DrawLine(Xpos-13,Ypos,10,LCD_DIR_HORIZONTAL);
	LCD_DrawLine(Xpos+4,Ypos,10,LCD_DIR_HORIZONTAL);
	LCD_DrawLine(Xpos, Ypos-13, 10, LCD_DIR_VERTICAL);
	LCD_DrawLine(Xpos, Ypos+4, 10, LCD_DIR_VERTICAL);	
	
}	
	
/*******************************************************************************
* Function Name  : Read_Ads7846
* Description    : Get TouchPanel X Y
* Input          : None
* Output         : None
* Return         : Coordinate *
* Attention		 : None
*******************************************************************************/
Coordinate *Read_Ads7846(void)
{
  static Coordinate  screen,screen_p;
  //int m0,m1,m2;,temp[3]
	int TP_X[1],TP_Y[1];
  uint8_t count=0,i,j;
  int buffer[2][32]={{0},{0}},data=0;  
  if((TP_INT_IN)) 
		{line_cnt=0;Line_Flag=0;}	
  if(!TP_INT_IN && (!Line_Flag))
		delay_ms(5);
// 	if((!TP_INT_IN))
// 		Line_Flag=1;
// 	else
// 		Line_Flag=0;	
  do
  {		   
		TP_GetAdXY(TP_X,TP_Y);  
		buffer[0][count]=TP_X[0];  
		buffer[1][count]=TP_Y[0];
		count++;  
		DelayUS(10);
  }
  while(!TP_INT_IN&& count<32);  /* TP_INT_IN  */
  if(count==32)   /* X Y  */ 
  {

		for(i=0;i<32;i++)
		{ 
			for(j=0;j<32-i;j++)
			{
				if(buffer[0][j]>buffer[0][j+1]) ///////X
				{
					data=buffer[0][j];
					buffer[0][j]=buffer[0][j+1];
					buffer[0][j+1]=data;
				}
				if(buffer[1][j]>buffer[1][j+1])   //Y
				{
					data=buffer[1][j];
					buffer[1][j]=buffer[1][j+1];
					buffer[1][j+1]=data;
				}
			}

		}
// 		screen.x=(buffer[0][4]+buffer[0][5]+buffer[0][6]+buffer[0][7]+buffer[0][8]+buffer[0][9]+buffer[0][10]+buffer[0][11])/8;
// 		screen.y=(buffer[1][4]+buffer[1][5]+buffer[1][6]+buffer[1][7]+buffer[1][8]+buffer[1][9]+buffer[1][10]+buffer[1][11])/8;		
		
		screen.x=(buffer[0][12]+buffer[0][13]+buffer[0][14]+buffer[0][15]+buffer[0][16]+buffer[0][17]+buffer[0][18]+buffer[0][19])/8;
		screen.y=(buffer[1][12]+buffer[1][13]+buffer[1][14]+buffer[1][15]+buffer[1][16]+buffer[1][17]+buffer[1][18]+buffer[1][19])/8;	
// 	  printf("X:%d\r\n",screen.x);
// 		printf("X:%d\r\n",screen_p.x);		
// 	  printf("Y:%d\r\n",screen.y);	
// 	  printf("Y:%d\r\n\r\n",screen_p.y);	
		if((screen.x-screen_p.x)<100 && (screen_p.x-screen.x)<100)
			if((screen.y-screen_p.y)<100 && (screen_p.y-screen.y)<100)
				line_cnt++;
		if(!Line_Flag)
		{
		screen_p.x=screen.x;
		screen_p.y=screen.y;		
		}			
   if(!TP_INT_IN && (line_cnt>10))
	 {
	  	Line_Flag=1;	 
	 }				
 		if((!TP_INT_IN) && Line_Flag)
		{
				
			if((screen.x-screen_p.x)>100 || (screen_p.x-screen.x)>100)
			{
	 			return 0; 
	 		}
	 		else if((screen.y-screen_p.y)>100 || (screen_p.y-screen.y)>100)
	 		{
	 			return 0; 
	 		}		
			else 
			{
				screen_p.x=screen.x;
				screen_p.y=screen.y;		
				return &screen;				
			}
		}
		//return &screen;		
  }  
  return 0; 
}
	 


Coordinate *Read_Ads7846_Cal(void)
{
	static Coordinate  screen;
  //int m0,m1,m2;,temp[3]
	int TP_X[1],TP_Y[1];
  uint8_t count=0,i,j;
  int buffer[2][32]={{0},{0}},data=0;  


  do
  {		   
		TP_GetAdXY(TP_X,TP_Y);  
		buffer[0][count]=TP_X[0];  
		buffer[1][count]=TP_Y[0];
		count++;  
		DelayUS(10);
  }
  while(!TP_INT_IN&& count<32);  /* TP_INT_IN  */
  if(count==32)   /* X Y  */ 
  {

		for(i=0;i<32;i++)
		{ 
			for(j=0;j<32-i;j++)
			{
				if(buffer[0][j]>buffer[0][j+1]) ///////X
				{
					data=buffer[0][j];
					buffer[0][j]=buffer[0][j+1];
					buffer[0][j+1]=data;
				}
				if(buffer[1][j]>buffer[1][j+1])   //Y
				{
					data=buffer[1][j];
					buffer[1][j]=buffer[1][j+1];
					buffer[1][j+1]=data;
				}
			}

		}
// 		screen.x=(buffer[0][4]+buffer[0][5]+buffer[0][6]+buffer[0][7]+buffer[0][8]+buffer[0][9]+buffer[0][10]+buffer[0][11])/8;
// 		screen.y=(buffer[1][4]+buffer[1][5]+buffer[1][6]+buffer[1][7]+buffer[1][8]+buffer[1][9]+buffer[1][10]+buffer[1][11])/8;		
		
		screen.x=(buffer[0][12]+buffer[0][13]+buffer[0][14]+buffer[0][15]+buffer[0][16]+buffer[0][17]+buffer[0][18]+buffer[0][19])/8;
		screen.y=(buffer[1][12]+buffer[1][13]+buffer[1][14]+buffer[1][15]+buffer[1][16]+buffer[1][17]+buffer[1][18]+buffer[1][19])/8;	


		return &screen;		
  }  
  return 0; 
}
/*******************************************************************************
* Function Name  : setCalibrationMatrix
* Description    : Calculate K A B C D E F
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,
                          Coordinate * screenPtr,
                          Matrix * matrixPtr)
{
	
	
	
	
	FunctionalState retTHRESHOLD = ENABLE ;
  long double a,b,c,d,e,f,g,h,i,j,k,l;
	a=screenPtr[0].x + screenPtr[1].x + screenPtr[2].x + screenPtr[3].x + screenPtr[4].x;
	b=screenPtr[0].y + screenPtr[1].y + screenPtr[2].y + screenPtr[3].y + screenPtr[4].y;
	c=5;
	d=displayPtr[0].x + displayPtr[1].x + displayPtr[2].x + displayPtr[3].x + displayPtr[4].x;
	 
	e=screenPtr[0].x*screenPtr[0].x + screenPtr[1].x*screenPtr[1].x + screenPtr[2].x*screenPtr[2].x + screenPtr[3].x*screenPtr[3].x + screenPtr[4].x*screenPtr[4].x;
	f=screenPtr[0].x*screenPtr[0].y + screenPtr[1].x*screenPtr[1].y + screenPtr[2].x*screenPtr[2].y + screenPtr[3].x*screenPtr[3].y + screenPtr[4].x*screenPtr[4].y;
	g=screenPtr[0].x + screenPtr[1].x + screenPtr[2].x + screenPtr[3].x + screenPtr[4].x;
  h=displayPtr[0].x*screenPtr[0].x + displayPtr[1].x*screenPtr[1].x + displayPtr[2].x*screenPtr[2].x + displayPtr[3].x*screenPtr[3].x + displayPtr[4].x*screenPtr[4].x;
	
	i=screenPtr[0].x*screenPtr[0].y + screenPtr[1].x*screenPtr[1].y + screenPtr[2].x*screenPtr[2].y + screenPtr[3].x*screenPtr[3].y + screenPtr[4].x*screenPtr[4].y;
	j=screenPtr[0].y*screenPtr[0].y + screenPtr[1].y*screenPtr[1].y + screenPtr[2].y*screenPtr[2].y + screenPtr[3].y*screenPtr[3].y + screenPtr[4].y*screenPtr[4].y;
	k=screenPtr[0].y + screenPtr[1].y + screenPtr[2].y + screenPtr[3].y + screenPtr[4].y;
	l=displayPtr[0].x*screenPtr[0].y + displayPtr[1].x*screenPtr[1].y + displayPtr[2].x*screenPtr[2].y + displayPtr[3].x*screenPtr[3].y + displayPtr[4].x*screenPtr[4].y;
	
	matrixPtr->An =(b*g*l - b*h*k - c*f*l + c*h*j + d*f*k - d*g*j)/(a*f*k - a*g*j - b*e*k + b*g*i + c*e*j - c*f*i);
	matrixPtr->Bn =(-a*g*l + a*h*k + c*e*l - c*h*i - d*e*k + d*g*i)/(a*f*k - a*g*j - b*e*k + b*g*i + c*e*j - c*f*i);
	matrixPtr->Cn =(a*f*l - a*h*j - b*e*l + b*h*i + d*e*j - d*f*i)/(a*f*k - a*g*j - b*e*k + b*g*i + c*e*j - c*f*i);
	
	a=screenPtr[0].x + screenPtr[1].x + screenPtr[2].x + screenPtr[3].x + screenPtr[4].x;
	b=screenPtr[0].y + screenPtr[1].y + screenPtr[2].y + screenPtr[3].y + screenPtr[4].y;
	c=5;
	d=displayPtr[0].y + displayPtr[1].y + displayPtr[2].y + displayPtr[3].y + displayPtr[4].y;
	 
	e=screenPtr[0].x*screenPtr[0].x + screenPtr[1].x*screenPtr[1].x + screenPtr[2].x*screenPtr[2].x + screenPtr[3].x*screenPtr[3].x + screenPtr[4].x*screenPtr[4].x;
	f=screenPtr[0].x*screenPtr[0].y + screenPtr[1].x*screenPtr[1].y + screenPtr[2].x*screenPtr[2].y + screenPtr[3].x*screenPtr[3].y + screenPtr[4].x*screenPtr[4].y;
	g=screenPtr[0].x + screenPtr[1].x + screenPtr[2].x + screenPtr[3].x + screenPtr[4].x;
  h=displayPtr[0].y*screenPtr[0].x + displayPtr[1].y*screenPtr[1].x + displayPtr[2].y*screenPtr[2].x + displayPtr[3].y*screenPtr[3].x + displayPtr[4].y*screenPtr[4].x;
	
	i=screenPtr[0].x*screenPtr[0].y + screenPtr[1].x*screenPtr[1].y + screenPtr[2].x*screenPtr[2].y + screenPtr[3].x*screenPtr[3].y + screenPtr[4].x*screenPtr[4].y;
	j=screenPtr[0].y*screenPtr[0].y + screenPtr[1].y*screenPtr[1].y + screenPtr[2].y*screenPtr[2].y + screenPtr[3].y*screenPtr[3].y + screenPtr[4].y*screenPtr[4].y;
	k=screenPtr[0].y + screenPtr[1].y + screenPtr[2].y + screenPtr[3].y + screenPtr[4].y;
	l=displayPtr[0].y*screenPtr[0].y + displayPtr[1].y*screenPtr[1].y + displayPtr[2].y*screenPtr[2].y + displayPtr[3].y*screenPtr[3].y + displayPtr[4].y*screenPtr[4].y;
	
	matrixPtr->Dn =(b*g*l - b*h*k - c*f*l + c*h*j + d*f*k - d*g*j)/(a*f*k - a*g*j - b*e*k + b*g*i + c*e*j - c*f*i);
	matrixPtr->En =(-a*g*l + a*h*k + c*e*l - c*h*i - d*e*k + d*g*i)/(a*f*k - a*g*j - b*e*k + b*g*i + c*e*j - c*f*i);
	matrixPtr->Fn =(a*f*l - a*h*j - b*e*l + b*h*i + d*e*j - d*f*i)/(a*f*k - a*g*j - b*e*k + b*g*i + c*e*j - c*f*i);
	
  return( retTHRESHOLD ) ;
}

/*******************************************************************************
* Function Name  : getDisplayPoint
* Description    : Touch panel X Y to display X Y
* Input          : None
* Output         : None
* Return         : 
* Attention		 : None
*******************************************************************************/
FunctionalState getDisplayPoint(Coordinate * displayPtr,
                     Coordinate * screenPtr,
                     Matrix * matrixPtr )
{
  FunctionalState retTHRESHOLD =ENABLE ;
  /*
	An=168
	*/
  if( matrixPtr->Divider != 0 )
  {
		
    /* XD = AX+BY+C */        
    displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                      (matrixPtr->Bn * screenPtr->y) + 
                       matrixPtr->Cn 
                    ) ;
	/* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    );
		
				
  }
  else
  {
    retTHRESHOLD = DISABLE;
  }
  return(retTHRESHOLD);
} 



/*******************************************************************************
* Function Name  : TouchPanel_Calibrate
* Description    : 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TouchPanel_Calibrate(void)
{
  uint8_t i,flag=1;
  Coordinate * Ptr;
  uint16_t AB,AC,CD,BD,AD,BC;
	u32 tem1,tem2;
	float fac; 
	while(flag==1)
	{
		for(i=0;i<5;i++)
		{
			LCD_SetFont(&Font12x12);
			LCD_Clear(LCD_COLOR_BLACK);
			LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);//LCD_SetTextColor(LCD_COLOR_BLACK); 
		 LCD_DisplayStringLine(LINE(1), (uint8_t*)"Touch crosshair to calibrate");
		 delay_ms(150);
		 DrawCross(DisplaySample[i].x,DisplaySample[i].y);
		 do
		 {
			 if(!TP_INT_IN)
			 {
				 delay_ms(100);//DelayUS(10000);
				 Ptr=Read_Ads7846_Cal();
			 }
			 else 
				 Ptr = (void*)0;
			 while(!TP_INT_IN);
		 }
		 while( Ptr == (void*)0 );
		 ScreenSample[i].x= Ptr->x; ScreenSample[i].y= Ptr->y;
		}
		tem1=(ScreenSample[0].x-ScreenSample[1].x)*(ScreenSample[0].x-ScreenSample[1].x);
		tem2=(ScreenSample[0].y-ScreenSample[1].y)*(ScreenSample[0].y-ScreenSample[1].y);
		AB=sqrt(tem1+tem2);

		tem1=(ScreenSample[0].x-ScreenSample[2].x)*(ScreenSample[0].x-ScreenSample[2].x);
		tem2=(ScreenSample[0].y-ScreenSample[2].y)*(ScreenSample[0].y-ScreenSample[2].y);
		AC=sqrt(tem1+tem2);

		tem1=(ScreenSample[3].x-ScreenSample[1].x)*(ScreenSample[3].x-ScreenSample[1].x);
		tem2=(ScreenSample[3].y-ScreenSample[1].y)*(ScreenSample[3].y-ScreenSample[1].y);
		BD=sqrt(tem1+tem2);		

		tem1=(ScreenSample[2].x-ScreenSample[3].x)*(ScreenSample[2].x-ScreenSample[3].x);
		tem2=(ScreenSample[2].y-ScreenSample[3].y)*(ScreenSample[2].y-ScreenSample[3].y);
		CD=sqrt(tem1+tem2);

		tem1=(ScreenSample[0].x-ScreenSample[3].x)*(ScreenSample[0].x-ScreenSample[3].x);
		tem2=(ScreenSample[0].y-ScreenSample[3].y)*(ScreenSample[0].y-ScreenSample[3].y);
		AD=sqrt(tem1+tem2);		

		tem1=(ScreenSample[2].x-ScreenSample[1].x)*(ScreenSample[2].x-ScreenSample[1].x);
		tem2=(ScreenSample[2].y-ScreenSample[1].y)*(ScreenSample[2].y-ScreenSample[1].y);
		BC=sqrt(tem1+tem2);	
		
		fac=(float)AD/BC;
		if(fac<(float)0.94||fac>(float)1.06)//不合格
		continue;		
		
		fac=(float)AB/CD;
		if(fac<(float)0.94||fac>(float)1.06)//不合格
		continue;
	
		flag=0;

	}
   setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix );
   LCD_Clear(LCD_COLOR_BLACK);
} 
void Xpos_Ypos(uint16_t Xpos, uint16_t Ypos)
{
	  uint32_t XAD; 
	  LCD_SetFont(&Font16x24);
	  //LCD_SetTextColor(LCD_COLOR_RED);
  	LCD_DisplayStringLine(LINE(1), (uint8_t*)"Xpos:");
	  LCD_DisplayStringLine(LINE(2), (uint8_t*)"Ypos:");
		XAD=16*5;
		/*x??*/
		LCD_DisplayChar(24,XAD,48+Xpos/1000); 			XAD=XAD+16;
		LCD_DisplayChar(24,XAD,48+Xpos%1000/100);   XAD=XAD+16; 
		LCD_DisplayChar(24,XAD,48+Xpos%100/10); 	  XAD=XAD+16;
		LCD_DisplayChar(24,XAD,48+Xpos%10);     		XAD=XAD+16;
		/*y??*/
		XAD=16*5;
		LCD_DisplayChar(48,XAD,48+Ypos/1000);   		XAD=XAD+16;
		LCD_DisplayChar(48,XAD,48+Ypos%1000/100);   XAD=XAD+16;
		LCD_DisplayChar(48,XAD,48+Ypos%100/10);     XAD=XAD+16;
		LCD_DisplayChar(48,XAD,48+Ypos%10);         XAD=XAD+16;
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
