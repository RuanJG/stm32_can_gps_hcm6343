#include "system.h"
//#include "stdio.h"
#include "stm32f10x.h"

system_error_t _system_error;

void system_error_init()
{
	memset(&_system_error,0,sizeof(system_error_t));
}
system_error_t* system_error_get()
{
	return &_system_error;
}



void system_Error_Callback(int type, int value)
{
	switch(type){
		case ERROR_UART_TX_FIFO_OVERFLOW_TYPE:{
			if( value == (int)USART1 ) _system_error.uart1_fifo_overflow_status |= ERROR_UART_TX_FIFO_OVERFLOW_BIT ;
			else if ( value == (int)USART2 ) _system_error.uart2_fifo_overflow_status |= ERROR_UART_TX_FIFO_OVERFLOW_BIT ;
			else if ( value == (int)USART3 ) _system_error.uart3_fifo_overflow_status |= ERROR_UART_TX_FIFO_OVERFLOW_BIT ;
			break;
		}
		case ERROR_UART_RX_FIFO_OVERFLOW_TYPE:{
			if( value == (int)USART1 ) _system_error.uart1_fifo_overflow_status |= ERROR_UART_RX_FIFO_OVERFLOW_BIT ;
			else if ( value == (int)USART2 ) _system_error.uart2_fifo_overflow_status |= ERROR_UART_RX_FIFO_OVERFLOW_BIT ;
			else if ( value == (int)USART3 ) _system_error.uart3_fifo_overflow_status |= ERROR_UART_RX_FIFO_OVERFLOW_BIT ;
			break;
		}
		case ERROR_CAN1_TX_RETRY_FAILED_TYPE:{
			_system_error.can1_error_status |= ERROR_CAN1_TX_RETRY_FAILED_BIT;
			break;
		}
		case ERROR_CAN1_RX1_GET_DATA_TYPE:{
			_system_error.can1_error_status |= ERROR_CAN1_RX1_GET_DATA_BIT;
			break;
		}
		case ERROR_CAN1_RX0_FIFO_OVERFLOW_TYPE:{
			_system_error.can1_error_status |= ERROR_CAN1_RX0_FIFO_OVERFLOW_BIT;
			break;
		}
		case ERROR_CAN1_TX_FIFO_OVERFLOW_TYPE:{
			_system_error.can1_error_status |= ERROR_CAN1_TX_FIFO_OVERFLOW_BIT;
			break;
		}
		case ERROR_CAN1_INIT_TYPE:{
			_system_error.can1_error_status |= ERROR_CAN1_INIT_BIT;
			break;
		}
		case ERROR_CAN1_PROGRAM_NOFIX_TYPE:{
			_system_error.can1_error_status |= ERROR_CAN1_PROGRAM_NOFIX_BIT;
			break;
		}
		case ERROR_HSE_SETUP_TYPE:{
			_system_error.hse_setup_status = 1;
			break;
		}
	}
}


