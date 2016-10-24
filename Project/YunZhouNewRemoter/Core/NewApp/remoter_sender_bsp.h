#ifndef _REMOTER_SENDER_JOSTICK_H
#define _REMOTER_SENDER_JOSTICK_H

#include "FreeRTOS.h"
#include "task.h"

typedef struct _remoter_sender_jostick_t{
	unsigned char updated;
	
	unsigned char button_menu;
	unsigned char button_ok;
	unsigned char button_cancel;
	
	unsigned char key_mode;
	unsigned char key_sample;
	unsigned char key_alarm;
	
	unsigned short left_x;
	unsigned short left_y;
	unsigned short right_x;
	unsigned short right_y;
	unsigned short knob;
	
	
}remoter_sender_jostick_t;

void remoter_sender_Joystick_Task(void * pvParameters);
remoter_sender_jostick_t *remoter_sender_jostick_get_data();
void remoter_sender_jostick_adc_dma_TCIF_callback();




extern xTaskHandle  remoter_sender_Joystick_Task_Handle;
#endif 