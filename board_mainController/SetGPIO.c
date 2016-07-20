#include "stm32f10x.h" 
#include "system.h"	

#define BIT0 1
#define BIT1 (1<<1)	//1
#define BIT2 (1<<2)	//2
#define BIT3 (1<<3)	//4
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT7 (1<<7)

#define VALVE12_1SET   	GPIOB->BSRR = GPIO_Pin_12
#define VALVE12_1RESET  GPIOB->BRR  = GPIO_Pin_12
#define VALVE12_2SET   	GPIOB->BSRR = GPIO_Pin_13
#define VALVE12_2RESET 	GPIOB->BRR  = GPIO_Pin_13
#define VALVE34_1SET   	GPIOB->BSRR = GPIO_Pin_14
#define VALVE34_1RESET 	GPIOB->BRR  = GPIO_Pin_14
#define VALVE34_2SET   	GPIOB->BSRR = GPIO_Pin_15
#define VALVE34_2RESET 	GPIOB->BRR  = GPIO_Pin_15
#define ROD_DOWN_SET  	GPIOB->BSRR = GPIO_Pin_0
#define ROD_DOWN_RESET	GPIOB->BRR  = GPIO_Pin_0
#define ROD_UP_SET 		GPIOB->BSRR = GPIO_Pin_1
#define ROD_UP_RESET	GPIOB->BRR  = GPIO_Pin_1
#define CAM_PWR_SET		GPIOA->BSRR = GPIO_Pin_6
#define CAM_PWR_RESET	GPIOA->BRR  = GPIO_Pin_6
#define GPRS_PWR_SET	GPIOB->BSRR = GPIO_Pin_5
#define GPRS_PWR_RESET	GPIOB->BRR  = GPIO_Pin_5
#define SPEAKER_SET		GPIOA->BSRR = GPIO_Pin_4
#define SPEAKER_RESET	GPIOA->BRR  = GPIO_Pin_4
#define ALERT_LED_SET	GPIOA->BSRR = GPIO_Pin_5
#define ALERT_LED_RESET	GPIOA->BRR  = GPIO_Pin_5
#define US_LEFT_SET		GPIOA->BSRR = GPIO_Pin_9
#define US_LEFT_RESET	GPIOA->BRR  = GPIO_Pin_9
#define US_RIGHT_SET	GPIOB->BSRR = GPIO_Pin_9
#define US_RIGHT_RESET	GPIOB->BRR  = GPIO_Pin_9

void GPIO_Configuration (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	//ADC_IN To Detect Bat. Life
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  //GPIO to Read Temp. Sensor, Ds18B20	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;				//DS18B20 Pin 11	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//ADC to Detect Water Leakage
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
													   
	//USART1								  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    										  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

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

	//USART3								  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    										  
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  

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

	//valve1
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//valve2
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//valve3
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//valve4
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//Rod_down
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	//Rod_up
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//Cam Power
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Alarm 
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Alert LED
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//US_LEFT_ENABLED
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//US_RIGHT_ENABLED
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPRS Enabled
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
}

void GPIO_Initialization(void){

	//To turn off all the valves first
////	GPIOB->BSRR   = GPIO_Pin_12;	//set VALVE12_1
////	GPIOB->BRR  = GPIO_Pin_13;	//reset VALVE12_2
////	GPIOB->BSRR   = GPIO_Pin_14;	//set VALVE34_1
////	GPIOB->BRR  = GPIO_Pin_15;	//reset VALVE34_2

	//To turn off the pump
	GPIOA->BSRR  = GPIO_Pin_8;   //pump active low

	//Reset all the rods
	GPIOB->BRR  = GPIO_Pin_0;
	GPIOB->BRR  = GPIO_Pin_1;

	//Turn off CAM_PWR
	GPIOB->BRR  = GPIO_Pin_2; //Not used in current board

	//Turn off alert LED, alarm speaker, and rsvd1&2
	GPIOA->BRR  = GPIO_Pin_4;
	GPIOA->BRR  = GPIO_Pin_5;
	GPIOA->BRR  = GPIO_Pin_6;
	GPIOA->BRR  = GPIO_Pin_7;
	
		//device init
	GPRS_PWR_SET;
	ALERT_LED_RESET;
	US_LEFT_SET;
	US_RIGHT_SET;
	CAM_PWR_SET;
	VALVE12_1RESET;
	VALVE12_2RESET;
	VALVE34_1RESET;
	VALVE34_2RESET;
}



//End of File
