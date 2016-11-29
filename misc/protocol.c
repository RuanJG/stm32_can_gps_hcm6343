#include <string.h>
#include "protocol.h"

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


//encode data to buffer ; return len of valid data in buffer , -1 error
int32_t protocol_encode(uint8_t *data, uint32_t size, uint8_t *buffer, uint32_t buf_len) {
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



//decode data[size], and return len of data decoded(without head tag and crc),  -1 error
int32_t protocol_decode(uint8_t *data, uint32_t size) {
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
