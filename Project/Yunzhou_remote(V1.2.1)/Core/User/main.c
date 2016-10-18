/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "timers.h "
#include <stddef.h>
#include "TouchPanel/TouchPanel.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "YunzhouDLG.c"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Background_Task_PRIO    ( tskIDLE_PRIORITY  + 10 )
#define Background_Task_STACK   ( 512 )

#define Demo_Task_PRIO          ( tskIDLE_PRIORITY  + 9 )
#define Demo_Task_STACK         ( 3048 )

/* Private macro -------------------------------------------------------------*/
/* Private const -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
xTaskHandle                   Task_Handle;
xTaskHandle                   Demo_Handle;
xTimerHandle                  TouchScreenTimer;

//GPS绝对速度仪表盘指针
static GUI_POINT aPointArrow[] = {
{ 0, 0},
{ 2, 0},
{ 2, -30},
{ 0, -45},
{ -2, -30},
{ -2, 0},
};
GUI_POINT aEnlargedPoints[GUI_COUNTOF(aPointArrow)];

//水流相对速度仪表盘指针
static GUI_POINT difSpPointArrow[] = {
{ 0, 0},
{ 1, 0},
{ 1, -30},
{ 0, -45},
{ -1, -30},
{ -1, 0},
};
GUI_POINT difSpEnlargedPoints[GUI_COUNTOF(difSpPointArrow)];

//船内水温仪表盘指针
static GUI_POINT tempIPointArrow[] = {
{ 0, 0},
{ 1, 0},
{ 1, -12},
{ 0, -20},
{ -1, -12},
{ -1, 0},
};
GUI_POINT tempIEnlargedPoints[GUI_COUNTOF(tempIPointArrow)];

//船外水温仪表盘指针
static GUI_POINT tempOPointArrow[] = {
{ 0, 0},
{ 1, 0},
{ 1, -12},
{ 0, -20},
{ -1, -12},
{ -1, 0},
};
GUI_POINT tempOEnlargedPoints[GUI_COUNTOF(tempOPointArrow)];

float mtheta = 0;								//GPS绝对速度指针转角
float mdelta = 0.031;
float difSp_theta = 0;					//水流相对速度指针转角
float difSp_delta = 0.031;
float inner_temp_theta = 0;					//船内温度
float inner_temp_delta = 0.031;
float outer_temp_theta = 0;					//船外温度
float outer_temp_delta = 0.031;

uint8_t USV_battery = 100;			//船体电量百分比
uint8_t Rem_battery = 100;			//遥控器电量百分比
uint8_t battery_delta = 1;			//电量展示DEMO变化量

uint8_t sample_bottle1 = 100;			//采样瓶1百分比
uint8_t sample_bottle2 = 100;
uint8_t sample_bottle3 = 100;
uint8_t sample_bottle4 = 100;

//uint32_t demo_mode = 0;

extern WM_HWIN  ALARM_hWin;

/* Private function prototypes -----------------------------------------------*/
static void Background_Task(void * pvParameters);
static void Demo_Task(void * pvParameters);
//static void vTimerCallback( xTimerHandle pxTimer );
extern void DEMO_Starup(void);
extern void DEMO_MainMenu(void);
extern void ALARM_BackgroundProcess (void);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);

/* Private functions ---------------------------------------------------------*/
static void cbMeterPointWin(WM_MESSAGE* pMsg) {
	switch (pMsg->MsgId) {
		case WM_PAINT:
//			GUI_SetBkColor(GUI_TRANSPARENT);
//			GUI_Clear();
		
//			GUI_DrawGradientH(0, 0, 30, 30, 0x0000FF, 0x00FFFF);

			printf("window notice: WM_PAINT. \n");
		
			break;
			
//		case WM_TIMER:

//		
////			printf("window timer activates. \n");
//			
//			WM_RestartTimer(pMsg->Data.v, 100);
//			break;
		
		default:
			WM_DefaultProc(pMsg);
	}
}

