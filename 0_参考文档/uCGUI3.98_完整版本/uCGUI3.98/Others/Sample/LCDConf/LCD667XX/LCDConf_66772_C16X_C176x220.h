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
File        : LCDConf_66772_C16X_C176x220.h
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

#define LCD_CONTROLLER  66772

#define LCD_BITSPERPIXEL   16

#define LCD_XSIZE         176
#define LCD_YSIZE         220

#define LCD_MIRROR_X        1

#define LCD_NUM_DUMMY_READS 5

#define LCD_USE_SERIAL_3PIN 1

#define LCD_SERIAL_ID       0

/*********************************************************************
*
*                   Simple bus configuration
*
**********************************************************************
*/

void LCD_X_WriteM(char * pData, int NumBytes);
void LCD_X_ReadM (char * pData, int NumBytes);
void LCD_X_ClrCS (void);
void LCD_X_SetCS (void);

#define LCD_WRITEM(Byte, NumBytes) LCD_X_WriteM(Byte, NumBytes)
#define LCD_READM(Byte, NumBytes)  LCD_X_ReadM(Byte, NumBytes)
#define LCD_SETCS()                LCD_X_SetCS()
#define LCD_CLRCS()                LCD_X_ClrCS()

/*********************************************************************
*
*                   Initialisation macro
*
**********************************************************************
*/

