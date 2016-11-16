/*-------------------------------------------------------------------------
工程名称：遥控器平台驱动程序文档
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150526 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "rtc.h"
#include "TouchPanel.h"
#include "config.h"
#include "set_timer.h"
#include "remoter_sender_jostick.h"
#include "bsp_xtend900_rssi_timer3.h"
#include "bsp_lcd_backlight_pwm_timer10.h"
#include "bsp_buzzer_pwm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TS_Pressed;
__IO uint32_t TS_Orientation = 0;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE            USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST                      USB_Host __ALIGN_END;



/**
  * @brief  Initializes the IO Expander registers.
  * @param  None
  * @retval 0: if all initializations are OK.
*/
uint32_t BSP_TS_Init(void)
{
  TP_Init();
//  TouchPanel_Calibrate(); 
	_InitTouch();
}

/**
  * @brief  Read the TS data and update the GUI Touch structure
  * @param  None
  * @retval None
  */
void BSP_Pointer_Update(void)
{
	Coordinate  display ;
  GUI_PID_STATE TS_State;
  TP_STATE  *ts;
  static uint8_t prev_state = 0;
  if(TP_INT_IN==0)
	{
		getDisplayPoint(&display, Read_Ads7846(), &matrix );		
		ts->X=display.x;
		ts->Y=display.y;
		if(TS_Orientation == 0)
		{
			TS_State.x = ts->X;
			TS_State.y = ts->Y;
		}
		else
		{
			TS_State.y = ts->X;
			TS_State.x = 272 - ts->Y;
		}
		TS_State.Pressed = 1;
		
		if(prev_state != TS_State.Pressed )
		{
			prev_state = TS_State.Pressed;
			TS_State.Layer = 1;
			GUI_TOUCH_StoreStateEx(&TS_State); 
		}
  }  
//   portENTER_CRITICAL();
// 	getDisplayPoint(&display, Read_Ads7846(), &matrix );		
// 	ts->X=display.x;
// 	ts->Y=display.y;
//   //ts = IOE_TP_GetState();
// 	
//   portEXIT_CRITICAL();
//   
//   if(TS_Orientation == 0)
//   {
//     TS_State.x = ts->X;
//     TS_State.y = ts->Y;
//   }
//   else
//   {
//     TS_State.y = ts->X;
//     TS_State.x = 320 - ts->Y;
//   }
//   TS_State.Pressed = (ts->TouchDetected == 0x80);
//   
//   if(prev_state != TS_State.Pressed )
//   {
//     prev_state = TS_State.Pressed;
//     TS_State.Layer = 1;
//     GUI_TOUCH_StoreStateEx(&TS_State); 
//   }
}


/**
  * @brief  RTC Alarm A Interrupt Configuration
  * @param  None
  * @retval None
*/
void Alarm_Init(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;
  
  __disable_irq();
  
  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  __enable_irq ();
}











//########################################################    jostick 
#define ADC_CHANNEL_SAMPLE_COUNT 20 
#define ADC_CHANNEL_COUNT 6
unsigned short remoter_sender_jostick_ADCValues[ADC_CHANNEL_SAMPLE_COUNT][ADC_CHANNEL_COUNT];


unsigned short bsp_Cali_Adc_Value(int id)
{
	unsigned short max=0,min=0,tmp,i;
	unsigned int sensor = 0; 
	
	if( id >= ADC_CHANNEL_COUNT ) return 0;
	
	max = remoter_sender_jostick_ADCValues[0][id];	
	min = remoter_sender_jostick_ADCValues[0][id];
	for(i=0;i<ADC_CHANNEL_SAMPLE_COUNT;i++)
	{
		tmp = remoter_sender_jostick_ADCValues[i][id];
		if( tmp > max ) max = tmp;
		if( tmp < min ) min = tmp;
		sensor += tmp;
	}
	sensor -= (max+min);
	sensor = sensor/(ADC_CHANNEL_SAMPLE_COUNT-2);
	return sensor;
}
void bsp_ADC1_CH6_DMA_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

//  DMA_DeInit(DMA2_Stream0);
  /* DMA2 Stream0 channe0 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&remoter_sender_jostick_ADCValues;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_SAMPLE_COUNT * ADC_CHANNEL_COUNT;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* Configure ADC1 Channel6 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
//	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 6;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channe6 configuration *************************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_480Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_480Cycles);
 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);


//#########  add irq 
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);     	//在main 里己定议 
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;    		//开串口中断1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   	//制定抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    				//指定从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ITConfig(DMA2_Stream0,DMA_IT_TC,ENABLE);
	
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);			//开始ADC采样
	
	//**********************************  key
	//按键及钮子开关GPIO配置
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);
	
	/* Configure Input pushpull mode */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);			
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

void DMA2_Stream0_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream0,DMA_IT_TCIF0))
 {
		DMA_ClearITPendingBit(DMA2_Stream0,DMA_IT_TCIF0);
	 remoter_sender_jostick_adc_dma_TCIF_callback();
	 	
 }
}