/**3

  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

void mCOMInit(void)
{
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

/* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable UART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(USART1, &USART_InitStructure);
    
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}

void mdelay(uint32_t timer)
{
	while(timer--);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */ 
int main(void)
{ 
//   GPIO_InitTypeDef GPIO_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	 
	/* Setup SysTick Timer for 1 msec interrupts.*/
	if (SysTick_Config(SystemCoreClock / 1000))
	{ 
	 /* Capture error */ 
	 while (1);
	}
   
	mCOMInit();
  
  /* Create background task */
  xTaskCreate(Background_Task,
              (signed char const*)"BK_GND",
              Background_Task_STACK,
              NULL,
              Background_Task_PRIO,
              &Task_Handle);
							
//  /* Create demo task */
  xTaskCreate(Demo_Task,
              (signed char const*)"GUI_DEMO",
              Demo_Task_STACK,
              NULL,
              Demo_Task_PRIO,
              &Demo_Handle);

//   LowLevel_Init();
//   
//   /* Init the STemWin GUI Library */
//   GUI_Init();
//   GUI_DispStringAt("Hello world!", 0, 0);  	
//   GUI_SetBkColor(GUI_TRANSPARENT);
//   GUI_SelectLayer(1);
//   GUI_Clear();
//   GUI_SetBkColor(GUI_TRANSPARENT); 
//   GUI_SelectLayer(0);	
	

  /* Start the FreeRTOS scheduler */
  vTaskStartScheduler();
}


/**
  * @brief  Background task
  * @param  pvParameters not used
  * @retval None
  */
static void Background_Task(void * pvParameters)
{   
  GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev;
	uint8_t pointi = 0, tempi = 0, difSp_active = 0;
	uint16_t pointi2 = 0;
	char tempChar[20];
	uint32_t colorTemp;				//暂存颜色
	uint8_t wait_send = 0;
	
	/* Initialize the BSP layer */
  LowLevel_Init();
	
  /* Init the STemWin GUI Library */
  GUI_Init(); 
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);
//	GUI_EnableAlpha(1);	
	
	GUI_RotatePolygon(aPointArrow, aPointArrow, GUI_COUNTOF(aPointArrow), 2);
	GUI_RotatePolygon(difSpPointArrow, difSpPointArrow, GUI_COUNTOF(difSpPointArrow), 2);
	GUI_RotatePolygon(tempIPointArrow, tempIPointArrow, GUI_COUNTOF(tempIPointArrow), 2);
	GUI_RotatePolygon(tempOPointArrow, tempOPointArrow, GUI_COUNTOF(tempOPointArrow), -2);
	
	mtheta = 0;
	difSp_theta = 0;
	difSp_delta = 0.031;

//	WM_SelectWindow(mFloatP);		
//	GUI_SelectLayer(1);	
//	GUI_RotatePolygon(difSpEnlargedPoints, difSpPointArrow, GUI_COUNTOF(difSpPointArrow), difSp_theta);
//	GUI_SetColor(GUI_BLUE);
//	GUI_FillPolygon (difSpEnlargedPoints, GUI_COUNTOF(difSpPointArrow), 64, 62);	

	GUI_BMP_Draw(_acImage_0, 358, 226);				//Yunzhou logo
	GUI_BMP_Draw(_acImage_1, 382, 0);					//GPS Rader
	GUI_BMP_Draw(_acImage_2, 6, 201);					//Boat picture
	
