#include "stm32f10x_conf.h"
#include "main_config.h"
#include "bsp.h"
#include "protocol.h"

protocol_t uart_encoder;
protocol_t uart_decoder;
protocol_t can1_encoder;
protocol_t can1_decoder;

void main()
{
	bsp_init();
	
}