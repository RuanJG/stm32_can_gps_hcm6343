#ifndef _ESC_BOX_H
#define _ESC_BOX_H

#include <stm32f10x.h>	
#include "stdio.h"	
#include "system.h"


#define MIN_PWM_VALUE 0
#define MAX_PWM_VALUE 200

void Esc_GPIO_Configuration (void);
void esc_pwmA_pwmB_Config();
int esc_set_pwmA(unsigned short pwm);
int esc_set_pwmB(unsigned short pwm);


#endif