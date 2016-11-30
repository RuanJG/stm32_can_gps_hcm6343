#ifndef _CUSTOM_SAMPLEBOX_H_
#define _CUSTOM_SAMPLEBOX_H_

#include <stdint.h>


void samplebox_loop();
void samplebox_init();
void samplebox_excute_cmd(uint8_t *cmd, uint32_t size);

#endif
