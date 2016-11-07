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
int32_t Pack_Data(uint8_t *data, uint32_t size, uint8_t *buffer, uint32_t buf_len);
/*
void Send_To_Port(uint8_t *data, uint32_t size, uint8_t port, uint8_t num);
void Handle_Packet_Down(uint8_t *packet, uint32_t size);
*/






typedef struct _protocol_t {
	unsigned char data[MAX_PACKET_SIZE];
	unsigned char error_count;
	unsigned char ok_count;
	unsigned char decode_rate;
	unsigned int index;
	int len;
}protocol_t;
void protocol_init(protocol_t * coder);
/*
encoder data into coder, reutrn 1 ok 0 false;
*/
int protocol_encode(protocol_t * coder, unsigned char* data, int len);
/*
parse a byte for protocol coder
return 0 : no packget reciver
return 1 decoder packget ok , len is coder->len; data is coder->data
*/
int protocol_parse(protocol_t * coder, unsigned char c );









#endif
