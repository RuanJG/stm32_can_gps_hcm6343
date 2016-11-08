#ifndef _USART1_H_
#define _USART1_H_

void USART1_Init(int32_t bitrate);
void USART1_Send(uint8_t *data, uint32_t size);

#endif