//	GUI_BMP_Draw(_acImage_3, 78, 200);				//Status board
	GUI_SetFont(&GUI_Font13_ASCII);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("Status:", 12 + 20, 176 + 7);
	
	GUI_BMP_Draw(_acImage_4, 13, 16 + 10);					//Temperature left board
	GUI_DispStringAt("100", 12, 10 + 10);
	GUI_DispStringAt("-40", 33, 83 + 10);
	GUI_DispStringAt("I", 33 + 20, 83 + 10 - 11);

	GUI_BMP_Draw(_acImage_5, 84, 16 + 10);					//Temperature right board 
	GUI_DispStringAt("100", 97, 10 + 10);	
	GUI_DispStringAt("-40", 78, 83 + 10);
	GUI_DispStringAt("W", 78 - 10, 83 + 10 - 11);
	
	GUI_BMP_Draw(_acImage_6, 52, 20);					//Temperature logo
	
	GUI_BMP_Draw(_acImage_7, 179, 11);				//USV Banner
	GUI_SelectLayer(1);
	GUI_SetFont(&GUI_Font20_ASCII);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_SetColor(GUI_YELLOW);
	GUI_DispStringAt("ESM 30", 241, 25);
	
	GUI_SelectLayer(0);	
	GUI_BMP_Draw(_acImage_8, 120, 49);				//Dashboard 
	//MPH单位
	GUI_SetFont(&GUI_Font13_ASCII);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_SetColor(GUI_YELLOW);	
	GUI_DispStringAt("MPH", 242, 136);	
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_SetColor(GUI_RED);	
	GUI_DispStringAt("G", 242 - 20, 136);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_SetColor(GUI_BLUE);	
	GUI_DispStringAt("W", 242 + 20, 136);	
	
	//刻度
	GUI_SetColor(GUI_WHITE);	
	GUI_DispStringAt("0", 205, 128);
	GUI_DispStringAt("4", 204, 103);
	GUI_DispStringAt("8", 222, 83);
	GUI_DispStringAt("12", 242, 79);
	GUI_SetColor(GUI_RED);	
	GUI_DispStringAt("16", 261, 92);
	GUI_DispStringAt("20", 270, 116);
  
	GUI_BMP_Draw(_acImage_9, 40, 115);				//Battery Banner1
	GUI_BMP_Draw(_acImage_10, 40, 145);				//Battery Banner2
	GUI_SetFont(&GUI_Font13_ASCII);
	GUI_SetBkColor(GUI_TRANSPARENT);
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_SetColor(GUI_RED);
	GUI_DispStringAt("Power:", 12, 105);	
	GUI_DispStringAt("USV:", 12, 124);
	GUI_DispStringAt("REM:", 12, 154);
	
	GUI_BMP_Draw(_acImage_11, 377, 112);				//Sample bottle
	GUI_DispStringAt("Sample volume", 372, 105);	
	
	GUI_BMP_Draw(_acImage_12, 364, 176);				//Alarm lamb
	GUI_BMP_Draw(_acImage_13, 438, 178);				//Nuclear
	GUI_BMP_Draw(_acImage_14, 400, 176);				//Light
	GUI_BMP_Draw(_acImage_15, 198, 228);				//Menu bar
	
	GUI_SelectLayer(1);	
	bg1_memdev = GUI_MEMDEV_Create(0, 0, 90, 90);
	bg1w_memdev = GUI_MEMDEV_Create(0, 0, 39, 43);
//	bg1w_memdev = GUI_MEMDEV_CreateFixed(0, 0, 35, 43, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
	GUI_SelectLayer(0);	
	bg0_memdev = GUI_MEMDEV_Create(0, 0, 90, 90);
	bg0s_memdev = GUI_MEMDEV_Create(0, 0, 58, 14);
