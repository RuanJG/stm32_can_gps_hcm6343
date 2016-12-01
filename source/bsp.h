#ifndef _BSP_CONFIG_H
#define _BSP_CONFIG_H


void bsp_init();


void Can1_Send(uint8_t id, uint8_t *data, int len);
u8 Can1_Configuration_mask(u8 FilterNumber, u16 ID, uint32_t id_type,  u16 ID_Mask , uint8_t sjw ,uint8_t bs1, uint8_t bs2, uint8_t prescale );


void Uart_Configuration (void);
void Uart_send(unsigned *data, int len);





#endif