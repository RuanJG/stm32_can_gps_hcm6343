#ifndef _CUSTOM_SAMPLEBOX_H_
#define _CUSTOM_SAMPLEBOX_H_

#include <stdint.h>


void samplebox_send_status_packget();
void sample_box_gpio_init();
void sample_box_gpio_init();
void sample_box_id_gpio_init();
void sample_box_led_gpio_init();
void samplebox_valve_init();
unsigned char samplebox_get_valve_status();
void samplebox_set_valve(unsigned int num, unsigned int onoff);
void sample_box_valve_gpio_init();
void samplebox_turn_on_valve34(int num);
void samplebox_turn_off_valve34();
void samplebox_turn_on_valve12(int num);
void samplebox_turn_off_valve12();





void samplebox_loop();
void samplebox_init();
void samplebox_excute_cmd(uint8_t *cmd, uint32_t size);

#endif
