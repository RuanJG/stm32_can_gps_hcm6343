#include <stm32f10x.h>	
#include "system.h"	
#include "stdio.h"	
#include  <ctype.h>

#include "navigation_box_led.h"





void Read_DS18B20(void);
void ds_writebyte(u8 cmd);
void ds_reset(void);

/*
*********************  ADC
*
*/
u16 Get_Temperature_Adc_value();
u16 Get_Voltage_Adc_value();
u16 Get_WaterLeakage_Adc_value();
void ADC_Configuration (void);

/*
*************  Board config
*/
void Board_Configuration()
{
	SetupPllClock(HSE_CLOCK_6MHZ);
	GPIO_Configuration ();
	GPIO_Initialization();
	ADC_Configuration ();
	//Timer_Configuration();
}

void report_status_to_arm9()
{
	Get_Temperature_Adc_value();
	Get_Voltage_Adc_value();
	Get_WaterLeakage_Adc_value();
}


void listen_misc()
{
	Read_DS18B20();
}


#define DEBUG 1
#define REPORT_STATUS_MS 20 // 50hz
systick_time_t report_status_t;
systick_time_t debug_t;
Uart_t Uart1 ;
Uart_t Uart2 ;
Uart_t Uart3 ;

cmdcoder_t arm9_coder;
Uart_t * arm9_uart;

int send_byte_to_arm9(unsigned char c)
{
	 Uart_PutChar(arm9_uart,c);
}
void handle_arm9_message(cmdcoder_t * coder)
{
	cmdcoder_encode_and_send(&arm9_coder);
}
void listen_arm9(Uart_t * uart)
{
	static char inited = 0;
	unsigned char data;
	
	arm9_uart = uart;
	if( inited == 0 ){
		cmdcoder_init(&arm9_coder, send_byte_to_arm9);
		inited = 1;
	}
	while( Uart_GetChar(arm9_uart, &data) > 0){
		if ( 0 < cmdcoder_Parse_byte(&arm9_coder,data) )
		{
			//get data
			handle_arm9_message(&arm9_coder);
			// one time one packet ???
			//break;
		}
	}
}


cmdcoder_t encode_packget;
cmdcoder_t decode_packget;
int test_code_res = 0;
int TEST_DATA_LEN =0;
int test_send_cb( unsigned char c)
{
	int i;
	if( cmdcoder_Parse_byte(&decode_packget,c) ){
		if( decode_packget.len == encode_packget.len && 
				decode_packget.id == encode_packget.id ){
				if( decode_packget.len > 0 ){
					for( i=0 ;i < decode_packget.len; i++){
						if( decode_packget.data[i] != encode_packget.data[i] )
							return 1;
					}
				}
					test_code_res = 1;
				}
	}
	return 1;
}

void test_protocol()
{
	int i;
	
	cmdcoder_init(&encode_packget, test_send_cb);
	cmdcoder_init(&decode_packget, NULL);
	
	//for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= (i%2 != 0) ? 0xff : 0 ;
	//for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= (i%2 != 0) ? 0xff : i ;
	//for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= 0xff;
	for( i=0; i<TEST_DATA_LEN; i++) encode_packget.data[i]= 0xff-i;
	
	encode_packget.len = TEST_DATA_LEN;
	encode_packget.id = TEST_DATA_LEN % 0x7f;
	cmdcoder_encode_and_send(&encode_packget);
	if( test_code_res == 1){
		Nbl_Led_toggle(GPS_LED_ID);
		test_code_res = 0;
	}else{
		Nbl_Led_off(GPS_LED_ID);
		Nbl_Led_off(COMPASS_LED_ID);
		while(1);
	}
	TEST_DATA_LEN++;
	TEST_DATA_LEN = (TEST_DATA_LEN)%CMD_CODER_MAX_DATA_LEN;
}

void listen_uart1()
{
	char data;
	if( Uart_GetChar(&Uart1, &data) > 0 ){
		Nbl_Led_toggle(COMPASS_LED_ID);
		Uart_PutChar(&Uart1,data);
	}
}
void listen_uart2()
{
	char data;
	if( Uart_GetChar(&Uart2, &data) > 0 ){
		//led_toggle(COMPASS_LED_ID);
		//Uart_PutChar(&Uart1,data);
	}
}
void listen_uart3()
{
	char data;
	if( Uart_GetChar(&Uart3, &data) > 0 ){
		//led_toggle(COMPASS_LED_ID);
		//Uart_PutChar(&Uart1,data);
	}
}
int report_error = 0 ;
char  report_error_id = 0 ;

system_error_t *system_error;


void report_system_status()
{
	if( system_error->hse_setup_status != 0){
		Uart_PutChar(&Uart1,'<');
		Uart_PutChar(&Uart1,'H');
		Uart_PutChar(&Uart1,system_error->hse_setup_status);
		Uart_PutChar(&Uart1,'>');
	}
	if( system_error->can1_error_status != 0 ){
		Uart_PutChar(&Uart1,0x00);
		Uart_PutChar(&Uart1,1);
		Uart_PutChar(&Uart1,system_error->can1_error_status);
		Uart_PutChar(&Uart1,0);
	}
	if( system_error->uart1_fifo_overflow_status != 0){
		Uart_PutChar(&Uart1,'<');
		Uart_PutChar(&Uart1,0x10|system_error->uart1_fifo_overflow_status);
		Uart_PutChar(&Uart1,'>');
	}
	if( system_error->uart2_fifo_overflow_status != 0){
		Uart_PutChar(&Uart1,0x20|system_error->uart2_fifo_overflow_status);
	}
	if( system_error->uart3_fifo_overflow_status != 0){
		Uart_PutChar(&Uart1,0x30|system_error->uart3_fifo_overflow_status);
	}
}

CanRxMsg rxmsg;
CanTxMsg txmsg;
void test_can_send()
{
	int i;
	uint8_t data[12]={0xff,0x1,0x3,0xfe,0xab,0x12,0xff,0x1,0x3,0xfe,0xab,0x12};
	static char send = 1;
	if( 1 ){
			//txmsg.StdId = 0x10;
			//txmsg.DLC = 5;
			//for( i = 0; i< 12; i++)
			//	txmsg.Data[i] = 0xff - i;
		Can1_Send(0x10,data,12);

	}
}
void listen_can1()
{
	if( Can1_Get_CanRxMsg(&rxmsg) ){
		Nbl_Led_toggle(GPS_LED_ID);
	}
}

void user_main_setup()
{
	Board_Configuration();
	Can1_Configuration (0x10);	//0x10CANµØÖ·
	Uart_Configuration (&Uart1, USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart3, USART3, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	
#if DEBUG //for debug
	Nbl_Led_Configuration();
#endif
	
	//time_t init 
	systick_time_start(&report_status_t,50);//REPORT_STATUS_MS);
	systick_time_start(&debug_t,10);
	
	system_error = system_error_get();
}

void user_main_loop()
{
	int i;
	
		listen_uart1();
		listen_uart2();
		listen_uart3();
		
		//listen_arm9(&Uart1);
		listen_can1();
		//listen_misc();
		if( check_systick_time(&report_status_t) ){
			report_status_to_arm9();
			#if DEBUG //for debug
			//led_toggle(GPS_LED_ID);
			#endif
		}
		if( check_systick_time(&debug_t) ){
			//test_protocol();
			Nbl_Led_toggle(COMPASS_LED_ID);
			test_can_send();
			report_system_status();
		}
		
		
}


