/*
 * cmdcoder.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: roamer
 */


#include "cmdcoder.h"


static int encode_variable_len(unsigned int value, unsigned char *buf, int size) {
	int offset = 0;
	do {
		//if (offset >= size) {
		// event 0 data , encode len too
		if (offset > size) {
			return -1;
		}

		buf[offset] = value % 128;
		value = value / 128;

		if (value > 0) {
			buf[offset] |= 128;
		}

		++offset;
	} while (value > 0);

	return offset;
}

void cmdcoder_init(cmdcoder_t* packget, unsigned char id, encodeSendCallback sendCallback)
{
	packget->parse_status = FIND_TAG;
	packget->last_byte_is_tag = 0;
	packget->len_multi = 1;
	packget->len = 0;
	packget->index = 0;
	packget->sum_crc = 0;
	packget->send_cb = sendCallback;
	packget->id = id;
}


void _cmdcoder_count_false_byte(cmdcoder_t* packget, int count) 
{
	if ( packget->false_bytes_count < 0x3df00000 ) //1G
		packget->false_bytes_count += count;
}




void _cmdcoder_count_all_byte(cmdcoder_t* packget, int count) 
{
	if ( packget->all_bytes_count < 0x3df00000 ) //1G
		packget->all_bytes_count += count;
}



void _cmdcoder_update_decode_rate(cmdcoder_t* packget)
{
	int rate ;
	if( packget->all_bytes_count > 10 )
	{
		rate =  100 - ( 100 * packget->false_bytes_count / packget->all_bytes_count );
		packget->false_bytes_count = 0;
		packget->all_bytes_count = 0;
		packget->decode_byte_rate = rate;
	}
}




// let the user call this update
int cmdcoder_update_Decode_Byte_Rate(cmdcoder_t* packget)
{
	_cmdcoder_update_decode_rate(packget);
	return packget->decode_byte_rate ;
}




int cmdcoder_Parse_byte(cmdcoder_t* packget,unsigned char pbyte )
{
	unsigned char  decode_a_packget = 0;
	unsigned char decode_failed = 0;
	
	_cmdcoder_count_all_byte(packget,1);
	
	switch( packget->parse_status ){
		case FIND_DONE:{
			cmdcoder_init(packget,packget->id,packget->send_cb);
		}
		case FIND_TAG:{
			if( pbyte != CMD_CODER_TAG ){
				_cmdcoder_count_false_byte(packget, packget->last_byte_is_tag+1);
				packget->last_byte_is_tag=0;
			}else {
				if( packget->last_byte_is_tag == 1 ){
					packget->parse_status++; // found tag,next step
					packget->last_byte_is_tag = 0; //id no need to check tag last byte
					packget->tmp_bytes_index = packget->all_bytes_count;
				}else{
					packget->last_byte_is_tag=1; //find next 0xff next time
				}
			}
			break;
		}
		
		case FIND_ID:{
			if( packget->last_byte_is_tag == 1 ){
				//ignore this byte or find a error
				if( pbyte == 0x00 ){
					packget->last_byte_is_tag = 0;
				}else{
					decode_failed = 1;
				}
				break;
			}else if( pbyte == CMD_CODER_TAG){
				packget->last_byte_is_tag=1;
			}
			
			//add for check sum
			packget->sum_crc += pbyte;
			
			//use this byte for id
			packget->id = pbyte;
			packget->parse_status++;
			break;
		}
		
		case FIND_LEN:{
			if( packget->last_byte_is_tag == 1 ){
				//ignore this byte or find a error
				if( pbyte == 0x00 ){
					packget->last_byte_is_tag = 0;
				}else{
					decode_failed = 1;
				}
				break;
			}else if( pbyte == CMD_CODER_TAG){
				packget->last_byte_is_tag=1;
			}
			
			//add for check sum
			packget->sum_crc += pbyte; 
			
			//decode len
			packget->len += (pbyte & 0x7f)* packget->len_multi ;
			if( (pbyte & 0x80) == 0 ){
				//len decoce finish
				packget->parse_status++;
				if( packget->len == 0 )
					packget->parse_status++; //ignore data collect
			}else{
				packget->len_multi *= 128;
			}
			if( packget->len > CMD_CODER_MAX_DATA_LEN ){
				//check len for too big or data error (检查长度，和，防止数据传输时被修改的可能)
				decode_failed=1;
			}
			break;
		}
		
		case FIND_DATA:{
			if( packget->last_byte_is_tag == 1 ){
				//ignore this byte or find a error
				if( pbyte == 0x00 ){
					packget->last_byte_is_tag = 0;
				}else{
					decode_failed = 1;
				}
				break;
			}else if( pbyte == CMD_CODER_TAG){
				packget->last_byte_is_tag=1;
			}
			
			//add for check sum
			packget->sum_crc += pbyte;
			
			//fill in buff
			packget->data[packget->index++]=pbyte;
			if( packget->index >= packget->len ){
				packget->parse_status++; //ok ,next to crc
				packget->index=0; //for crc
			}
			break;
		}
		
		case FIND_CRC:{
			if( packget->last_byte_is_tag == 1 ){
				//ignore this byte or find a error
				if( pbyte == 0x00 ){
					packget->last_byte_is_tag = 0;
				}else{
					decode_failed = 1;
				}
				break;
			}else if( pbyte == CMD_CODER_TAG){
				packget->last_byte_is_tag=1;
			}
			
			// check crc
			if( pbyte != packget->sum_crc ){
				decode_failed = 1;
			}else{
				decode_a_packget = 1;
				packget->parse_status++; //ok ,next packget
				packget->decode_ok_count++;
			}
			break;
		}
	}
	
	if( decode_failed == 1 ){
		packget->decode_false_count++;
		_cmdcoder_count_false_byte(packget, packget->all_bytes_count - packget->tmp_bytes_index);
		cmdcoder_init(packget,packget->id,packget->send_cb);
	}
	
	if( (packget->decode_false_count + packget->decode_ok_count) >= 10 )
	{
		packget->decode_rate =  packget->decode_ok_count*10; // 8/10=80% 
		packget->decode_false_count = 0;
		packget->decode_ok_count = 0;
	}
	
	return decode_a_packget;
}




