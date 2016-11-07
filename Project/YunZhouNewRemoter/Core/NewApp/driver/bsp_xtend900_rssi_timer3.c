
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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      		//ʱ������
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					
	
	TIM_PrescalerConfig(TIM3, 1000, TIM_PSCReloadMode_Update);
	//TIM_PrescalerConfig(TIM3, 7200, TIM_PSCReloadMode_Update);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                               //GPIO����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                     //NVIC����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;                   //ͨ��ѡ��
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       	//�����ش���
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;    	//�ܽ���Ĵ�����Ӧ��ϵ
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;								//����Ԥ��Ƶ����˼�ǿ����ڶ��ٸ�����������һ�β�����������ź�Ƶ��û�б䣬���
	//������Ҳ����䣬����ѡ��4��Ƶ����ÿ�ĸ��������ڲ���һ�β��������������źŲ�Ƶ��������£����Լ�������������жϵĴ���
	
	TIM_ICInitStructure.TIM_ICFilter = 0x0;                            //�˲����ã������������������϶������ȶ�0x0~0xf
  TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);                 //���ݲ�������TIM������Ϣ
  TIM_SelectInputTrigger(TIM3, TIM_TS_TI2FP2);                //ѡ��IC2Ϊʼ�մ���Դ
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);								//TIM��ģʽ�������źŵ����������³�ʼ���������ʹ����Ĵ����ĸ����¼�
  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable); 				//������ʱ���ı�������
	
  TIM_Cmd(TIM3, ENABLE);                                 							//����TIM3      
  TIM_ITConfig(TIM3, TIM_IT_CC2|TIM_IT_Update, ENABLE);     												//���ж�
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
    duty = (_IC1Value * 100) / _IC2Value;         //��ȡIC1����Ĵ�����ֵ��������ռ�ձ�
  }
  else
  {
    duty = 0;
  }
	
	rdbm = duty * 2 / 3 - 113;
	return rdbm;
}