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
#include "main.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define Background_Task_PRIO    ( tskIDLE_PRIORITY  + 10 )
#define Background_Task_STACK   ( 512 )

#define Demo_Task_PRIO          ( tskIDLE_PRIORITY  + 9 )
#define Demo_Task_STACK         ( 3048 )

#define IS42S16400J_SIZE             0x400000

int testp = 99;
#define abc 11

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
xTaskHandle                   Task_Handle;
xTaskHandle                   Demo_Handle;
xTimerHandle                  TouchScreenTimer;

FRAMEWIN_Handle mFrame;
int visible_frame = 0;

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

/**
  * @brief  Read memory
  * @param  Address: specifies the address to be readed.
  * @retval Return the specific address flash data
  */
uint16_t ReadMemory(uint32_t Address)
{
	return *(uint32_t *)Address;
}

void WriteMemory(uint32_t Address, uint16_t Value)
{
	*(uint32_t *)Address = Value;
}

//FLASH_ProgramHalfWord(DATA_FLASH_BASE + HALL_ANGLE_1, flash_tmp);

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

///*******************************************************************
//*
//* Callback routine for foreground window
//*
//********************************************************************
//*/
//static void cbForegroundWin(WM_MESSAGE* pMsg) {
//	switch (pMsg->MsgId) {
//		case WM_PAINT:
//			GUI_SetBkColor(GUI_GREEN);
//			GUI_Clear();
//			GUI_DispString("Foreground window");
//			break;
//		
//		default:
//			WM_DefaultProc(pMsg);
//	}
//}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */ 
int main(void)
{ 
//   GPIO_InitTypeDef GPIO_InitStructure;
	
	int counter = 2;
	/* SDRAM Initialization */  
  SDRAM_Init();
  
  /* FMC SDRAM GPIOs Configuration */
  SDRAM_GPIOConfig();
  
  /* Disable write protection */
  FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM,DISABLE); 
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//	 
//	/* Setup SysTick Timer for 1 msec interrupts.*/
//	if (SysTick_Config(SystemCoreClock / 1000))
//	{ 
//	 /* Capture error */ 
//	 while (1);
//	}
//   
	mCOMInit();
//	
	/* Erase SDRAM memory */
//  for (counter = 0x00; counter < IS42S16400J_SIZE; counter++)
//  {
//    *(__IO uint8_t*) (SDRAM_BANK_ADDR + counter) = (uint8_t)0x69;
//  }
  
	while(1)
	{
		printf("read sdram 0xD0000000 is %x\n", ReadMemory(0xD0000000));
		mdelay(1000000);
	}
//  /* Create background task */
//  xTaskCreate(Background_Task,
//              (signed char const*)"BK_GND",
//              Background_Task_STACK,
//              NULL,
//              Background_Task_PRIO,
//              &Task_Handle);						

//  /* Start the FreeRTOS scheduler */
//  vTaskStartScheduler();
}


/**
  * @brief  Background task
  * @param  pvParameters not used
  * @retval None
  */
static void Background_Task(void * pvParameters)
{
	PROGBAR_Handle hProgBar;
	GUI_HWIN hWnd;
	uint8_t i = 0;
	uint8_t j = 0;
//	uint8_t visible_frame = 0;

	BUTTON_Handle mButton;
	
	WM_HWIN mGUIB;
	
	char mTitle[] = "test frame";
	
  /* Initialize the BSP layer */
  LowLevel_Init();
	

  GUI_Init();

  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);	
	GUI_CURSOR_Show();


//	hWnd = WM_CreateWindow(0, 0, 480, 272, WM_CF_SHOW, cbBackgroundWin, 0);
//	GUI_DispStringAt("Progress bar", 100, 20);


//	mGUIB = CreateWindow();
//	
//	hProgBar = PROGBAR_Create(100, 40, 100, 20, WM_CF_SHOW);
////	WIDGET_SetDefaultEffect(&WIDGET_Effect_3D);
//	
//	PROGBAR_SetBarColor(hProgBar, 0, GUI_GREEN);
//	PROGBAR_SetBarColor(hProgBar, 1, GUI_RED);
//	
//	PROGBAR_SetSkin(hProgBar, PROGBAR_SKIN_FLEX);
//	WM_EnableMemdev(hProgBar);
//	
//	
//	mFrame = FRAMEWIN_Create(mTitle, cbBackgroundWin, WM_CF_SHOW, 0, 100, 150, 100);
//	FRAMEWIN_AddCloseButton(mFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(mFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMinButton(mFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	
//	BUTTON_CreateEx(20, 20, 30, 20, mFrame, WM_CF_SHOW, 0, 1);
//	
//	FRAMEWIN_SetSkin(mFrame, FRAMEWIN_SKIN_FLEX);
	
