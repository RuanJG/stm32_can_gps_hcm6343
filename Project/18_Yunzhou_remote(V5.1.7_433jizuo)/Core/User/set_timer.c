/*-------------------------------------------------------------------------
�������ƣ���ʱ�ж���س���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150704 ������    5.0.0		�½���
																		
					
					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "set_timer.h"


/* Exported variables ---------------------------------------------------------*/
uint32_t IC2Value, DutyCycle, Frequency;		//ռ�ձ��Լ�Ƶ��
uint8_t RSSI_Interrupt_flag = 1;						//�ź�ǿ���жϱ�־


/* Exported function prototypes -----------------------------------------------*/
/*-------------------------------------------------------------------------
	��������capture_Timer3_init
	��  �ܣ���ʱ��3��ʼ��			���ڻ�ȡ����ģ���ź�ǿ�� RSSI
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void capture_Timer3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      		//ʱ������
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);					
	
	TIM_PrescalerConfig(TIM3, 1000, TIM_PSCReloadMode_Update);

	/* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                               //GPIO����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                     //NVIC����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
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
//  TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);     												//���ж�
}


/*-------------------------------------------------------------------------
	��������TIM3_IRQHandler
	��  �ܣ����ڻ�ȡ����ģ���ź�ǿ�� RSSI  ��PWM
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void TIM3_IRQHandler(void)
{
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);                //���TIM���жϴ�����λ
 
  IC2Value = TIM_GetCapture2(TIM3);                         //��ȡIC2����Ĵ�����ֵ����ΪPWM���ڵļ���ֵ
	
  if(IC2Value != 0)
  {
   
    DutyCycle = (TIM_GetCapture1(TIM3) * 100) / IC2Value;         //��ȡIC1����Ĵ�����ֵ��������ռ�ձ�
   
    Frequency = 180000 * 120 / (IC2Value * 267);                                          //����PWMƵ��
  }
  else
  {
    DutyCycle = 0;
    Frequency = 0;
  }
	
	RSSI_Interrupt_flag = 1;						//RSSIʵ�ֲ����жϣ����Ϊ1
	
	//��׽��������ж�
	TIM_ITConfig(TIM3, TIM_IT_CC2, DISABLE);
}

