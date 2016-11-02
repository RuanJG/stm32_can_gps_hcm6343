#ifndef _REMOTER_SENDER_UI_H
#define _REMOTER_SENDER_UI_H



#include "FreeRTOS.h"
#include "task.h"
void remoter_sender_UI_Task(void * param);
extern xTaskHandle remoter_sender_UI_Task_Handle;
void remoter_sender_UI_setLog(char *str);












#endif