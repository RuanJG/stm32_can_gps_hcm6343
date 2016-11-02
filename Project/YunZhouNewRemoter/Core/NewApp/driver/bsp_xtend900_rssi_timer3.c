
#include "bsp_xtend900_rssi_timer3.h"
#include "stm32f4xx.h"
#include "stdio.h"

/* Exported variables ---------------------------------------------------------*/
volatile unsigned int _IC1Value,_IC2Value;
volatile static int _timeout=0;


void bsp_xtend900_rssi_timer3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      		//时钟配置
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					
	
	TIM_PrescalerConfig(TIM3, 1000, TIM_PSCReloadMode_Update);
	//TIM_PrescalerConfig(TIM3, 7200, TIM_PSCReloadMode_Update);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                               //GPIO配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                     //NVIC配置
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
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
  TIM_ITConfig(TIM3, TIM_IT_CC2|TIM_IT_Update, ENABLE);     												//打开中断
}



void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		if( _timeout >= 3 )
		{
			_IC1Value = 0;
			_IC2Value = 0;
		}else
			_timeout++;
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
	if(TIM_GetITStatus(TIM3,TIM_IT_CC2)!=RESET)
	{
		_timeout=0;
		_IC1Value = TIM_GetCapture1(TIM3);
		_IC2Value = TIM_GetCapture2(TIM3);
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
	}
}


int bsp_xtend900_rssi_timer3_get_rssi()
{
	int rdbm;
	int duty;
	
	//printf("ic1=%d,ic2=%d,timeout=%d\r\n",_IC1Value,_IC2Value,_timeout);
  if(_IC2Value != 0)
  {
    duty = (_IC1Value * 100) / _IC2Value;         //读取IC1捕获寄存器的值，并计算占空比
  }
  else
  {
    duty = 0;
  }
	
	rdbm = duty * 2 / 3 - 113;
	return rdbm;
}