//################################  遥控器的电池电量
#define _BATTERY_ADC_MAX 3210
#define _BATTERY_ADC_MIN 2480
unsigned char bsp_Get_Battery_Level()
{
	unsigned short val ;
	unsigned char level;
	
	val = bsp_Cali_Adc_Value(0);
	
	/*
					电池标称10.8 ~ 12.3V，通过1M及270K电阻分压后为2.30 ~ 2.61V，ADC换算为 2854 ~ 3238
					电池电量粗劣的计算公式为：1-(4.2-x)/(4.2-3.6)
	*/
//			Battery_Power_Percent = 100 - (3238 - Battery_Power_Avg) * 100 / (3238 - 2854);
			level = 100 - (_BATTERY_ADC_MAX - val) * 100 / (_BATTERY_ADC_MAX - _BATTERY_ADC_MIN);
			
			if(level <= 0)
			{
				level = 0;
			}
			else if(level >= 100)
			{
				level = 100;
			}		
	
			return level;
}


void bsp_Gui_lcd_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/*
	//液晶屏幕点亮GPIO配置
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOF, GPIO_Pin_6);			//关闭液晶屏，避免初始化时的白屏现象	
	*/
	  /* LCD initialization */
  LCD_Init();
  /* LCD Layer initialization */
  LCD_LayerInit();
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
  /* Enable the CRC Module */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	
	/*Init Touchscreen */
	BSP_TS_Init();
	
}



/*
*********************  调试串口
*/

#include "fifo.h"
#define UART2_TX_BUFFER_SIZE 256
unsigned char uart2_tx_buffer[UART2_TX_BUFFER_SIZE];
fifo_t uart2_tx_fifo;


void bsp_debug_uart2_init(void)
{
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef 	GPIO_InitStructure;
	DMA_InitTypeDef   DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	fifo_init(&uart2_tx_fifo,uart2_tx_buffer,UART2_TX_BUFFER_SIZE);
	
	//初始化USART2 ---------------------------------------------------------
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;	
	
	/* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable UART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
  /* Connect PXx to USARTx_Tx*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  /* USART configuration */
	USART_Init(USART2, &USART_InitStructure);
    
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);	 
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;    		//开串口中断1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;   	//制定抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    				//指定从优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
  /* Enable USART */	
	USART_Cmd(USART2, ENABLE);
}




void USART2_IRQHandler(void)
{
	u8 c;				//char c;
	
	//接收中断---------------------------------------------------------
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);			//中断标志软复位
	}	
	
	//发送中断---------------------------------------------------------
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_TXE);			//中断标志软复位
		
		//if( USART_GetFlagStatus(USART2, USART_FLAG_TXE) != RESET)
		{
			// empty , can to send data
			if( 0 < fifo_avail(&uart2_tx_fifo) ){
				fifo_get(&uart2_tx_fifo,&c);
				USART_SendData(USART2, c);
			}else{
				USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			}
		}
	}		
	
} 

void bsp_uart2_PutChar( char ch)
{
	char c;
	USART_TypeDef *uartDev = USART2;
	
	USART_ITConfig(uartDev, USART_IT_TXE, DISABLE);
	fifo_recovery_put(&uart2_tx_fifo,ch);
	USART_ITConfig(uartDev, USART_IT_TXE, ENABLE);
}



#define UART2_LOG_UART1 1
void bps_log_uart1_to_uart2(char c)
{
	#if UART2_LOG_UART1
		bsp_uart2_PutChar(c);
	#endif
}

//for micorlib printf
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
	
	#if UART2_LOG_UART1
	;//no display log
	#else
		bsp_uart2_PutChar( (char) ch);
	#endif
	
  return ch;
}












#define _BSP_SAVE_START_ADDRESS 	0x080e0000					//参数存储地址
#define _BSP_FIRST_SAVE	0xffffffff			//没存入参数前的数据

void bsp_save_parameter_to_flash(void)
{
	uint32_t address = _BSP_SAVE_START_ADDRESS;
	uint16_t temp_i;

	//解锁FLASH后才能向FLASH中写数据
	FLASH_Unlock(); 			
	
	//清状态标记
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);	
	
	//擦除FLASH
	while(FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3) != FLASH_COMPLETE);
	
	/*
	//写入RF的8个参数
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		while(FLASH_ProgramWord(address, Xtend_900_para_buf[temp_i]) != FLASH_COMPLETE);
		address = address + 4;
	}
	*/
	//锁定FLASH
	FLASH_Lock();	
	
	return;
}


void bsp_load_parameter_from_flash(void)
{
	uint32_t address = _BSP_SAVE_START_ADDRESS;
	uint16_t temp_i;

	//判断是否已经存入信道参数，没存入则不从flash中读出，而是用默认参数
	if(*(__IO uint32_t*)address != _BSP_FIRST_SAVE)
	{
		/*
		//读出RF的8个参数
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			Xtend_900_para[temp_i] = *(__IO uint32_t*)address;
			address = address + 4;
		}
		*/
	}
		
	return;
}










void  bsp_Init (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

		//液晶屏亮度
	bsp_lcd_backlight_pwm_init();

	//串口初始化
	bsp_debug_uart2_init();
	
	//及蜂鸣器PWM初始化
	bsp_buzzer_pwm_init();
	
	
//   /*Init USB Host */
//   USBH_Init(&USB_OTG_Core,
//             USB_OTG_HS_CORE_ID,
//             &USB_Host,
//             &USBH_MSC_cb,
//             &USBH_USR_cb);
  
  /*Init the RTC */
  RTC_Configuration();
  
  /* Initialize the Alarm */
  Alarm_Init();
  
}













/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
