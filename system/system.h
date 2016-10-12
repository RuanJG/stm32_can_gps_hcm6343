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
// iap firmware : 0x8000000   0x4000                         ; 0x4000/1024 = 16 = 16kB
// iap tag :      0x8004000   0x400
// app firmware : 0x8004400   0xN000  = (0x10000 - 0xM000) 
#define IAP_FIRMWARE 1
//chose board , use in setgpio
#define IAP_FIRMWARE_BOARD_NAVIGATION 0
#define IAP_FIRMWARE_BOARD_80_ESC 0
#define IAP_FIRMWARE_BOARD_MAINCONTROLLER 0
#define IAP_FIRMWARE_BOARD_103V 1
//iap config
#define IAP_UART_BAUDRATE 115200
#define IAP_UART_DEV USART1


//*******************  may need to configure these paramter
/*
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)
#endif
*/
#define FLASH_PAGE_SIZE    ((uint16_t)0x400)

// Define Application Address Area */
// iap firmware : 0x8000000   0x4000                         ; 0x4000/1024 = 16 = 16kB         
// app firmware : 0x8004000   (0x10000 - 0x4000) = 0xC000
#define ApplicationOffset 0x4400
#define ApplicationAddress  (0x08000000 | ApplicationOffset)
#define IAP_TAG_ADRESS 0x8004000
#define IAP_TAG_UPDATE_VALUE 1
#define IAP_TAG_JUMP_VALUE 2

// ********************************************************************************************************************








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
//u8 Can1_Configuration_withRate(u16 ID, uint8_t sjw ,uint8_t bs1, uint8_t bs2, uint8_t prescale );
u8 Can1_Configuration_withRate(u16 ID, uint32_t id_type, uint8_t sjw ,uint8_t bs1, uint8_t bs2, uint8_t prescale );
void Can1_Send(uint8_t id, uint8_t *data, int len);
void Can1_Send_Ext(uint8_t id, uint8_t *data, int len, uint32_t id_type, uint32_t frame_type);


/*
************* systick 
*/
typedef struct _systick_time_t {
	u32 systick_ms;
	u32 systick_ms_overflow;
	u32 interval_ms;
}systick_time_t;
u32 get_system_ms();
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





void Iap_Configure(Uart_t *uart);
void Iap_Jump();
int set_iap_tag(int tag);
int get_iap_tag();



// misc

//crc
unsigned short crc_calculate(const unsigned char* pBuffer, int length);
//rtu 485 uart
void rtu_485_send_cmd(unsigned char addr, unsigned char func, unsigned short reg_addr , unsigned short len);
typedef struct _rtu_485_ack {
	unsigned char frame[259];
	unsigned char *data;
	unsigned char addr;
	unsigned char func;
	unsigned char len;
	unsigned char step;
	unsigned char index;
	unsigned short crc;
}rtu_485_ack_t;
void Rtu_485_Configure(Uart_t* uart);
rtu_485_ack_t * Rtu_485_Get_Ack(int *res);
void Rtu_485_Event();
int Rtu_485_send_cmd(unsigned char addr, unsigned char func, unsigned short reg_addr , unsigned short len);
int Rtu_485_send_raw_cmd(unsigned char *data,int len);

#endif
//End of File
