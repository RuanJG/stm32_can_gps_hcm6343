#ifndef _CMD_CODER_H
#define _CMD_CODER_H



/*
*
* Cmd COder : tag[2]{0xff,0xff}+id[1]{0x0~0x7f]}+len[0~4]{0~CMD_CODER_MAX_DATA_LEN}+data[len~2*len]+crc[1]
*
*/

#define CMD_CODER_MAX_DATA_LEN 128
// tag[2]+id[1]+len[1~4]+data[len~2*len]+crc[1]
#define CMD_CODER_MAX_PACKGET_LEN (2+1+4+CMD_CODER_MAX_DATA_LEN*2+1)
#define CMD_CODER_TAG 0xff

//for encodeSendCallback fucntion , if NULL set it NULL or CMD_CODER_CALL_BACK_NULL
#ifndef NULL
#define CMD_CODER_CALL_BACK_NULL 0
#else
#define CMD_CODER_CALL_BACK_NULL NULL
#endif


enum CmdCoderStep{
	FIND_TAG=0,
	FIND_ID,
	FIND_LEN,
	FIND_DATA,
	FIND_CRC,
	FIND_DONE
};

//encodeSendCallback , send encoded byte in encode function, if fail return 0 ,else 1
typedef int (*encodeSendCallback) ( unsigned char c );
typedef struct _cmdcoder_t{
	//a frame 
	unsigned int len; //default = 0
	unsigned char data[CMD_CODER_MAX_DATA_LEN];
	unsigned char id;
	unsigned char sum_crc; // defult = 0
	//for parse
	char parse_status; // defalut= CmdCoderStep::FIND_TAG
	char last_byte_is_tag; // defalt=0
	unsigned int len_multi ; // defalut = 1 , step 128
	unsigned int index; //default = 0, <PROTOCOL_MAX_DATA_LEN ;
	encodeSendCallback send_cb;
	//decode rate //½âÂëÂÊ
	 unsigned int all_bytes_count;
	 unsigned int tmp_bytes_index;
	 unsigned int false_bytes_count;
	 unsigned char decode_ok_count;
	 unsigned char decode_false_count;
	int decode_rate;
	int decode_byte_rate;
}cmdcoder_t;

void cmdcoder_init(cmdcoder_t* packget, unsigned char id, encodeSendCallback sendCallback);
int cmdcoder_Parse_byte( cmdcoder_t* packget, unsigned char pbyte);
int cmdcoder_encode_and_send(cmdcoder_t* packget);
void cmdcoder_send_bytes(cmdcoder_t* packget, unsigned char *data,int len);
int cmdcoder_update_Decode_Byte_Rate(cmdcoder_t* packget);

#define MAINCONTROLLER_BOX_CMDCODER_ID 1
#define NAVIGATION_BOX_CMDCODER_ID 2
#define ESC_BOX_CMDCODER_ID 3

/*
**********************************  example 

cmdcoder_t encode_packget;
cmdcoder_t decode_packget;
int test_code_res = 0;
int TEST_DATA_LEN =0;

int test_send_cb( unsigned char c)
{
	int i;
	if( cmdcoder_Parse_byte(&decode_packget,c) ){
		if( decode_packget.len == encode_packget.len && 
				decode_packget.id == encode_packget.id ){
				if( decode_packget.len > 0 ){
					for( i=0 ;i < decode_packget.len; i++){
						if( decode_packget.data[i] != encode_packget.data[i] )
							return 1;
					}
				}
					test_code_res = 1;
				}
	}
	return 1;
}

void test_protocol()
{
	int i;
	
	cmdcoder_init(&encode_packget, test_send_cb);
	cmdcoder_init(&decode_packget, CMD_CODER_CALL_BACK_NULL);
	
	//for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= (i%2 != 0) ? 0xff : 0 ;
	//for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= (i%2 != 0) ? 0xff : i ;
	//for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= 0xff;
	for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= 0xff-i;
	
	encode_packget.len = TEST_DATA_LEN;
	encode_packget.id = TEST_DATA_LEN % 0x7f;
	cmdcoder_encode_and_send(&encode_packget);
	if( test_code_res == 1){
		led_toggle(GPS_LED_ID);
		test_code_res = 0;
	}else{
		led_off(GPS_LED_ID);
		led_off(COMPASS_LED_ID);
		while(1);
	}
	TEST_DATA_LEN++;
	TEST_DATA_LEN = (TEST_DATA_LEN)%CMD_CODER_MAX_DATA_LEN;
}



*/







#endif//_CMD_CODER_H