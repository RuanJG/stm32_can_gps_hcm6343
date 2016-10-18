/*-------------------------------------------------------------------------
工程名称：定时中断相关程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150704 赵铭章    5.0.0		新建立
																		
					
					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "set_timer.h"


/* Exported variables ---------------------------------------------------------*/
uint32_t IC2Value, DutyCycle, Frequency;		//占空比以及频率
uint8_t RSSI_Interrupt_flag = 1;						//信号强度中断标志


/* Exported function prototypes -----------------------------------------------*/
/*-------------------------------------------------------------------------
	函数名：capture_Timer3_init
	功  能：定时器3初始化			用于获取数传模块信号强度 RSSI
	参  数：
	返回值：
-------------------------------------------------------------------------*/
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
//  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);     												//打开中断
}


/*-------------------------------------------------------------------------
	函数名：TIM3_IRQHandler
	功  能：用于获取数传模块信号强度 RSSI  的PWM
	参  数：
	返回值：
-------------------------------------------------------------------------*/
void TIM3_IRQHandler(void)
{
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);                //清除TIM的中断待处理位
 
  IC2Value = TIM_GetCapture2(TIM3);                         //读取IC2捕获寄存器的值，即为PWM周期的计数值
	
  if(IC2Value != 0)
  {
   
    DutyCycle = (TIM_GetCapture1(TIM3) * 100) / IC2Value;         //读取IC1捕获寄存器的值，并计算占空比
   
    Frequency = 180000 * 120 / (IC2Value * 267);                                          //计算PWM频率
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
	
	RSSI_Interrupt_flag = 1;						//RSSI实现捕获中断，标记为1
	
	//捕捉结束后关中断
	TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
}

