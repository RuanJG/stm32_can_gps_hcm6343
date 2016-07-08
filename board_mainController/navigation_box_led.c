#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>

#include "navigation_box_led.h"


void Nbl_Led_on(int id)
{
	if( id == GPS_LED_ID )
		GPIO_ResetBits(LED_GPS_GPIO_BANK,LED_GPS_GPIO_PIN);
	else if (id == COMPASS_LED_ID)
		GPIO_ResetBits(LED_COMPASS_GPIO_BANK,LED_COMPASS_GPIO_PIN);
}
void Nbl_Led_off(int id)
{
		if( id == GPS_LED_ID )
		GPIO_SetBits(LED_GPS_GPIO_BANK,LED_GPS_GPIO_PIN);
	else if (id == COMPASS_LED_ID)
		GPIO_SetBits(LED_COMPASS_GPIO_BANK,LED_COMPASS_GPIO_PIN);
}
bool is_led_on(int id){
	if( id == GPS_LED_ID )
		return (GPIO_ReadOutputDataBit(LED_GPS_GPIO_BANK,LED_GPS_GPIO_PIN) == 0);
	else if (id == COMPASS_LED_ID)
		return (GPIO_ReadOutputDataBit(LED_COMPASS_GPIO_BANK,LED_COMPASS_GPIO_PIN) == 0);
	 
	return FALSE;
}

void Nbl_Led_toggle(int id)
{
	if( is_led_on(id)){
		Nbl_Led_off(id);
	}else{
		Nbl_Led_on(id);
	}
}
void Nbl_Led_Configuration()
{
		//led
		GPIO_InitTypeDef GPIO_InitStructure;	
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LED_GPS_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LED_GPS_GPIO_BANK, &GPIO_InitStructure);
	
	
		GPIO_InitStructure.GPIO_Pin = LED_COMPASS_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LED_COMPASS_GPIO_BANK, &GPIO_InitStructure);
	
		Nbl_Led_off(COMPASS_LED_ID);
		Nbl_Led_off(GPS_LED_ID);
}
					