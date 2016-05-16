#include "misc.h"
#include "core_cm3.h"


#define SysTick_Reloadvalue  9

volatile u32 TimingDelay = 0;

void SysTick_Configuration(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);		//×î¸ß9MHz

	SysTick->LOAD=SysTick_Reloadvalue;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_SetPriority(SysTick_IRQn, 230);
	SysTick->CTRL|=0x02; 
}

void SysTick_Handler(void)
{
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}

void delay_us(u32 us)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_SetPriority(SysTick_IRQn, 0);
	SysTick->CTRL|=0x01;
	TimingDelay = us;
	while(TimingDelay != 0);
	SysTick->CTRL&=0xFFFE;
	SysTick->VAL =0x00000000;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_SetPriority(SysTick_IRQn, 230);	
}

//End of File
