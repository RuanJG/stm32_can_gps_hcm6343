#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "esc_box.h"

#define LED_RED_BANK GPIOC
#define LED_RED_PIN GPIO_Pin_14
#define LED_GREEN_BANK GPIOC
#define LED_GREEN_PIN GPIO_Pin_15
#define LED_YELLOW_BANK GPIOC
#define LED_YELLOW_PIN GPIO_Pin_13

#define LED_MAX_COUNT 3
static int led_toggle_ms[LED_MAX_COUNT]={0};

void _Esc_Led_on(int id)
{
	if( id == LED_RED_ID )
		GPIO_ResetBits(LED_RED_BANK,LED_RED_PIN);
	else if (id == LED_GREEN_ID)
		GPIO_ResetBits(LED_GREEN_BANK,LED_GREEN_PIN);
	else if ( id == LED_YELLOW_ID)
		GPIO_ResetBits(LED_YELLOW_BANK,LED_YELLOW_PIN);
}
void _Esc_Led_off(int id)
{
	if( id == LED_RED_ID )
		GPIO_SetBits(LED_RED_BANK,LED_RED_PIN);
	else if (id == LED_GREEN_ID)
		GPIO_SetBits(LED_GREEN_BANK,LED_GREEN_PIN);
	else if ( id == LED_YELLOW_ID)
		GPIO_SetBits(LED_YELLOW_BANK,LED_YELLOW_PIN);
}
static bool is_led_on(int id){
	if( id == LED_RED_ID )
		return (GPIO_ReadOutputDataBit(LED_RED_BANK,LED_RED_PIN) == 0);
	else if (id == LED_GREEN_ID)
		return (GPIO_ReadOutputDataBit(LED_GREEN_BANK,LED_GREEN_PIN) == 0);
	else if ( id == LED_YELLOW_ID)
		return (GPIO_ReadOutputDataBit(LED_YELLOW_BANK,LED_YELLOW_PIN) == 0);
	return FALSE;
}

void Esc_Led_toggle(int id)
{
	if( is_led_on(id)){
		_Esc_Led_off(id);
	}else{
		_Esc_Led_on(id);
	}
}

static volatile int led_count = 0; // 
void Esc_Led_set_toggle(int id, int ms)
{
	led_toggle_ms[id] = ms;
}
void Esc_Led_Event()
{
	int i;
	led_count++;
	if(led_count > 1000000) led_count = 0;
	for( i=0; i< LED_MAX_COUNT; i++){
		if( led_toggle_ms[i] > 0 ){
			if( led_count%led_toggle_ms[i] == 0 )
				Esc_Led_toggle(i);
		}			
	}
}
void Esc_Led_on(int id)
{
	_Esc_Led_on(id);
	Esc_Led_set_toggle(id , 0);
}
void Esc_Led_off(int id)
{
	_Esc_Led_off(id);
	Esc_Led_set_toggle(id , 0);
}
void Esc_Led_Configuration()
{
		//led
		GPIO_InitTypeDef GPIO_InitStructure;	

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED_RED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_RED_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED_GREEN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GREEN_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LED_YELLOW_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_YELLOW_BANK, &GPIO_InitStructure);
	
	Esc_Led_off(LED_RED_ID);
	Esc_Led_off(LED_YELLOW_ID);
	Esc_Led_off(LED_GREEN_ID);
}
					