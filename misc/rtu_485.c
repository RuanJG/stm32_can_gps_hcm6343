#include <stm32f10x.h>
#include "system.h"
#include "stdio.h"

Uart_t* rtu485_uart = NULL;
rtu_485_ack_t rtu485_ack;

#define _485_NONE 1
#define _485_CMD_PENDING 2
#define _485_CMD_ACK_RECEIVED 3
#define _485_CMD_ACK_FALSE 4
volatile char is_485_bus_status = _485_NONE;

#define _485_TIME_OUT_MS 30
volatile u32 _rtu_timeout_ms=0;
volatile u32 _rtu_last_read_byte_ms = 0;

int is_rtu485_bus_busy()
{
	u32 now_ms = get_system_ms();
	now_ms  = now_ms - _rtu_last_read_byte_ms;
	if( now_ms >= _485_TIME_OUT_MS ){
		return 0;
	}
	return 1;
}
int _485_cail_ack_len(rtu_485_ack_t* ack, unsigned char abyte)
{
	// if data cmd , will get this abyte = len, if set cmd , ack len == cmd_len
	unsigned char func;
	func = ack->func;
	if( (func & 0x80) != 0 ){
		logd("rtu485 get false func=");
		logd_num(func);
		logd_uint(" addr=",ack->addr);
		func &= 0x7f;
	}
	if( func == 0x03 || func == 0x04 || func == 0x01 || func == 0x02){
		ack->len = abyte;
	}else if( func == 0x05 || func == 0x10 ){
		ack->len = 3;
	}else{
		logd("rtu485 unknow func=");
		logd_num(func);
		logd_uint(" addr=",ack->addr);
		return -1;
	}
	return 1;
}

void rtu_485_ack_init(rtu_485_ack_t* ack)
{
	ack->len = 0;
	ack->step = 0;
	ack->index = 0;
	ack->addr = 0;
	ack->func = 0;
	ack->data = &ack->frame[3];
}

int recive_485_parse(rtu_485_ack_t* ack, unsigned char abyte)
{
	// -1 error, 0 reading ,  ok return buff valid_len
	uint16_t crc;
	
	switch(ack->step){
		case 0:{
			ack->step++;
			ack->addr = abyte;
			ack->index = 0;
			ack->frame[0]=abyte;
			break;
		}
		case 1:{
			ack->step++;
			ack->func = abyte;
			ack->frame[1]=abyte;
			break;
		}
		case 2:{
			ack->step++;
			if( -1 == _485_cail_ack_len(ack, abyte) )
			{ //get a unknown cmd
				return -1;
			}
			if( ack->len <= 0 ) 
				ack->step++; // no data
			ack->index = 0;
			ack->frame[2]=abyte;
			break;
		}
		case 3:{
			ack->frame[3+ack->index]=abyte;
			ack->data[ ack->index++ ] = abyte;
			if( ack->index >= ack->len)
				ack->step++;
			break;
		}
		case 4:{
			ack->crc = abyte;
			ack->frame[3+ack->len]=abyte;
			ack->step++;
			break;
		}
		case 5:{
			ack->frame[4+ack->len]=abyte;
			ack->crc |= (abyte<<8);
			crc = crc_calculate(ack->frame, ack->len+3);
			if( ack->crc == crc ){
				ack->step = 0;
				return 1;
			}else{
				ack->step = 0;
				return -1;
			}
			break;
		}
	}
	
	return 0;
}


extern Uart_t Uart1;
void Esc_Led_toggle(int id);

void clean_uart_buff()
{
	char data;
	while( Uart_GetChar(rtu485_uart,&data) );
}

int Rtu_485_send_cmd(unsigned char addr, unsigned char func, unsigned short reg_addr , unsigned short len)
{
	unsigned char cmd[8]={0};
	unsigned short crc;
	
	if( is_485_bus_status == _485_CMD_PENDING || is_rtu485_bus_busy() ) return 0 ;
	
	//logd("send 485 cmd\r\n");
	cmd[0]=addr;
	cmd[1]=func;
	cmd[2]=((reg_addr>>8) & 0xff);
	cmd[3]=(reg_addr&0xff);
	cmd[4]=((len>>8) & 0xff);
	cmd[5]=(len&0xff);
	crc = crc_calculate(cmd,6);
	cmd[6]=(crc&0xff);
	cmd[7]=((crc>>8) & 0xff);
	Uart_PutBytes(rtu485_uart,cmd,8);
	//Uart_PutBytes(&Uart1,cmd,8);
	
	rtu_485_ack_init(&rtu485_ack);
	clean_uart_buff();
	is_485_bus_status = _485_CMD_PENDING;
	return 1;
}

int Rtu_485_send_raw_cmd(unsigned char *data,int len)
{
	if( is_485_bus_status == _485_CMD_PENDING || is_rtu485_bus_busy()) return 0 ;
	

	Uart_PutBytes(rtu485_uart,data,len);
	
	rtu_485_ack_init(&rtu485_ack);
	clean_uart_buff();
	is_485_bus_status = _485_CMD_PENDING;
	return 1;
}



void Rtu_485_Event()
{
		unsigned char data;
		char res;
	
		if( is_485_bus_status != _485_CMD_PENDING )
		{
			
			if( Uart_GetChar(rtu485_uart,&data) > 0 ){
				_rtu_last_read_byte_ms = get_system_ms();
				//Uart_PutChar(rtu485_uart,data);
				//logd_uint("get data=",data);
			}
			return ;
		}
		
		if( _rtu_timeout_ms == 0 ) _rtu_timeout_ms = get_system_ms();
	
		if( Uart_GetChar(rtu485_uart,&data) > 0 ){
			/*
			Uart_PutChar(rtu485_uart,data);
			*/
			_rtu_last_read_byte_ms = get_system_ms();
			_rtu_timeout_ms = _rtu_last_read_byte_ms;
			res = recive_485_parse( &rtu485_ack,data);
			if( res != 0 ){
				_rtu_timeout_ms = 0;
				if( res == 1 )
					is_485_bus_status = _485_CMD_ACK_RECEIVED;
				else
					is_485_bus_status = _485_CMD_ACK_FALSE;
			}
	  }
		//add timeout check
		if( is_485_bus_status == _485_CMD_PENDING && (get_system_ms() - _rtu_timeout_ms ) > _485_TIME_OUT_MS ){
			logd("rtu485 timeout\r\n");
			is_485_bus_status = _485_CMD_ACK_FALSE; //timeout
			_rtu_timeout_ms = 0;
		}			
}

rtu_485_ack_t * Rtu_485_Get_Ack(int *res)
{
	rtu_485_ack_t * resack=NULL;
	
	if( is_485_bus_status == _485_CMD_ACK_RECEIVED){
		resack = &rtu485_ack;
		*res = 1;
	}else if( is_485_bus_status == _485_CMD_ACK_FALSE ){
		*res = -1;
	}else{
		*res = 0;
	}
	return resack;
}

void Rtu_485_Configure(Uart_t* uart)
{
	rtu485_uart = uart;
	rtu_485_ack_init(&rtu485_ack);
}