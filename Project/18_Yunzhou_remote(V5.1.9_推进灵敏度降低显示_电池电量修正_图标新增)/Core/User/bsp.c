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
	
	//初始化ADC DMA通道
	ADC1_CH6_DMA_Config();
	ADC_SoftwareStartConv(ADC1);			//开始ADC采样
	
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
