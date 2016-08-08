#ifndef _global_h_
#define _global_h_ 

#include <stm32f10x.h>	
#include "cmdcoder.h"
#include "base64_data.h"

#define MAIN_CONTROLLER_BOARD 0
#define NAVIGATION_BOX 0
#define M80_ESC_BOX 0

// if make iap firmware 
// modify keil flash  ; note : if the firmware offset's value is define in iap_firmware/iap.c
// iap firmware : 0x8000000   0xM000                         ; 0x4000/1024 = 16 = 16kB         
// app firmware : 0x800M000   0xN000  = (0x10000 - 0xM000) 
#define IAP_FIRMWARE 1


/*
*
*  Pll clock 
*/
#define HSE_CLOCK_NO_USE  0
#define HSE_CLOCK_6MHZ  6
#define HSE_CLOCK_8MHZ  8

extern uint32_t systemClk ;
int SetupPllClock(unsigned char hse_mhz) ;


/*
*********   UART
*/
#include "fifo.h"
#define UART_BUFFER_LEN 512
typedef void (*uartReadCallBack)(char c);
typedef struct _uart_t {
	USART_TypeDef *uartDev;
	uartReadCallBack read_cb;
	fifo_t txfifo;
	fifo_t rxfifo;
	char txbuff[UART_BUFFER_LEN];
	char rxbuff[UART_BUFFER_LEN];
}Uart_t;
void Uart_Configuration (Uart_t *uart, USART_TypeDef *uartDev, uint32_t USART_BaudRate, uint16_t USART_WordLength, uint16_t USART_StopBits, uint16_t USART_Parity);
void Uart_PutChar(Uart_t *uart,char ch);
void Uart_PutString (Uart_t *uart,char *buffer);
void Uart_PutBytes (Uart_t *uart,const char *buffer, int len);
int Uart_GetChar(Uart_t *uart, char *c);
void Uart_DeInit (Uart_t *uart);



/*
*
*##### CAN 
*/
int Can1_Get_CanRxMsg(CanRxMsg *msg);
void Can1_Send_Message(CanTxMsg* TxMessage);
u8 Can1_Configuration(u16 ID);
void Can1_Send(uint8_t id, uint8_t *data, int len);


/*
************* systick 
*/
typedef struct _systick_time_t {
	u32 systick_ms;
	u32 systick_ms_overflow;
	u32 interval_ms;
}systick_time_t;
//float get_system_s();
int check_systick_time(systick_time_t *time_t);
int systick_time_start(systick_time_t *time_t, int ms);
void delay_us(u32 us);
void Systick_Event();
void SysTick_Configuration(void);
void SysTick_Deinit(void);

/*
*************** Board Config
*/
void SetClock(void);
void NVIC_Configuration(void);
void GPIO_Configuration (void);
void GPIO_Initialization(void);



/*
****************  global function
*/
#define ERROR_UART_TX_FIFO_OVERFLOW_TYPE 1
#define ERROR_UART_RX_FIFO_OVERFLOW_TYPE 2
#define ERROR_CAN1_RX1_GET_DATA_TYPE 3
#define ERROR_CAN1_RX0_FIFO_OVERFLOW_TYPE 4
#define ERROR_CAN1_TX_RETRY_FAILED_TYPE 5
#define ERROR_CAN1_PROGRAM_NOFIX_TYPE 6
#define ERROR_CAN1_TX_FIFO_OVERFLOW_TYPE 7
#define ERROR_CAN1_INIT_TYPE 8
#define ERROR_HSE_SETUP_TYPE 9

//uart
#define ERROR_UART_TX_FIFO_OVERFLOW_BIT 1
#define ERROR_UART_RX_FIFO_OVERFLOW_BIT 2
//cna
#define ERROR_CAN1_INIT_BIT (1<<0)
#define ERROR_CAN1_RX0_FIFO_OVERFLOW_BIT (1<<1)
#define ERROR_CAN1_TX_FIFO_OVERFLOW_BIT (1<<2)
#define ERROR_CAN1_TX_RETRY_FAILED_BIT (1<<3)
#define ERROR_CAN1_PROGRAM_NOFIX_BIT (1<<4)
#define ERROR_CAN1_RX1_GET_DATA_BIT (1<<5)

typedef struct _system_error_t {
	unsigned char uart1_fifo_overflow_status; //
	unsigned char uart2_fifo_overflow_status;
	unsigned char uart3_fifo_overflow_status;
	unsigned char hse_setup_status;
	unsigned char can1_error_status;
}system_error_t;
void system_error_init();
void system_Error_Callback(int type, int value);
system_error_t* system_error_get();






//iap
//void jump_iap();
void Iap_Event();
void Iap_Configure(Uart_t *uart);

#define IAP_TAG_ADRESS 0xFC00
#define IAP_TAG_UPDATE_VALUE 1
#define IAP_TAG_JUMP_VALUE 2
int set_iap_tag(int tag);
int get_iap_tag();

#endif
//End of File
