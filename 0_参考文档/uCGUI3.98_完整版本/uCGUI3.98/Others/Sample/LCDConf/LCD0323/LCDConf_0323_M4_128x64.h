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
File        : LCDConf_0323_M4_128x64.h
Purpose     : Sample configuration file
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

#define LCD_CONTROLLER       323

#define LCD_XSIZE            128      /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE             64      /* Y-resolution of LCD, Logical coor. */

#define LCD_MIRROR_Y           1

#define LCD_ENABLE_COM_SPLIT   1

#define LCD_BITSPERPIXEL       4

#if (LCD_BITSPERPIXEL == 1)
  #define LCD_FIXEDPALETTE    -1
#endif

void LCD_X_Write00(char c);
void LCD_X_Write01(char c);
void LCD_X_WriteM01(char * pData, int NumBytes);
char LCD_X_Read00(void);
char LCD_X_Read01(void);
#define LCD_WRITE_A1(Byte) LCD_X_Write01(Byte)
#define LCD_WRITE_A0(Byte) LCD_X_Write00(Byte)
#define LCD_WRITEM_A1(pData, NumBytes) LCD_X_WriteM01(pData, NumBytes)
#define LCD_READ_A1(Byte)  Byte = LCD_X_Read01()
#define LCD_READ_A0(Byte)  Byte = LCD_X_Read00()

/*********************************************************************
*
*       Initialisation macro
*
**********************************************************************
*/

#define LCD_INIT_CONTROLLER()                                                                       \
  LCD_X_Init();                                                                                     \
  LCD_WRITE_A0(0x15); /* Set Column Address */                                                      \
  LCD_WRITE_A0(0x00); /* Start = 0 */                                                               \
  LCD_WRITE_A0(0x3F); /* End = 127 */                                                               \
  LCD_WRITE_A0(0x75); /* Set Row Address */                                                         \
  LCD_WRITE_A0(0x00); /* Start = 0 */                                                               \
  LCD_WRITE_A0(0x3F); /* End =  63 */                                                               \
  LCD_WRITE_A0(0x81); /* Set Contrast Control */                                                    \
  LCD_WRITE_A0(0x3e); /* 0 ~ 127 */	                                                                \
  LCD_WRITE_A0(0x86); /* Set Current Range 84h:Quarter, 85h:Half, 86h:Full */                       \
  LCD_WRITE_A0(0xA1); /* Set Display Start Line */                                                  \
  LCD_WRITE_A0(0x00); /* Top */                                                                     \
  LCD_WRITE_A0(0xA2); /* Set Display Offset */                                                      \
  LCD_WRITE_A0(0x40); /* No offset */                                                               \
  LCD_WRITE_A0(0xA4); /* Set DisplaMode,A4:Normal, 	 A5:All ON, A6: All OFF, A7:Inverse	*/          \
  LCD_WRITE_A0(0xA8); /* Set Multiplex Ratio */                                                     \
  LCD_WRITE_A0(0x3F); /* [6:0]16~128, 64 rows=3Fh*/                                                 \
  LCD_WRITE_A0(0xB1); /* Set Phase Length */                                                        \
  LCD_WRITE_A0(0x22); /* [3:0]:Phase 1 period of 1~16 clocks [7:4]:Phase 2 period of 1~16 clocks */ \
  LCD_WRITE_A0(0xB2); /* Set Row Period */                                                          \
  LCD_WRITE_A0(0x46); /* [7:0]:18~255, K=P1+P2+GS15 */                                              \
  LCD_WRITE_A0(0xB3); /* Set Clock Divide */                                                        \
  LCD_WRITE_A0(0x21); /* [3:0]:1~16, [7:4]:0~16 */                                                  \
  LCD_WRITE_A0(0xBF); /* Set VSL */                                                                 \
  LCD_WRITE_A0(0x0B); /* [3:0]:VSL */                                                               \
  LCD_WRITE_A0(0xBE); /* Set VCOMH */                                                               \
  LCD_WRITE_A0(0x0B); /* [7:0]:VCOMH */                                                             \
  LCD_WRITE_A0(0xBC); /* Set VP */                                                                  \
  LCD_WRITE_A0(0x1B); /* [7:0]:VP */                                                                \
  LCD_WRITE_A0(0xB8); /* Set Gamma with next 8 bytes */                                             \
  LCD_WRITE_A0(0x01); /* L1[2:1] */                                                                 \
  LCD_WRITE_A0(0x11); /* L3[6:4], L2[2:0] 0001 0001 */                                              \
  LCD_WRITE_A0(0x22); /* L5[6:4], L4[2:0] 0010 0010 */                                              \
  LCD_WRITE_A0(0x32); /* L7[6:4], L6[2:0] 0011 1011 */	                                            \
  LCD_WRITE_A0(0x43); /* L9[6:4], L8[2:0] 0100 0100 */                                              \
  LCD_WRITE_A0(0x54); /* LB[6:4], LA[2:0] 0101 0101 */                                              \
  LCD_WRITE_A0(0x65); /* LD[6:4], LC[2:0] 0110 0110 */                                              \
  LCD_WRITE_A0(0x76); /* LF[6:4], LE[2:0] 1000 0111 */	                                            \
  LCD_WRITE_A0(0xAF)  /* AF=ON, AE=Sleep Mode */

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
