#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	

#include "system.h"

#if STM32F103V_BOARD
	

Uart_t Uart1,Uart2,Uart3;

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
	//Can1_Send(0x10,data,12);
	Can1_Send_Ext(0x10, data, 12, CAN_ID_EXT, CAN_RTR_DATA);
}

void Stm32f103v_GPIO_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
													   
	//USART1								  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    										  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//USART2
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    										  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//CAN
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
									
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	//led 
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}

void led_on()
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
}
void led_off()
{
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
}

void main_setup()
{
	//SetupPllClock(HSE_CLOCK_8MHZ);
	Stm32f103v_GPIO_Configuration ();
	Can1_Configuration (0x21);	//0x11CANµÿ÷∑
	Uart_Configuration (&Uart1, USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	//Uart_Configuration (&Uart3, USART3, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	
	Iap_Configure(&Uart1);
	slog_init(&Uart1);
	
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
	static int led_status = 0;
	
	//uart_echo_test(&Uart1);
	//slog_string("stm32\r\n");
	//Uart_PutChar(&Uart1, 0x3);
	USART_SendData(USART1, 0x01);
	delay_us(50000);
	
	
	if( 1 == led_status )
	{
		led_off();
		led_status = 0;
	}else{
		led_on();
		led_status = 1;
	}
	
}





#endif 






//End of file
