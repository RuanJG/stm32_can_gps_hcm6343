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

#include "global_includes.h"


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
void capture_Timer3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      		//时钟配置
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					
	
	TIM_PrescalerConfig(TIM3, 1000, TIM_PSCReloadMode_Update);

	/* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                               //GPIO配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                     //NVIC配置
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                   //通道选择
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       	//上升沿触发
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;    	//管脚与寄存器对应关系
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;								//输入预分频，意思是控制在多少个输入周期做一次捕获，如果输入信号频率没有变，测得
	//的周期也不会变，比如选择4分频，则每四个输入周期才做一次捕获，这样在输入信号不频繁的情况下，可以减少软件被不断中断的次数
	
	TIM_ICInitStructure.TIM_ICFilter = 0x0;                            //滤波设置，经历几个周期跳变认定波形稳定0x0~0xf
  TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);                 //根据参数配置TIM外设信息
  TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);                //选择IC2为始终触发源
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);								//TIM从模式，触发信号的上升沿重新初始化计数器和触发寄存器的更新事件
  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable); 				//启动定时器的被动触发
  TIM_Cmd(TIM3, ENABLE);                                 							//启动TIM3      
  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);     												//打开中断
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
  GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev;
	int16_t rdbm = 0;
	int correct_counter = 0;
	uint8_t refresh_count = 0;
	char tempChar[20];
	uint8_t time_axis = 30, last_DutyCycle;
	uint32_t colorTemp;				//暂存颜色
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Initialize the BSP layer */
  LowLevel_Init();
	
	capture_Timer3_init();
	
	//PF6初始化
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOF, GPIO_Pin_6);			//常亮液晶屏
	
	
  /* Init the STemWin GUI Library */
  GUI_Init(); 
  GUI_SetBkColor(GUI_TRANSPARENT);
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT); 
  GUI_SelectLayer(0);
	
	GUI_BMP_Draw(_acImage_0, 358, 226);				//Yunzhou logo
		
	bg0_memdev = GUI_MEMDEV_Create(0, 0, 170, 150);
	bg1_memdev = GUI_MEMDEV_Create(0, 0, 215, 150);

	
  /* Run the background task */
  while (1)
  {			
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) != 0)
		{
			receive_counter++;
		}
		
//		if(USART_GetFlagStatus(USART1, USART_IT_RXNE) != RESET)
//		{
//			colorTemp = USART_ReceiveData(USART1);
//			
//			if(colorTemp == 0x18)
//				correct_counter++;
//			
//			USART_ClearITPendingBit(USART1, USART_IT_RXNE);			//中断标志软复位
//		}
		
		//显示接收信号强度
		refresh_count++;
		
		if(refresh_count >= 10)
		{
			refresh_count = 0;

			GUI_MEMDEV_Select(bg0_memdev);
			GUI_SetColor(GUI_TRANSPARENT);
			GUI_FillRect(0, 0, 170, 150);

			GUI_SetFont(&GUI_Font16B_ASCII);
			GUI_SetColor(GUI_YELLOW);		
			GUI_DispStringAt("Antenna Test Tool", 30, 20);
			
			GUI_SetFont(&GUI_Font13B_ASCII);
			GUI_SetColor(GUI_RED);
			GUI_DispStringAt("             ---for 900M(V1.0)", 30, 35);
			
			GUI_SetColor(GUI_GREEN);

			float_to_string(DutyCycle, tempChar, 0, 0, 0, 0);
			GUI_DispStringAt("the dutycycle is:", 30, 59);
			GUI_DispStringAt(tempChar, 130, 59);
			
			float_to_string(Frequency, tempChar, 0, 4, 0, 0);
			GUI_DispStringAt("the Frequency is:", 30, 75);
			GUI_DispStringAt(tempChar, 130, 75);
			
			rdbm = DutyCycle * 2 / 3 - 113;		
			float_to_string(rdbm, tempChar, 0, 3, 0, 0);				
			GUI_DispStringAt("The RSSI is:", 30, 91);
			GUI_DispStringAt(tempChar, 100, 91);
			
			float_to_string(receive_counter, tempChar, 0, 0, 0, 0);				
			GUI_DispStringAt("receive no:", 30, 107);
			GUI_DispStringAt(tempChar, 120, 107);
			
			float_to_string(correct_counter, tempChar, 0, 0, 0, 0);				
			GUI_DispStringAt("correct no:", 30, 122);
			GUI_DispStringAt(tempChar, 120, 122);
			
	//		float_to_string(receive_counter, tempChar, 0, 0, 0, 0);				
	//		GUI_DispStringAt("receive count:", 30, 92);
	//		GUI_DispStringAt(tempChar, 120, 103);
			 
			GUI_MEMDEV_CopyToLCDAt(bg0_memdev, 0, 0);
			
			GUI_MEMDEV_Select(bg1_memdev);
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(time_axis, 0, time_axis + 3, 150);	
			if(DutyCycle < 20)
				GUI_SetColor(GUI_RED);
			 
			else if(DutyCycle < 50)
				GUI_SetColor(GUI_YELLOW);
			
			else
				GUI_SetColor(GUI_GREEN);
						
			GUI_DrawLine(time_axis, 67 - last_DutyCycle * 67 / 100, time_axis + 3, 67 - DutyCycle * 67 / 100);
			
			time_axis = time_axis + 3;
			
			if(time_axis >= 200)
				time_axis = 30;
			
			last_DutyCycle = DutyCycle;
			
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font8_ASCII);
			float_to_string(-40, tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 0, 0);
			float_to_string(-120, tempChar, 0, 0, 0, 0);
			GUI_DispStringAt(tempChar, 0, 67);		
			
			GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 0, 150);
		}		
		
//		printf("the dutycycle is %d, the frequence is %d, 1 for high: %d\n", DutyCycle, Frequency, GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7));
//		USART_SendData(USART1, 0x85);
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)
		{
			DutyCycle = 0;
			Frequency = 0;			
		}

		//触摸屏坐标更新
		ExecTouch();
				
		
		
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
	else if(unit == 3)
	{
		str[i++] = ' ';
		str[i++] = 'D';
		str[i++] = 'B';
		str[i++] = 'm';
	}
	else if(unit == 4)
	{
		str[i++] = ' ';
		str[i++] = 'H';
		str[i++] = 'z';
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
