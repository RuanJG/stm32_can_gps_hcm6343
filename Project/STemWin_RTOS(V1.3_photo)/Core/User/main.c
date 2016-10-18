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

///*********************************************************************
//* *
//_
//cbWin
//*/
//static void _cbWin(WM_MESSAGE * pMsg) {
//	GUI_COLOR Color;
//	switch (pMsg->MsgId)
//	{
//		case WM_PAINT:
//			WM_GetUserData(pMsg->hWin, &Color, 4);
//			GUI_SetBkColor(Color);
//			GUI_Clear();
//			break;
//		
//		default:
//			WM_DefaultProc(pMsg);
//	}
//}

///*********************************************************************
//* *
//_
//cbDoSomething
//*/
//static void _cbDoSomething(WM_HWIN hWin, void * p) 
//{
//	int Value = *(int *)p;
//	WM_MoveWindow(hWin, Value, Value);
//}

//static void _cbWin(WM_MESSAGE * pMsg) {
//	switch (pMsg->MsgId) {
//		case WM_TIMER:
//		/*
//		... do something ...
//		*/
//		WM_RestartTimer(pMsg->Data.v, 1000);
//		break;
//		default:
//		WM_DefaultProc(pMsg);
//	}
//}

/*******************************************************************
*
* Callback routine for background window
*
********************************************************************
*/
static void cbBackgroundWin(WM_MESSAGE* pMsg) {
	switch (pMsg->MsgId) {
		case WM_PAINT:
		GUI_Clear();
		default:
		WM_DefaultProc(pMsg);
	}
}

