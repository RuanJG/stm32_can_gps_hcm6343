#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "esc_box.h"

extern Uart_t Uart1;

void _print_uint( Uart_t *uart, int n)
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

void logd(char *str)
{
	Uart_PutString(&Uart1,str);
	//Uart_PutString(&Uart1,"\r\n");
}

void logd_uint(char *str, unsigned int num)
{
	Uart_PutString(&Uart1,str);
	_print_uint(&Uart1,num);
	Uart_PutString(&Uart1,"\r\n");
}