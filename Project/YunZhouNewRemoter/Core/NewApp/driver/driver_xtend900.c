#include "driver_xtend900.h"
#include "stm32f4xx.h"
#include "stdio.h"


//发送参数查询命令 ATHP,ID,MY,DT,MK,PL,AP,TP<CR>
//读取信道，VID，本地地址，目的地址，掩码地址，发射功率，API模式，板载温度等信息
const unsigned char _inquiry_para[26] = {0x41, 0x54, 0x48, 0x50, 0x2c, 0x49, 0x44, 0x2c, 0x4d, \
																	0x59, 0x2c, 0x44, 0x54, 0x2c, 0x4d, 0x4b, 0x2c, 0x50, \
																	0x4c, 0x2c, 0x41, 0x50, 0x2c, 0x54, 0x50, 0x0d};																									


volatile char _xtend900_lock = 0;
xtend900_Recive_Handler_t _xtend900_user_handler = NULL;

#define _UART_BUFF_SIZE 256																	
volatile unsigned char _uart_buffer[_UART_BUFF_SIZE];
volatile unsigned short _uart_buffer_index = 0;														



#if 1 //DEBUG
#define printLog(X...) printf(X...)
#else
#define printLog(X...) 
#endif																	

																	
void xtend900_set_reciver_handler( xtend900_Recive_Handler_t cb)
{
	_xtend900_user_handler = cb;
}


void _xtend_putchar(unsigned char c)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, c);
}

void _xtend_putCmd(unsigned char *str)
{
	int i=0;
	while( str[i] != 0 )_xtend_putchar(str[i++]);
}
																	
void xtend900_putchar(unsigned char c)
{
	if( _xtend900_lock == 1 ) return;
	
	_xtend_putchar(c);
}


//xtend900_parase
// call by uart irq function, to call xtend900 receive char 
// can't block
void xtend900_parase(unsigned char c)
{
	//if( _xtend900_lock == 1 )
	{
		_uart_buffer[_uart_buffer_index] = c;
		_uart_buffer_index++;
		if( _uart_buffer_index >= _UART_BUFF_SIZE ) _uart_buffer_index = 0;
		
	}
	if( _xtend900_lock == 0 )
	{
		if( _xtend900_user_handler != NULL )
			_xtend900_user_handler(c);
	}

}

















void _xtend900_enter_AT_Command(void)
{
	uint16_t xt900_i = 0;
	
	//进入AT模式： +++   
	for(xt900_i = 0; xt900_i < 3; xt900_i++)
	{
		_xtend_putchar('+');
	}	

}

void _xtend900_exit_AT_Command()
{
	_xtend_putchar('A');
	_xtend_putchar('T');
	_xtend_putchar('C');
	_xtend_putchar('N');
	_xtend_putchar(0x0d);
}


//get: 35 0D 33 33 33 32 0D 46 46 46 46 0D 46 46 46 46 0D 46 46 46 46 0D 34 0D 30 0D 
/*  receive 29 Byte
5
3332
FFFF
FFFF
FFFF
4
0
1E

*/
void _xtend900_send_AT_inquiry(void)
{
	uint16_t xt900_i = 0;
	
	//发送命令 ATHP,ID,MY,DT,MK,PL,AP,TP<CR>  
	for(xt900_i = 0; xt900_i < 26; xt900_i++)
	{
		_xtend_putchar(_inquiry_para[xt900_i]);
	}	

	return;
}


int _xtend900_wait_string(char *str, int cmlen, int timeout_ms)
{
	int stridx=0,index=0, count=0;
	
	for( count=0; count < timeout_ms; count++)
	{
		while( index < _uart_buffer_index)
		{	
			if( str[stridx] == _uart_buffer[index] )
			{
				stridx++;
				if( stridx >= cmlen ) //get it
					return 1;
			}else{
				stridx = 0;
			}
			index++;
		}

		vTaskDelay(1);
	}
	return 0;
}

