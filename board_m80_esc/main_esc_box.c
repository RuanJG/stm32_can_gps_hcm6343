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
	Esc_Led_toggle(LED_YELLOW_ID);
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


void print_uint( Uart_t *uart, int n)
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
int read_num_by_uart1(uint32_t *num)
{
	static char data[32];// #200#, num is 200
	static char tag = 0;
	static char index = 0;
	char tmp;
	if( Uart_GetChar(&Uart1, &tmp) > 0 ){
		switch(tag)
		{
			case 0:{
				if( tmp == '#'){
					tag++;
					index = 0;
				}
				break;
			}
			case 1:{
				if( tmp == '#' && index > 0){
					*num = str_to_int(data,index);
					tag = 0;
					index = 0;
					return 1;
				}else{
					data[index++]=tmp;
				}
				break;
			}
		}
	}
	return 0;
}

void test_pwm()
{
	uint32_t pwm=0;
	static int a;
	if(read_num_by_uart1( &pwm ) == 1)
	{
		print_uint(&Uart1,pwm);
		Uart_PutString(&Uart1,"\r\n");
		if( a==1 ){
			Esc_Led_on(LED_YELLOW_ID);
			a=0;
		}else{
			a=1;
			Esc_Led_off(LED_YELLOW_ID);
		}
		esc_set_pump_pitch_pwm(pwm);
	}
}

#define a_yaw_control_shutdown() 	GPIO_ResetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN)
#define a_yaw_control_poweron()  GPIO_SetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN);
void a_yaw_control_forward()
{
	a_yaw_control_shutdown();
	GPIO_SetBits(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN);
	GPIO_ResetBits(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN);
	a_yaw_control_poweron();
}
void a_yaw_control_back()
{
	a_yaw_control_shutdown();
	GPIO_ResetBits(H_BRIDGE_A_CTRL3_BANK,H_BRIDGE_A_CTRL3_PIN);
	GPIO_SetBits(H_BRIDGE_A_CTRL4_BANK,H_BRIDGE_A_CTRL4_PIN);
	a_yaw_control_poweron();
}
void test_h_bridge()
{
	uint32_t pwm=0;

	if(read_num_by_uart1( &pwm ) == 1)
	{
		if( pwm == 0 ){
			Uart_PutString(&Uart1,"back\r\n");
			a_yaw_control_back();
		}else if( pwm == 1){
			Uart_PutString(&Uart1,"stop\r\n");
			a_yaw_control_shutdown();
		}else if( pwm == 2){
			Uart_PutString(&Uart1,"forward\r\n");
			a_yaw_control_forward();
		}
	}
	
	

}


void main_setup()
{
	SetupPllClock(HSE_CLOCK_6MHZ);
	Esc_GPIO_Configuration();
	Can1_Configuration (0x12);
	Uart_Configuration (&Uart1, USART1, IAP_UART_BAUDRATE, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart3, USART3, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Esc_Pump_Pitch_Config();
	Esc_Yaw_Control_Configure(); 
	Esc_Led_Configuration();
	
	//time_t init 
	systick_time_start(&report_t,250);//REPORT_STATUS_MS);
	systick_time_start(&led_t,10);
	
	//system error 
	//system_error = system_error_get();
	
	Iap_Configure(&Uart1);
	
	
	
	//test
	Esc_Led_set_toggle(LED_RED_ID,100);//100*10ms each toggle
	Esc_Led_set_toggle(LED_GREEN_ID,50);//50*10ms each toggle
}

void main_loop()
{
	
	if( check_systick_time(&report_t) ){
		//Uart_PutChar(&Uart1,'9');
		#if 1
		Uart_PutString(&Uart1,"angle;isa;iol>>");
		print_uint(&Uart1,Get_PUMP_ANGLE_Adc_value());Uart_PutChar(&Uart1,' ');
		print_uint(&Uart1,Get_ISA_Adc_value());Uart_PutChar(&Uart1,' ');
		print_uint(&Uart1,Get_Oil_Mass_Adc_value());
		Uart_PutString(&Uart1,"\r\n");
		#endif
		test_can_send();
	}
	test_uart123_can1();
	//test_pwm();
	//test_h_bridge();
	
	
	
	
	
	if( check_systick_time(&led_t) ){
		Esc_Led_Event();
	}
	
}





#endif 






//End of file
