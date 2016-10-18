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
File        : LCDConf_8822_M1_M160x160.h
Purpose     : Sample configuration file
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
#define LCD_CONTROLLER     6902

#define LCD_XSIZE          160
#define LCD_YSIZE          160

#define LCD_BITSPERPIXEL   1

/*********************************************************************
*
*       Simple bus configuration
*
**********************************************************************
*/
char LCD_X_Read01(void);
void LCD_X_Write00(char c);
void LCD_X_Write01(char c);
#define LCD_READ_A1(Byte)  Byte = LCD_X_Read01()
#define LCD_WRITE_A1(Byte) LCD_X_Write01(Byte)
#define LCD_WRITE_A0(Byte) LCD_X_Write00(Byte)

/*********************************************************************
*
*       Initialisation macro
*
**********************************************************************
*/
#define LCD_INIT_CONTROLLER() \
  LCD_X_Init(); \
  {volatile unsigned i; for (i = 0; i < 65535; i++); }    /* Wait a while */                                        \
  LCD_WRITE_A0(0x00); LCD_WRITE_A0(0xC5);                 /* LCD Control Register(WLCR)                             */ \
  LCD_WRITE_A0(0x01); LCD_WRITE_A0(0xF1);                 /* Misc.Register(MIR)                                     */ \
  LCD_WRITE_A0(0x02); LCD_WRITE_A0(0x10);                 /* Advance Power Setup Register(APSR)                     */ \
  LCD_WRITE_A0(0x10); LCD_WRITE_A0(0x68);                 /* Whole Chip Cursor Control Register (WCCR)              */ \
  LCD_WRITE_A0(0x20); LCD_WRITE_A0((LCD_XSIZE >> 3) - 1); /* Active Window Right Register(AWRR)                     */ \
  LCD_WRITE_A0(0x30); LCD_WRITE_A0(LCD_YSIZE - 1);        /* Active Window Bottom Register(AWBR)                    */ \
  LCD_WRITE_A0(0x40); LCD_WRITE_A0(0x00);                 /* Active Window Left Register(AWLR)                      */ \
  LCD_WRITE_A0(0x50); LCD_WRITE_A0(0x00);                 /* Active Window Top Register(AWTR)                       */ \
  LCD_WRITE_A0(0x21); LCD_WRITE_A0((LCD_XSIZE >> 3) - 1); /* Display Window Right Register(DWRR)                    */ \
  LCD_WRITE_A0(0x31); LCD_WRITE_A0(LCD_YSIZE - 1);        /* Display Window Bottom Register(DWBR)                   */ \
  LCD_WRITE_A0(0x41); LCD_WRITE_A0(0x00);                 /* Display Window Left Register(DWLR)                     */ \
  LCD_WRITE_A0(0x51); LCD_WRITE_A0(0x00);                 /* Display Window Top Register(DWTR)                      */ \
  LCD_WRITE_A0(0x81); LCD_WRITE_A0(0x0C);                 /* Frame Rate Polarity Change at Common_A Register (FRCA) */ \
  LCD_WRITE_A0(0xD0); LCD_WRITE_A0(0x1F)                  /* LCD Contrast Control Register (LCCR)                   */

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
