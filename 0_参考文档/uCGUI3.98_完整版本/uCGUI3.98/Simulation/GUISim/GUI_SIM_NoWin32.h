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
File        : GUI_SIM_NoWin32.h
Purpose     : Declares public functions of Simulation
----------------------------------------------------------------------
*/

#ifndef GUI_SIM_NOWIN32_H
#define GUI_SIM_NOWIN32_H

/********************************************************************
*
*       Publics for LCD
*
*********************************************************************
*/
void     SIM_GUI_SetLCDPos(int x, int y);
int      SIM_GUI_SetTransColor(int Color);
int      SIM_GUI_SetLCDColorBlack (unsigned int Index, int Color);
int      SIM_GUI_SetLCDColorWhite (unsigned int Index, int Color);
void     SIM_GUI_SetMag(int MagX, int MagY);
int      SIM_GUI_GetMagX(void);
int      SIM_GUI_GetMagY(void);
int      SIM_GUI_GetForwardRButton(void);
void     SIM_GUI_SetForwardRButton(int OnOff);

/********************************************************************
*
*       Routine(s) in user application
*
*********************************************************************
*/
void  SIM_GUI_X_Init(void);   /* Allow init before application starts ... Use it to set LCD offset etc */

/********************************************************************
*
*       Publics used by GUI_X module
*
*********************************************************************
*/
void SIM_GUI_Delay (int ms);
void SIM_GUI_ExecIdle(void);
int  SIM_GUI_GetTime(void);
int  SIM_GUI_GetKey(void);
int  SIM_GUI_WaitKey(void);
void SIM_GUI_StoreKey(int);


/********************************************************************
*
*       Publics for logging, warning, errorout
*
*********************************************************************
*/
void SIM_GUI_Log(const char *s);
void SIM_GUI_Log1(const char *s, int p0);
void SIM_GUI_Log2(const char *s, int p0, int p1);
void SIM_GUI_Log3(const char *s, int p0, int p1, int p2);
void SIM_GUI_Log4(const char *s, int p0, int p1, int p2,int p3);
void SIM_GUI_Warn(const char *s);
void SIM_GUI_Warn1(const char *s, int p0);
void SIM_GUI_Warn2(const char *s, int p0, int p1);
void SIM_GUI_Warn3(const char *s, int p0, int p1, int p2);
void SIM_GUI_Warn4(const char *s, int p0, int p1, int p2, int p3);
void SIM_GUI_ErrorOut(const char *s);
void SIM_GUI_ErrorOut1(const char *s, int p0);
void SIM_GUI_ErrorOut2(const char *s, int p0, int p1);
void SIM_GUI_ErrorOut3(const char *s, int p0, int p1, int p2);
void SIM_GUI_ErrorOut4(const char *s, int p0, int p1, int p2, int p3);
void SIM_GUI_EnableMessageBoxOnError(int Status);

/********************************************************************
*
*       Commandline support
*
*********************************************************************
*/
const char *SIM_GUI_GetCmdLine(void);

/********************************************************************
*
*       Multitasking support
*
*********************************************************************
*/
void SIM_GUI_CreateTask(char * pName, void * pFunc);
void SIM_GUI_Start(void);
unsigned long SIM_GUI_GetTaskID(void);
void SIM_GUI_Lock(void);
void SIM_GUI_Unlock(void);
void SIM_GUI_InitOS(void);

#endif /* LCD_H */




