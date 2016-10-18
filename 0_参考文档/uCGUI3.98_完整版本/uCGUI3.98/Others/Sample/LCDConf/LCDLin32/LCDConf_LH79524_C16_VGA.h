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
File        : LCDConf_LH79524_C16_VGA.h
Purpose     : Configuration file for LH79524, 16bpp, 640x480
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

/*********************************************************************
*
*       General configuration
*
**********************************************************************
*/
#define LCD_VRAM_ADR         0x2006A000
#define LCD_BITSPERPIXEL   16
#define LCD_CONTROLLER   3200
#define LCD_XSIZE         640
#define LCD_YSIZE         480
#define LCD_ENDIAN_BIG      0
#define LCD_FIXEDPALETTE  555

/*********************************************************************
*
*       SFR register definitions
*
**********************************************************************
*/
/* RESET, CLOCK AND POWER CONTROLLER */
#define __RCPCBASE  0xFFFE2000
#define __PERIPHCLKCTRL1    *(volatile U32*)(__RCPCBASE + 0x0028)
#define __AHBCLKCTRL        *(volatile U32*)(__RCPCBASE + 0x002C)
#define __LCDCLKPRESCALE	  *(volatile U32*)(__RCPCBASE + 0x0040)

/* COLOR LCD CONTROLLER */
#define __CLCDCBASE  0xFFFF4000
#define __LCDTIMING0        *(volatile U32*)(__CLCDCBASE + 0x0000)
#define __LCDTIMING1        *(volatile U32*)(__CLCDCBASE + 0x0004)
#define __LCDTIMING2        *(volatile U32*)(__CLCDCBASE + 0x0008)
#define __LCDUPBASE         *(volatile U32*)(__CLCDCBASE + 0x0010)
#define __LCDLPBASE	        *(volatile U32*)(__CLCDCBASE + 0x0014)
#define __LCDINTRENABLE     *(volatile U32*)(__CLCDCBASE + 0x0018)
#define __LCDCONTROL        *(volatile U32*)(__CLCDCBASE + 0x001C)
#define __LCDSTATUS	        *(volatile U32*)(__CLCDCBASE + 0x0020)
#define __LCDINTERRUPT      *(volatile U32*)(__CLCDCBASE + 0x0024)
#define __LCDPALLETTE       *(volatile U32*)(__CLCDCBASE + 0x0200)

/* HR-TFT LCD TIMING CONTROLLER */
#define __HRTFTCBASE  0xFFFE4000
#define __HRTFTCSETUP       *(volatile U32*)(__HRTFTCBASE + 0x0000)
#define __HRTFTCCONTROL     *(volatile U32*)(__HRTFTCBASE + 0x0004)
#define __HRTFTCTIMING1     *(volatile U32*)(__HRTFTCBASE + 0x0008)
#define __HRTFTCTIMING2     *(volatile U32*)(__HRTFTCBASE + 0x000C)

/* INPUT/OUTPUT CONTROL */
#define __IOCONBASE  0xFFFE5000
#define __MUXCTL1             *(volatile U32*)(__IOCONBASE + 0x0000)
#define __MUXCTL19            *(volatile U32*)(__IOCONBASE + 0x0090)
#define __MUXCTL20            *(volatile U32*)(__IOCONBASE + 0x0098)
#define __MUXCTL21            *(volatile U32*)(__IOCONBASE + 0x00A0)
#define __MUXCTL22            *(volatile U32*)(__IOCONBASE + 0x00A8)

