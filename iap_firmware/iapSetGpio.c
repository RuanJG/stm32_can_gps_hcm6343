
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "system.h"


#if IAP_FIRMWARE

#if IAP_FIRMWARE_BOARD_NAVIGATION
#include <navigation_box.h>
#endif
#if IAP_FIRMWARE_BOARD_80_ESC
#include <esc_box.h>
#endif

void Iap_GPIO_Configuration (void)
{
	
	
#if IAP_FIRMWARE_BOARD_80_ESC
	SetupPllClock(HSE_CLOCK_6MHZ);
	Esc_GPIO_Configuration();
#endif
#if IAP_FIRMWARE_BOARD_NAVIGATION
	SetupPllClock(HSE_CLOCK_6MHZ);
	Navi_GPIO_Configuration();
#endif
	
	
}





#endif
