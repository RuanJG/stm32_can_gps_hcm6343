#ifndef _BSP_BUZZER_PWM_H
#define _BSP_BUZZER_PWM_H

void bsp_buzzer_pwm_set_level(unsigned int level);
void bsp_buzzer_pwm_init(void);
void bsp_buzzer_pwm_set_voice(unsigned int freq, unsigned int level);

#endif