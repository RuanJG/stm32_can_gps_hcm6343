#include <string.h>
#include "protocol.h"
#include "custom.h"
#include "usart1.h"
#include "can1.h"

uint8_t ComputeCRC8(uint8_t *data, uint32_t size)
{
    uint32_t i = 0;
	uint32_t j = 0;
    uint8_t crc = 0;
    
    for (i = 0; i < size; ++i) {
        crc = crc ^ data[i];
		for (j = 0; j < 8; ++j) {
            if ((crc & 0x01) != 0) {
                crc = (crc >> 1) ^ 0x8C;
		    } else {
                crc >>= 1;
			}
		}
    }
    return crc;  
}

#define APPEND_BYTE(b, d, idx, max) \
do { \
    if(PACKET_START == (b) || PACKET_END == (b) || PACKET_ESCAPE == (b)) \
    { \
        if ((idx) >= max) return -1; \
        (d)[(idx)++] = PACKET_ESCAPE; \
        if ((idx) >= max) return -1; \
        (d)[(idx)++] = (b) ^ PACKET_ESCAPE_MASK; \
    } \
    else \
    { \
        if ((idx) >= max) return -1; \
        (d)[(idx)++] = (b); \
    } \
} while(0)

int32_t Pack_Data(uint8_t *data, uint32_t size, uint8_t *buffer, uint32_t buf_len) {
    uint32_t index = 0;
    uint32_t ret_len = 0;
    uint8_t crc = ComputeCRC8(data, size);
    
    buffer[ret_len++] = PACKET_START;
    for (index = 0; index < size; ++index) {
        APPEND_BYTE(data[index], buffer, ret_len, buf_len);
    }
    APPEND_BYTE(crc, buffer, ret_len, buf_len);
    
    if (ret_len >= buf_len) {
        return -1;
    }
    buffer[ret_len++] = PACKET_END;    
    return ret_len;
}

int32_t Extract_Packet(uint8_t *data, uint32_t size) {
    uint32_t index = 0;
    int32_t pack_len = 0;
    uint8_t crc = 0;
    
    for (index = 0; index < size; ++index) {
        if ((PACKET_START == data[index]) || (PACKET_END == data[index])) {
            continue;
        }
        
        if (PACKET_ESCAPE == data[index]) {
            ++index;
            if (index >= size) {
                return -1;
            }            
            
            data[pack_len] = data[index] ^ PACKET_ESCAPE_MASK;
            ++pack_len;
            continue;
        }
        
        data[pack_len] = data[index];
        ++pack_len;
    }
    
    if (pack_len > 0) {
        crc = ComputeCRC8(data, pack_len - 1);
        if (crc != data[pack_len - 1]) {
            return -1; // CRC error
        }
    }
    
    return pack_len - 1;
}

void Send_To_Port(uint8_t *data, uint32_t size, uint8_t port, uint8_t num) {
    uint16_t canId = 0;
    
    if (PORT_USART == port && 0 == num) { // Send to USART1
        USART1_Send(data, size);
        return;
    }
    
    if (PORT_CAN == port && 0 == num) { // Send to CAN1
        if (size < 3) { // 2 bytes ID + data
            return;
        }
        
        canId = ((data[0] << 8) & 0xFF00)  + (data[1] & 0x00FF);
        CAN1_Send(canId, data + 2, size - 2);
        return;
    }
}

void Handle_Packet_Down(uint8_t *packet, uint32_t size) {
    uint8_t transmit;
    uint8_t port_type;
    uint8_t port_num;
    
    if (size < 2) {
        return;
    }
    
    transmit  = ((packet[0] >> 6) & 0x01);
    port_type = ((packet[0] >> 3) & 0x07);
    port_num  = ( packet[0]       & 0x07);
    
    if (TRANSFER_DONE == transmit) {
        Excute_Command(packet + 1, size - 1);
        return;
    }
    
    Send_To_Port(packet + 1, size - 1, port_type, port_num);
}
