#ifndef _global_h_
#define _global_h_ 

#include <stm32f10x.h>	
#include "cmdcoder.h"

/*
*
*  Pll clock 
*/
enum HSE_CLOCK_MHZ{
	HSE_CLOCK_NO_USE = 0,
	HSE_CLOCK_6MHZ = 6,
	HSE_CLOCK_8MHZ = 8
};
int SetupPllClock(enum HSE_CLOCK_MHZ hse_mhz) ;


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

#endif
//End of File
