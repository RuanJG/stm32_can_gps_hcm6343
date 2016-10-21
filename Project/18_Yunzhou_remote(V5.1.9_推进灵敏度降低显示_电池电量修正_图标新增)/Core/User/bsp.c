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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TS_Pressed;
__IO uint32_t TS_Orientation = 0;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE            USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST                      USB_Host __ALIGN_END;


/* Private function prototypes -----------------------------------------------*/
uint32_t BSP_TS_Init(void);
void Alarm_Init(void);

/* Private functions ---------------------------------------------------------*/
/*-------------------------------------------------------------------------
	函数名：LCD_PWM_Config
	功  能：液晶屏PWM调节
	参  数：status 表示液晶开关状态，0或DISABLE表示关液晶背光灯，1或ENABLE表示根据所设置的参数进行配置
	返回值：
-------------------------------------------------------------------------*/
void LCD_PWM_Config(uint8_t status)
{
	//PWM GPIO配置
	//PF8初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
//  GPIO_Init(GPIOF, &GPIO_InitStructure);
//  
//  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	
	//PF6初始化
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_TIM10);	
	
	//定时器13作PF8的PWM输出-------------------------------	
//	int TimerPeriod =  (SystemCoreClock / 600 ) - 1;
	int TimerPeriod =  (SystemCoreClock / 30000 );
  int ccr1 = TimerPeriod / 2;  	//50%
	int ccr2;
	
	if(status == DISABLE)
	{
		ccr2 = 0;
	}
	else
	{
		ccr2 = TimerPeriod / (local_para_conf[0] * 2 + 1);  
	}
	   
//	int ccr2 = TimerPeriod / 1;  	//33%
  int ccr3 = TimerPeriod / 4;  	//25%
  int ccr4 = TimerPeriod / 5;  	//20%
  
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
//  //时基初始化
//  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
//  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
//  TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
//  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
//  TIM_TimeBaseInitStructure.TIM_Period = TimerPeriod - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
//  TIM_TimeBaseInit(TIM13, &TIM_TimeBaseInitStructure);

//  
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = ccr1;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
//  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
//  
//  TIM_OC1Init(TIM13, &TIM_OCInitStructure);

////  TIM_OCInitStructure.TIM_Pulse = ccr2;
////  TIM_OC2Init(TIM1,&TIM_OCInitStructure);
////  
////  TIM_OCInitStructure.TIM_Pulse = ccr3;
////  TIM_OC3Init(TIM1,&TIM_OCInitStructure);
////  
////  TIM_OCInitStructure.TIM_Pulse = ccr4;
////  TIM_OC4Init(TIM1,&TIM_OCInitStructure);
//  
//  TIM_Cmd(TIM13, ENABLE);
//  TIM_CtrlPWMOutputs(TIM13, ENABLE);


	//定时器10作PF6的PWM输出-------------------------------
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
  //时基初始化
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
  TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
  TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInitStructure.TIM_Period = TimerPeriod - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseInitStructure);

  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ccr2;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
  
  TIM_OC1Init(TIM10, &TIM_OCInitStructure);

//  TIM_OCInitStructure.TIM_Pulse = ccr2;
//  TIM_OC2Init(TIM1,&TIM_OCInitStructure);
//  
//  TIM_OCInitStructure.TIM_Pulse = ccr3;
//  TIM_OC3Init(TIM1,&TIM_OCInitStructure);
//  
//  TIM_OCInitStructure.TIM_Pulse = ccr4;
//  TIM_OC4Init(TIM1,&TIM_OCInitStructure);
  
  TIM_Cmd(TIM10, ENABLE);
  TIM_CtrlPWMOutputs(TIM10, ENABLE);
}


