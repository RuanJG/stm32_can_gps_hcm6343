#include "custom.h"
#include "custom_samplebox.h"






void Excute_Command(uint8_t *cmd, uint32_t size) {
	
	samplebox_excute_cmd(cmd,size);
}

void Custom_Init()
{
	SysTick_Configuration();
	samplebox_init();
}

void Custom_Loop()
{
	Systick_Event();
	samplebox_loop();
	
}
