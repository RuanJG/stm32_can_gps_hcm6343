#include "system.h"
#include "iap.h"


// Define IAP Application Address Area */
//#define ApplicationOffset 0x0000
#define IapAddress  0x08000000

typedef void(*pFunction)(void);


// define in board main file, if you want to deinit some import think
__weak void main_deinit();

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


Uart_t * remoterUart = 0;
static cmdcoder_t aip_decoder;

void Iap_Configure(Uart_t *uart)
{
	remoterUart = uart;
}
void Iap_Event()
{
	char ubyte ;
	if( remoterUart != 0 && Uart_GetChar(remoterUart,&ubyte) > 0 )
	{
		if( cmdcoder_Parse_byte(&aip_decoder,ubyte) ){
			if( aip_decoder.id == PACKGET_START_ID && aip_decoder.len==1 ){
				jump_iap();
			}
		}
	}
}