#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>

void main_setup();
void main_loop();

int main(void)														 
{		
	/*
	vect tab setting compile in iap or bootloader
#ifdef  VECT_TAB_RAM  
  	// Set the Vector Table base location at 0x20000000  
  	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  // VECT_TAB_FLASH  
  	// Set the Vector Table base location at 0x08000000 
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif	
	*/

	NVIC_PriorityGroupConfig(CUSTOM_IRQ_GROUP);
	system_error_init();
	SysTick_Configuration();
	main_setup();
	
	while(1)
	{
		main_loop();
		Systick_Event();
	}
}


/*
__asm void wait()
{
	BX LR
}
void HardFault_Handler(void)
{
    // Go to infinite loop when Hard Fault exception occurs 
       wait();
}
*/



//End of file


 			  