//encode packget->data&&packget->id,  and send it by packget->send_cb
//return count of bytes in encode frame;  if count < 0 error;  if count ==0 send_callback not set; 
int cmdcoder_encode_and_send(cmdcoder_t* packget){
	int offset=0;
	int i;
	unsigned char sendbuff[8];//len buff , it can not bigger than 8byte
	unsigned char sum=0;
	unsigned int count = 0;
	
	if( packget->send_cb == CMD_CODER_CALL_BACK_NULL )
		return 0;
	
	#define Check_and_Send(byte) \
	do{ \
		packget->send_cb(byte); \
		count++; \
		if( byte == CMD_CODER_TAG){ \
			packget->send_cb(0x00); \
			count++; \
		} \
	}while(0)
	
	packget->send_cb(CMD_CODER_TAG);
	packget->send_cb(CMD_CODER_TAG);
	count+=2;

	//id
	sum += packget->id;
	Check_and_Send(packget->id);
	//len
	offset = encode_variable_len(packget->len,sendbuff,8);
	if( offset <= 0 ) 
		return -1;
	for( i=0; i< offset; i++){
		Check_and_Send(sendbuff[i]);
		sum += sendbuff[i];
	}
	//data
	for( i=0; i< packget->len; i++){
		Check_and_Send(packget->data[i]);
		sum += packget->data[i];
	}
	//crc
	Check_and_Send(sum);
	//packget->sum_crc = sum;
	
	return count;
}





void cmdcoder_send_bytes(cmdcoder_t* packget, unsigned char *data,int len)
{
	int i;
	
	//packget.id = id;
	packget->len = 0;
	for( i=0; i<  len; i++)
	{
		packget->data[i] = data[i];
		packget->len++;
		if( packget->len >= CMD_CODER_MAX_DATA_LEN ){
			cmdcoder_encode_and_send( packget );
			packget->len = 0;
		}
	}
	if( packget->len > 0 ){
			cmdcoder_encode_and_send( packget );
			packget->len = 0;
	}
}



