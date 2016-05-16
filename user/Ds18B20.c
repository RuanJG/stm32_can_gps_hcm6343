/******************************************************************************
 * @file:    Ds18B20.c
 * @purpose: Control DS18B20 temperature sensor with Stm32 Source File
 * @version: V1.00
 * @date:    11. Jul 2011
 *----------------------------------------------------------------------------
 ******************************************************************************/

#include <stm32f10x.h>
#include "global.h"													   
#include "stm32f10x_tim.h"

/* Config GPIO setting, use Pin11 as the single data line*/
#define DS_H         GPIOA->BSRR = GPIO_Pin_1 				//set data line to high
#define DS_L         GPIOA->BRR  = GPIO_Pin_1  				//set data line to low
#define DS_READ      GPIOA->IDR  & GPIO_Pin_1 				//read the pin connected to data line

#define	DS_SKIP_ROM	0xCC
#define DS_CONVERT	0x44
#define DS_READ_PAD	0xBE
#define WRITE	1
#define READ	0					 

// declaration of functions
//local
void ds_GPIO(u8 mode);
void ds_reset(void);
u8 ds_readbyte(void);
void ds_writebyte(u8 cmd);
void ds_convert(void);
void Read_DS18B20(void);
//extern
extern void delay_us(u16 us);
extern FtoBsTypeDef FloatToBytes(float data);

// declaration of variables
//local
FtoBsTypeDef TempBytes;
//extern
extern u8 rod_down;
	char number[300] = {0};
		u16 nu = 0;
// definition of functions
/**
 * @brief  Change the GPIO mode of the pin for data line
 * @param  u8, 1 for WRITE, 0 for READ
 * @return none
 */
void ds_GPIO(u8 mode){
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;				//DS18B20 Pin 11	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(mode==WRITE){
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  
	}else{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		
	}
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
}

/**
 * @brief  Stm32 resets the data line
 * @param  none
 * @return none
 *
 * should reset the data line before each command sent
 */
void ds_reset(void){
	u8 flag = 0;
	u8 i,j;
	for(i=0; i<10; i++){									
		ds_GPIO(WRITE);										//stm32 send reset signal to DS18B20
		delay_us(5);
		DS_L;
		delay_us(500);										//min 480 us
		DS_H;
		delay_us(2);
		ds_GPIO(READ);										
		delay_us(60);

		for(j=0; j<10; j++){
			if(~DS_READ){ 									//receive response from DS18B20
				delay_us(240);								//max 240us
				if(DS_READ){
					flag = 1;
					break;
				}
			}
		}
		if(flag == 1){
			break;
		}   
	}
	ds_GPIO(WRITE);
}

/**
 * @brief  Stm32 receive 1 byte from DS18B20
 * @param  none
 * @return u8 byte
 *
 */
u8 ds_readbyte(void){
	u8 temp=0;
	u8 i;
	for(i=0;i<8;i++){
		temp>>=1;											// receive LSB first
		ds_GPIO(WRITE);
		delay_us(2);
		DS_L;		  										
		delay_us(2);
		ds_GPIO(READ);
		delay_us(1);

		if(DS_READ){
			temp |= 0x80;
		}
		delay_us(70);										// wait for DS18B20 to release within the slot, max 60
	//	delay_us(2);										// inter-bit delay at least 1us
	}
	ds_GPIO(WRITE);
	return temp;
}

/**
 * @brief  Stm32 transmit 1 byte to DS18B20
 * @param  u8 1 byte command
 * @return none
 *
 */
void ds_writebyte(u8 cmd){
	u8 i;
	ds_GPIO(WRITE);
	delay_us(2);
	for(i=0;i<8;i++){
		DS_L;
		delay_us(8);										// should release within 15us if write 1 
		if(cmd&0x01){
			DS_H;
		}
		delay_us(80);										// transmit slot should be longer than 60us
		DS_H;												// pull data line high after transmitting 1 bit
		cmd >>= 1;											// transmit LSB first
		delay_us(2);										// at least 1us between bits
	}
	ds_GPIO(WRITE);	
}
 
/**
 * @brief  Get 1 temperature conversion and modify the global variable of temperature
 * 			ask temperature, wait 760ms, read value
 * @param  u8 1 byte command
 * @return none, modify GlobalVariable.tempWater
 *
 * to get 12 bit reading, conversion time is 750ms
 */
void ds_convert(void){
	u16 tempL,tempH;
	s16 temp = 0;
	u8 i;
		
	ds_reset();
	ds_writebyte(DS_SKIP_ROM);
	ds_writebyte(DS_CONVERT);
	//delay_us(760000);	delay 760ms
	ds_reset();
	ds_writebyte(DS_SKIP_ROM);
	ds_writebyte(DS_READ_PAD);
	tempL = (u16)ds_readbyte();
	tempH = (u16)ds_readbyte();
	ds_reset();

	temp = (tempH<<8)|tempL;
	GlobalVariable.tempWater = temp*0.0625f;					//bit 8 in tempL is 2^-4
	TempBytes = FloatToBytes(GlobalVariable.tempWater);
	for(i=0; i<4; i++){
		GlobalVariable.tempWaterBytes[i] = TempBytes.bArray[i];	
	}
}


/**
 * @brief  Get 1 temperature conversion and modify the global variable of temperature
 *			read last temp, ask one new conversion
 * @param  u8 1 byte command
 * @return none, modify GlobalVariable.tempWater
 *
 * to get 12 bit reading, conversion time is 750ms
 */
void Read_DS18B20(void){
	u16 tempL, tempH;
	s16 temp = 0;



	u8 i = 0;
//	if(rod_down==1) 
	//无论收放杆有没有放下去，都要测水温
	{
		// get last conversion value
		ds_reset();
		ds_writebyte(DS_SKIP_ROM);
		ds_writebyte(DS_READ_PAD);
		tempL = (u16)ds_readbyte();
		tempH = (u16)ds_readbyte();
		ds_reset();
	
		temp = (tempH<<8)|tempL;
		GlobalVariable.tempWater = temp*0.0625f;					//bit 8 in tempL is 2^-4
		/**********************增加判断**********************/
	
		TempBytes = FloatToBytes(GlobalVariable.tempWater);
		for(i=0; i<4; i++)
		{
			GlobalVariable.tempWaterBytes[i] = TempBytes.bArray[i];	
		}			
		// ask for next conversion
		ds_reset();
		ds_writebyte(DS_SKIP_ROM);
		ds_writebyte(DS_CONVERT);
			
		number[nu] = GlobalVariable.tempWater;
		nu ++;
		if(nu > 300)
		{
			nu = 0;
		}		
	}
}

// end of file
