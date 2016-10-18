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
//#include "test_program.h"
#include <stddef.h>
#include "TouchPanel/TouchPanel.h"
#include "GUIDEMO.h"
#include "stm32f4xx.h"
#include <stdio.h>

#include "GUI.h"
#include "songti16.c"
#include <math.h>
#include <stdlib.h>
#include "logo.c"
//#include "logo1.c"
//#include "acautomaticship.c"
//#include "png.h"  
//#include "yunzhou_logo.c"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Background_Task_PRIO    ( tskIDLE_PRIORITY  + 10 )
#define Background_Task_STACK   ( 512 )

#define Demo_Task_PRIO          ( tskIDLE_PRIORITY  + 9 )
#define Demo_Task_STACK         ( 3048 )


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
xTaskHandle                   Task_Handle;
xTaskHandle                   Demo_Handle;
xTimerHandle                  TouchScreenTimer;

//uint32_t demo_mode = 0;

extern WM_HWIN  ALARM_hWin;

/* Private function prototypes -----------------------------------------------*/
static void Background_Task(void * pvParameters);
static void Demo_Task(void * pvParameters);
static void vTimerCallback( xTimerHandle pxTimer );
extern void DEMO_Starup(void);
extern void DEMO_MainMenu(void);
extern void ALARM_BackgroundProcess (void);
static void mMainTask(void * pvParameters);
/* Private functions ---------------------------------------------------------*/

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
  USART_InitStructure.USART_BaudRate = 115200;
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


static const char * _apStrings[] = {
//  "Japanese:",
//  "1 - \xe3\x82\xa8\xe3\x83\xb3\xe3\x82\xb3\xe3\x83\xbc"
//      "\xe3\x83\x87\xe3\x82\xa3\xe3\x83\xb3\xe3\x82\xb0",
//  "2 - \xe3\x83\x86\xe3\x82\xad\xe3\x82\xb9\xe3\x83\x88",
//  "3 - \xe3\x82\xb5\xe3\x83\x9d\xe3\x83\xbc\xe3\x83\x88",
  "English:",
  "1 - encoding",
  "2 - text",
  "3 - support",
	"Chinese:",
	"1 - \xe4\xba\x91\xe6\xb4\xb2\xe6\x99\xba\xe8\x83\xbd"
};

//static const GUI_POINT aPointArrow[] = {
//{ 0, -5},
//{-40, -35},
//{-10, -25},
//{-10, -85},
//{ 10, -85},
//{ 10, -25},
//{ 40, -35},
//};

static const GUI_POINT aPointArrow[] = {
{ 0, 0},
{ 5, 0},
{ 5, -50},
{ 0, -80},
{ -5, -50},
{ -5, 0},
};


void m_MainTask(void) {
  int i;
//  GUI_Init();
//  GUI_SetFont(&GUI_Font16_1HK);
	GUI_SetFont(&GUI_Fontsongti16);
  GUI_UC_SetEncodeUTF8();
  for (i = 0; i < GUI_COUNTOF(_apStrings); i++) {
    GUI_DispString(_apStrings[i]);
    GUI_DispNextLine();
  }
//  while(1) {
//    GUI_Delay(500);
//  }
}

// Draw concentric circles
void ShowCircles(void) {
	int i;
	for (i=10; i<50; i += 3)
	GUI_DrawCircle(320, 60, i);
}