//	GUI_SelectLayer(2);
//	GUI_SetColor(GUI_RED);
//	GUI_FillRect(0, 0, 100, 100);
//	
//	GUI_SelectLayer(1);
//	GUI_SetColor(GUI_BLUE);
//	GUI_FillRect(50, 50, 150, 150);
//	
//	GUI_SelectLayer(0);
//	GUI_SetColor(GUI_GREEN);
//	GUI_FillRect(100, 100, 200, 200);

	GUI_SelectLayer(0);
//	GUI_SetColor(GUI_RED);
//	GUI_FillRect(0, 0, 199, 33);
//	GUI_SetColor(GUI_GREEN);
//	GUI_FillRect(0, 34, 199, 66);
//	GUI_SetColor(GUI_BLUE);
//	GUI_FillRect(0, 67, 199, 99);
//	GUI_DrawGradientV(0, 0, 199, 199, 0x0000FF, 0x00FF00);
//	GUI_SelectLayer(1);
//	GUI_SetBkColor(GUI_WHITE);
//	GUI_Clear();
//	GUI_SetColor(GUI_BLACK);
//	GUI_DispStringHCenterAt("Layer 1", 100, 4);
//	GUI_SetColor(GUI_TRANSPARENT);
//	GUI_FillCircle(100, 50, 35);
//	GUI_FillRect(10, 10, 40, 90);
//	GUI_FillRect(160, 10, 190, 90);

//	GUI_SetColor(GUI_RED);
//	GUI_FillRect(0, 0, 479, 271);
//	GUI_SetColor(GUI_GREEN);
//	GUI_FillRect(0, 272, 479, 272 * 2 - 1);
//	GUI_SetColor(GUI_BLUE);
//	GUI_FillRect(0, 272 * 2, 479, 272 * 3 - 1);
//	GUI_SetColor(GUI_WHITE);
//	GUI_SetTextMode(GUI_TM_TRANS);
//	GUI_DispStringAt("Screen 0", 0, 0);
//	GUI_DispStringAt("Screen 1", 0, 272);
//	GUI_DispStringAt("Screen 2", 0, 272 * 2);
//	
//	vTaskDelay(1000);
//	GUI_SetOrg(0, 272); /* Set origin to screen 1 */
//	
//	vTaskDelay(1000);	
//	GUI_SetOrg(0, 272 * 2); /* Set origin to screen 2 */
	
//	WriteMemory(0x2002ff00, 0x8569);			//内存写入访问调试

//	FLASH_Status my_FLASH_Status;
//	FLASH_Unlock();
//	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
//                   FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
//  my_FLASH_Status = FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
//	while(my_FLASH_Status != FLASH_COMPLETE);
//	
//	my_FLASH_Status = FLASH_ProgramHalfWord(0x080fff00, 0x8569);
//	while(my_FLASH_Status != FLASH_COMPLETE);
//	
//	FLASH_Lock();
	 
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
		
		j++;
		
		if(j > 10)
		{
			j = 0;
			
//			PROGBAR_SetValue(hProgBar, i++);
			
//			printf("the number of layers is %d\n", LCD_GetNumLayers());
//			printf("the system time is: %d\n", GUI_GetTime());
//			printf("read sram 0x2002ff00 is: %x\n", ReadMemory(0x2002ff00));				//内存读出访问调试
//			printf("read sram 0x080fff00 is: %x\n", ReadMemory(0x080fff00));				//flash读出访问调试
			printf("read sdram 0xD0000000 is %x\n", ReadMemory(0xD0000000));
//			printf("read the indepent sdram 0x68000000 is %x\n", ReadMemory(0x68000000));
		
//			if(i > 100)
//			{
//				i = 0;
//			}
		}	
//		
//		i++;
//		
//		if(i == 1)
//		{
//			GUI_SetOrg(0, 0); /* Set origin to screen 0 */
//		} 
//		
//		else if(i == 2)
//		{
//			GUI_SetOrg(0, 272); /* Set origin to screen 1 */
//		}

//		else if(i == 3)
//		{
//			GUI_SetOrg(0, 272 * 2); /* Set origin to screen 1 */
//		}
//		
//		else
//			i = 0;

//		i++;
//		
//		if(i > 100)
//		{
//			i = 0;
////			printf("Test sending...\n");
//			
////			if(visible_frame != 0)
////			{
////				visible_frame = 0;
////				WM_HideWindow(mFrame);
////			}
////			
////			else
////			{
////				visible_frame = 1;
////				WM_ShowWindow(mFrame);							
////			}
//		}
		
		GUI_Exec();


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
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);
	
//	GUIDEMO_Main();
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
