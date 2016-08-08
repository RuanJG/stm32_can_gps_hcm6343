#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "esc_box.h"


#if M80_ESC_BOX
	

systick_time_t report_t;
systick_time_t led_t;
Uart_t Uart1 ;
Uart_t Uart2 ;
Uart_t Uart3 ;

CanTxMsg TxMessage1;
CanRxMsg RxMessage1;









void uart_echo_test(Uart_t* uartp)
{
	char data;
	if( Uart_GetChar(uartp, &data) > 0 ){
		Uart_PutChar(uartp,data);
	}
}
CanRxMsg rxmsg;
CanTxMsg txmsg;
void test_can_send()
{
	int i;
	uint8_t data[12]={0xff,0x1,0x3,0xfe,0xab,0x12,0xff,0x1,0x3,0xfe,0xab,0x12};
	Can1_Send(0x10,data,12);
}
void test_can1()
{
	if( Can1_Get_CanRxMsg(&rxmsg) ){
		Can1_Send(0x10,rxmsg.Data,rxmsg.DLC);
	}
}
void test_uart123_can1()
{
	test_can1();
	uart_echo_test(&Uart1);
	uart_echo_test(&Uart3);
	uart_echo_test(&Uart2);
}


int str_to_int(char *data, int len)
{
	int i;
	int res=0,muli=1;
	for( i=(len-1) ; i>=0; i--)
	{
		res += (data[i]-0x30)*muli;
		muli*=10;
	}
	return res;
}
int read_pwm_by_uart1(uint16_t *pwm)
{
	static char data[5];// #200#, num is 200
	static char tag = 0;
	static char index = 0;
	if( Uart_GetChar(&Uart1, &data[index]) > 0 ){
		if( data[index] == '#'){
			tag++;
			if( tag >= 2 ){ // get a frame
				index = 0;
				tag = 0;
				if(index ==4 ){
					*pwm = str_to_int(&data[1],5-2);
					Uart_PutBytes (&Uart1,&data[1],5-2);
				}
			}else{
				index++;
			}				
		}else{
			index++;
		}
	}
	return 0;
}
void test_pwm()
{
	uint16_t pwm=0;
	if(read_pwm_by_uart1( &pwm ) == 1)
	{
		esc_set_pump_pitch_pwm(pwm);
	}
}

void main_setup()
{
	SetupPllClock(HSE_CLOCK_6MHZ);
	Esc_GPIO_Configuration();
	Can1_Configuration (0x15);	//0x15CANµÿ÷∑
	Uart_Configuration (&Uart1, USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart3, USART3, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Esc_Pump_Pitch_Config();
	Esc_ADC_Configuration ();
	Esc_Led_Configuration();
	
	//time_t init 
	systick_time_start(&report_t,250);//REPORT_STATUS_MS);
	systick_time_start(&led_t,10);
	
	//system error 
	//system_error = system_error_get();
	
	
	//test
	Esc_Led_set_toggle(LED_RED_ID,100);//100*10ms each toggle
	Esc_Led_set_toggle(LED_GREEN_ID,50);//50*10ms each toggle
	Esc_Led_set_toggle(LED_YELLOW_ID,10);//100ms
}

void main_loop()
{
	
	if( check_systick_time(&report_t) ){
		//Uart_PutChar(&Uart1,'9');
	}
	//test_uart123_can1();
	test_pwm();
	
	
	
	
	
	
	if( check_systick_time(&led_t) ){
		Esc_Led_Event();
	}
	
}





#endif 






//End of file
