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
File        : LCDConf_0108_M1_M128x64.h
Purpose     : Sample configuration file
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

/*********************************************************************
*
*                   General configuration of LCD
*
**********************************************************************
*/

#define LCD_CONTROLLER     1502

#define LCD_XSIZE          128
#define LCD_YSIZE          64
#define LCD_BITSPERPIXEL   1

#define LCD_NUM_CONTROLLERS   2
#define LCD_FIRSTSEG0         0
#define LCD_LASTSEG0         63
#define LCD_FIRSTSEG1        64
#define LCD_LASTSEG1        127
#define LCD_FIRSTCOM1          LCD_FIRSTCOM0
#define LCD_LASTCOM1           LCD_LASTCOM0

/*********************************************************************
*
*                   Simple bus configuration
*
**********************************************************************
*/

void LCD_X_Write00(char c);
void LCD_X_Write01(char c);
char LCD_X_Read00(void);
char LCD_X_Read01(void);
void LCD_X_Write10(char c);
void LCD_X_Write11(char c);
char LCD_X_Read10(void);
char LCD_X_Read11(void);
#define LCD_WRITE_A1(Byte)   LCD_X_Write01(Byte)
#define LCD_WRITE_A0(Byte)   LCD_X_Write00(Byte)
#define LCD_READ_A1(Byte)    Byte = LCD_X_Read01()
#define LCD_READ_A0(Byte)    Byte = LCD_X_Read00()
#define LCD_WRITE_A1C1(Byte) LCD_X_Write11(Byte)
#define LCD_WRITE_A0C1(Byte) LCD_X_Write10(Byte)
#define LCD_READ_A1C1(Byte)  Byte = LCD_X_Read11()
#define LCD_READ_A0C1(Byte)  Byte = LCD_X_Read10()

/*********************************************************************
*
*                   Initialisation macro
*
**********************************************************************
*/

void LCD_X_Init(void);
#define LCD_INIT_CONTROLLER() LCD_X_Init()

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
