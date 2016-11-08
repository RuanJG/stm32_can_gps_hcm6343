#ifndef _CAN_H_
#define _CAN_H_

void CAN1_Init(void);
void CAN1_Send(uint16_t id, uint8_t *data, uint32_t size);

#endif