//	bg1_memdev = GUI_MEMDEV_CreateFixed(0, 0, 90, 90, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
	
  /* Run the background task */
  while (1)
  {			
		pointi++;
		
		if(pointi >= 10)
		{
			pointi = 0;
			
			//模拟显示读数修改
			if(mtheta > -0.03)
				mdelta = -mdelta;
			
			else if(mtheta <= -4)
				mdelta = -mdelta;	
			
			mtheta += mdelta;
			
			if(mtheta < -2)
				difSp_active = 1;			
			
			if(difSp_active == 1)
			{			
				if(difSp_theta > -0.03)
					difSp_delta = -difSp_delta;
				
				else if(difSp_theta <= -4)
					difSp_delta = -difSp_delta;	

				difSp_theta = difSp_theta + difSp_delta;	
			}
			
			GUI_SelectLayer(1);
			//在存储设备写入新内容前清空存储区
			GUI_MEMDEV_Select(bg1_memdev);
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 90, 90);
			
			//旋转GPS船速指针
			GUI_RotatePolygon(aEnlargedPoints, aPointArrow, GUI_COUNTOF(aPointArrow), mtheta);
			GUI_SetColor(GUI_RED);
			GUI_FillPolygon (aEnlargedPoints, GUI_COUNTOF(aPointArrow), 45, 45);
			
			//旋转水流相对船速指针
			GUI_RotatePolygon(difSpEnlargedPoints, difSpPointArrow, GUI_COUNTOF(difSpPointArrow), difSp_theta);
			GUI_SetColor(GUI_BLUE);
			GUI_FillPolygon (difSpEnlargedPoints, GUI_COUNTOF(difSpPointArrow), 45, 45);			

			//中心圆点
			for(tempi = 1; tempi < 6; tempi++)
			{
				GUI_SetColor(GUI_YELLOW / tempi);
				GUI_DrawCircle(45, 45, tempi);
			}	
						
			//显示GPS船速数值
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_SetColor(GUI_RED);
			float_to_string(-mtheta * 22 / 4, tempChar, 1, 0, 0, 0);
			GUI_DispStringAt(tempChar, 30, 79);
			
			//显示水流相对船速数值
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_SetColor(GUI_BLUE);
			float_to_string(-difSp_theta * 22 / 4, tempChar, 1, 0, 0, 0);
			GUI_DispStringAt(tempChar, 60, 79);

			//将存储设备的暂存显存写入LCD缓冲区
			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 197, 70);
			
			//电池电量展示-------------------------------------------------------------
			//在存储设备写入新内容前清空存储区
			GUI_MEMDEV_Select(bg1_memdev);
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 90, 90);

			if(USV_battery >= 100)
			{
				battery_delta = -battery_delta;
			}
			else if(USV_battery == 0)
			{
				battery_delta = -battery_delta;
			}
			
			USV_battery += battery_delta;
			Rem_battery += battery_delta;
			
			if(USV_battery >= 40)
				colorTemp = GUI_GREEN;
			else if(USV_battery >= 20)
				colorTemp = GUI_ORANGE;
			else 
				colorTemp = GUI_RED;			
			
			GUI_SelectLayer(0);
			GUI_MEMDEV_Select(bg0_memdev);
			GUI_MEMDEV_Clear(bg0_memdev);			
			//无人船电量条
			GUI_DrawGradientV(1, 1, 45 * USV_battery / 100, 9, GUI_GRAY, colorTemp);
			GUI_DrawGradientV(1, 10, 45 * USV_battery / 100, 18, colorTemp, GUI_GRAY);
			GUI_DrawGradientV(1 + 45 * USV_battery / 100, 1, 45, 9, GUI_GRAY, GUI_WHITE);
			GUI_DrawGradientV(1 + 45 * USV_battery / 100, 10, 45, 18, GUI_WHITE, GUI_GRAY);	
			
			//遥控器电量条
			GUI_DrawGradientV(1, 32, 45 * Rem_battery / 100, 40, GUI_GRAY, colorTemp);
			GUI_DrawGradientV(1, 41, 45 * Rem_battery / 100, 49, colorTemp, GUI_GRAY);
			GUI_DrawGradientV(1 + 45 * Rem_battery / 100, 32, 45, 40, GUI_GRAY, GUI_WHITE);
			GUI_DrawGradientV(1 + 45 * Rem_battery / 100, 41, 45, 49, GUI_WHITE, GUI_GRAY);				
			
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 50, 115);			
			
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1_memdev);
			//显示无人船电量									
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(GUI_TRANSPARENT);
			float_to_string(USV_battery, tempChar, 0, 1, 0, 0);
			GUI_DispStringAt(tempChar, 26, 10);	

			//显示遥控器电量
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(GUI_TRANSPARENT);
			float_to_string(Rem_battery, tempChar, 0, 1, 0, 0);
			GUI_DispStringAt(tempChar, 26, 41);				

			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 50, 115);
			
			//采样瓶容量展示-----------------------------------------------------------
			//在存储设备写入新内容前清空存储区
			sample_bottle1 += battery_delta;
			sample_bottle2 += battery_delta;
			sample_bottle3 += battery_delta;
			sample_bottle4 += battery_delta;

			GUI_SelectLayer(0);
			GUI_MEMDEV_Select(bg0_memdev);	
			GUI_MEMDEV_Clear(bg0_memdev);					
			//1号瓶采样容量
			GUI_DrawGradientH(1, 0, 8, 31 - 31 * sample_bottle1 / 100, 0x00b0b0b0, GUI_WHITE);
			GUI_DrawGradientH(9, 0, 17, 31 - 31 * sample_bottle1 / 100, GUI_WHITE, 0x00b0b0b0);
			GUI_DrawGradientH(1, 31 - 31 * sample_bottle1 / 100, 8, 31, 0x00b0b0b0, GUI_BLUE);
			GUI_DrawGradientH(9, 31 - 31 * sample_bottle1 / 100, 17, 31, GUI_BLUE, 0x00b0b0b0);
			
			//2号瓶采样容量
			GUI_DrawGradientH(1 + 21, 0, 8 + 21, 31 - 31 * sample_bottle2 / 100, 0x00b0b0b0, GUI_WHITE);
			GUI_DrawGradientH(9 + 21, 0, 17 + 21, 31 - 31 * sample_bottle2 / 100, GUI_WHITE, 0x00b0b0b0);
			GUI_DrawGradientH(1 + 21, 31 - 31 * sample_bottle2 / 100, 8 + 21, 31, 0x00b0b0b0, GUI_BLUE);
			GUI_DrawGradientH(9 + 21, 31 - 31 * sample_bottle2 / 100, 17 + 21, 31, GUI_BLUE, 0x00b0b0b0);		

			//3号瓶采样容量
			GUI_DrawGradientH(1 + 42, 0, 8 + 42, 31 - 31 * sample_bottle2 / 100, 0x00b0b0b0, GUI_WHITE);
			GUI_DrawGradientH(9 + 42, 0, 17 + 42, 31 - 31 * sample_bottle2 / 100, GUI_WHITE, 0x00b0b0b0);
			GUI_DrawGradientH(1 + 42, 31 - 31 * sample_bottle2 / 100, 8 + 42, 31, 0x00b0b0b0, GUI_BLUE);
			GUI_DrawGradientH(9 + 42, 31 - 31 * sample_bottle2 / 100, 17 + 42, 31, GUI_BLUE, 0x00b0b0b0);	
			
			//4号瓶采样容量
			GUI_DrawGradientH(1 + 63, 0, 8 + 63, 31 - 31 * sample_bottle2 / 100, 0x00b0b0b0, GUI_WHITE);
			GUI_DrawGradientH(9 + 63, 0, 17 + 63, 31 - 31 * sample_bottle2 / 100, GUI_WHITE, 0x00b0b0b0);
			GUI_DrawGradientH(1 + 63, 31 - 31 * sample_bottle2 / 100, 8 + 63, 31, 0x00b0b0b0, GUI_BLUE);
			GUI_DrawGradientH(9 + 63, 31 - 31 * sample_bottle2 / 100, 17 + 63, 31, GUI_BLUE, 0x00b0b0b0);	
			
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 378, 127);
			
			//1号瓶数值
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1_memdev);	
			GUI_MEMDEV_Clear(bg1_memdev);	
			GUI_SetFont(&GUI_Font8_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_SetColor(GUI_BLACK);
			GUI_SetBkColor(GUI_TRANSPARENT);
			GUI_DispStringAt("1#", 10, 22);	
			float_to_string(sample_bottle1 * 4.0 / 100, tempChar, 1, 0, 0, 0);
			GUI_DispStringAt(tempChar, 3, 8);	

			//2号瓶数值
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_DispStringAt("2#", 10 + 21, 22);	
			float_to_string(sample_bottle2 * 4.0 / 100, tempChar, 1, 0, 0, 0);
			GUI_DispStringAt(tempChar, 3 + 21, 8);	
			
			//3号瓶数值
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_DispStringAt("3#", 10 + 42, 22);	
			float_to_string(sample_bottle3 * 4.0 / 100, tempChar, 1, 0, 0, 0);
			GUI_DispStringAt(tempChar, 3 + 42, 8);	
			
			//4号瓶数值
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_DispStringAt("4#", 10 + 63, 22);	
			float_to_string(sample_bottle4 * 4.0 / 100, tempChar, 1, 0, 0, 0);
			GUI_DispStringAt(tempChar, 3 + 63, 8);	
			
			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 378, 127);
			
			//显示船体状态信息---------------------------------------------------
			GUI_SelectLayer(0);
			GUI_MEMDEV_Select(bg0s_memdev);	
			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
			GUI_SetColor(GUI_RED);
			GUI_SetBkColor(GUI_TRANSPARENT);
			GUI_DispStringAt("Not connect", 2, 2);	
			
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 24 + 2, 188);
			
			//显示左下方状态信息--------------------------------------------------
			GUI_SelectLayer(0);
			GUI_MEMDEV_Select(bg0_memdev);	
			GUI_MEMDEV_Clear(bg0_memdev);	
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
			GUI_SetColor(GUI_RED);
			GUI_SetBkColor(GUI_TRANSPARENT);
			GUI_DispStringAt("LM: 0%", 21, 0);	
			GUI_DispStringAt("CH: NC", 21, 0 + 15);
			GUI_DispStringAt("GPSR:", 21, 50);
			GUI_SetFont(&GUI_Font8_ASCII);
			GUI_DispStringAt("N: 23.266", 21 + 8, 50 + 12 + 4);
			GUI_DispStringAt("E: 120.688", 21 + 8, 50 + 24 + 4);
			
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 104, 170);
			
			//显示右下方状态信息-------------------------------------------------
			GUI_SelectLayer(0);
			GUI_MEMDEV_Select(bg0s_memdev);	
			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
			GUI_SetColor(GUI_RED);
			GUI_SetBkColor(GUI_TRANSPARENT);
			GUI_DispStringAt("RM: 0%", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 299, 170 + 0);
			
			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_DispStringAt("MCL: 100%", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 299, 185 + 0);	
			
			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_DispStringAt("GPSU:", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 299, 185 + 30 + 3);
			
			GUI_MEMDEV_Clear(bg0s_memdev);
			GUI_SetFont(&GUI_Font8_ASCII);
			GUI_DispStringAt("N: 23.268", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 299 + 6, 185 + 30 + 12 + 7);
			
			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_DispStringAt("E: 120.568", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 299 + 6, 185 + 30 + 24 + 7);
			
			//显示遥控控制加速度数值----------------------------------------------
			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_SetFont(&GUI_Font13_ASCII);
			GUI_DispStringAt("LC: 0%", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 115 + 10, 48);

			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_DispStringAt("RC: 0%", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 299 + 10, 48);		

			//显示双GPS间距离------------------------------------------------------
			GUI_MEMDEV_Clear(bg0s_memdev);	
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
			GUI_DispStringAt("0m", 2, 2);				
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 362 + 10, 13);			
			
			//显示船内外温度信息---------------------------------------------------
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1_memdev);	
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 90, 90);

			//旋转船内温度指针
			if(inner_temp_theta > 0.8)
				inner_temp_delta = -inner_temp_delta;
			
			else if(inner_temp_theta <= -1.6)
				inner_temp_delta = -inner_temp_delta;	
			
			inner_temp_theta += inner_temp_delta;	

			GUI_RotatePolygon(tempIEnlargedPoints, tempIPointArrow, GUI_COUNTOF(tempIPointArrow), inner_temp_theta);
			GUI_SetColor(GUI_RED);
			GUI_FillPolygon (tempIEnlargedPoints, GUI_COUNTOF(tempIPointArrow), 31, 57);		

			//旋转船外温度指针
			if(outer_temp_theta > 1.6)
				outer_temp_delta = -outer_temp_delta;
			
			else if(outer_temp_theta <= -0.8)
				outer_temp_delta = -outer_temp_delta;	
			
			outer_temp_theta += outer_temp_delta;	

			GUI_RotatePolygon(tempOEnlargedPoints, tempOPointArrow, GUI_COUNTOF(tempOPointArrow), outer_temp_theta);
			GUI_SetColor(GUI_RED);
			GUI_FillPolygon (tempOEnlargedPoints, GUI_COUNTOF(tempOPointArrow), 31 + 30, 57);	

			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 18, 0);
			
			//显示船内外温度读数数值
			GUI_SelectLayer(0);
			GUI_MEMDEV_Select(bg0s_memdev);	
