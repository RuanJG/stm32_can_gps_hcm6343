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
{ 1, 0},
{ 1, -30},
{ 0, -45},
{ -1, -30},
{ -1, 0},
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

//uint32_t demo_mode = 0;

extern WM_HWIN  ALARM_hWin;

/* Private function prototypes -----------------------------------------------*/
static void Background_Task(void * pvParameters);
static void Demo_Task(void * pvParameters);
//static void vTimerCallback( xTimerHandle pxTimer );
extern void DEMO_Starup(void);
extern void DEMO_MainMenu(void);
extern void ALARM_BackgroundProcess (void);

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


/**
  * @brief  Background task
  * @param  pvParameters not used
  * @retval None
  */
static void Background_Task(void * pvParameters)
{   
  WM_HWIN mGUIB, mFloatP, mGUI_AccL, mGUI_AccR;	
	uint8_t pointi = 0, tempi = 0, difSp_active = 0;
	
	/* Initialize the BSP layer */
  LowLevel_Init();
	
  /* Init the STemWin GUI Library */
  GUI_Init(); 
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);	
	
	mGUIB = CreateYunzhou();
	mFloatP = WM_CreateWindowAsChild(178, 53, 128, 128, mGUIB, WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, cbMeterPointWin, 0);
	mGUI_AccL = WM_CreateWindowAsChild(121, 65, 56, 100, mGUIB, WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, NULL, 0);
	mGUI_AccR = WM_CreateWindowAsChild(303, 65, 56, 100, mGUIB, WM_CF_SHOW | WM_CF_MEMDEV | WM_CF_HASTRANS, NULL, 0);
	
	GUI_RotatePolygon(aPointArrow, aPointArrow, GUI_COUNTOF(aPointArrow), 2);
	GUI_RotatePolygon(difSpPointArrow, difSpPointArrow, GUI_COUNTOF(difSpPointArrow), 2);
	mtheta = 0;
	difSp_theta = 0;
	
	WM_SelectWindow(mFloatP);		
	GUI_SelectLayer(1);	
	GUI_RotatePolygon(difSpEnlargedPoints, difSpPointArrow, GUI_COUNTOF(difSpPointArrow), difSp_theta);
	GUI_SetColor(GUI_BLUE);
	GUI_FillPolygon (difSpEnlargedPoints, GUI_COUNTOF(difSpPointArrow), 64, 62);	

	
//  /* Create demo task */				
//  xTaskCreate(Demo_Task,
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
   
  /* Run the background task */
  while (1)
  {			
		pointi++;
		
		if(pointi >= 10)
		{
			pointi = 0;
			
			if(mtheta > -0.03)
				mdelta = -mdelta;
			
			else if(mtheta <= -4)
				mdelta = -mdelta;	
			
			if(difSp_theta > -0.03)
				difSp_delta = -difSp_delta;
			
			else if(difSp_theta <= -4)
				difSp_delta = -difSp_delta;	
			
			mtheta += mdelta;
			
			if(mtheta < -2)
				difSp_active = 1;
			
			WM_SelectWindow(mFloatP);	
			
			GUI_SelectLayer(1);	
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillPolygon (aEnlargedPoints, GUI_COUNTOF(aPointArrow), 64, 62);
			
			GUI_RotatePolygon(aEnlargedPoints, aPointArrow, GUI_COUNTOF(aPointArrow), mtheta);
			GUI_SetColor(GUI_RED);
			GUI_FillPolygon (aEnlargedPoints, GUI_COUNTOF(aPointArrow), 64, 62);
			
			if(difSp_active == 1)
			{
				difSp_theta += difSp_delta;
				GUI_SetColor(GUI_TRANSPARENT);
				GUI_FillPolygon (difSpEnlargedPoints, GUI_COUNTOF(difSpPointArrow), 64, 62);
				
				GUI_RotatePolygon(difSpEnlargedPoints, difSpPointArrow, GUI_COUNTOF(difSpPointArrow), difSp_theta);
				GUI_SetColor(GUI_BLUE);
				GUI_FillPolygon (difSpEnlargedPoints, GUI_COUNTOF(difSpPointArrow), 64, 62);				
			}

			for(tempi = 1; tempi < 6; tempi++)
			{
				GUI_SetColor(GUI_YELLOW / tempi);
				GUI_DrawCircle(64, 62, tempi);
			}		
			
			WM_SelectWindow(mGUI_AccL);
			GUI_SelectLayer(1);	
			
			if(max_acc_lim >= 100)
			{
				max_acc_delta = -max_acc_delta;
			}				
			else if(max_acc_lim == 0)
			{
				max_acc_delta = -max_acc_delta;
			}
			
			
			//清除原线
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_DrawHLine(50 - max_acc_lim * 2 / 5, 5, 42 - (100 - max_acc_lim) / 5);
			
			//绘制新线
			max_acc_lim += max_acc_delta;
			GUI_SetColor(GUI_GREEN);
			GUI_DrawHLine(50 - max_acc_lim * 2 / 5, 5, 42 - (100 - max_acc_lim) / 5);			
			
			
			
//			float_to_string(-mtheta * 22 / 4, tempStr, 1);
//			printf("the speed is: %s\n", tempStr);
			
			WM_SendMessageNoPara(mGUIB, WM_NOTIFICATION_VALUE_CHANGED);
		}

		//触摸屏坐标更新
		ExecTouch();
		
		GUI_Exec();

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
		vTaskDelay(1000);
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
