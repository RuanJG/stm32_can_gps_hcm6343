
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "system.h"


#if IAP_FIRMWARE

#include "iap.h"



#define PROGRAM_STEP_GET_START 1
#define PROGRAM_STEP_GET_DATA 2
#define PROGRAM_STEP_END 3

char program_step = 0;
#define PROGRAM_BUFF_SIZE FLASH_PAGE_SIZE
unsigned char programm_buff[FLASH_PAGE_SIZE];
unsigned int program_buff_index = 0;
uint32_t  program_addr = ApplicationAddress;
unsigned char program_data_frame_seq = 0;

volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
Uart_t Uart1;
cmdcoder_t decoder;
cmdcoder_t encoder;





void _memcpy(void *dst, const void *src, unsigned n)
{
	const char *p = src;
	char *q = dst;

	while (n--) {
		*q++ = *p++;
	}

	//return dst;
}
void _memset(void *dst, unsigned char data, unsigned n)
{
	unsigned char *q = dst;
	while (n--) {
		*q++ = data;
	}
	//return dst;
}

int encodeCallback ( unsigned char c )
{
	Uart_PutChar(&Uart1,c);
	return 1;
}

void main_deinit();
void jump_to_main_program()
{
	iapFunction Jump_To_Application;
	uint32_t JumpAddress;
		/* If Program has been written */
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
		{
			main_deinit();
			
			/* Set system control register SCR->VTOR  */
			NVIC_SetVectorTable(NVIC_VectTab_FLASH, ApplicationOffset);
			//jump
			JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
			Jump_To_Application = (iapFunction) JumpAddress;
			__set_MSP(*(__IO uint32_t*) ApplicationAddress);
			Jump_To_Application(); 
		}
}

void answer_ack_ok()
{
	unsigned char data[4];
	data[0] = PACKGET_ACK_OK;
	cmdcoder_send_bytes(&encoder, data , 1);
}
void answer_ack_false(char error)
{
	unsigned char data[4];
	data[0] = PACKGET_ACK_FALSE;
	data[1] = error;
	cmdcoder_send_bytes(&encoder, data , 2);
}
void answer_ack_restart()
{
	unsigned char data[4];
	data[0] = PACKGET_ACK_RESTART;
	cmdcoder_send_bytes(&encoder, data , 1);
}


int program_page_to_flash(uint32_t page_addr , unsigned char *data, int len)
{
	// ok return 1 ; len < page return 0 ; erase error return -1 ;program error return  -2
	int timeout ,index;
	uint32_t word, addr;
	
	if( len < FLASH_PAGE_SIZE ) return 0;

	// Erase Page
	timeout = 0;
	do{
		timeout++;
		if(timeout > 100) return -1;
		FLASHStatus = FLASH_ErasePage(page_addr);
	}while( FLASHStatus != FLASH_COMPLETE );
	
	// Program data
	addr = page_addr;
	for( index = 3 ; index < len && index < FLASH_PAGE_SIZE ; index+=4){
		timeout = 0;
		do{
			timeout++;
			if(timeout > 100) return -2;
			word = (data[index]<<24) | (data[index-1]<<16) | (data[index-2]<<8) | data[index-3];
			FLASHStatus = FLASH_ProgramWord(addr,word);
		}while( FLASHStatus != FLASH_COMPLETE );
		addr+=4;
	}
	
	return 1;
	
}



void flash_process_init()
{
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	program_addr = ApplicationAddress;
	program_data_frame_seq = 0;
}

void flash_process_end()
{
	FLASH_Lock();
}

void init_program_buff()
{
	//初始化写flash app的操作
	_memset(programm_buff,0,PROGRAM_BUFF_SIZE);
	program_buff_index = 0;
	
}
char flush_program_buff()
{
	//将还没有写到flash的数据写入
		// return 0 ok;
	// return PACKGET_ACK_FALSE_SEQ_FALSE seq error 
	//return PACKGET_ACK_FALSE_ERASE_ERROR / PACKGET_ACK_FALSE_Program_ERROR program error
	int res;
		if( program_buff_index > 0 ){
			res = program_page_to_flash(program_addr, programm_buff, FLASH_PAGE_SIZE);
			// ok return 1 ; len < page return 0 ; erase error return -1 ;program error return  -2
			if( res == -1){
				return PACKGET_ACK_FALSE_ERASE_ERROR;
			}else if( res == -2){
				return PACKGET_ACK_FALSE_PROGRAM_ERROR;
			}else{
				//ok
				init_program_buff();
				program_addr+= FLASH_PAGE_SIZE;
			}
		}
		return 0;
}

