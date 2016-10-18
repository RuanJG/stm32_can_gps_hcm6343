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
File        : LCDConf_1201_C16_LDB4_C176x220.h
Purpose     : Sample configuration file for Toppoly C0E0 controller
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
#define LCD_CONTROLLER  1201

#define LCD_XSIZE        176
#define LCD_YSIZE        220
#define LCD_MIRROR_X       1

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
#define LCD_INIT_CONTROLLER()                                                                    \
  LCD_X_Init();                                                                                  \
  LCD_WRITE_A0(0x0001); LCD_WRITE_A1(0x0000); /* 7-6 =     00: Panel resolution 176x220          \
                                                 5   =      0: Horizontal normal scan            \
                                                 4   =      0: Vertical normal scan              \
                                                 3   =      0: SYNC polarity negative            \
                                                 2   =      0: VInput mode selection DE          \
                                                 1   =      1: Input data selection 16 bit */    \
  LCD_WRITE_A0(0x0002); LCD_WRITE_A1(0x0080); /* 7   =      1: Full color mode                   \
                                                 6-5 =     00: Moving mode                       \
                                                 4   =      0: Out of window data select disable \
                                                 3   =      0: 1 line inversion                  \
                                                 2   =      0: Out of window data white          \
                                                 1   =      0: Vcom output from OP driver        \
                                                 0   =      0: Dithering OFF */                  \
  LCD_WRITE_A0(0x0018); LCD_WRITE_A1(0x0006)  /* 3-2 =     01: 16 bit mode                       \
                                                 1-0 =     10: 16 bit transfer, 18 bit data */

#endif
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
