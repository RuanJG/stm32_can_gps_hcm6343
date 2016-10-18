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
File        : LCDConf_Xylon_C8_C400x234.h
Purpose     : Configuration file for Xylon FPGA display driver
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

/*********************************************************************
*
*       General configuration
*/
#define LCD_CONTROLLER               9100 /* Selects the Xylon FPGA display driver */

#define LCD_XSIZE                     400 /* Physical display size in X */
#define LCD_YSIZE                     234 /* Physical display size in Y */
#define LCD_VXSIZE                   1024 /* Virtual  display size in X */
#define LCD_BITSPERPIXEL                8 /* Color depth */

#define FPGA_REGISTER_BASE      0x2000000 /* Register base address */
#define LCD_VRAM_ADR            0x1000000 /* Video RAM base address */

/*********************************************************************
*
*       Full bus configuration
*/
#define LCD_READ_MEM(Off)       *((volatile unsigned short *)(LCD_VRAM_ADR       + (((U32)(Off)) << 1)))
#define LCD_WRITE_MEM(Off,Data) *((volatile unsigned short *)(LCD_VRAM_ADR       + (((U32)(Off)) << 1))) = Data
#define LCD_READ_REG(Reg)       *((volatile unsigned short *)(FPGA_REGISTER_BASE + (((U32)(Reg)) << 1)))
#define LCD_WRITE_REG(Reg,Data) *((volatile unsigned short *)(FPGA_REGISTER_BASE + (((U32)(Reg)) << 1))) = Data

/*********************************************************************
*
*       Register definitions
*/
#define REG_INT_STATUS      (0x0000 >> 1) // Interrupt Status and Reset register
#define REG_INT_MASK        (0x0002 >> 1) // Interrupt Mask register
#define REG_ERROR_STATUS    (0x0004 >> 1) // Error status
#define REG_ERROR_MASK      (0x0006 >> 1) // Error mask
#define REG_CVC_PHASE_SHFT  (0x0008 >> 1) // CVC Œ data/control/pixel clock phase shift
#define REG_OVL_PHASE_SHFT  (0x000A >> 1) // Overlay Œ Amux control phase shift
#define REG_FPGA_GPIO_DATA0 (0x000C >> 1) // FPGA Genereal Purpose Data Register
#define REG_FGPA_GPIO_CTL0  (0x000E >> 1) // FPGA General purpose IO control Register
#define REG_FPGA_GPIO_DATA1 (0x0010 >> 1) // FPGA Genereal Purpose Data Register
#define REG_FGPA_GPIO_CTL1  (0x0012 >> 1) // FPGA General purpose IO control Register
#define REG_VPAGE           (0x0014 >> 1) // Video page
#define REG_SHSY_FP         (0x0020 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SHSY            (0x0022 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SHSY_BP         (0x0024 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SHSY_RESL       (0x0026 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SHSY_RESH       (0x0048 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SVSY_FP         (0x0028 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SVSY            (0x002A >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SVSY_BP         (0x002C >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SVSY_RESL       (0x002E >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SVSY_RESH       (0x004A >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SCTRL1          (0x0030 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SCTRL2          (0x0032 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SL_SCREENL      (0x0034 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SL_SCREENH      (0x004C >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SDTYPE1         (0x0036 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SDTYPE2         (0x0038 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SDVDAC          (0x003A >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SM              (0x003C >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SPWRCTRL        (0x003E >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_SXCOLOR         (0x0040 >> 1) // logiCVC registers (logiCVC Users Manual)
#define REG_OVL_COL_KEY0_L  (0x0044 >> 1) // Overlay Color Key Blue
#define REG_OVL_COL_KEY0_H  (0x0045 >> 1) // Overlay Color Key Green
#define REG_OVL_COL_KEY1_L  (0x0046 >> 1) // Overlay Color Key Red
#define REG_SSRC_AL         (0x0050 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SSRC_AH         (0x0052 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SDST_AL         (0x0054 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SDST_AH         (0x0056 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SSTRIPE         (0x0058 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SDSTRIPE        (0x005A >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SWIDTH          (0x005C >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SHEIGHT         (0x005E >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SBLT_CTRL0      (0x0060 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SBLT_CTRL1      (0x0062 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SROP_CEXP       (0x0064 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SOP             (0x0066 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SFG_COL         (0x0068 >> 1) // logiBITBLK registers (logiBITBLK Users Manual)
#define REG_SBG_COL         (0x006A >> 1) // logiBITBLK registers (logiBITBLK Users Manual)

/*********************************************************************
*
*       Register initialization
*/
#define LCD_INIT_CONTROLLER()  \
  LCD_WRITE_REG(REG_CVC_PHASE_SHFT, 0x000a); \
  LCD_WRITE_REG(REG_OVL_PHASE_SHFT, 0x007f); \
  LCD_WRITE_REG(REG_SHSY_FP       , 0x0016); \
  LCD_WRITE_REG(REG_SHSY          , 0x0024); \
  LCD_WRITE_REG(REG_SHSY_BP       , 0x002f); \
  LCD_WRITE_REG(REG_SHSY_RESL     , 0x008f); \
  LCD_WRITE_REG(REG_SVSY_FP       , 0x0008); \
  LCD_WRITE_REG(REG_SVSY          , 0x0002); \
  LCD_WRITE_REG(REG_SVSY_BP       , 0x000f); \
  LCD_WRITE_REG(REG_SVSY_RESL     , 0x00e9); \
  LCD_WRITE_REG(REG_SCTRL1        , 0x001f); \
  LCD_WRITE_REG(REG_SCTRL2        , 0x0000); \
  LCD_WRITE_REG(REG_SL_SCREENL    , 0x0000); \
  LCD_WRITE_REG(REG_SDTYPE2       , 0x0004); \
  LCD_WRITE_REG(REG_SDVDAC        , 0x0000); \
  LCD_WRITE_REG(REG_SM            , 0x0000); \
  LCD_WRITE_REG(REG_SPWRCTRL      , 0x000f); \
  LCD_WRITE_REG(REG_OVL_COL_KEY0_L, 0x0001); \
  LCD_WRITE_REG(REG_OVL_COL_KEY0_H, 0x0001); \
  LCD_WRITE_REG(REG_OVL_COL_KEY1_L, 0x0001); \
  LCD_WRITE_REG(REG_SHSY_RESH     , 0x0001); \
  LCD_WRITE_REG(REG_SVSY_RESH     , 0x0000); \
  LCD_WRITE_REG(REG_SL_SCREENH    , 0x0000); \
  LCD_WRITE_REG(REG_SDTYPE1       , 0x00A3)

#endif
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