int _xtend900_wait_data_count(int need_size, int timeout_ms)
{
	int index=0, count=0;
	
	if( need_size >= _UART_BUFF_SIZE ) return 0;
	
	for( count=0; count < timeout_ms; count++)
	{
		if( _uart_buffer_index >= need_size )
			return 1;
		
		vTaskDelay(1);
	}
	return 0;
}

unsigned char hexChar_to_byte(char c)
{
	if( c >= '0' && c <= '9' )
	{
		return (c-0x30);
	}else if( c >= 'A' && c <= 'F')
	{
		return (c-55); // A=65   c-65+10 -> c-55
	}
	return 0xff;
}



//ATHP,ID,MY,DT,MK,PL,AP,TP
//get: 35 0D 33 33 33 32 0D 46 46 46 46 0D 46 46 46 46 0D 46 46 46 46 0D 34 0D 30 0D 
/*  receive 29 Byte
5
3332 //0x3332
FFFF //0xffff
FFFF
FFFF
4
0
1E

*/
int translate_parma_frome_string(xtend900_config_t *config, unsigned char *buffer, int len)
{
	unsigned short tmp;
	
	buffer[len] = 0;
	
	printf("xtend900 translate string:\n");
	printf(buffer);

	//hp
	if( buffer[0] > 0x30 && buffer[0] < 0x39 && buffer[1] == 0x0d )
		config->hp = (buffer[0]-0x30);
	else
		return 0;
	
	//id
	tmp = ( (hexChar_to_byte(buffer[2]) << 12 )
				| (hexChar_to_byte(buffer[3]) << 8 )
				| (hexChar_to_byte(buffer[4]) << 4 )
				| hexChar_to_byte(buffer[5]) );
	config->id = tmp;
	
	// MY,
	tmp = ( (hexChar_to_byte(buffer[7]) << 12 )
				| (hexChar_to_byte(buffer[8]) << 8 )
				| (hexChar_to_byte(buffer[9]) << 4 )
				| hexChar_to_byte(buffer[10]) );
	config->my = tmp;
	
	//DT,
	tmp = ( (hexChar_to_byte(buffer[12]) << 12 )
				| (hexChar_to_byte(buffer[13]) << 8 )
				| (hexChar_to_byte(buffer[14]) << 4 )
				| hexChar_to_byte(buffer[15]) );
	config->dt = tmp;
	
	//MK,PL,AP,TP
	tmp = ( (hexChar_to_byte(buffer[17]) << 12 )
				| (hexChar_to_byte(buffer[18]) << 8 )
				| (hexChar_to_byte(buffer[19]) << 4 )
				| hexChar_to_byte(buffer[20]) );
	config->mk = tmp;
	
	//PL
	tmp =  hexChar_to_byte(buffer[22]);
	config->pl = tmp;
	
	//ap
	config->ap = hexChar_to_byte(buffer[24]);
	
	//tp
	config->tp = (hexChar_to_byte(buffer[26])<<4) | hexChar_to_byte(buffer[27]) ;
	
	printf("hp=%x,id=%x,my=%x,dt=%x,mk=%x,pl=%x,ap=%x,tp=%x\n",config->hp,config->id,config->my,config->dt,config->mk,config->pl,config->ap,config->tp);
	
	return 1;
}