/*******************************************************************
*
* Callback routine for foreground window
*
********************************************************************
*/
static void cbForegroundWin(WM_MESSAGE* pMsg) {
	switch (pMsg->MsgId) {
		case WM_PAINT:
			GUI_SetBkColor(GUI_GREEN);
			GUI_Clear();
			GUI_DispString("Foreground window");
			break;
		
		default:
			WM_DefaultProc(pMsg);
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
  uint32_t ticks = 0;
 	GUI_MEMDEV_Handle hMem0, hMem1, hMem2;
	
  /* Initialize the BSP layer */
  LowLevel_Init();
	

  GUI_Init();

  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);	
	GUI_CURSOR_Show();
//		
//	GUI_MEMDEV_Handle hMem = GUI_MEMDEV_Create(100,100,60,32);
//	GUI_MEMDEV_Select(hMem);
//	GUI_SetFont(&GUI_Font32B_ASCII);
////	GUI_DispString("Text");
//	GUI_DispStringAt("Text", 100, 100);
////	GUI_MEMDEV_CopyToLCDAA(hMem);
////	GUI_MEMDEV_CopyToLCD(hMem);
//	GUI_MEMDEV_CopyToLCDAt(hMem, 10, 10);
//	
//	GUI_SelectLayer(0);
//	GUI_SetFont(&GUI_Font32B_ASCII);
//	GUI_DispStringAt("Text", 200, 200);

//	GUI_JPEG_Draw(aclogo, sizeof(aclogo), 0, 0);

	hMem0 = GUI_MEMDEV_Create(0, 0, 400, 200);
	hMem1 = GUI_MEMDEV_Create(0, 0, 400, 200);
	hMem2 = GUI_MEMDEV_Create(0, 0, 400, 200);

////	hMem0 = GUI_MEMDEV_CreateFixed(0, 0, 400, 200, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
////	hMem1 = GUI_MEMDEV_CreateFixed(0, 0, 400, 200, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);
////	hMem2 = GUI_MEMDEV_CreateFixed(0, 0, 400, 200, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_8888);

////	hMem0 = GUI_MEMDEV_CreateFixed(0, 0, 150, 150, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_888);
////	hMem1 = GUI_MEMDEV_CreateFixed(0, 0, 75, 150, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_888);
////	hMem2 = GUI_MEMDEV_CreateFixed(0, 0, 75, 150, GUI_MEMDEV_HASTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_888);
	GUI_MEMDEV_Select(hMem0);
	GUI_JPEG_Draw(aclogo, sizeof(aclogo), 0, 0);
	GUI_MEMDEV_Select(hMem1);
	GUI_MEMDEV_DrawPerspectiveX(hMem0, 0, 0, 150, 110, 75, 20);
	GUI_MEMDEV_Select(hMem2);
	GUI_MEMDEV_DrawPerspectiveX(hMem0, 0, 20, 110, 150, 75, -20);
//	GUI_MEMDEV_CopyToLCDAt(hMem0, 0, 10);
////	GUI_MEMDEV_CopyToLCDAt(hMem1, 160, 100);
//	GUI_MEMDEV_CopyToLCDAt(hMem2, 245, 200);

//	GUI_MEMDEV_Handle hMemSource;
//	GUI_MEMDEV_Handle hMemDest;
//	GUI_RECT RectSource = {0, 0, 69, 39};
//	GUI_RECT RectDest = {0, 0, 79, 79};
////	hMemSource = GUI_MEMDEV_CreateFixed(RectSource.x0, RectSource.y0, RectSource.x1 - RectSource.x0 + 1, RectSource.y1 - RectSource.y0 + 1, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_888);
////	hMemDest = GUI_MEMDEV_CreateFixed(RectDest.x0, RectDest.y0, RectDest.x1 - RectDest.x0 + 1, RectDest.y1 - RectDest.y0 + 1, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_888);
//	hMemSource = GUI_MEMDEV_CreateFixed(0, 0, 100, 100, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_888);
//	hMemDest = GUI_MEMDEV_CreateFixed(0, 0, 100, 100, GUI_MEMDEV_NOTRANS, GUI_MEMDEV_APILIST_32, GUI_COLOR_CONV_888);
//	GUI_MEMDEV_Select(hMemSource);
//	GUI_DrawGradientV(RectSource.x0, RectSource.y0, RectSource.x1, RectSource.y1, GUI_WHITE, GUI_DARKGREEN);
//	GUI_SetColor(GUI_BLUE);
//	GUI_SetFont(&GUI_Font20B_ASCII);
//	GUI_SetTextMode(GUI_TM_TRANS);
//	GUI_DispStringInRect("emWin", &RectSource, GUI_TA_HCENTER | GUI_TA_VCENTER);
//	GUI_DrawRect(0, 0, RectSource.x1, RectSource.y1);
//	GUI_MEMDEV_RotateHQ(hMemSource, hMemDest, (RectDest.x1 - RectSource.x1) / 2, (RectDest.y1 - RectSource.y1) / 2, 30 * 1000, 1000);
//	GUI_MEMDEV_RotateHQ(hMemSource, hMemDest, 0, 0, 30 * 1000, 2000);
//	GUI_MEMDEV_CopyToLCDAt(hMemSource, 10, (RectDest.y1 - RectSource.y1) / 2);
////	GUI_MEMDEV_CopyToLCDAt(hMemDest, 100, 0);
//	GUI_MEMDEV_CopyToLCDAt(hMemDest, 10, (RectDest.y1 - RectSource.y1) / 2);


//GUI_MEMDEV_Handle hMem0, hMem1;

//	GUI_SetFont(&GUI_Font8_1);

//	hMem0 = GUI_MEMDEV_Create(0, 0, 140, 110);
//	hMem1 = GUI_MEMDEV_Create(0, 0, 180, 120);
//	GUI_MEMDEV_Select(hMem0);
////	GUI_SetTextMode(GUI_TM_TRANS);
//	GUI_DispString("Text");
//	GUI_MEMDEV_Select(hMem1);
//	GUI_SetBkColor(GUI_RED);
//	GUI_Clear();
//	GUI_DispStringAt("Text", 0, 0);
//	GUI_MEMDEV_WriteExAt(hMem0, 0, 0, 2000, 2000, 160);
//	GUI_MEMDEV_CopyToLCD(hMem1);

//	GUI_MEASDEV_Handle hMeasdev;
//	GUI_RECT Rect;
//	hMeasdev = GUI_MEASDEV_Create();
//	GUI_MEASDEV_Select(hMeasdev);
//	GUI_DrawLine(10, 20, 30, 40);
//	GUI_MEASDEV_GetRect(hMeasdev, &Rect);
//	GUI_MEASDEV_Delete(hMeasdev);
//	GUI_DispString("X0:");
//	GUI_DispDec(Rect.x0, 3);
//	GUI_DispString(" Y0:");
//	GUI_DispDec(Rect.y0, 3);
//	GUI_DispString(" X1:");
//	GUI_DispDec(Rect.x1, 3);
//	GUI_DispString(" Y1:");
//	GUI_DispDec(Rect.y1, 3);

//	WM_HWIN hWin_1, hWin_2, hWin_3;
//	int Value = 10;
//	GUI_COLOR aColor[] = {GUI_RED, GUI_GREEN, GUI_BLUE};
////	GUI_Init();
//	WM_SetDesktopColor(GUI_BLACK);
//	hWin_1 = WM_CreateWindow( 10, 10, 100, 100, WM_CF_SHOW, _cbWin, 4);
//	hWin_2 = WM_CreateWindowAsChild(10, 10, 80, 80, hWin_1, WM_CF_SHOW, _cbWin, 4);
//	hWin_3 = WM_CreateWindowAsChild(10, 10, 60, 60, hWin_2, WM_CF_SHOW, _cbWin, 4);
//	WM_SetUserData(hWin_1, &aColor[0], 4);
//	WM_SetUserData(hWin_2, &aColor[1], 4);
//	WM_SetUserData(hWin_3, &aColor[2], 4);

//	GUI_RECT r;
//	uint8_t x1 = 100;

//	WM_HWIN hWin;
//	WM_HTIMER hTimer;
//	hWin = WM_CreateWindow(10, 10, 100, 100, WM_CF_SHOW, _cbWin, 0);
//	hTimer = WM_CreateTimer(hWin, 0, 1000, 0);

	GUI_HWIN hWnd;
	 
	 
  /* Run the background task */
  while (1)
  {	
		//触摸屏坐标更新
		ExecTouch();
		
//		USART_SendData(USART1, 0x86);
//		GUI_MEMDEV_CopyToLCDAt(hMemSource, 10, (RectDest.y1 - RectSource.y1) / 2);
//    vTaskDelay(1000);
//		GUI_MEMDEV_CopyToLCDAt(hMemDest, 10, (RectDest.y1 - RectSource.y1) / 2);
//		vTaskDelay(1000);
		
//		WM_Exec();
		vTaskDelay(10);
//		WM_ForEachDesc(WM_HBKWIN, _cbDoSomething, (void *)&Value);
//		Value *= -1;
//		GUI_Delay(500);
		
//		/* Draw left part of the bar */
//		r.x0=0; r.x1=x1-1; r.y0=0; r.y1 = GUI_YMAX;
//		WM_SetUserClipRect(&r);
//		GUI_SetBkColor(GUI_BLACK);
//		GUI_SetColor(GUI_WHITE);
//		GUI_Clear();
////		GUI_GotoXY(xText,yText); GUI_DispDecMin(pThis->v); GUI_DispChar('%');
//		/* Draw right part of the bar */
//		r.x0=r.x1; r.x1=GUI_XMAX;
//		WM_SetUserClipRect(&r);
//		GUI_SetBkColor(GUI_GRAY);
//		GUI_SetColor(GUI_BLACK);
//		GUI_Clear();
////		GUI_GotoXY(xText,yText); GUI_DispDecMin(pThis->v); GUI_DispChar('%');

//		/* Create foreground window */
//		hWnd = WM_CreateWindow(10, 10, 100, 100, WM_CF_SHOW, cbForegroundWin, 0);
//		/* Show foreground window */
//		GUI_Delay(1000);
//		/* Delete foreground window */
//		WM_DeleteWindow(hWnd);
//		GUI_DispStringAt("Background of window has not been redrawn", 10, 10);
//		/* Wait a while, background will not be redrawn */
//		GUI_Delay(1000);
//		GUI_Clear();
//		/* Set callback for Background window */
//		WM_SetCallback(WM_HBKWIN, cbBackgroundWin);
//		/* Create foreground window */
//		hWnd = WM_CreateWindow(10, 10, 100, 100,WM_CF_SHOW, cbForegroundWin, 0);
//		/* Show foreground window */
//		GUI_Delay(1000);
//		/* Delete foreground window */
//		WM_DeleteWindow(hWnd);
//		/* Wait a while, background will be redrawn */
//		GUI_Delay(1000);
//		/* Delete callback for Background window */
//		WM_SetCallback(WM_HBKWIN, 0);
		
		while(1);

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
