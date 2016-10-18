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
File        : LCDConf_13701_C12_C256x64.h
Purpose     : emWin sample configuration file for Epson S1D13701 OLED controller
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

#define LCD_XSIZE             256
#define LCD_YSIZE              64

#define LCD_CONTROLLER      13701

#define LCD_BITSPERPIXEL       12 /* Currently supported modes are 9 and 12 bpp */

/*********************************************************************
*
*                   Simple bus configuration
*
**********************************************************************
*/

void           LCD_X_Write00_16 (unsigned short c);
void           LCD_X_Write01_16 (unsigned short c);
unsigned short LCD_X_Read00_16  (void);
unsigned short LCD_X_Read01_16  (void);

#define LCD_READ_A0(Word)              Word = LCD_X_Read00_16()
#define LCD_READ_A1(Word)              Word = LCD_X_Read01_16()
#define LCD_WRITE_A1(Word)             LCD_X_Write01_16(Word)
#define LCD_WRITE_A0(Word)             LCD_X_Write00_16(Word)

/*********************************************************************
*
*       Initialisation macro
*
**********************************************************************
*/

#if   LCD_BITSPERPIXEL == 12
  #define REG_GREY_SCALE   0x0003
#elif LCD_BITSPERPIXEL ==  9
  #define REG_GREY_SCALE   0x0002
  #define LCD_FIXEDPALETTE     -1
#else
  #error Unsupported color depth!
#endif

#define LCD_INIT_CONTROLLER()                                                                                    \
  LCD_X_Init();                                                                                                  \
  LCD_WRITE_A0(0x0001); LCD_WRITE_A1(0x0001); /* Host Bus Width Setup, 1: Selects 16-bit data bus */             \
  LCD_WRITE_A0(0x0003); LCD_WRITE_A1(0x0003); /* Interrupt enable setup: Enable */                               \
  LCD_WRITE_A0(0x0005); LCD_WRITE_A1(0x0048); /* Interrupt cycle/type setup: Level output, each one frame */     \
  LCD_WRITE_A0(0x000F); LCD_WRITE_A1(0x0001); /* Interrupt cancel: H; clear */                                   \
  LCD_WRITE_A0(0x0011); LCD_WRITE_A1(REG_GREY_SCALE); /* Gray scale setup: 16 gray scale */                      \
  LCD_WRITE_A0(0x0013); LCD_WRITE_A1(0x005F); /* Display horizontal dot setup: 256dot (256*3/8-1=5Fh) */         \
  LCD_WRITE_A0(0x0015); LCD_WRITE_A1(0x003F); /* Display line setup: 64line(64-1=3Fh) */                         \
  LCD_WRITE_A0(0x0017); LCD_WRITE_A1(0x0000); /* Display data mask: None */                                      \
  LCD_WRITE_A0(0x001F); LCD_WRITE_A1(0x0000); /* Display start address: adress "0" */                            \
  LCD_WRITE_A0(0x0021); LCD_WRITE_A1(0x0000); /* Driver select: Select OKI driver IC */                          \
  LCD_WRITE_A0(0x0023); LCD_WRITE_A1(0x0005); /* DCLK setup: 1/6 system clock */                                 \
  LCD_WRITE_A0(0x0025); LCD_WRITE_A1(0x0010); /* Discharge period 1a setup: 16DCLK */                            \
  LCD_WRITE_A0(0x0027); LCD_WRITE_A1(0x000C); /* Discharge period 1b setup: 16DCLK */                            \
  LCD_WRITE_A0(0x0029); LCD_WRITE_A1(0x0006); /* Discharge period 2a setup: 12DCLK */                            \
  LCD_WRITE_A0(0x002B); LCD_WRITE_A1(0x0010); /* Discharge period 2b setup: 12DCLK */                            \
  LCD_WRITE_A0(0x002D); LCD_WRITE_A1(0x0001); /* Free running period setup: FR=(5.0E+6)/64/(256*3+2)=101Hz */    \
  LCD_WRITE_A0(0x002F); LCD_WRITE_A1(0x0000); /* Offset period setup: None */                                    \
  LCD_WRITE_A0(0x0031); LCD_WRITE_A1(0x0000); /* Driver output signal reverse setup: None */                     \
  LCD_WRITE_A0(0x0041); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  1st gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0045); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  2nd gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0049); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  3rd gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x004d); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  4th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0051); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  5th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0055); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  6th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0059); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  7th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x005d); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  8th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0061); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /*  9th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0065); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /* 10th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0069); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /* 11th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x006D); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /* 12th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0071); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /* 13th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0075); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /* 14th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0079); LCD_WRITE_A1(0x3131); LCD_WRITE_A1(0x0031); /* 15th gray scale pulse setup: 49DCLK */    \
  LCD_WRITE_A0(0x0093); LCD_WRITE_A1(0x0001); /* General-purpose output setup */                                 \
  {                                                                                                              \
    U8 i, Level;                                                                                                 \
    U16 Data;                                                                                                    \
    Level = 0xff;                                                                                                \
    for (i = 0; i < 7; i++) {                                                                                    \
      Data = Level << 1;                                                                                         \
      Level >>= 1;                                                                                               \
      LCD_WRITE_A0(0x0091);                                                                                      \
      LCD_WRITE_A1(0x0000 | Data);                                                                               \
      LCD_WRITE_A0(0x0091);                                                                                      \
      LCD_WRITE_A1(0x0001 | Data);                                                                               \
    }                                                                                                            \
  }                                                                                                              \
  LCD_WRITE_A0(0x007f); LCD_WRITE_A1(0x0001)  /* Driver Output On */

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
