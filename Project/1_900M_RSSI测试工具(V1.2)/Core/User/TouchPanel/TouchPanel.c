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
#include "global_includes.h"


/* Private variables ---------------------------------------------------------*/
Matrix matrix ;
Coordinate  *mdisplay;
u8 _PenIsDown = 0;

//u32 xScreen, yScreen;
int xScreen, yScreen;

Coordinate ScreenSample[3];

Coordinate DisplaySample[3] = {
//                                  { 48, 27 },
// 											           { 432,136},
//                                  { 240,243}
//                                 { 45, 45 },
//											           { 400,60},
//                                 { 240,250}
																	{0, 0},
																	{480, 272}
	                          };

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 2


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
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
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


  /*7 inch TP_CS  */
  GPIO_InitStructure.GPIO_Pin = Open_TP_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(Open_TP_CS_PORT, &GPIO_InitStructure);

   /*4.3 inch TP_CS  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOI, &GPIO_InitStructure);
	
	/*TP_IRQ */
	GPIO_InitStructure.GPIO_Pin = Open_TP_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(Open_TP_IRQ_PORT, &GPIO_InitStructure);	

	GPIO_SetBits(GPIOI,GPIO_Pin_3);
	TP_CS(1); 
	ADS7843_SPI_Init(); 
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
	
//   LCD_DrawLine(Xpos-15,Ypos,Xpos-2,Ypos,0xffff);
//   LCD_DrawLine(Xpos+2,Ypos,Xpos+15,Ypos,0xffff);
//   LCD_DrawLine(Xpos,Ypos-15,Xpos,Ypos-2,0xffff);
//   LCD_DrawLine(Xpos,Ypos+2,Xpos,Ypos+15,0xffff);
//   
//   LCD_DrawLine(Xpos-15,Ypos+15,Xpos-7,Ypos+15,RGB565CONVERT(184,158,131));
//   LCD_DrawLine(Xpos-15,Ypos+7,Xpos-15,Ypos+15,RGB565CONVERT(184,158,131));

//   LCD_DrawLine(Xpos-15,Ypos-15,Xpos-7,Ypos-15,RGB565CONVERT(184,158,131));
//   LCD_DrawLine(Xpos-15,Ypos-7,Xpos-15,Ypos-15,RGB565CONVERT(184,158,131));

//   LCD_DrawLine(Xpos+7,Ypos+15,Xpos+15,Ypos+15,RGB565CONVERT(184,158,131));
//   LCD_DrawLine(Xpos+15,Ypos+7,Xpos+15,Ypos+15,RGB565CONVERT(184,158,131));

//   LCD_DrawLine(Xpos+7,Ypos-15,Xpos+15,Ypos-15,RGB565CONVERT(184,158,131));
//   LCD_DrawLine(Xpos+15,Ypos-15,Xpos+15,Ypos-7,RGB565CONVERT(184,158,131));	  	
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
  static Coordinate  screen;
  int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
  uint8_t count=0;
  int buffer[2][9]={{0},{0}};
  
  do
  {		   
    TP_GetAdXY(TP_X,TP_Y);  
		buffer[0][count]=TP_X[0];  
		buffer[1][count]=TP_Y[0];
		count++;  
  }
  while(!TP_INT_IN&& count<9);  /* TP_INT_IN  */
  if(count==9)   /* Average X Y  */ 
  {
	/* Average X  */
		temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
		temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
		temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;

		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];

		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);

		if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;

		if(m0<m1)
		{
			if(m2<m0) 
				screen.x=(temp[0]+temp[2])/2;
			else 
				screen.x=(temp[0]+temp[1])/2;	
		}
		else if(m2<m1) 
			screen.x=(temp[0]+temp[2])/2;
		else 
			screen.x=(temp[1]+temp[2])/2;

		/* Average Y  */
		temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
		temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
		temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
		m0=temp[0]-temp[1];
		m1=temp[1]-temp[2];
		m2=temp[2]-temp[0];
		m0=m0>0?m0:(-m0);
		m1=m1>0?m1:(-m1);
		m2=m2>0?m2:(-m2);
		if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD) return 0;

		if(m0<m1)
		{
			if(m2<m0) 
				screen.y=(temp[0]+temp[2])/2;
			else 
				screen.y=(temp[0]+temp[1])/2;	
			}
		else if(m2<m1) 
			 screen.y=(temp[0]+temp[2])/2;
		else
			 screen.y=(temp[1]+temp[2])/2;

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
  /* K＝(X0－X2) (Y1－Y2)－(X1－X2) (Y0－Y2) */
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  if( matrixPtr->Divider == 0 )
  {
    retTHRESHOLD = DISABLE;
  }
  else
  {
    /* A＝((XD0－XD2) (Y1－Y2)－(XD1－XD2) (Y0－Y2))／K	*/
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
	/* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
    /* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
    /* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
    /* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
  }
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
                    ) / matrixPtr->Divider ;
	/* YD = DX+EY+F */        
    displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                      (matrixPtr->En * screenPtr->y) + 
                       matrixPtr->Fn 
                    ) / matrixPtr->Divider ;
  }
  else
  {
    retTHRESHOLD = DISABLE;
  }
  return(retTHRESHOLD);
} 

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay Time
* Input          : - cnt: Delay Time
* Output         : None
* Return         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void delay_ms(uint16_t ms)    
{ 
	uint16_t i,j; 
	for( i = 0; i < ms; i++ )
	{ 
		for( j = 0; j < 0xffff; j++ );
	}
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
  uint8_t i;
  Coordinate * Ptr;

  for(i=0;i<3;i++)
  {
	  LCD_SetFont(&Font12x12);
		LCD_Clear(LCD_COLOR_WHITE);
		LCD_SetTextColor(LCD_COLOR_BLACK); 
		LCD_DisplayStringLine(LINE(1), (uint8_t*)"Touch crosshair to calibrate");
		delay_ms(50);
		DrawCross(DisplaySample[i].x,DisplaySample[i].y);
		do
		{
			Ptr=Read_Ads7846();		
		} while( Ptr == (void*)0 );
		
		ScreenSample[i].x= Ptr->x; ScreenSample[i].y= Ptr->y;		
	}
	
   setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix );
   LCD_Clear(LCD_COLOR_BLACK);
} 