char _sendBuffer[128];
int xtend900_save_param(xtend900_config_t * config, xtend900_config_t * reloadConfig)
{
	int res;
	
	_xtend900_lock = 1;
	
	vTaskDelay(3000);//在进入AT模式前，要停止发送数据 2S 以上 
	//clear uart buffer
	_uart_buffer_index = 0;
	_xtend900_enter_AT_Command(); // 发送AT指令后，xtend900大概要1s后再进入AT模式，并返回"OK\n"三个符  
	res = _xtend900_wait_string("OK\r",3, 2000); // 4s delay 
	if( res == 0 ){
		//TODO display timeout
		printf("save_param enter at mode timeout\n");
		goto error_out;
	}
	
	/*
	
	ATHP0006,ID3332,MYffff,DTffff,MKffff,PL0004,AP0000,WR,CN
ATHP0005,ID3332,MYffff,DTffff,MKffff,PL0004,AP0000,WR,CN
	*/
	
	sprintf(_sendBuffer,"ATHP%04x,ID%04x,MY%04x,DT%04x,MK%04x,PL%04x,AP%04x,WR\r",
												config->hp, config->id, config->my, config->dt, config->mk, config->pl, config->ap);
	
	printf(_sendBuffer);
	
	//clear uart buffer
	_uart_buffer_index = 0;
	_xtend_putCmd(_sendBuffer);
	res = _xtend900_wait_string("OK\r",3, 2000); // 2s delay 
	if( res == 0 )
	{
		//TODO display timeout
		printf("safe param set timeout\n");
		goto error_out;
	}	
	
	//read param again
	//clear uart buffer
	_uart_buffer_index = 0;
	//发送查询命令
	_xtend900_send_AT_inquiry();
	res = _xtend900_wait_data_count(29, 2000); // 2s delay 
	if( res == 0 )
	{
		//TODO display timeout
		printf("save param at inquiry timeout\n");
		goto error_out;
	}
	translate_parma_frome_string(reloadConfig, _uart_buffer, _uart_buffer_index);
	
	
	//clear uart buffer
	_uart_buffer_index = 0;
	_xtend900_exit_AT_Command();
	res = _xtend900_wait_string("OK\r",3, 2000); // 4s delay 
	if( res == 0 )
	{
		//TODO display timeout
		printf("save param : exit timeout\n");
		goto error_out;
	}	
	
	
	vTaskDelay(100);
	_xtend900_lock = 0;
	return 1;
	
error_out:
	vTaskDelay(5000); //and delay 5s for modem return nomal status auto
	_xtend900_lock = 0;
	return 0;
	
}



int xtend900_load_param(xtend900_config_t * config)
{
	int res;
	
	_xtend900_lock = 1;
	
	//在进入AT模式前，要停止发送数据 2S 以上 
	vTaskDelay(3000);
	//clear uart buffer
	_uart_buffer_index = 0;
	//send "+++"
	_xtend900_enter_AT_Command(); // 发送AT指令后，xtend900大概要1s后再进入AT模式，并返回"OK\n"三个符  
	//wait return
	res = _xtend900_wait_string("OK\r",3, 4000); // 4s delay 
	if( res == 0 )
	{
		//TODO display timeout
		//printf(_uart_buffer);
		printf("load param at timeout\n");
		goto error_out;
	}
	
	//clear uart buffer
	_uart_buffer_index = 0;
	//发送查询命令
	_xtend900_send_AT_inquiry();
	res = _xtend900_wait_data_count(29, 2000); // 2s delay 
	if( res == 0 )
	{
		//TODO display timeout
		printf("load param at inquiry timeout\n");
		goto error_out;
	}
	translate_parma_frome_string(config, _uart_buffer, _uart_buffer_index);
	
	//clear uart buffer
	_uart_buffer_index = 0;
	_xtend900_exit_AT_Command();
	res = _xtend900_wait_string("OK\r",3, 2000); // 2s delay 
	if( res == 0 )
	{
		//TODO display timeout
		printf("at exit timeout\n");
		goto error_out;
	}	
	
	vTaskDelay(100);
	_xtend900_lock = 0;
	return 1;
	
error_out:
	vTaskDelay(5000); //and delay 5s for modem return nomal status auto
	_xtend900_lock = 0;
	return 0;
}


/**
  * @brief  get_TX_Power		获取发射功率字符串
  * @param  power_select		功率索引
  * @retval 返回显示字符串
  */
char * xtend900_get_TX_Power_string(int power_select)
{
	switch(power_select)
	{
		case 0:
			return "1 mW";
		
		case 1:
			return "10 mW";
		
		case 2:
			return "100 mW";
		
		case 3:
			return "500 mW";
				
		case 4:
			return "1000 mW";
		
		default:
			break;
	}
	
	return "0";
}
