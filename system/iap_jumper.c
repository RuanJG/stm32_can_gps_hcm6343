#include "system.h"
#include "iap.h"


// Define IAP Application Address Area */
//#define ApplicationOffset 0x0000
#define IapAddress  0x08000000

typedef void(*pFunction)(void);

Uart_t * remoterUart = 0;
static cmdcoder_t aip_decoder;

// define in board main file, if you want to deinit some import think
__weak void main_deinit();




int get_iap_tag()
{
	return *(__IO int*) IAP_TAG_ADRESS;
}

int set_iap_tag(int tag)
{//1 ok, 0 flase
	int timeout = 10;
	char res, ntag;
	volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	res = 0;
	timeout = 10;
	while( timeout-- > 0)
	{
		FLASHStatus = FLASH_ErasePage(IAP_TAG_ADRESS);
		if( FLASHStatus == FLASH_COMPLETE ){
			res = 1;
			break;
		}
	}
	
	if( res == 1 )
	{
		res = 0;
		timeout = 10;
		while( timeout-- > 0)
		{
			FLASHStatus = FLASH_ProgramWord(IAP_TAG_ADRESS,tag) ;//FLASH_ProgramOptionByteData(IAP_TAG_ADRESS,tag);
			if( FLASHStatus == FLASH_COMPLETE ){
				res = 1;
				break;
			}
		}
	}

	FLASH_Lock();
	
	if( res == 1)
	{
		ntag = get_iap_tag();
		if( ntag != tag )
			res = 0;
	}
	
	return res;
}


int aipjumperEnodeCallback ( unsigned char c )
{
	Uart_PutChar(remoterUart,c);
	return 1;
}
cmdcoder_t iapJumperEncoder;
void iap_jumper_answer_ack_false(char error)
{
	unsigned char data[4];
	
	cmdcoder_init(&iapJumperEncoder, PACKGET_ACK_ID,  aipjumperEnodeCallback);
	
	data[0] = PACKGET_ACK_FALSE;
	data[1] = error;
	cmdcoder_send_bytes(&iapJumperEncoder, data , 2);
}

void jump_by_reset()
{
	main_deinit();
	
		//¹ØÖÐ¶Ï
	__set_PRIMASK(1);
	__set_FAULTMASK(1);
	
	// vectreset reset cm3 but other extern hardword
	//*((uint32_t*)0xE000ED0C) = 0x05FA0001;
	// sysresetReq reset all ic hardword system
	*((uint32_t*)0xE000ED0C) = 0x05FA0004;
	
	
	while(1);
}
void jump_to_iap_program()
{
			pFunction Jump_To_Application;
			uint32_t JumpAddress;
	
			main_deinit();
	
			//close irq
			//__set_PRIMASK(1);
			//__set_FAULTMASK(1);
	
			/* Set system control register SCR->VTOR  */
			NVIC_SetVectorTable(NVIC_VectTab_FLASH, ApplicationOffset);
	
			//jump
			JumpAddress = *(__IO uint32_t*) (IapAddress + 4);
			Jump_To_Application = (pFunction) JumpAddress;
			__set_MSP(*(__IO uint32_t*) IapAddress);
			Jump_To_Application(); 
}

void jump_iap()
{
	//use reset jump to iap
	jump_by_reset();
	//use jump to iap function no fix
	//jump_to_iap_program();
}



#define IAP_USE_READCALLBACK 1

#if IAP_USE_READCALLBACK
void iapUartReadCallBack(char c)
{
		if( cmdcoder_Parse_byte(&aip_decoder,c) ){
			if( aip_decoder.id == PACKGET_START_ID && aip_decoder.len==1 ){
				if( set_iap_tag(IAP_TAG_UPDATE_VALUE) ){
					jump_iap();
				}else{
					iap_jumper_answer_ack_false(PACKGET_ACK_FALSE_PROGRAM_ERROR);
				}
			}
		}
}
#endif

void Iap_Configure(Uart_t *uart)
{
	remoterUart = uart;
	#if IAP_USE_READCALLBACK
	remoterUart->read_cb = iapUartReadCallBack;
	#endif
}

void Iap_Jump()
{
		if( set_iap_tag(IAP_TAG_UPDATE_VALUE) ){
				jump_iap();
		}else{
				iap_jumper_answer_ack_false(PACKGET_ACK_FALSE_PROGRAM_ERROR);
		}
}

#if !IAP_USE_READCALLBACK
void Iap_Event()
{
	char ubyte ;
	if( remoterUart != 0 && Uart_GetChar(remoterUart,&ubyte) > 0 )
	{
		if( cmdcoder_Parse_byte(&aip_decoder,ubyte) ){
			if( aip_decoder.id == PACKGET_START_ID && aip_decoder.len==1 ){
				if( set_iap_tag(IAP_TAG_UPDATE_VALUE) ){
					jump_iap();
				}else{
					iap_jumper_answer_ack_false(PACKGET_ACK_FALSE_PROGRAM_ERROR);
				}
			}
		}
	}
}
#endif