void ExecTouch(void)
{
	GUI_PID_STATE State;
//	uint8_t hasTouchUpdate;

//	hasTouchUpdate = mCalculate_light_touch();	
	
//	if(hasTouchUpdate)
	if(mADS7843())
	{
//		hasTouchUpdate = 1;
		
//		USART_SendData(USART1, mdisplay->x >> 8);
		
//		printf("x axis is: %x", mdisplay->x);
		
//		mADS7843();
		
		_PenIsDown = 1;		
		GUI_TOUCH_Exec();
		
		GUI_TOUCH_StoreState(xScreen + 10, yScreen + 10);
	}	
	else if(_PenIsDown)
	{
		_PenIsDown = 0;
		GUI_PID_GetState(&State);
		State.Pressed = 0;
		GUI_PID_StoreState(&State);
	}
}



int GUI_TOUCH_X_MeasureX(void)
{
//	USART_SendData(USART1, mdisplay->x);
	return xScreen;
}

int GUI_TOUCH_X_MeasureY(void)
{
	return yScreen;
}

void GUI_TOUCH_X_ActivateX(void)
{}

void GUI_TOUCH_X_ActivateY(void) 
{}
	
void _InitTouch(void)
{
	u32 TouchOrientation;
//	int xdata, ydata;
//	int txScreen[2], tyScreen[2];
	
	//Calibrate touch
	
	TouchOrientation = (GUI_MIRROR_X * LCD_GetMirrorXEx(0)) | (GUI_MIRROR_Y * LCD_GetMirrorYEx(0)) | (GUI_SWAP_XY * LCD_GetSwapXYEx(0));
	GUI_TOUCH_SetOrientation(TouchOrientation);
	
//	//校准X轴
//	LCD_SetFont(&Font12x12);
//	LCD_Clear(LCD_COLOR_WHITE);
//	LCD_SetTextColor(LCD_COLOR_BLACK); 
//	LCD_DisplayStringLine(LINE(1), (uint8_t*)"Touch left top to calibrate");
//	delay_ms(50);
//	
//	//第一点校正点 
////	DrawCross(DisplaySample[0].x, DisplaySample[0].y);
//	while(TP_INT_IN)
//	{
//		delay_ms(20);
//	}
//	
//	xdata = Read_ADS7846(CHX);
//	ydata = Read_ADS7846(CHY);

////	txScreen[0] = 0x0fff - ydata;
//	txScreen[0] = ydata;
//  tyScreen[0] = xdata;	
//	
//	
////	TP_GetAdXY(&xdata, &ydata);		
////	txScreen[0] = _AD2X(ydata);
////  tyScreen[0] = _AD2Y(xdata);
//	
//	LCD_Clear(LCD_COLOR_WHITE);
//	LCD_DisplayStringLine(LINE(1), (uint8_t*)"Touch right bottom to calibrate");
//	delay_ms(200);

//	
//	//第二点校正点 
////	DrawCross(DisplaySample[1].x, DisplaySample[1].y);
//	while(TP_INT_IN)
//	{
//		delay_ms(20);
//	}

//	xdata = Read_ADS7846(CHX);
//	ydata = Read_ADS7846(CHY);	
////	TP_GetAdXY(&xdata, &ydata);		
////	txScreen[1] = 0x0fff - ydata;
//	txScreen[1] = ydata;
//  tyScreen[1] = xdata;	
	
//	GUI_TOUCH_Calibrate(GUI_COORD_X, DisplaySample[0].x, DisplaySample[1].x, txScreen[0] - 80, txScreen[1] + 80);	
//	GUI_TOUCH_Calibrate(GUI_COORD_Y, DisplaySample[0].y, DisplaySample[1].y, tyScreen[0] - 100, tyScreen[1] + 100);	
	
	GUI_TOUCH_Calibrate(GUI_COORD_X, 0, 480, 0, 3600);	
	GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, 272, 0, 3400);	
}

