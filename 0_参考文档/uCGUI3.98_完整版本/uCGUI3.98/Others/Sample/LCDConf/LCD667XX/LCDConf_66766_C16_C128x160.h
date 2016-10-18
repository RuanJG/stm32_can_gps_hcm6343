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
File        : LCDConf_66766_C16_C128x160.h
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

#define LCD_CONTROLLER 66766

#define LCD_XSIZE        128
#define LCD_YSIZE        160

#define LCD_BITSPERPIXEL  16

/*********************************************************************
*
*                   Simple bus configuration
*
**********************************************************************
*/

void LCD_X_WriteM00(char * pData, int NumBytes);
void LCD_X_ReadM01 (char * pData, int NumBytes);
#define LCD_WRITEM_A1(Byte, NumBytes) LCD_X_WriteM01(Byte, NumBytes)
#define LCD_WRITEM_A0(Byte, NumBytes) LCD_X_WriteM00(Byte, NumBytes)
#define LCD_READM_A1(Byte, NumBytes)  LCD_X_ReadM01(Byte, NumBytes)

/*********************************************************************
*
*                   Initialisation macro
*
**********************************************************************
*/

#define LCD_INIT_CONTROLLER() \
  LCD_X_Init();                                                                                                                       \
  _WriteU16_A0(0x0000); _WriteU16_A1(0x0001); /* start oscillation                                                                 */ \
  GUI_Delay(100);                                                                                                                     \
  _WriteU16_A0(0x0001); _WriteU16_A1(0x0013); /* CMS=0 COM1-COM176 SGS=0 SEG1-SEG396, driver output control 1/160 DUTY NL4-0=10011 */ \
  _WriteU16_A0(0x0002); _WriteU16_A1(0x0000); /* RST=0 B/C=0 EOR=0, B pattern waveform NW5-0=000000                                */ \
  _WriteU16_A0(0x0004); _WriteU16_A1(0x0664); /* VR2-0=110, contrast control ct6-ct0:1111000                                       */ \
  _WriteU16_A0(0x0003); _WriteU16_A1(0x6578); /* BS3-0=0110 BT3-BT0=0101, 1/10 bias dc2-0=111 ap1-0=10 SLP=0 STB=0                 */ \
  _WriteU16_A0(0x000C); _WriteU16_A1(0x0005); /* vc2-0=101 0.73VCC                                                                 */ \
  GUI_Delay(100);                                                                                                                     \
  _WriteU16_A0(0x0003); _WriteU16_A1(0x4D78); /* BS3-0=0100 BT3-BT0=1101, 1/10 bias dc2-0=111 ap1-0=10 SLP=0 STB=0                 */ \
  _WriteU16_A0(0x0005); _WriteU16_A1(0x0030); /* spr=0 65K,hwm=0,id1-0=11,am=0,lg2-0=000                                           */ \
  _WriteU16_A0(0x0006); _WriteU16_A1(0x0000); /* CP7-0=00H                                                                         */ \
  _WriteU16_A0(0x0007); _WriteU16_A1(0x0001); /* no scrol, no screen-division,not reversal,display off                             */ \
  _WriteU16_A0(0x000B); _WriteU16_A1(0x0100); /* frame cycle control                                                               */ \
  _WriteU16_A0(0x0011); _WriteU16_A1(0x0000); /* VERTICAL SCROLL CONTROLL                                                          */ \
  _WriteU16_A0(0x0014); _WriteU16_A1(0xA000); /* se17-10=00h ss17-10=160h(com1-160)                                                */ \
  _WriteU16_A0(0x0015); _WriteU16_A1(0x0000); /* se27-20=00h ss27-20=00h(com1-160)                                                 */ \
  _WriteU16_A0(0x0016); _WriteU16_A1(0x7F00); /* hea7-0=7fh(end address),hsa7-0=00h(start address)                                 */ \
  _WriteU16_A0(0x0017); _WriteU16_A1(0x9F00); /* vea7-0=9fh,vsa7-0=00h                                                             */ \
  _WriteU16_A0(0x0020); _WriteU16_A1(0x0000); /* RAM MASK                                                                          */ \
  _WriteU16_A0(0x0007); _WriteU16_A1(0x0003)  /* no scrol, no screen-division,not reversal,display ON                              */

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