char  handle_packget(unsigned char *data, int len)
{
//	储存数据，达到1page时，写入, 如果数据序列不符合，要求重发
	// return 0 ok;
	// return PACKGET_ACK_FALSE_SEQ_FALSE seq error 
	//return PACKGET_ACK_FALSE_ERASE_ERROR / PACKGET_ACK_FALSE_Program_ERROR program error
	int i,res;
	unsigned char new_seq;
	
	//check seq
	new_seq = (program_data_frame_seq+1)% PACKGET_MAX_DATA_SEQ;
	if( new_seq != data[0] )
	{
		return PACKGET_ACK_FALSE_SEQ_FALSE;
	}
	program_data_frame_seq = new_seq;
	
	// prgram data
	for( i=1; i< len; i++){
		programm_buff[ program_buff_index++ ] =  data[i];
		if( program_buff_index >= FLASH_PAGE_SIZE ){
			res = program_page_to_flash(program_addr, programm_buff, FLASH_PAGE_SIZE);
			// ok return 1 ; len < page return 0 ; erase error return -1 ;program error return  -2
			if( res == -1){
				return PACKGET_ACK_FALSE_ERASE_ERROR;
			}else if( res == -2){
				return PACKGET_ACK_FALSE_PROGRAM_ERROR;
			}else{
				//ok
				init_program_buff();
				program_addr+= FLASH_PAGE_SIZE;
			}
		}
	}
	return 0;
}

int catch_program_app_head_in_ms(int ms)
{
	char ubyte;
	#if 1
	int retry = 0;
	#else
	systick_time_t mstime;
	systick_time_start(&mstime,ms);
	#endif
	while(1) 
	{
		#if 1
		if( retry++ > ms )
			break;
		#else
		if( check_systick_time(&mstime) )
			break;
		#endif
		
		while( Uart_GetChar(&Uart1,&ubyte) > 0 ){
			if( cmdcoder_Parse_byte(&decoder,ubyte) ){
				if( decoder.id == PACKGET_START_ID )
					return 1;
			}
		}
		delay_us(1000);
	}
	return 0;
}


#include <navigation_box.h>
#include <navigation_box_led.h>
void main_setup()
{
	int res;
	
	SetupPllClock(HSE_CLOCK_6MHZ);
	//Esc_GPIO_Configuration();
	Navi_GPIO_Configuration();
	Nbl_Led_Configuration();
	Uart_Configuration (&Uart1, USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	
	
	cmdcoder_init(&decoder, 1,  CMD_CODER_CALL_BACK_NULL);
	cmdcoder_init(&encoder, PACKGET_ACK_ID,  encodeCallback);

	Nbl_Led_on(GPS_LED_ID);
	res = catch_program_app_head_in_ms(500);
	if(  res == 0 )
	{
		jump_to_main_program();
	}
	//no app program or get a program start , it will do main_loop
}
void main_deinit()
{
	SysTick_Deinit();
	Uart_DeInit(&Uart1);
	Nbl_Led_off(GPS_LED_ID);
}
void main_loop()
{
	char ubyte;
	char  res;
	
	if( Uart_GetChar(&Uart1,&ubyte) > 0 )
	{
		if( cmdcoder_Parse_byte(&decoder,ubyte) ){
			if( decoder.id == PACKGET_START_ID ){
				if( program_step != PROGRAM_STEP_GET_DATA){
					program_step = PROGRAM_STEP_GET_DATA ;
					flash_process_init();
					init_program_buff();
				}
				answer_ack_ok();
			}else if (decoder.id == PACKGET_DATA_ID){
				if( program_step != PROGRAM_STEP_GET_DATA ) {
					program_step = PROGRAM_STEP_GET_START ;
					answer_ack_restart();
				}else{
					res = handle_packget(decoder.data,decoder.len);
					if( 0 ==  res){
						answer_ack_ok();
					}else {
						answer_ack_false(res);// remote will resend this packget
					}
				}
			}else if( decoder.id == PACKGET_END_ID ){
				if( program_step == PROGRAM_STEP_GET_DATA ) {
					res = flush_program_buff();
					if( 0 ==  res){
						answer_ack_ok();
						if(decoder.data[0] == PACKGET_END_JUMP ){
							delay_us(50000);
							jump_to_main_program();
						}
					}else {
						answer_ack_false(res);// remote will resend this packget
					}
					flash_process_end();
					program_step = PROGRAM_STEP_END;
				}else{
					answer_ack_ok();
					if(decoder.data[0] == PACKGET_END_JUMP )
						delay_us(50000);
						jump_to_main_program();
				}
			}
		}
	}	
	
}














#endif
