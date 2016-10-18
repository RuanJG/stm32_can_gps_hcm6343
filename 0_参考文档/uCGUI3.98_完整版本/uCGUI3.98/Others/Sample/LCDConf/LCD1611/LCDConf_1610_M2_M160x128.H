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
File        : LCDConf_1610_M2_M160x128.H
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

#define LCD_CONTROLLER 1801             /* Select UC1610, currently not in documentation */

#define LCD_XSIZE            (160)      /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE            (128)      /* Y-resolution of LCD, Logical coor. */

#define LCD_BITSPERPIXEL     (2)

char LCD_X_Read00  (void);
char LCD_X_Read01  (void);
void LCD_X_Write00 (char c);
void LCD_X_Write01 (char c);
void LCD_X_WriteM01(char * pData, int NumBytes);
#define LCD_READ_A1(Byte)              Byte = LCD_X_Read01()
#define LCD_READ_A0()                  Byte = LCD_X_Read00()
#define LCD_WRITE_A1(Byte)             LCD_X_Write01(Byte)
#define LCD_WRITE_A0(Byte)             LCD_X_Write00(Byte)
#define LCD_WRITEM_A1(pData, NumBytes) LCD_X_WriteM01(pData, NumBytes)

/*********************************************************************
*
*       Initialisation macro
*
**********************************************************************
*/

#define LCD_INIT_CONTROLLER() /* tbd by customer */

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