//			GUI_MEMDEV_Clear(bg0s_memdev);
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(1, 1, 57, 13);			

			//船内温度数值
			GUI_SetFont(&GUI_Font8_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
			
			if(((-inner_temp_theta + 0.8) * 160 / 2.4 - 40) > -10 && ((-inner_temp_theta + 0.8) * 160 / 2.4 - 40) < 60)
				GUI_SetColor(GUI_GREEN);
			else if(((-inner_temp_theta + 0.8) * 160 / 2.4 - 40) >= 60)
				GUI_SetColor(GUI_RED);
			else if(((-inner_temp_theta + 0.8) * 160 / 2.4 - 40) <= -10)
				GUI_SetColor(GUI_BLUE);
			
			GUI_SetBkColor(GUI_TRANSPARENT);			
			float_to_string(((-inner_temp_theta + 0.8) * 160 / 2.4 - 40), tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 10, 1);		

			//船外温度数值
			GUI_SetFont(&GUI_Font8_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
			
			if(((outer_temp_theta + 0.8) * 160 / 2.4 - 40) >= 60)
				GUI_SetColor(GUI_RED);
			else if(((outer_temp_theta + 0.8) * 160 / 2.4 - 40) <= -10)
				GUI_SetColor(GUI_BLUE);
			else
				GUI_SetColor(GUI_GREEN);
			
			float_to_string(((outer_temp_theta + 0.8) * 160 / 2.4 - 40), tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 33, 1);				
			
			GUI_MEMDEV_CopyToLCDAt(bg0s_memdev, 42, 62);
			
			//模式，菜单及推进器方向-------------------------------------------------------------
			//在存储设备写入新内容前清空存储区
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1_memdev);
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 90, 90);
			
			GUI_SetFont(&GUI_Font20B_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_SetColor(GUI_RED);
			GUI_SetBkColor(GUI_TRANSPARENT);
			GUI_DispStringAt("Dual", 45, 50);	

			GUI_SetFont(&GUI_Font13B_ASCII);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
			GUI_SetColor(GUI_RED);
			GUI_SetBkColor(GUI_TRANSPARENT);
			GUI_DispStringAt("MENU", 50, 76);						
			
			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 197, 167);
		}
		
		//警示图标展示-----------------------------------------------------------------------
		pointi2++;
		
		if((pointi2 > 100) && (pointi2 < 200))
		{
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1w_memdev);	
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(0, 0, 39, 43);	
			GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 361, 173);
