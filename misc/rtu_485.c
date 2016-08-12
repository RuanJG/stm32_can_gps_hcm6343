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
			ack->len = abyte;
			if( ack->len <= 0 ) 
				ack->step++; // no data
			ack->index = 0;
			ack->frame[2]=abyte;
			break;
		}
		case 3:{
			ack->data[ ack->index++ ] = abyte;
			if( ack->index >= ack->len)
				ack->step++;
			break;
		}
		case 4:{
			ack->crc = abyte;
			ack->step++;
			break;
		}
		case 5:{
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

int Rtu_485_send_cmd(unsigned char addr, unsigned char func, unsigned short reg_addr , unsigned short len)
{
	unsigned char cmd[8]={0};
	unsigned short crc;
	
	if( is_485_bus_status == _485_CMD_PENDING ) return 0 ;
	
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
	is_485_bus_status = _485_CMD_PENDING;
	return 1;
}

int Rtu_485_send_raw_cmd(unsigned char *data,int len)
{
	if( is_485_bus_status == _485_CMD_PENDING ) return 0 ;
	

	Uart_PutBytes(rtu485_uart,data,len);
	
	rtu_485_ack_init(&rtu485_ack);
	is_485_bus_status = _485_CMD_PENDING;
	return 1;
}


volatile u32 _rtu_ms=0;
void Rtu_485_Event()
{
		unsigned char data;
		char res;
	
		if( is_485_bus_status != _485_CMD_PENDING )return ;
		
		if( _rtu_ms == 0 ) _rtu_ms = get_system_ms();
	
		if( Uart_GetChar(rtu485_uart,&data) > 0 ){
			res = recive_485_parse( &rtu485_ack,data);
			if( res != 0 ){
				_rtu_ms = 0;
				if( res == 1 )
					is_485_bus_status = _485_CMD_ACK_RECEIVED;
				else
					is_485_bus_status = _485_CMD_ACK_FALSE;
			}
	  }
		//add timeout check
		if( is_485_bus_status == _485_CMD_PENDING && (get_system_ms() - _rtu_ms ) > 500 ){
			//logd("485 timeout\r\n");
			is_485_bus_status = _485_CMD_ACK_FALSE; //timeout
			_rtu_ms = 0;
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