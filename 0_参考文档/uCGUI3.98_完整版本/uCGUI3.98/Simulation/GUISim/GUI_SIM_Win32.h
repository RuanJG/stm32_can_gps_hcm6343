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
File        : GUI_SIM_Win32.h
Purpose     : Declares public functions of Simulation
----------------------------------------------------------------------
*/

#ifndef SIM_GUI_H
#define SIM_GUI_H

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/********************************************************************
*
*       Types
*
*********************************************************************
*/
typedef int  SIM_GUI_tfHook           (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, int * pResult);
typedef void SIM_GUI_tfDelayHandler   (int ms);
typedef void SIM_GUI_tfExecIdleHandler(void);

/********************************************************************
*
*       Interface
*
*********************************************************************
*/
void SIM_GUI_HandleKeyEvents     (unsigned Msg, WPARAM wParam);
HWND SIM_GUI_CreateLCDWindow     (HWND hParent, int x, int y, int xSize, int ySize, int LayerIndex);
HWND SIM_GUI_CreateLOGWindow     (HWND hParent, int x, int y, int xSize, int ySize);
HWND SIM_GUI_CreateLCDInfoWindow (HWND hParent, int x, int y, int xSize, int ySize, int LayerIndex);
int  SIM_GUI_Init                (HINSTANCE hInst, HWND hWndMain, char * pCmdLine, const char * sAppName);
void SIM_GUI_LogClear            (void);
void SIM_GUI_CopyToClipboard     (int LayerIndex);
void SIM_GUI_SetLCDWindowHook    (SIM_GUI_tfHook         * pfHook);
void SIM_GUI_SetDelayHandler     (SIM_GUI_tfDelayHandler * pfHandler);
void SIM_GUI_SetExecIdleHandler  (SIM_GUI_tfExecIdleHandler * pfHandler);
int  SIM_GUI_GetTransColor       (void);
void SIM_GUI_GetLCDPos           (int * px, int * py);
void SIM_GUI_Exit                (void);
void SIM_GUI_SetMessageBoxOnError(int OnOff);
int  SIM_GUI_App                 (HINSTANCE hInstance, HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow);
void SIM_GUI_SetPixel            (int x, int y, unsigned Color);


#if defined(__cplusplus)
}
#endif 

#endif /* SIM_GUI_H */