#define LCD_INIT_CONTROLLER() \
  LCD_X_Init();                               /* Init port lines                      */ \
  GUI_Delay(10);                              /* Wait 10 ms                           */ \
  _WriteU16_A0(0x0000); _WriteU16_A1(0x0001); /* Start Oscillation                  : */ \
  GUI_Delay(10);                              /* Wait 10 ms                           */ \
  _WriteU16_A0(0x0014); _WriteU16_A1(0x381F); /* Power Control 5                    : VCOMG = 1, VD4-0 = 11000, VCM4-0 = 11111 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0102); /* Gate driver interface control      : TE = 1, IDX2-0 = 001 */ \
  _WriteU16_A0(0x0007); _WriteU16_A1(0x0001); /* Display control 1                  : PT1-0 = 00, VLE2-1 = 0, SPT = 0, GON = 0, DTE = 0, CL = 0, REV = 0, D1-0 = 00 */ \
  _WriteU16_A0(0x0010); _WriteU16_A1(0x0000); /* Power control 1                    : SAP2-0 = 000, BT2-0 = 000, DC2-0 = 000, AP2-0 = 000, SLP = 0, STB = 0 */ \
  _WriteU16_A0(0x0011); _WriteU16_A1(0x0000); /* Power control 2                    : CAD = 0, VRN4-0 = 00000, VRP4-0 = 00000 */ \
  _WriteU16_A0(0x0012); _WriteU16_A1(0x0000); /* Power control 3                    : VC2-0 = 000 */ \
  _WriteU16_A0(0x0013); _WriteU16_A1(0x0608); /* Power control 4                    : VRL3-0 = 0110, PON = 0, VRH3-0 = 1000 */ \
  _WriteU16_A0(0x0014); _WriteU16_A1(0x300F); /* Power control 5                    : VCOMG=1, VD4-VD0=10000, VCM4-VCM0=01111 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0100); /* Gate driver interface control      : TE=1, IDX2-IDX0=000 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0101); /* Gate driver interface control      : TE=1, IDX2-IDX0=001 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0102); /* Gate driver interface control      : TE=1, IDX2-IDX0=010 */ \
  _WriteU16_A0(0x0010); _WriteU16_A1(0x0808); /* Power control 1                    : SAP2-0 = 001, BT2-0 = 000, DC2-0 = 000, AP2-0 = 010, SLP = 0, STB = 0 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0100); /* Gate driver interface control      : TE=1, IDX2-IDX0=000 */ \
  _WriteU16_A0(0x0013); _WriteU16_A1(0x0619); /* Power control 4                    : VRL3-0 = 0110, PON = 1, VRH3-0 = 1001 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0101); /* Gate driver interface control      : TE=1, IDX2-IDX0=001 */ \
  _WriteU16_A0(0x0001); _WriteU16_A1(0x001B); /* Driver output control              : EPL = 0, GS = 0, SS = 0, NL4-0 = 11011 */ \
  _WriteU16_A0(0x0002); _WriteU16_A1(0x0700); /* LCD-driving-waveform               : FLD1-0 = 01, B/C = 1, EOR = 1, NW5-0 = 000000 */ \
  _WriteU16_A0(0x0003); _WriteU16_A1(0x0030); /* Entry mode                         : BGR = 0, HWM = 0, ID1-0 = 11, AM = 0, LG2-0 = 0 */ \
  _WriteU16_A0(0x0004); _WriteU16_A1(0x0000); /* Compare register 1                 : CP11-6 = 000000, CP5-0 = 000000 */ \
  _WriteU16_A0(0x0005); _WriteU16_A1(0x0000); /* Compare register 2                 : CP17-12 = 000000 */ \
  _WriteU16_A0(0x0008); _WriteU16_A1(0x0808); /* Display control 2                  : FP3-0 = 1000, BP3-0 = 1000 */ \
  _WriteU16_A0(0x0023); _WriteU16_A1(0x0000); /* RAM data write mask 1              : WM11-6 = 000000, WM5-0 = 000000 */ \
  _WriteU16_A0(0x0024); _WriteU16_A1(0x0000); /* RAM data write mask 2              : WM17-12 = 000000 */ \
  _WriteU16_A0(0x000B); _WriteU16_A1(0x0005); /* Frame cycle control                : NO1-0 = 00, SDT1-0 = 00, EQ1-0 = 00, DIV1-0 = 00, RTN3-0 = 0101 */ \
  _WriteU16_A0(0x000C); _WriteU16_A1(0x0000); /* External display interface control : RM = 0, DM1-0 = 00, RIM1-0 = 00 */ \
  _WriteU16_A0(0x0040); _WriteU16_A1(0x0000); /* Gate scan start position           : SCN4-0 = 00000 */ \
  _WriteU16_A0(0x0041); _WriteU16_A1(0x0000); /* Vertical scroll control            : VL7-0 = 00000000 */ \
  _WriteU16_A0(0x0042); _WriteU16_A1(0xEF00); /* 1st screen driving position        : SE17-10 = 11101111, SS17-10 = 00000000 */ \
  _WriteU16_A0(0x0043); _WriteU16_A1(0xEF00); /* 2nd screen driving position        : SE27-10 = 11101111, SS27-10 = 00000000 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0106); /* Gate driver interface control      : TE=1, IDX2-0=110 */ \
  _WriteU16_A0(0x0030); _WriteU16_A1(0x0100); /* Gamma Control 1                    : */ \
  _WriteU16_A0(0x0031); _WriteU16_A1(0x0707); /* Gamma Control 2                    : */ \
  _WriteU16_A0(0x0032); _WriteU16_A1(0x0102); /* Gamma Control 3                    : */ \
  _WriteU16_A0(0x0033); _WriteU16_A1(0x0000); /* Gamma Control 4                    : */ \
  _WriteU16_A0(0x0034); _WriteU16_A1(0x0506); /* Gamma Control 5                    : */ \
  _WriteU16_A0(0x0035); _WriteU16_A1(0x0000); /* Gamma Control 6                    : */ \
  _WriteU16_A0(0x0036); _WriteU16_A1(0x0706); /* Gamma Control 7                    : */ \
  _WriteU16_A0(0x0037); _WriteU16_A1(0x0000); /* Gamma Control 8                    : */ \
  _WriteU16_A0(0x003F); _WriteU16_A1(0x0000); /* Gamma Control 9                    : */ \
  _WriteU16_A0(0x0044); _WriteU16_A1(0xAF00); /* Horizontal RAM address position    : HEA7-0 = 10101111, HSA7-0 = 00000000 */ \
  _WriteU16_A0(0x0045); _WriteU16_A1(0xDB00); /* Vertical RAM address position      : VEA7-0 = 11011011, VSA7-0 = 00000000 */ \
  _WriteU16_A0(0x0021); _WriteU16_A1(0x0000); /* RAM address set                    : AD15-0 = 0000000000000000 */ \
  _WriteU16_A0(0x0007); _WriteU16_A1(0x0021); /* Display control 1                  : PT1-0 = 00, VLE2-1 = 0, SPT = 0, GON = 1, DTE = 0, CL = 0, REV = 0, D1-0 = 01 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0100); /* Gate driver interface control      : TE=1, IDX2-IDX0=000 */ \
  _WriteU16_A0(0x0007); _WriteU16_A1(0x0023); /* Display control 1                  : PT1-0 = 00, VLE2-1 = 0, SPT = 0, GON = 1, DTE = 0, CL = 0, REV = 0, D1-0 = 11 */ \
  _WriteU16_A0(0x0007); _WriteU16_A1(0x0033); /* Display control 1                  : PT1-0 = 00, VLE2-1 = 0, SPT = 0, GON = 1, DTE = 1, CL = 0, REV = 0, D1-0 = 11 */ \
  _WriteU16_A0(0x0012); _WriteU16_A1(0x0000); /* Power control 3                    : VC2-0 = 000 */ \
  _WriteU16_A0(0x000A); _WriteU16_A1(0x0101)  /* Gate driver interface control      : TE=1, IDX2-IDX0=001 */

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
