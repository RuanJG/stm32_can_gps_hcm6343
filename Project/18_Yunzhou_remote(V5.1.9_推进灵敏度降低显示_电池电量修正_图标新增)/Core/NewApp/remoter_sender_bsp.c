#include "remoter_sender_jostick.h"
#include "stm32f4xx.h"



remoter_sender_jostick_t _jostick_data;






//#############################  bsp  port
#include "FreeRTOS.h"
#include "task.h"
#include "bsp.h"

xTaskHandle  remoter_sender_Joystick_Task_Handle;

void _jostick_delayms(unsigned int ms)
{
	vTaskDelay(ms); 
}

uint8_t menu_temp = 0;
uint8_t ok_temp = 0;
uint8_t cancel_temp = 0;
uint8_t alarm_temp = 0;
uint8_t sample_temp = 0;
uint8_t modeup_temp = 0;
uint8_t modedown_temp = 0;
uint8_t _key_update_count = 0;

void _update_key_status()
{
	
	_key_update_count ++;
	
		//B_MENU 菜单键采样------------------------------------------
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == 0)
	{
			menu_temp++;
	}

	//B_OK 确认键采样------------------------------------------
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_13) == 0)
	{
		ok_temp++;
	}

	//B_CANCEL 取消键采样------------------------------------------
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12) == 0)
	{
		cancel_temp++;
	}			

	//S_Alarm 警报警灯键采样----------------------------------------------
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0)
	{
		alarm_temp++;
	}	

	//S_Sample 采样键采样----------------------------------------------
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
	{
		sample_temp++;
	}

	//S_MODE up
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		modeup_temp++;
	}

	//S_MODE down
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
	{
		modedown_temp++;
	}
	
	if( _key_update_count >= 5 )
	{
		if( menu_temp >= 4 ) {_jostick_data.button_menu = 1; menu_temp = 0;}else{ _jostick_data.button_menu = 0;}
		if( ok_temp >= 4 ) {_jostick_data.button_ok = 1;ok_temp=0;}else{ _jostick_data.button_ok = 0;}
		if( cancel_temp >= 4 ) {_jostick_data.button_cancel = 1;cancel_temp=0;}else{ _jostick_data.button_cancel = 0;}
		if( alarm_temp >= 4 ) {_jostick_data.key_alarm = 1;alarm_temp=0;}else{ _jostick_data.key_alarm = 0;}
		if( sample_temp >= 4 ) {_jostick_data.key_sample = 1;sample_temp=0;}else{ _jostick_data.key_sample = 0;}
		if( modeup_temp >= 4 ){
			_jostick_data.key_mode = 2;
		}else if( modedown_temp >= 4 ){
			_jostick_data.key_mode = 0;
		}else{
			_jostick_data.key_mode = 1;
		}
		modeup_temp=0;
		modedown_temp=0;
		
		_key_update_count = 0;
	}
	
}





//##################################  command 

void remoter_sender_jostick_adc_dma_TCIF_callback()
{
	if( _jostick_data.updated < 0x7f )
			_jostick_data.updated ++;
}

void _update_adc_values()
{
	if( _jostick_data.updated < 2 )
	{
		return;
	}
	
	_jostick_data.left_x = bsp_Cali_Adc_Value(1);
	_jostick_data.left_y = bsp_Cali_Adc_Value(3);
	_jostick_data.knob = bsp_Cali_Adc_Value(2);
	_jostick_data.right_x = bsp_Cali_Adc_Value(4);
	_jostick_data.right_y = bsp_Cali_Adc_Value(5);
	
	
	_jostick_data.updated = 0;
	
}

void remoter_sender_Joystick_Task(void * pvParameters)
{
	_jostick_data.updated = 0;
	bsp_ADC1_CH6_DMA_Config();
	
	while(1)
	{

		_update_key_status();
	
		_update_adc_values();
		
		_jostick_delayms(10);//10ms
		
	}
	
}

remoter_sender_jostick_t *remoter_sender_jostick_get_data()
{
	return &_jostick_data;
	
}