//			printf("1\n");
		}
		else if(pointi2 > 200 && pointi2 < 300)
		{
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1w_memdev);	
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(0, 0, 39, 43);	
			GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 398, 173);	
//			printf("2\n");
		}
		else if((pointi2 > 300) && (pointi2 < 400))
		{
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1w_memdev);	
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(0, 0, 39, 43);	
			GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 438, 173);	
//			printf("3\n");
		}
		else if(pointi2 >= 400)
		{
			pointi2 = 0;
			GUI_SelectLayer(1);
			GUI_MEMDEV_Select(bg1w_memdev);	
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 39, 43);
			GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 361, 173);
			GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 398, 173);				
			GUI_MEMDEV_CopyToLCDAt(bg1w_memdev, 438, 173);
//			printf("4\n");
		}
			

		//触摸屏坐标更新
		ExecTouch();
		
		wait_send++;
		
		if(wait_send > 10)
		{
			wait_send = 0;
//			printf("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$\n");
			printf("1234567890\n");
			GPIO_SetBits(GPIOF, GPIO_Pin_6);			//常亮液晶屏
		}	
		
//		GUI_Exec();

//		WM_Exec();
		
//		USART_SendData(USART1, 0x86);
		
    vTaskDelay(10);
  }
}


/**
  * @brief  Demonstration task
  * @param  pvParameters not used
  * @retval None
  */
