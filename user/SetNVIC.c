/******************************************************************
STM32 NVIC Configuration
2010.1.13:creat time
******************************************************************/
#include <stm32f10x.h>

void NVIC_Configuration(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
  	/* Set the Vector Table base location at 0x20000000 */ 
  	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  	/* Set the Vector Table base location at 0x08000000 */ 
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif	 										 

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);			
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   
	NVIC_InitStructure.NVIC_IRQChannel = 39;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
					
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;							 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
//   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
// 	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX1_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure); 

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

}
