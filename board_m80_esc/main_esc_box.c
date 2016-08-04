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
volatile uint16_t pwma=0;
volatile uint16_t pwmb = 0;
systick_time_t test_pwm_t;
void test_pwm()
{
	esc_set_pwmA(pwma);
	esc_set_pwmB(pwmb);
	pwma+=10;
	pwmb+=10;
	if( pwma >= MAX_PWM_VALUE ) pwma = MIN_PWM_VALUE;
	if( pwmb >= MAX_PWM_VALUE ) pwmb = MIN_PWM_VALUE;
}

void main_setup()
{
	SetupPllClock(HSE_CLOCK_6MHZ);
	Esc_GPIO_Configuration();
	Can1_Configuration (0x15);	//0x15CANµÿ÷∑
	Uart_Configuration (&Uart1, USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart3, USART3, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	esc_pwmA_pwmB_Config();
	
	//time_t init 
	systick_time_start(&report_t,250);//REPORT_STATUS_MS);
	systick_time_start(&test_pwm_t,250);
	
	//system error 
	//system_error = system_error_get();
}

void main_loop()
{
	
	if( check_systick_time(&report_t) ){
		//Uart_PutChar(&Uart1,'9');
		//test_pwm();
	}
		
	if( check_systick_time(&test_pwm_t) ){
		//Uart_PutChar(&Uart1,'9');
		test_pwm();
	}
	test_uart123_can1();
}





#endif 






//End of file