/*-------------------------------------------------------------------------
	函数名：Buzzer_PWM_Config
	功  能：蜂鸣器PWM调节
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void Buzzer_PWM_Config(void)
{
	//PWM GPIO配置
	//PF8初始化
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	
	if(local_para_conf_buf[5] == 0)
	{
		TIM_Cmd(TIM13, DISABLE);
		TIM_CtrlPWMOutputs(TIM13, DISABLE);
	}	

	else
	{
		//定时器13作PF8的PWM输出-------------------------------	
//		int TimerPeriod =  (SystemCoreClock / 543) - 1;
		int TimerPeriod = (SystemCoreClock / 581 );
		int ccr1 = TimerPeriod / 2;  	//50%
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
		
		//时基初始化
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//死区控制用
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向
		TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   //Timer clock = sysclock /(TIM_Prescaler+1) = 180M
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
		TIM_TimeBaseInitStructure.TIM_Period = TimerPeriod - 1;    //Period = (TIM counter clock / TIM output clock) - 1 = 20K
		TIM_TimeBaseInit(TIM13, &TIM_TimeBaseInitStructure);

		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_Pulse = ccr1;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
		
		TIM_OC1Init(TIM13, &TIM_OCInitStructure);		

		TIM_Cmd(TIM13, ENABLE);
		TIM_CtrlPWMOutputs(TIM13, ENABLE);
	}
}


/**
* @brief  Inititialize the target hardware.
* @param  None
* @retval 0: if all initializations are OK.
*/
uint32_t LowLevel_Init (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	//液晶屏幕点亮GPIO配置
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOF, GPIO_Pin_6);			//关闭液晶屏，避免初始化时的白屏现象	


	//Xtend 900 CMD引脚GPIO配置
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
//	
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//  GPIO_Init(GPIOI, &GPIO_InitStructure);	
//	
//	GPIO_ResetBits(GPIOI, GPIO_Pin_7);			//关闭Xtend 900 的CMD
	
  /* LCD initialization */
  LCD_Init();
  
  /* LCD Layer initialization */
  LCD_LayerInit();
    
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
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
  
  /* Enable the CRC Module */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
	
	/*Init Touchscreen */
	BSP_TS_Init();
	
	//串口初始化
	mCOMInit();
	
//	//液晶屏亮度及蜂鸣器PWM初始化
//	void PWM_Config(void);
	

	
	//RSSI信号PWM捕捉初始化
	capture_Timer3_init();
  
  return 0;
}


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
	
	//液晶屏幕点亮GPIO配置
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOF, GPIO_Pin_6);			//关闭液晶屏，避免初始化时的白屏现象	
	
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
void  bsp_Init (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;


	//Xtend 900 CMD引脚GPIO配置
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE);
//	
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//  GPIO_Init(GPIOI, &GPIO_InitStructure);	
//	
//	GPIO_ResetBits(GPIOI, GPIO_Pin_7);			//关闭Xtend 900 的CMD
	

	
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
  

	
	//串口初始化
	mCOMInit();
	
//	//液晶屏亮度及蜂鸣器PWM初始化
//	void PWM_Config(void);
	

	
	//RSSI信号PWM捕捉初始化
	capture_Timer3_init();
  
}




#define _BSP_SAVE_START_ADDRESS 	0x080e0000					//参数存储地址
#define _BSP_FIRST_SAVE	0xffffffff			//没存入参数前的数据

void save_parameter_to_flash(void)
{
	uint32_t address = _BSP_SAVE_START_ADDRESS;
	uint16_t temp_i;

	//解锁FLASH后才能向FLASH中写数据
	FLASH_Unlock(); 			
	
	//清状态标记
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);	
	
	//擦除FLASH
	while(FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3) != FLASH_COMPLETE);
	
	//写入RF的8个参数
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		while(FLASH_ProgramWord(address, Xtend_900_para_buf[temp_i]) != FLASH_COMPLETE);
		address = address + 4;
	}
	
	//锁定FLASH
	FLASH_Lock();	
	
	return;
}


/**
  * @brief  load_parameter 参数读出函数
  * @param  
  * @retval 
  */
void load_parameter_from_flash(void)
{
	uint32_t address = SAVE_START_ADDRESS;
	uint16_t temp_i;

	//判断是否已经存入信道参数，没存入则不从flash中读出，而是用默认参数
	if(*(__IO uint32_t*)address != FIRST_SAVE)
	{
		//读出RF的8个参数
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			Xtend_900_para[temp_i] = *(__IO uint32_t*)address;
			address = address + 4;
		}
	}
		
	return;
}









/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
