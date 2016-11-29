#include "systick.h"


#define SysTick_Reloadvalue  8 //9-1


#define SYSTICK_MS_MAX 0x0fffffff
#define SYSTICK_OVERYFLOW_MAX 0x3fffffff
#define u32 unsigned int

volatile u32 TimingDelay = 0;
volatile u32 systick_ms=0;
volatile u32 systick_us=0;
volatile char systick_ms_overflow = 0;


//#define SysTick_CLKSource_HCLK_Div8    ((uint32_t)0xFFFFFFFB)
//#define SysTick_CLKSource_HCLK         ((uint32_t)0x00000004)
void SysTick_Configuration(void)
{

	SysTick->CTRL &= ((uint32_t)0xFFFFFFFB);		//×î¸ß9MHz
	SysTick->LOAD=SysTick_Reloadvalue;
	NVIC_SetPriority(SysTick_IRQn, 0);
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
		systick_ms = 0;//0.0;
	}
}

void delay_us(u32 us)
{
	TimingDelay = us;
	while(TimingDelay != 0);
}

int systick_time_start(systick_time_t *time_t, int ms)
{
	time_t->interval_ms = ms;
	time_t->systick_ms = (systick_ms+ time_t->interval_ms +  (systick_us+200)/1000 );// ((float)systick_us/1000.0+0.5) ) + ;
	time_t->systick_ms_overflow = systick_ms_overflow;
	return 1;
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

u32 get_system_ms()
{
	return systick_ms;
}

//End of File
