#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "navigation_box_led.h"

#if NAVIGATION_TEST_BOX




unsigned char _cmd_buffer[8];
unsigned char _index = 0;
CanRxMsg can_msg;
Uart_t Uart1;
cmdcoder_t encoder;




int encodeCallback ( unsigned char c )
{
	Uart_PutChar(&Uart1,c);
	return 1;
}

void Navi_Test_box_GPIO_Configuration (void);

void main_setup()
{
	//SetupPllClock(HSE_CLOCK_6MHZ);
	Navi_Test_box_GPIO_Configuration ();
	Nbl_Led_Configuration();
	Can1_Configuration_with_mask (0x33,0);	//0x11CAN地址
	Uart_Configuration (&Uart1, USART1, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	//Uart_Configuration (&Uart2, USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	//Uart_Configuration (&Uart3, USART3, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	
	Iap_Configure(&Uart1);

	cmdcoder_init(&encoder, NAVIGATION_BOX_CMDCODER_ID, encodeCallback);

	//system error 
	//system_error = system_error_get();
}





// if use iap , you can setup this function for your deinit
void main_deinit()
{
	// TODO something that import for extern ic or machine befor reset
}





void main_loop()
{
	

	//将来自导航盒的can的数据直接发到串口上
	if( 0 < Can1_Get_CanRxMsg(&can_msg) )
	{
		if( can_msg.Data[0] == 0x11 )
		{
			cmdcoder_send_bytes(&encoder, can_msg.Data , can_msg.DLC);
		}
	}		

	//将串口发过来的命令，整理后再发到can上
	if( 0 < Uart_GetChar(&Uart1, &_cmd_buffer[_index]) )
	{
		if( _cmd_buffer[_index] == '\n' )
		{
			//send cmd without '\n'
			Can1_Send(0x11 , _cmd_buffer, _index);
			_index=0;
			
		}else{
			_index++;
			if( _index >= 8 ) _index=0;
		}
		
	}
	
	Nbl_Led_on(COMPASS_LED_ID);
	
	delay_us(1000);
}





#endif 






//End of file