static void Demo_Task(void * pvParameters)
{  
//   /* Change Skin */
//   PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
//   RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
//   SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
//   SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
//   SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);
//   BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
  
  /* Setup layer configuration during startup */
//  GUI_SetBkColor(GUI_TRANSPARENT);
//  GUI_SelectLayer(1);
//  GUI_Clear();
//  GUI_SetBkColor(GUI_TRANSPARENT); 
//  GUI_SelectLayer(0);
	
	while(1)
	{			
		vTaskDelay(100);
	}
	
//	GUIDEMO_Main();
//   /* Run the Startup frame */
//   DEMO_Starup();
//   
//   /* Show the main menu */
//   DEMO_MainMenu();
}


/**
  * @brief  Error callback function
  * @param  None
  * @retval None
  */
void vApplicationMallocFailedHook( void )
{
  while (1)
  {}
}

/**
  * @brief  float to string
  * @param  the input float data
  * @retval string
  */
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength)
{
  int i = 0, j, k;
  long temp, tempoten;
  u8 intpart[20], dotpart[20]; 

	//处理前缀部分
	for(k = 0; k < prelength; k++)
	{
		str[i++] = prechar[k];
	}

	//处理正负数
  if(data < 0) 
	{
		str[i++] = '-';
		data = -data;
	}
//  else str[0]='+';

	//处理整数部分
  temp = (long) data;

	j = 0;
  tempoten = temp / 10;
  while(tempoten != 0)
  {
    intpart[j] = temp - 10 * tempoten + 48; 	//to ascii code
    temp = tempoten;
    tempoten = temp / 10;
    j++;
  }
  intpart[j++] = temp + 48;

	//处理小数部分
	if(length >= 1)
	{
		data = data - (long) data;  
		for(k = 0; k < length; k++)
		{
			dotpart[k] = (int)(data*10) + 48;
			data = data * 10.0;
			data = data - (long)data;
		}
	}

	for(; j > 0; j--)
	{
		str[i++] = intpart[j - 1];
	}
	
	if(length >= 1)
	{
		str[i++] = '.';
		
		for(k = 0; k < length; k++)
		{
			str[i++] = dotpart[k];
		}	
	}		

	if(unit == 1)
	{
		str[i++] = '%';
	}
	else if(unit == 2)
	{
		str[i++] = 'm';
	}
	
	str[i++] = '\0';
	
	return i;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
