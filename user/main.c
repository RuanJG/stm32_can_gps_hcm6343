#include <stm32f10x.h>	
#include "global.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "stm32f10x_tim.h"

/*****declaration of functions ******************************************/
void Read_DS18B20(void);
void ds_writebyte(u8 cmd);
void ds_reset(void);





#define GPS_LED_ID 0
#define COMPASS_LED_ID 1
#define LED_GPS_GPIO_PIN GPIO_Pin_10
#define LED_GPS_GPIO_BANK GPIOB
#define LED_COMPASS_GPIO_PIN GPIO_Pin_11
#define LED_COMPASS_GPIO_BANK GPIOB
void led_on(int id)
{
	if( id == GPS_LED_ID )
		GPIO_ResetBits(LED_GPS_GPIO_BANK,LED_GPS_GPIO_PIN);
	else if (id == COMPASS_LED_ID)
		GPIO_ResetBits(LED_COMPASS_GPIO_BANK,LED_COMPASS_GPIO_PIN);
}
void led_off(int id)
{
		if( id == GPS_LED_ID )
		GPIO_SetBits(LED_GPS_GPIO_BANK,LED_GPS_GPIO_PIN);
	else if (id == COMPASS_LED_ID)
		GPIO_SetBits(LED_COMPASS_GPIO_BANK,LED_COMPASS_GPIO_PIN);
}
bool is_led_on(int id){
	if( id == GPS_LED_ID )
		return (GPIO_ReadOutputDataBit(LED_GPS_GPIO_BANK,LED_GPS_GPIO_PIN) == 0);
	else if (id == COMPASS_LED_ID)
		return (GPIO_ReadOutputDataBit(LED_COMPASS_GPIO_BANK,LED_COMPASS_GPIO_PIN) == 0);
	 
	return FALSE;
}

void led_toggle(int id)
{
	if( is_led_on(id)){
		led_off(id);
	}else{
		led_on(id);
	}
}
void Led_Configuration()
{
		//led
		GPIO_InitTypeDef GPIO_InitStructure;	
		GPIO_StructInit(&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = LED_GPS_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LED_GPS_GPIO_BANK, &GPIO_InitStructure);
	
	
		GPIO_InitStructure.GPIO_Pin = LED_COMPASS_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(LED_COMPASS_GPIO_BANK, &GPIO_InitStructure);
	
		led_off(COMPASS_LED_ID);
		led_off(GPS_LED_ID);
}
																
																
																
																
																
																
																
																
																
																
																
																
																
																
																
																
																

/*
*
*#################  Can 
*/
void Can_Error_CallBack(int type, int value)
{

}



/*
*************  Board config
*/
void Board_Configuration()
{
	SetClock ();				 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

	NVIC_Configuration ();
	GPIO_Configuration ();
	GPIO_Initialization();
	SysTick_Configuration ();
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
void handle_arm9_message(cmdcoder_t * coder)
{
	
}
int send_byte_to_arm9(unsigned char c)
{
	 Uart_PutChar(arm9_uart,c);
}
void listen_arm9(Uart_t * uart)
{
	static char inited = 0;
	unsigned char data;
	
	arm9_uart = uart;
	if( inited == 0 ){
		cmdcoder_init(&arm9_coder, send_byte_to_arm9);
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
		led_toggle(GPS_LED_ID);
		test_code_res = 0;
	}else{
		led_off(GPS_LED_ID);
		led_off(COMPASS_LED_ID);
		while(1);
	}
	TEST_DATA_LEN++;
	TEST_DATA_LEN = (TEST_DATA_LEN)%CMD_CODER_MAX_DATA_LEN;
}
int main(void)														 
{		
	int i=0;

	Board_Configuration();
	Can_Configuration (0, 0x10, 0x1ff, Can_Error_CallBack);	//0x10CANµØÖ·
	Uart_Configuration (&Uart1, USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart3, USART3, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	
#if DEBUG //for debug
	Led_Configuration();
#endif
	
	//time_t init 
	systick_time_start(&report_status_t,REPORT_STATUS_MS);
	systick_time_start(&debug_t,20);
	
	while(1)
	{
		listen_arm9(&Uart1);
		//listen_uart2();
		//listen_uart3();
		//listen_can1();
		//listen_misc();
		if( check_systick_time(&report_status_t) ){
			report_status_to_arm9();
			#if DEBUG //for debug
			//led_toggle(GPS_LED_ID);
			#endif
			i++;
		}
		if( check_systick_time(&debug_t) ){
			test_protocol();
			led_toggle(COMPASS_LED_ID);
		}
		Can_event();
		Systick_Event();
	}
} //end of main


//End of file


 			  
