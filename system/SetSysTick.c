#include "misc.h"
#include "core_cm3.h"
#include "system.h"

#define SysTick_Reloadvalue  8 //9-1


#define SYSTICK_MS_MAX 0x0fffffff
#define SYSTICK_OVERYFLOW_MAX 0x3fffffff
volatile u32 TimingDelay = 0;
volatile u32 systick_ms=0;
volatile u32 systick_us=0;
volatile char systick_ms_overflow = 0;

void SysTick_Configuration(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);		//×î¸ß9MHz

	SysTick->LOAD=SysTick_Reloadvalue;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_SetPriority(SysTick_IRQn, 0);//230
	SysTick->CTRL|=0x02;
	SysTick->CTRL|=0x01;
}

void SysTick_Deinit(void)
{
	SysTick->CTRL &= ~0x02;
	SysTick->CTRL &= ~0x01;
}

void SysTick_Handler(void)
{
	systick_us++;
	
	/*
	if( systick_us >= 1000 ){
		systick_ms ++;
		systick_us =0;
	}
	*/
	if(TimingDelay > 0)
	{
		TimingDelay--;
	}
}

void Systick_Event()
{
	//this event for check the systick_us overflow 
	int ms ;
	if( systick_us >= 1000 ){
		ms = systick_us/1000;
		systick_ms += ms;
		systick_us -= ms*1000;
	}
	if( systick_ms >= SYSTICK_MS_MAX ){
		systick_ms_overflow ++;
		systick_ms_overflow %= SYSTICK_OVERYFLOW_MAX;
		systick_ms = 0.0;
	}
}

void delay_us(u32 us)
{
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//NVIC_SetPriority(SysTick_IRQn, 0);
	//SysTick->CTRL|=0x01;
	TimingDelay = us;
	while(TimingDelay != 0);
	//SysTick->CTRL&=0xFFFE;
	//SysTick->VAL =0x00000000;
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//NVIC_SetPriority(SysTick_IRQn, 230);	
}
/*
float get_system_s()
{
	return (systick_s+(float)systick_us/1000000.0);
}
*/
int systick_time_start(systick_time_t *time_t, int ms)
{
	time_t->interval_ms = ms;
	time_t->systick_ms = (systick_ms+ time_t->interval_ms +  (systick_us+200)/1000 );// ((float)systick_us/1000.0+0.5) ) + ;
	time_t->systick_ms_overflow = systick_ms_overflow;
}
int check_systick_time(systick_time_t *time_t)
{
	char force_update = 0;
	unsigned int now_ms =systick_ms+ (systick_us+200)/1000;// ((float)systick_us/1000.0+0.5) ;
	
	if( systick_ms_overflow != time_t->systick_ms_overflow ){
		time_t->systick_ms_overflow = systick_ms_overflow;
		force_update = 1;
	}		
	if( force_update || now_ms >= time_t->systick_ms ){
		time_t->systick_ms = now_ms + time_t->interval_ms ;
		return 1;
	}
	return 0;
}

//End of File
