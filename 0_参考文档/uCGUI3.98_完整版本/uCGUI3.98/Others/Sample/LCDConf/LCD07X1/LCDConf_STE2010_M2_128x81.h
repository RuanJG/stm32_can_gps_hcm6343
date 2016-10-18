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
File        : LCDConf_STE2010_M2_128x81.h
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

#define LCD_XSIZE        128   /* X-resolution of LCD, Logical coor. */
#define LCD_YSIZE         81   /* Y-resolution of LCD, Logical coor. */

#define LCD_CONTROLLER   702

#define LCD_BITSPERPIXEL   2

/*********************************************************************
*
*       Simple bus configuration
*
**********************************************************************
*/

void LCD_X_Write01(char Data);
void LCD_X_WriteM01(char * pData, int NumBytes);
void LCD_X_Write00(char Cmd);
#define LCD_WRITE_A1(data)            LCD_X_Write01(data)
#define LCD_WRITEM_A1(data, NumBytes) LCD_X_WriteM01(data, NumBytes)
#define LCD_WRITE_A0(cmd)             LCD_X_Write00(cmd)

/*********************************************************************
*
*       Initialisation macro
*
**********************************************************************
*/

#define LCD_INIT_CONTROLLER() /* TBD by customer ... */

#endif /* LCDCONF_H */
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
