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
	//uart_echo_test(&Uart2);
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
int read_num_by_uart(Uart_t *uart ,uint32_t *num)
{
	static char data[32];// #200#, num is 200
	static char tag = 0;
	static char index = 0;
	char tmp;
	if( Uart_GetChar(uart, &tmp) > 0 ){
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
	if(read_num_by_uart(&Uart2, &pwm ) == 1)
	{
		print_uint(&Uart2,pwm);
		Uart_PutString(&Uart2,"\r\n");
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

	if(read_num_by_uart( &Uart2,&pwm ) == 1)
	{
		if( pwm == 0 ){
			Uart_PutString(&Uart2,"back\r\n");
			a_yaw_control_back();
		}else if( pwm == 1){
			Uart_PutString(&Uart2,"stop\r\n");
			a_yaw_control_shutdown();
		}else if( pwm == 2){
			Uart_PutString(&Uart2,"forward\r\n");
			a_yaw_control_forward();
		}
	}
}

unsigned char char_to_hex(char data)
{
	//'a'=0x61 62 63 64 65 66 'A'=0x41 42 43 44 45 46;  0=0x30 - 0x39
	// ['0' 'f'] = 0x0f 

	if( data >= 0x61)
		return (0x0a+(data-0x61));
	
	else if (data >= 0x41 )
		return (0x0a+(data-0x41));
	
	else 
		return (data-0x30);
	
}
void cmd_uart_485cmd(Uart_t *uarts, Uart_t *uartd)
{
	static char step=0;
	char tmp,data[32];
	unsigned char sendbuff[32];
	int len,i,j;
	unsigned short crc;
	
	
	
	// cmd : #3#3010203 , send  3byte 0x01 0x02 0x03 to uartd 
	while( Uart_GetChar(uarts, &tmp) <= 0 )delay_us(1000);
	//first byte is len
	len = char_to_hex(tmp)*2;
	
	for( i=0 ; i< len; i++){
		while( Uart_GetChar(uarts, &tmp) <= 0 ) delay_us(1000);
		data[i] = tmp;
	}
	
	for( i=0,j=0; i< len;)
	{
		tmp = char_to_hex(data[i])*16 + char_to_hex(data[i+1]);
		Uart_PutChar(uartd,tmp);
		sendbuff[j++]=tmp;
		i+=2;
	}
	crc = crc_calculate(sendbuff,j);
	Uart_PutChar(uartd,crc&0x00ff);
	Uart_PutChar(uartd,crc>>8);
}
void listen_cmd(Uart_t *uart)
{
	uint32_t cmd;
	if(read_num_by_uart(uart, &cmd ) == 1)
	{
		//print_uint(uart,cmd);
		//Uart_PutString(uart,"\r\n");

		if( cmd == 1 ){
			Uart_PutString(uart,"angle;isa;iol>>");
			print_uint(uart,Get_PUMP_ANGLE_Adc_value());Uart_PutChar(uart,' ');
			print_uint(uart,Get_ISA_Adc_value());Uart_PutChar(uart,' ');
			print_uint(uart,Get_Oil_Mass_Adc_value());
			Uart_PutString(uart,"\r\n");
		}
		if(cmd == 0  ){
			Uart_PutString(uart,"jump \r\n");
			Iap_Jump();
		}
		if(cmd==3){
			//uart ×ª·¢
			cmd_uart_485cmd(&Uart1,&Uart2);
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
	systick_time_start(&report_t,200);//REPORT_STATUS_MS);
	systick_time_start(&led_t,10);
	
	//system error 
	//system_error = system_error_get();
	
	Iap_Configure(&Uart1);
	
	Th11sb_Configure(0x33);
	Rtu_485_Configure(&Uart2);
	
	//test
	//Esc_Led_set_toggle(LED_RED_ID,100);//100*10ms each toggle
	Esc_Led_set_toggle(LED_GREEN_ID,50);//50*10ms each toggle
}

void main_loop()
{
	char data;
	//Esc_Yaw_Control_Event();
	
	if( check_systick_time(&report_t) ){
		//test_can_send();
		Th11sb_485_runtime();
		logd_uint("wet:",Th11sb_get_wet()/10);
		logd_uint("tempture:",Th11sb_get_tempture()/10);
	}
	//test_pwm();
	//test_h_bridge();
	listen_cmd(&Uart1);
	//test_can1();
	//uart_echo_test(&Uart2);
	//if( Uart_GetChar(&Uart2, &data) > 0 ){
	//	Uart_PutChar(&Uart1,data);
	//}

	
	
	
	
	if( check_systick_time(&led_t) ){
		Esc_Led_Event();
	}
	
	Rtu_485_Event();
	
}





#endif 






//End of file