void DrawArcScale(void) {
	int x0 = 160;
	int y0 = 180;
	int i;
	char ac[4];
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_SetPenSize( 4 );
	GUI_SetTextMode(GUI_TM_TRANS);
//	GUI_SetFont(&GUI_FontComic18B_ASCII);
	GUI_SetFont(&GUI_Font8_1);
	GUI_SetColor(GUI_BLACK);
	GUI_DrawArc( x0,y0,120, 120,-30, 210 );
//	GUI_Delay(1000);
	for (i=0; i<= 23; i++) 
	{
		if(i < 14)
			GUI_SetColor(GUI_GREEN);
		else if(i < 18)
			GUI_SetColor(GUI_ORANGE);
		else
			GUI_SetColor(GUI_RED);
		
		float a = (-30+i*10)*3.1415926/180;
		int x = -110*cos(a)+x0;
		int y = -110*sin(a)+y0;
		if (i%2 == 0)
			GUI_SetPenSize( 5 );
		else
			GUI_SetPenSize( 4 );
			GUI_DrawPoint(x,y);
		
		if (i%2 == 0) {
			x = -90*cos(a)+x0;
			y = -90*sin(a)+y0;
			sprintf(ac, "%d", 10*i);
			GUI_SetTextAlign(GUI_TA_VCENTER);
			GUI_DispStringHCenterAt(ac,x,y);
		}
	}
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
//  xTaskCreate(Demo_Task,
//              (signed char const*)"GUI_DEMO",
//              Demo_Task_STACK,
//              NULL,
//              Demo_Task_PRIO,
//              &Demo_Handle);

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

//const GUI_POINT aPoints[] = {
//{ 40, 20},
//{ 0, 20},
//{ 20, 0}
//};

const GUI_POINT aPoints[] = {
	{ 40, 20},
	{ 20, 40},
	{ 0, 20},
	{ 20, 0}
};

const GUI_POINT aPoints2[] = {
	{ 40, 20},
	{ 20, 40},
	{ 0, 20},
	{ 20, 0}
};

GUI_POINT aEnlargedPoints[GUI_COUNTOF(aPointArrow)];

/*******************************************************************
*
* Draws a polygon
*/
static void DrawPolygon(void) {
	int Cnt =0;
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_SetFont(&GUI_Font8x16);
	GUI_SetColor(0x0);
	GUI_DispStringAt("Polygons of arbitrary shape ", 0, 0);
	GUI_DispStringAt("in any color", 120, 20);
	GUI_SetColor(GUI_BLUE);
	/* Draw filled polygon */
	GUI_FillPolygon (&aPointArrow[0],7,100,100);
	GUI_RotatePolygon(aEnlargedPoints, aPointArrow, GUI_COUNTOF(aPointArrow), 1);
	GUI_FillPolygon (aEnlargedPoints, 7, 200, 200);
}

void Sample(void) {
//	int i;
//	GUI_Clear();
//	GUI_SetDrawMode(GUI_DM_XOR);
//	GUI_FillPolygon(aPoints, GUI_COUNTOF(aPoints), 140, 110);
//	for (i = 1; i < 10; i++) {
//		vTaskDelay(200);
//		GUI_EnlargePolygon(aEnlargedPoints, aPoints, GUI_COUNTOF(aPoints), i * 5);
//		GUI_FillPolygon(aEnlargedPoints, GUI_COUNTOF(aPoints), 140, 110);
//	}
	
	int Mag, y = 0, Count = 4;
	GUI_Clear();
	GUI_SetColor(GUI_GREEN);
	for (Mag = 1; Mag <= 4; Mag *= 2, Count /= 2) {
		int i, x = 0;
		GUI_MagnifyPolygon(aEnlargedPoints, aPoints, GUI_COUNTOF(aPoints), Mag);
		for (i = Count; i > 0; i--, x += 40 * Mag) {
			GUI_FillPolygon(aEnlargedPoints, GUI_COUNTOF(aPoints), x, y);
	}
	y += 20 * Mag;
	}
}

I16 aY[100];

/**
  * @brief  Background task
  * @param  pvParameters not used
  * @retval None
  */
static void Background_Task(void * pvParameters)
{
  uint32_t ticks = 0;
//	GUI_ALPHA_STATE AlphaState;
	uint8_t i;
	float mtheta;
	float mdelta;
    
  /* Initialize the BSP layer */
  LowLevel_Init();
	
//  BSP_TS_Init(); 
  /* Init the STemWin GUI Library */
  GUI_Init();
//	WM_SelectWindow(WM_HBKWIN);
//  GUI_Clear();
//  GUI_DispStringAt("Hello world!", 100, 50);  
//	GUI_UC_SetEncodeUTF8();
	
//	GUI_JPEG_Draw(aclogo, sizeof(aclogo), 100, 0);
	
//	GUI_PNG_Draw(aclogo1, sizeof(aclogo1), 10, 10);
//	
//	GUI_GIF_Draw(acacautomaticship, sizeof(acacautomaticship), 100, 100);
//	GUI_GIF_Draw(acyunzhou_logo, sizeof(acyunzhou_logo), 100, 100);		


  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);	
	GUI_CURSOR_Show();
		
//	m_MainTask();
//	
//	vTaskDelay(200);
////	GUI_SetDrawMode(GUI_DRAWMODE_XOR);
//	GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
//	GUI_FillCircle(120, 64, 40);
//	
//	vTaskDelay(200);
////	GUI_CopyRect(0, 0, 100, 100, 100, 100);	
//	GUI_ClearRect(0, 0, 50, 50);
//	
//	vTaskDelay(200);
//	GUI_DrawGradientH(100, 100, 140, 140, 0x0000FF, 0x00FFFF);
//	GUI_DrawGradientV(140, 100, 180, 140, 0x1100FF, 0x00FF33);
//	GUI_DrawGradientRoundedH(180, 100, 250, 140, 25, 0x1100FF, 0x00FF33);
//	GUI_SetColor((0x40uL << 24) | GUI_RED);
//	GUI_DrawRect(260, 100, 300, 140);
//	
////	GUI_SetColor((0x80uL << 24) | GUI_GREEN);
//	GUI_FillRect(310, 100, 330, 140);
	
//	GUI_EnableAlpha(1);
//	GUI_SetBkColor(GUI_WHITE);
//	GUI_Clear();
//	GUI_SetColor(GUI_BLACK);
//	GUI_SetFont(&GUI_Font8_1);
//	GUI_DispStringHCenterAt("Alphablending", 45, 41);
//	GUI_SetColor((0x40uL << 24) | GUI_RED);
//	GUI_FillRect(0, 0, 49, 49);
//	GUI_SetColor((0x80uL << 24) | GUI_GREEN);
//	GUI_FillRect(20, 20, 69, 69);
//	GUI_SetColor((0x20uL << 24) | GUI_BLUE);
//	GUI_FillRect(40, 40, 89, 89);

//	GUI_EnableAlpha(1);
//	GUI_SetBkColor(GUI_WHITE);
//	GUI_Clear();
//	GUI_SetColor(GUI_BLACK);
//	GUI_DispStringHCenterAt("Alphablending", 45, 41);
//	
//	printf("alpha before set: %x\n", AlphaState.UserAlpha);
//	
//	GUI_SetUserAlpha(&AlphaState, 0xC0);
//	
//	printf("alpha after set: %x\n", AlphaState.UserAlpha);
//	GUI_SetColor(GUI_RED);
//	GUI_FillRect(0, 0, 49, 49);
//	GUI_SetColor(GUI_GREEN);
//	GUI_FillRect(20, 20, 69, 69);
//	GUI_SetColor(GUI_BLUE);
//	GUI_FillRect(40, 40, 89, 89);
//	GUI_RestoreUserAlpha(&AlphaState);
//	printf("alpha after restore: %x\n", AlphaState.UserAlpha);

//	GUI_SetColor(GUI_BLUE);
//	GUI_FillCircle(100, 50, 49);
//	GUI_SetColor(GUI_YELLOW);
//	for (i = 0; i < 100; i++) {
//		U8 Alpha;
//		Alpha = (i * 255 / 100);
//		GUI_SetAlpha(Alpha);
//		GUI_DrawHLine(i, 100 - i, 100 + i);
//	}
	
//	GUI_SetColor(GUI_YELLOW);
//	GUI_DrawLine(200, 100, 260, 120);
//	GUI_DrawLineRel(-30, 50);
//	GUI_DrawLineRel(30, 50);
//	GUI_DrawLineRel(30, -50);

//	GUI_SetColor(GUI_YELLOW);
//	GUI_SetLineStyle(GUI_LS_DASH);
//	GUI_DrawLine(300, 130, 260, 220);
//	
//	Sample();
//	
//	GUI_SetDrawMode(GUI_DM_NORMAL);
//	GUI_SetColor(GUI_YELLOW);
//	GUI_SetLineStyle(GUI_LS_SOLID);

//	GUI_DrawHLine(110, 140, 180);

//	GUI_DrawPolygon(aPoints2, GUI_COUNTOF(aPoints2), 300, 200);
//	GUI_DrawPolyLine(aPoints2, GUI_COUNTOF(aPoints2), 400, 200);
//	
//	DrawPolygon();
//	
//	vTaskDelay(1000);
//	ShowCircles();
//	
//	GUI_DrawEllipse(260, 200, 20, 10);
//	
//	vTaskDelay(1000);
//	// Demo ellipses
//	GUI_SetColor(0xff);
//	GUI_FillEllipse(100, 180, 50, 70);
//	GUI_SetColor(0x0);
//	GUI_DrawEllipse(100, 180, 50, 70);
//	GUI_SetColor(0x000000);
//	GUI_FillEllipse(100, 180, 10, 50);
//	
//	vTaskDelay(1000);
	DrawArcScale();
	
	GUI_SetAlpha(0);
	
	GUI_JPEG_Draw(aclogo, sizeof(aclogo), 280, 200);

	GUI_SetColor(GUI_BLUE);
	/* Draw filled polygon */
	GUI_FillPolygon (&aPointArrow[0], GUI_COUNTOF(aPointArrow), 160, 180);
//	vTaskDelay(1000);
	GUI_SetColor(GUI_WHITE);
	GUI_FillPolygon (&aPointArrow[0], GUI_COUNTOF(aPointArrow), 160, 180);
//	vTaskDelay(1000);
	GUI_RotatePolygon(aEnlargedPoints, aPointArrow, GUI_COUNTOF(aPointArrow), 0);
	GUI_SetColor(GUI_BLUE);
	GUI_FillPolygon (aEnlargedPoints, GUI_COUNTOF(aPointArrow), 160, 180);

//	printf("cos(1) = %f\n", cos(1));

	mdelta = 0.031;
//  /* Create demo task */				
//  xTaskCreate(Demo_Task,
//              (signed char const*)"GUI_DEMO",
//              Demo_Task_STACK,
//              NULL,
//              Demo_Task_PRIO,
//              &Demo_Handle);

//  xTaskCreate(mMainTask,
//              (signed char const*)"GUI_DEMO",
//              Demo_Task_STACK,
//              NULL,
//              Demo_Task_PRIO,
//              &Demo_Handle);

//  /* Launch Touchscreen Timer */
//  TouchScreenTimer = xTimerCreate ("Timer", 50, pdTRUE, ( void * ) 1, vTimerCallback );
//  
//  if( TouchScreenTimer != NULL )
//  {
//    if( xTimerStart( TouchScreenTimer, 0 ) != pdPASS )
//    {
//      /* The timer could not be set into the Active state. */
//    }
//  }

	int a0, a1;
	const unsigned aValues[] = { 100, 135, 190, 240, 340, 360};
	const GUI_COLOR aColors[] = { GUI_BLUE, GUI_GREEN, GUI_RED,
	GUI_CYAN, GUI_MAGENTA, GUI_YELLOW };
	for (i = 0; i < GUI_COUNTOF(aValues); i++) {
		a0 = (i == 0) ?0 :aValues[i - 1];
		a1 = aValues[i];
		GUI_SetColor(aColors[i]);
		GUI_DrawPie(400, 100, 50, a0, a1, 0);
	}
   
  /* Run the background task */
  while (1)
  {
//     if(ticks++ > 10)
//     {
//       ticks = 0;
//       /* toggle LED3 each 100ms */
//       STM_EVAL_LEDToggle(LED3);
//     }
//     
//     /* Handle USB Host background process */
//     USBH_USR_BackgroundProcess();
//     
//     /* Handle ALARM background process */
//      ALARM_BackgroundProcess();
//     /* This task is handled periodically, each 10 mS */
		
//		/* Capture input event and updade cursor */
//    BSP_Pointer_Update();
//		
		//触摸屏坐标更新
		ExecTouch();
		
//		USART_SendData(USART1, 0x86);
		
    vTaskDelay(100);
		
		mtheta += mdelta;
		
		GUI_SetColor(GUI_WHITE);
		GUI_FillPolygon (aEnlargedPoints, GUI_COUNTOF(aPointArrow), 160, 180);
		
		GUI_RotatePolygon(aEnlargedPoints, aPointArrow, GUI_COUNTOF(aPointArrow), mtheta);
		GUI_SetColor(GUI_BLUE);
		GUI_FillPolygon (aEnlargedPoints, GUI_COUNTOF(aPointArrow), 160, 180);
		
		GUI_ClearRect(150, 200, 190, 220);
		
		a0 = 120 - mtheta * 180 / 3.14;
		
		if(a0 < 120)
			GUI_SetColor(GUI_GREEN);
		else if(a0 < 140)
			GUI_SetColor(GUI_ORANGE);
		else
			GUI_SetColor(GUI_RED);
		
		GUI_DispDecAt (a0, 150, 210, 3);
		
		if(mtheta > 1.7)
			mdelta = -mdelta;
		
		else if(mtheta < -1.7)
			mdelta = -mdelta;
		
		for (i = 0; i < GUI_COUNTOF(aY); i++) {
			aY[i] = rand() % 50;
		}
		
		GUI_ClearRect(200, 0, 300, 50);
		GUI_DrawGraph(aY, GUI_COUNTOF(aY), 200, 0);
		
		GUI_Exec();
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
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);
	
	GUIDEMO_Main();
//   /* Run the Startup frame */
//   DEMO_Starup();
//   
//   /* Show the main menu */
//   DEMO_MainMenu();
}


/**
  * @brief  Timer callback
  * @param  pxTimer 
  * @retval None
  */
static void vTimerCallback( xTimerHandle pxTimer )
{
   BSP_Pointer_Update();
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
