#include <stm32f10x.h>
#include "system.h"
#include "stdio.h"

Uart_t *log_uart= NULL;

void __print_uint( Uart_t *uart, int n)
{
	int intt,les,count=0;
	int num = n;
	char data[16]={0};
	while(1){
		intt = num/10;
		les = num%10;
		data[count++] = 0x30+les;
		
		if( intt == 0 )
			break;
		
		num = intt;
	}
	while(count-->0){
		Uart_PutChar(uart, data[count]);
	}
}

int is_enable()
{
	if( LOGD_ENABLE == 1 && log_uart != NULL )
	{
		return 1;
	}
	return 0;
}

void slog_string(char *str)
{
	if( 0 == is_enable() )
		return ;
	
	Uart_PutString(log_uart,str);
	//Uart_PutString(&Uart1,"\r\n");
}
void slog_num(unsigned int num)
{
	if( 0 == is_enable() )
		return ;
	__print_uint(log_uart,num);
}

void slog(char *str, unsigned int num)
{
	if( 0 == is_enable() )
		return ;
	
	slog_string(str);
	slog_num(num);
	slog_string("\r\n");
}

void slog_init(Uart_t *uart)
{
	log_uart = uart;
}

//for micorlib printf
int fputc(int ch, FILE *f)
{
  if( 1 == is_enable() )
	{
		Uart_PutChar(log_uart,(char)ch);
	}
  return ch;
}