/*********************************************************************
*
*       Initialization macro
*
**********************************************************************
*/
#define LCD_INIT_CONTROLLER()                                                                              \
  __AHBCLKCTRL       = 0x00000000;                                                                         \
  __PERIPHCLKCTRL1   &= ~(  1 <<  0);                 /* Enable clock for LCD Controller */                \
  __LCDCLKPRESCALE   = 0x00000000;                    /* LCD clock prescaler */                            \
  __MUXCTL1          |=  (  1 <<  0)                  /* PL1 = LCDVDD15 */                                 \
                     |   (  1 <<  2);                 /* PL0 = LCDVDD14 */                                 \
  __MUXCTL19         |=  (  1 << 12)                  /* PE6 = LCDVEEN */                                  \
                     |   (  1 <<  8)                  /* PE5 = LCDVDDEN */                                 \
                     |   (  2 <<  4)                  /* PE4 = LCDREV */                                   \
                     |   (  1 <<  2);                 /* PE3 = LCDCLS */                                   \
  __MUXCTL20         |=  (  1 << 10)                  /* PE1 = LCDDCLK */                                  \
                     |   (  1 <<  6)                  /* PE0 = LCDLP */                                    \
                     |   (  1 <<  2)                  /* PF7 = LCDFP */                                    \
                     |   (  1 <<  0);                 /* PF6 = LCDEN */                                    \
  __MUXCTL21         |=  (  1 << 10)                  /* PF5 = LCDVDD11 */                                 \
                     |   (  1 <<  8)                  /* PL3 = LCDVDD13 */                                 \
                     |   (  1 <<  6)                  /* PF4 = LCDVDD10 */                                 \
                     |   (  1 <<  4)                  /* PL2 = LCDVDD12 */                                 \
                     |   (  1 <<  2)                  /* PF3 = LCDVDD9 */                                  \
                     |   (  1 <<  0);                 /* PF2 = LCDVDD8 */                                  \
  __MUXCTL22         |=  (  1 << 14)                  /* PF1 = LCDVDD7 */                                  \
                     |   (  1 << 12)                  /* PF0 = LCDVDD6 */                                  \
                     |   (  1 << 10)                  /* PG7 = LCDVDD5 */                                  \
                     |   (  1 <<  8)                  /* PG6 = LCDVDD4 */                                  \
                     |   (  1 <<  6)                  /* PG5 = LCDVDD3 */                                  \
                     |   (  1 <<  4)                  /* PG4 = LCDVDD2 */                                  \
                     |   (  1 <<  2)                  /* PG3 = LCDVDD1 */                                  \
                     |   (  1 <<  0);                 /* PG2 = LCDVDD0 */                                  \
  __LCDTIMING0       =   ( 96 << 24)                  /* Horizontal Back Porch */                          \
                     |   ( 50 << 16)                  /* Horizontal Front Porch */                         \
                     |   ( 16 <<  8)                  /* Horizontal Synchronization Pulse Width */         \
                     |   ((LCD_XSIZE / 16 - 1) << 2); /* Pixels-Per-Line */                                \
  __LCDTIMING1       =   ( 26 << 24)                  /* Vertical Back Porch */                            \
                     |   (  4 << 16)                  /* Vertical Front Porch */                           \
                     |   (  7 << 10)                  /* Vertical Synchronization (Pulse) Width */         \
                     |   (LCD_YSIZE - 1);             /* Lines Per Panel */                                \
  __LCDTIMING2       =   (  0 << 27)                  /* Panel Clock Divisor (upper five bits) */          \
                     |   (  0 << 26)                  /* Bypass Pixel Clock Divider */                     \
                     |   ((LCD_XSIZE - 1) << 16)      /* Clocks Per Line */                                \
                     |   (  0 << 14)                  /* Invert Output Enable */                           \
                     |   (  0 << 13)                  /* Invert Panel Clock */                             \
                     |   (  1 << 12)                  /* Invert Horizontal Synchronization */              \
                     |   (  1 << 11)                  /* Invert the Vertical Synchronization Signal */     \
                     |   (  0 <<  6)                  /* AC Bias Signal Frequency */                       \
                     |   (  0 <<  0);                 /* Panel Clock Divisor */                            \
  __LCDUPBASE        = LCD_VRAM_ADR;                  /* Upper Panel Frame Buffer Base Address Register */ \
  __LCDINTRENABLE    = 0x00000000;                    /* Interrupt Enable Register */                      \
  __LCDCONTROL       =   (  1 << 16)                  /* LCD DMA FIFO Watermark Level */                   \
                     |   (  0 << 12)                  /* LCD Vertical Compare */                           \
                     |   (  1 << 11)                  /* LCD Power Enable */                               \
                     |   (  0 << 10)                  /* Big-Endian Pixel Ordering */                      \
                     |   (  0 <<  9)                  /* Big-Endian Byte Ordering to the LCD */            \
                     |   (  0 <<  8)                  /* RGB or BGR Format Selection */                    \
                     |   (  0 <<  7)                  /* Dual Panel STN LCD */                             \
                     |   (  0 <<  6)                  /* Monochrome LCD */                                 \
                     |   (  1 <<  5)                  /* TFT LCD */                                        \
                     |   (  0 <<  4)                  /* Monochrome STN LCD */                             \
                     |   (  4 <<  1)                  /* LCD Bits-Per-Pixel */                             \
                     |   (  1 <<  0);                 /* Color LCD Controller Enable */                    \
  __HRTFTCSETUP      =   (  0 <<  4)                  /* Pixels Per Line */                                \
                     |   (  0 <<  0);                 /* Conversion Mode Select */                         \
  __HRTFTCCONTROL    =   (  0 <<  9)                  /* MOD Signal Override Enable */                     \
                     |   (  0 <<  8)                  /* Mod Signal Value */                               \
                     |   (  1 <<  4)                  /* LCDVEEEN Output Enable */                         \
                     |   (  0 <<  3)                  /* Display Control Signal Output */                  \
                     |   (  0 <<  1)                  /* CLS Enable */                                     \
                     |   (  0 <<  0)                  /* SPS Enable */

#define LCD_ON()
#define LCD_OFF()

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
