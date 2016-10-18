/*
*********************************************************************************************************
*                                             uC/GUI V3.98
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : LCDConf_1200_C16_C128x160.h
Purpose     : Sample configuration file for Toppoly C0C0 controller
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

/*********************************************************************
*
*       General configuration of LCD
*
**********************************************************************
*/
#define LCD_CONTROLLER  1200

#define LCD_XSIZE        128
#define LCD_YSIZE        160

#define LCD_BITSPERPIXEL  16

#define LCD_SWAP_RB        1

/*********************************************************************
*
*       Simple bus configuration
*
**********************************************************************
*/
void           LCD_X_Write00_16 (unsigned short c);
void           LCD_X_Write01_16 (unsigned short c);
void           LCD_X_WriteM01_16(unsigned short * pData, int NumWords);
unsigned short LCD_X_Read01_16  (void);

#define LCD_READ_A1(Word)              Word = LCD_X_Read01_16()
#define LCD_WRITE_A1(Word)             LCD_X_Write01_16(Word)
#define LCD_WRITE_A0(Word)             LCD_X_Write00_16(Word)
#define LCD_WRITEM_A1(pData, NumWords) LCD_X_WriteM01_16(pData, NumWords)

/*********************************************************************
*
*       Initialisation macro
*
**********************************************************************
*/
#define LCD_INIT_CONTROLLER() \
  LCD_X_Init(); \
  LCD_WRITE_A0(0x0001); LCD_WRITE_A1(0x0002); /* 7-6=00: panel resolution 128x160, 4-2=000: 16 bit color mod, 1=0: power on, 0=0: out of range data white */ \
  LCD_WRITE_A0(0x0002); LCD_WRITE_A1(0x0012); /* 7=0: sub panel off (CSV=1), 5=0: line inversion, 4=1: sub panel off, 3=0: main panel on */                  \
  LCD_WRITE_A0(0x0003); LCD_WRITE_A1(0x0060); /* 7-6=01: 16 bit mode, 5-4=10: 16 bit transfer, 18 bit data (BODR=10) */                                      \
  LCD_WRITE_A0(0x0005); LCD_WRITE_A1(0x0008); /* 4-3=01: typical bias, 5,2-0=0,00: 1.82MHz */                                                                \
  LCD_WRITE_A0(0x0008); LCD_WRITE_A1(0x000C); /* 6-0=0001100: VCOM_L 0.197 V */                                                                              \
  LCD_WRITE_A0(0x0007); LCD_WRITE_A1(0x0074); /* 6-0=1110100: VCOM_H 4.003 V */                                                                              \
  LCD_WRITE_A0(0x0021); LCD_WRITE_A1(0x0000); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0022); LCD_WRITE_A1(0x0000); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0023); LCD_WRITE_A1(0x0024); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0024); LCD_WRITE_A1(0x001B); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0025); LCD_WRITE_A1(0x0009); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0026); LCD_WRITE_A1(0x0000); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0027); LCD_WRITE_A1(0x0000); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0028); LCD_WRITE_A1(0x0000); /* gamma adjustment 2.0 */                                                                                     \
  LCD_WRITE_A0(0x0006); LCD_WRITE_A1(0x00C7)  /* 7=1: internal register, 6=1: power on */

#endif
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