uint8_t mADS7843(void)
{
//	int xdata, ydata;
	int mxpos, mypos, mz1pos, mz2pos, mft;
	unsigned char t = 0, t1, count = 0;
	unsigned short int dataxbuffer[SAMPLE_TIMES];	 	//数据组
	unsigned short int dataybuffer[SAMPLE_TIMES];		//数据组
	unsigned short temp = 0;

//	TP_GetAdXY(&mxpos, &mypos);	
	
	//获取多次x, y轴位置
	while(!TP_INT_IN && count < SAMPLE_TIMES)
	{
		dataxbuffer[count] = Read_ADS7846(CHX);
		dataybuffer[count] = Read_ADS7846(CHY);
		count++;
	}
	
	if(count == SAMPLE_TIMES)
	{
		do			//将数据升序排列
		{	
			t1=0;		  
			for(t=0; t < count-1; t++)
			{
				//将数据升序排列
				if(dataxbuffer[t] > dataxbuffer[t+1])	//升序排列
				{
					temp = dataxbuffer[t+1];
					dataxbuffer[t+1] = dataxbuffer[t];
					dataxbuffer[t] = temp;
					t1=1; 
				} 

				if(dataybuffer[t] > dataybuffer[t+1])	//升序排列
				{
					temp = dataybuffer[t+1];
					dataybuffer[t+1] = dataybuffer[t];
					dataybuffer[t] = temp;
					t1=1; 
				}				
			}
		}while(t1); 
		
		mxpos = (dataxbuffer[(SAMPLE_TIMES >> 1) - 2] + dataxbuffer[(SAMPLE_TIMES >> 1) - 1] + dataxbuffer[SAMPLE_TIMES >> 1] + dataxbuffer[(SAMPLE_TIMES >> 1) + 1]) >> 2;
		mypos = (dataybuffer[(SAMPLE_TIMES >> 1) - 2] + dataybuffer[(SAMPLE_TIMES >> 1) - 1] + dataybuffer[SAMPLE_TIMES >> 1] + dataybuffer[(SAMPLE_TIMES >> 1) + 1]) >> 2;
	}
	
	else
		return 0;
		
//	mxpos = Read_ADS7846(CHX);			//获取十次x轴位置
//	mypos = Read_ADS7846(CHY);			//获取十次y轴位置
	mz1pos = Read_ADS7846(CHZ1);			//获取Z1轴位置
	mz2pos = Read_ADS7846(CHZ2);			//获取Z2轴位置

	//计算当前按压压力值，绝对值越小代表按下越可靠，采样值有效，反之无效
	mft = ((mxpos * mz2pos) / mz1pos) - mxpos;
	
	printf("the xdata is: %d, the ydata is: %d, mz1pos is: %d, mz2pos is: %d, mft is: %d\n", mxpos, mypos, mz1pos, mz2pos, mft);
	
	if(!((mft < PRESS_THRESHOLD) && (mft > -PRESS_THRESHOLD)))
	{
		return 0;
	}
	
	xScreen = _AD2X(mxpos);
  yScreen = _AD2Y(mypos);
	
//	printf("the xdata is: %d, the ydata is: %d, TP_INT is %d\n", xScreen, yScreen, TP_INT_IN);

	if(!TP_INT_IN)
	{
		return 1;
	}		
	else 
	{
		return 0;
	}
}

int _AD2Y(int adx) //272
{
  int sx = 0;
  int r = adx - 200;	
  r *= 272;
  sx = 272 - (r / 3400);
//	sx = (r / 4096);

  if (sx <= -10 || sx > 272)
    return 0;
  return sx;
}


int _AD2X(int ady) //480
{
  int sy = 0;
  int r = ady - 300;
  r *= 480;
  sy = r / 3600;
	
  if (sy <= -10 || sy > 480)
    return 0;
  return sy;
}

//判断当前按压是否有效
//返回1表明按压有效，0则无效
uint8_t mCalculate_light_touch(void)
{
	int mxpos, mz1pos, mz2pos, mft;
	
	mxpos = Read_ADS7846(CHX);			//获取x轴位置
	mz1pos = Read_ADS7846(CHZ1);			//获取Z1轴位置
	mz2pos = Read_ADS7846(CHZ2);			//获取Z2轴位置
	
	//计算当前按压压力值，绝对值越小代表按下越可靠，采样值有效，反之无效
	mft = mxpos * mz2pos / mz1pos - mxpos;
	
//	printf("the press_ft is %d\n", mft);
	
	if((mft < PRESS_THRESHOLD) && (mft > -PRESS_THRESHOLD))
	{
		return 1;
	}
	
	else
		return 0;
}

int Read_ADS7846(uint8_t cmd_reg)  
{  
  int i; 
  TP_CS(0); 
  DelayUS(1); 
  WR_CMD(cmd_reg); 
  DelayUS(1); 
  i = RD_AD(); 
  TP_CS(1); 
  return i;    
} 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
