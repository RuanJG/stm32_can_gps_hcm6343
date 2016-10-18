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
File        : LCD_X_8080.c
Purpose     : Port routines 8080 interface, 16 bit data bus
----------------------------------------------------------------------
*/

/*********************************************************************
*
*           Hardware configuration
*
**********************************************************************
  Needs to be adapted to your target hardware.
*/

/* Configuration example:

#define Chip_30600
#include <IOM16C.H>

#define LCD_CLR_RESET()   P7 &= ~(1<<0)
#define LCD_SET_RESET()   P7 |=  (1<<0)
#define LCD_CLR_A0()      P8 &= ~(1<<0)
#define LCD_SET_A0()      P8 |=  (1<<0)
#define LCD_CLR_WR()      P8 &= ~(1<<1)
#define LCD_SET_WR()      P8 |=  (1<<1)
#define LCD_CLR_RD()      P8 &= ~(1<<2)
#define LCD_SET_RD()      P8 |=  (1<<2)
#define LCD_CLR_CS()      P8 &= ~(1<<4)
#define LCD_SET_CS()      P8 |=  (1<<4)
#define LCD_DATA_IN_L     P10
#define LCD_DATA_IN_H     P1
#define LCD_DATA_OUT_L    P10
#define LCD_DATA_OUT_H    P1
#define LCD_SET_DIR_IN()  P1D = 0;    P10D = 0
#define LCD_SET_DIR_OUT() P1D = 0xff; P10D = 0xff
#define LCD_DELAY(ms)     OS_Delay(ms)
#define LCD_DELAY(ms)     GUI_Delay(ms)
*/

/*********************************************************************
*
*           High level LCD access macros
*
**********************************************************************
  Usually, there is no need to modify these macros.
  It should be sufficient ot modify the low-level macros
  above.
*/

#define LCD_X_READ(c)                                       \
  LCD_SET_DIR_IN();                                         \
  LCD_CLR_CS();                                             \
  LCD_CLR_RD();                                             \
  c = LCD_DATA_IN_L | ((unsigned short)LCD_DATA_IN_H << 8); \
  LCD_SET_CS();                                             \
  LCD_SET_RD()

#define LCD_X_WRITE(c)                                      \
  LCD_SET_DIR_OUT();                                        \
  LCD_DATA_OUT_L = (unsigned char)c;                        \
  LCD_DATA_OUT_H = c >> 8;                                  \
  LCD_CLR_CS();                                             \
  LCD_CLR_WR();                                             \
  LCD_SET_WR();                                             \
  LCD_SET_CS()

/*********************************************************************
*
*           Initialisation
*
**********************************************************************
  This routine should be called from your application program
  to set port pins to their initial values
*/

void LCD_X_Init(void) {
  LCD_SET_CS();
  LCD_SET_RD();
  LCD_SET_WR();
  LCD_CLR_RESET();
  LCD_DELAY(2);
  LCD_SET_RESET();
}

/*********************************************************************
*
*           Access routines
*
**********************************************************************
  Usually, there is no need to modify these routines.
  It should be sufficient ot modify the low-level macros
  above.
*/

/* Write to controller, with A0 = 0 */
void LCD_X_Write00_16(unsigned short c) {
  LCD_CLR_A0();
  LCD_X_WRITE(c);
}

/* Write to controller, with A0 = 1 */
void LCD_X_Write01_16(unsigned short c) {
  LCD_SET_A0();
  LCD_X_WRITE(c);
}

/* Write multiple bytes to controller, with A0 = 1 */
void LCD_X_WriteM01_16(unsigned short * pData, int NumWords) {
  LCD_SET_A0();
  for (; NumWords; NumWords--) {
    LCD_X_WRITE(*pData);
    pData++;
  }
}

/* Write multiple bytes to controller, with A0 = 0 */
void LCD_X_WriteM00_16(unsigned short * pData, int NumWords) {
  LCD_CLR_A0();
  for (; NumWords; NumWords--) {
    LCD_X_WRITE(*pData);
    pData++;
  }
}

/* Read from controller, with A0 = 0 */
unsigned short LCD_X_Read00_16(void) {
  unsigned short c;
  LCD_CLR_A0();
  LCD_X_READ(c);
  return c;
}

/* Read from controller, with A0 = 1 */
unsigned short LCD_X_Read01_16(void) {
  unsigned short c;
  LCD_SET_A0();
  LCD_X_READ(c);
  return c;
}

/* Read multiple bytes from controller, with A0 = 0 */
void LCD_X_ReadM00_16(unsigned short * pData, int NumWords) {
  LCD_CLR_A0();
  for (; NumWords; NumWords--) {
    LCD_X_READ(*pData);
    pData++;
  }
}
/* Read multiple bytes from controller, with A0 = 1 */
void LCD_X_ReadM01_16(unsigned short * pData, int NumWords) {
  LCD_SET_A0();
  for (; NumWords; NumWords--) {
    LCD_X_READ(*pData);
    pData++;
  }
}
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
