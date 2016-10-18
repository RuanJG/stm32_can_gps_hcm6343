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
File        : LCDConf_LH75411_M4_320x240.h
Purpose     : Sample configuration file for Sharp LH75411
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
#define LCD_CONTROLLER   3200

#define LCD_BITSPERPIXEL    4
#define LCD_XSIZE         320
#define LCD_YSIZE         240

#define LCD_ENDIAN_BIG      0

/*********************************************************************
*
*       Full bus configuration
*
**********************************************************************
*/
#define LCD_VRAM_ADR 0x44000000

/*********************************************************************
*
*       LCD_INIT_CONTROLLER
*
**********************************************************************
*/
/* SFR definitions */
#define RCPC_APBPERIPHCLKCTRL1 *(volatile U32*)(0xfffe2028)
#define RCPC_AHBCLKCTRL        *(volatile U32*)(0xfffe202c)
#define RCPC_LCDPRESCALER      *(volatile U32*)(0xfffe2040)
#define CLCDC_TIMING0          *(volatile U32*)(0xffff4000)
#define CLCDC_TIMING1          *(volatile U32*)(0xffff4004)
#define CLCDC_TIMING2          *(volatile U32*)(0xffff4008)
#define CLCDC_UPBASE           *(volatile U32*)(0xffff4010)
#define CLCDC_LPBASE           *(volatile U32*)(0xffff4014)
#define CLCDC_INTRENABLE       *(volatile U32*)(0xffff4018)
#define CLCDC_CTRL             *(volatile U32*)(0xffff401c)
#define HRTFTC_SETUP           *(volatile U32*)(0xfffe4000)
#define HRTFTC_CTRL            *(volatile U32*)(0xfffe4004)
#define HRTFTC_TIMING1         *(volatile U32*)(0xfffe4008)
#define HRTFTC_TIMING2         *(volatile U32*)(0xfffe400c)
#define IOCON_PD_MUX           *(volatile U32*)(0xfffe5004)
#define IOCON_LCD_MUX          *(volatile U32*)(0xfffe5010)
#define GPIO_PDDR              *(volatile U32*)(0xfffde004)
#define GPIO_PDDDR             *(volatile U32*)(0xfffde00c)

#define HBP_INIT             17
#define HSW_INIT            100

#define LCD_INIT_CONTROLLER() \
  RCPC_APBPERIPHCLKCTRL1 &= ~(1 << 0);                       /* U0 peripheral clock is running. */  \
  RCPC_AHBCLKCTRL        &= ~(1 << 0);                       /* AHB DMA clock is running. */  \
  IOCON_LCD_MUX          =  (0x1 << 0);                      /* 4-bit Mono STN Mode */ \
  RCPC_LCDPRESCALER      =  (0x2 << 0);                      /* f(HCLK)/4 */ \
  CLCDC_UPBASE           =  LCD_VRAM_ADR;                    /* Upper Panel Frame Buffer Base Address Register */  \
  CLCDC_LPBASE           =  LCD_VRAM_ADR;                    /* Lower Panel Frame Buffer Base Address Register */  \
  CLCDC_TIMING0          =  (HBP_INIT << 24)                 /* Horizontal Back Porch */  \
                         |  (32 << 16)                       /* Horizontal Front Porch */  \
                         |  (HSW_INIT <<  8)                 /* Horizontal Synchronization Pulse Width */  \
                         |  (((XSIZE / 16) - 1) << 2);       /* Pixels-Per-Line */ \
  CLCDC_TIMING1          =  (2 << 24)                        /* Vertical Back Porch */ \
                         |  (2 << 16)                        /* Vertical Front Porch */ \
                         |  (0 << 10)                        /* Vertical Synchronisation */ \
                         |  ((YSIZE - 1) << 0);              /* Lines Per Panel, should be odd */ \
  CLCDC_TIMING2          =  ( 0 << 26)                       /* Use the pixel clock divider logic */ \
                         |  (((XSIZE / 4) - 1) << 16)        /* Clocks per line init for 4-bit mono STN panels */ \
                         |  ( 1 << 14)                       /* LCDSPL aktive high */ \
                         |  ( 0 << 13)                       /* Data is driven out on rising edge of LCDDCLK  */ \
                         |  ( 0 << 12)                       /* LCDHRLP (Horizontal Synchronisation)is aktive high  */ \
                         |  ( 0 << 11)                       /* LCDSPS (Vertikal synchronisation) )is active high */ \
                         |  (22 <<  6)                       /* AC Bias Pin Frequency */ \
                         |  ( 2 <<  0);                      /* Panel Clock Divisor */ \
  CLCDC_INTRENABLE       =  0x00000000;                      /* Interrupt Enable Register, all disabled */  \
  CLCDC_CTRL             =  (0 << 16)                        /* FIFO Watermark 4 or more empty locations */  \
                         |  (0 << 15)                        /* No FIFO RAM Test */ \
                         |  (0 << 12)                        /* Interrupt at start of vertical synchronisation */ \
                         |  (1 << 11)                        /* LCD Powerenable: Pin LCDVDDEN in High wenn bit LcdEN is High  */ \
                         |  (0 <<  8)                        /* Normal output: Red and Blue not swapped */ \
                         |  (0 <<  7)                        /* Single panel LCD */ \
                         |  (0 <<  5)                        /* LCD is STN */ \
                         |  (1 <<  4)                        /* LCD is monochrome */ \
                         |  (2 <<  1)                        /* 4 bpp (bits per Pixel) */ \
                         |  (1 <<  0);                       /* Enable  LCD */ \
  IOCON_PD_MUX           |= 0x50;                            /* GPIO Config */  \
  GPIO_PDDDR             |= 0x60;                            /* GPIO Config */  \
  GPIO_PDDR              |= 0x60                             /* GPIO Config */

/*********************************************************************
*
*       LCD_SET_LUT_ENTRY
*
**********************************************************************
*/
#define LCD_SET_LUT_ENTRY(Pos, Color)                   \
  volatile U32 * pPalette;                              \
  U32 Index;                                            \
  pPalette = (volatile U32 *)(0xffff4200) + (Pos >> 1); \
  Index    = LCD_Color2Index_444_16(Color) >> 1;        \
  if (Pos & 1) {                                        \
    *pPalette &= 0x0000FFFF;                            \
    *pPalette |= Index << 16;                           \
  } else {                                              \
    *pPalette &= 0xFFFF0000;                            \
    *pPalette |= Index;                                 \
  }
#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
