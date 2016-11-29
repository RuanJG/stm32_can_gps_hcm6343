#ifndef _CUSTOM_H_
#define _CUSTOM_H_

#include <stdint.h>


#define CUSTOM_CAN1_ID 0x06
#define CUSTOM_CAN1_MASK 0xff
#define MAIN_CONTROLLER_CAN_ID 0x10
void Excute_Command(uint8_t *cmd, uint32_t size);
void Custom_Init(void);
void Custom_Loop(void);

#endif
