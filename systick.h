#include "stm32f10x.h"
#include "core_cm3.h"


typedef struct _systick_time_t {
	u32 systick_ms;
	u32 systick_ms_overflow;
	u32 interval_ms;
}systick_time_t;

u32 get_system_ms(void);
int check_systick_time(systick_time_t *time_t);
int systick_time_start(systick_time_t *time_t, int ms);
void delay_us(u32 us);
void Systick_Event(void);
void SysTick_Configuration(void);
void SysTick_Deinit(void);

//End of File
