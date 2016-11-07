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

/*
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
*/






/*
		protocol_get_decode_rate
		返回解包的成功率：如返回80 ， 指80%的解包成功
*/
int _protocol_update_decode_rate(protocol_t *coder)
{
	int res  = coder->ok_count*100/(coder->ok_count+coder->error_count);
	coder->ok_count =0;
	coder->error_count =0;
	return res;
}


void _protocol_check_decode_rate(protocol_t * coder)
{
	if( 10 <= (coder->error_count+coder->ok_count) )
	{
		coder->decode_rate = _protocol_update_decode_rate(coder);
	}
}


/*
*  parse a byte for protocol coder
return 0 : no packget reciver
return 1 decoder packget ok , len is coder->len; data is coder->data
*
*/
int protocol_parse(protocol_t * coder, unsigned char c )
{
	if( coder->index >= MAX_PACKET_SIZE )
	{
		coder->error_count ++;
		coder->index = 0;
		_protocol_check_decode_rate(coder);
	}
	
	if( coder->index == 0 )
	{// in get PACKET_START loop
		if( c == PACKET_START )
		{
			coder->data[coder->index++] = c;
		}
	}else{
		//in get PACKET_END loop
		if( c == PACKET_END )
		{
			coder->data[coder->index++] = c;
			coder->len = Extract_Packet(coder->data,coder->index);
			if( coder->len >= 0 ){
				coder->ok_count ++;
				_protocol_check_decode_rate(coder);
				coder->index = 0; // prepare for next packget
				return 1;
			}else{
				coder->error_count ++;
				coder->index = 0;
				_protocol_check_decode_rate(coder);
				return 0;
			}
		}else{
			// or get data loop
			coder->data[coder->index++] = c;
		}
	}
	return 0;
}

/*
encoder data into coder, reutrn 1 ok 0 false;
*/
int protocol_encode(protocol_t * coder, unsigned char* data, int len)
{
	int res;
	
	if( len > MAX_PACKET_SIZE )
		return 0;
	
	res = Pack_Data(data, len, coder->data, MAX_PACKET_SIZE);
	if( res < 0 ){
		return 0;
	}
	
	coder->len = res;
	return 1;
}

void protocol_init(protocol_t * coder)
{
	coder->decode_rate = 0;
	coder->error_count = 0;
	coder->ok_count = 0;
	coder->index = 0;
	coder->len = 0;
}

