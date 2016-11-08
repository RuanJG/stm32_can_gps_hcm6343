#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>

#define PACKET_START         0xAC
#define PACKET_END           0xAD
#define PACKET_ESCAPE        0xAE
#define PACKET_ESCAPE_MASK   0x80

/********************************************************
| bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
|   0  | next |    port type       |    port number     |


next:        0 for current device, 1 for next device.
port type:   0 for UART, 1 for CAN
port number: 0 for the first device, 1 for the second device ...
*********************************************************/
#define TRANSFER_DONE        0x00
#define TRANSFER_NEXT        0x01

#define PORT_USART           0x00
#define PORT_CAN             0x01

#define MAX_PACKET_SIZE      128

uint8_t ComputeCRC8(uint8_t *data, uint32_t size);
int32_t Extract_Packet(uint8_t *data, uint32_t size);
void Send_To_Port(uint8_t *data, uint32_t size, uint8_t port, uint8_t num);
void Handle_Packet_Down(uint8_t *packet, uint32_t size);
int32_t Pack_Data(uint8_t *data, uint32_t size, uint8_t *buffer, uint32_t buf_len);

#endif
