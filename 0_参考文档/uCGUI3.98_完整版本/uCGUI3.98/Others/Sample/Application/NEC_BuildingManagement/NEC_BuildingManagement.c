/*
*********************************************************************************************************
*                                                uC/GUI
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
File        : NEC_BuildingManagement.c
Purpose     : NEC Building Management Demo
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>
#include <stdio.h>
#include "GUI.h"
#include "WM.h"
#include "BUTTON.h"
#include "EDIT.h"
#include "TEXT.h"
#include "SLIDER.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define MSG_SELECTSENSOR    (WM_USER+0)
#define MSG_SENSORDATA      (WM_USER+1)

#define ID_ENTER            (GUI_ID_USER+0)
#define ID_ESCAPE           (GUI_ID_USER+1)
#define ID_BACK             (GUI_ID_USER+2)

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct {
  const char* pName;
  const char* pUnit;
  int         ActValue;
  int         MinValue;
  int         MaxValue;
  int         IsAdjustable;
} SENSOR;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static int      _DateDay    = 11;
static int      _DateMonth  = 10;
static int      _DateYear   = 2004;
static int      _TimeOffset = 39600000;

static char     _acUserName[34];
static char     _acPassword[34];

static int      _ActSensor;

static SENSOR   _Sensor[4] = {
  { "Temperature A", "°C",   23, 20, 25, 1 },
  { "Temperature B", "°C",   25, 20, 25, 1 },
  { "Pressure",      "mBar", 21, 20, 25, 0 },
  { "Humidity",      "RH",   50, 20, 25, 0 }
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _CopyCurrentValuesToEdit
*/
static void _CopyCurrentValuesToEdit(WM_HWIN hWin) {
  char ac[32];
  int i;
  for (i = 0; i < 4; i++) {
    sprintf(ac, "%d %s", _Sensor[i].ActValue, _Sensor[i].pUnit);
    EDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0 + i), ac);
  }
}

/*********************************************************************
*
*       _CopyMinMaxToEdit
*/
static void _CopyMinMaxToEdit(WM_HWIN hWin) {
  char ac[32];
  sprintf(ac, "%d %s", _Sensor[_ActSensor].MinValue, _Sensor[_ActSensor].pUnit);
  EDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), ac);
  sprintf(ac, "%d %s", _Sensor[_ActSensor].MaxValue, _Sensor[_ActSensor].pUnit);
  EDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), ac);
}

/*********************************************************************
*
*       Static code, callbacks
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbDialogSensor
*/
static void _cbDialogSensor(WM_MESSAGE* pMsg) {
  WM_HWIN hWin, hItem;
  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_CREATE: {
    WM_SetFocus(hWin);
    hItem = BUTTON_CreateEx((LCD_GetXSize() >> 1) - 70, LCD_GetYSize() - 42, 140, 36, hWin, WM_CF_SHOW, 0, GUI_ID_OK);
    BUTTON_SetText(hItem, "Exit To Menu");
    TEXT_CreateEx(40, 160 + (0 * 45), 300, 30, hWin, WM_CF_SHOW, 0, GUI_ID_TEXT0, "1.        Minimum setting");
    TEXT_CreateEx(40, 160 + (1 * 45), 300, 30, hWin, WM_CF_SHOW, 0, GUI_ID_TEXT1, "2.        Maximum setting");
    hItem = EDIT_CreateEx(350, 157 + (0 * 45), 110, 30, hWin, WM_CF_SHOW, 0, GUI_ID_EDIT0, 0);
    WM_DisableWindow(hItem);
    hItem = EDIT_CreateEx(350, 157 + (1 * 45), 110, 30, hWin, WM_CF_SHOW, 0, GUI_ID_EDIT1, 0);
    WM_DisableWindow(hItem);
    if (_Sensor[_ActSensor].IsAdjustable) {
      hItem = SLIDER_CreateEx(480, 152 + (0 * 45), 110, 36, hWin, WM_CF_SHOW, 0, GUI_ID_SLIDER0);
      SLIDER_SetWidth(hItem, 12);
      SLIDER_SetRange(hItem, 0, 40);
      SLIDER_SetValue(hItem, _Sensor[_ActSensor].MinValue);
      hItem = SLIDER_CreateEx(480, 152 + (1 * 45), 110, 36, hWin, WM_CF_SHOW, 0, GUI_ID_SLIDER1);
      SLIDER_SetWidth(hItem, 12);
      SLIDER_SetRange(hItem, 0, 40);
      SLIDER_SetValue(hItem, _Sensor[_ActSensor].MaxValue);
      _CopyMinMaxToEdit(hWin);
    } else {
      EDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), "-----");
      EDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), "-----");
    }
    hItem = BUTTON_CreateEx( 80, 275, 230, 42, hWin, WM_CF_SHOW, 0, GUI_ID_BUTTON0);
    BUTTON_SetText(hItem, "View Trend Graph");
    hItem = BUTTON_CreateEx(340, 275, 230, 42, hWin, WM_CF_SHOW, 0, GUI_ID_BUTTON1);
    BUTTON_SetText(hItem, "View User Log");
  } break;
  case WM_PAINT:
    {
      char ac[64];
      int Time;
      Time = GUI_GetTime() + _TimeOffset;
      GUI_SetBkColor(GUI_BLACK);
      GUI_Clear();
      GUI_SetColor(GUI_WHITE);
      GUI_SetFont(&GUI_Font32B_ASCII);
      GUI_DispStringHCenterAt("NEC Building Management Demo", LCD_GetXSize() >> 1, 20);
      GUI_SetFont(&GUI_Font24B_ASCII);
      sprintf(ac, "Sensor %d  -  %s", _ActSensor + 1, _Sensor[_ActSensor].pName);
      GUI_DispStringHCenterAt(ac, LCD_GetXSize() >> 1, 70);
      GUI_SetFont(&GUI_Font24_ASCII);
      sprintf(ac, "Date: %.2d/%.2d/%.2d", _DateDay, _DateMonth, _DateYear % 100);
      GUI_DispStringAt(ac, 10, LCD_GetYSize() - 35);
      sprintf(ac, "Time: %.2d:%.2d", (Time / 3600000) % 24, (Time / 60000) % 60);
      GUI_DispStringAt(ac, LCD_GetXSize() - 120, LCD_GetYSize() - 35);
    }
    break;
  case WM_NOTIFY_PARENT:
    if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) {
      int Id = WM_GetId(pMsg->hWinSrc);
      if (Id == GUI_ID_OK) {
        WM_DeleteWindow(hWin);
      } else if (Id == GUI_ID_BUTTON0) {
        /* View Trend Graph */
      } else if (Id == GUI_ID_BUTTON1) {
        /* View User Log */
      }
    } else if (pMsg->Data.v == WM_NOTIFICATION_VALUE_CHANGED) {
      int Id = WM_GetId(pMsg->hWinSrc);
      if (Id == GUI_ID_SLIDER0) {
        hItem = WM_GetDialogItem(hWin, GUI_ID_SLIDER0);
        _Sensor[_ActSensor].MinValue = SLIDER_GetValue(hItem);
        if (_Sensor[_ActSensor].MinValue > _Sensor[_ActSensor].MaxValue) {
          SLIDER_SetValue(hItem, _Sensor[_ActSensor].MaxValue);
        }
        _CopyMinMaxToEdit(hWin);
      }
      if (Id == GUI_ID_SLIDER1) {
        hItem = WM_GetDialogItem(hWin, GUI_ID_SLIDER1);
        _Sensor[_ActSensor].MaxValue = SLIDER_GetValue(hItem);
        if (_Sensor[_ActSensor].MaxValue < _Sensor[_ActSensor].MinValue) {
          SLIDER_SetValue(hItem, _Sensor[_ActSensor].MinValue);
        }
        _CopyMinMaxToEdit(hWin);
      }
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbDialogSelect
*/
static void _cbDialogSelect(WM_MESSAGE* pMsg) {
  WM_HWIN hWin, hItem;
  int i;
  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_CREATE:
    WM_SetFocus(hWin);
    hItem = BUTTON_CreateEx((LCD_GetXSize() >> 1) - 70, LCD_GetYSize() - 42, 140, 36, hWin, WM_CF_SHOW, 0, GUI_ID_OK);
    BUTTON_SetText(hItem, "Exit To Menu");
    for (i = 0; i < 4; i++) {
      char ac[64];
      sprintf(ac, "%d.        %s", i + 1, _Sensor[i].pName);
      TEXT_CreateEx(40, 160 + (i * 45), 300, 30, hWin, WM_CF_SHOW, 0, GUI_ID_TEXT0 + i, ac);
      hItem = EDIT_CreateEx(350, 157 + (i * 45), 110, 30, hWin, WM_CF_SHOW, 0, GUI_ID_EDIT0 + i, 0);
      WM_DisableWindow(hItem);
      hItem = BUTTON_CreateEx(480, 156 + (i * 45), 110, 32, hWin, WM_CF_SHOW, 0, GUI_ID_BUTTON0 + i);
      BUTTON_SetText(hItem, "Select");
    }
    /* Fill edit controls */
    _CopyCurrentValuesToEdit(hWin);
    break;
  case WM_PAINT:
    {
      char ac[32];
      int Time;
      Time = GUI_GetTime() + _TimeOffset;
      GUI_SetBkColor(GUI_BLACK);
      GUI_Clear();
      GUI_SetColor(GUI_WHITE);
      GUI_SetFont(&GUI_Font32B_ASCII);
      GUI_DispStringHCenterAt("NEC Building Management Demo", LCD_GetXSize() >> 1, 20);
      GUI_SetFont(&GUI_Font24B_ASCII);
      GUI_DispStringHCenterAt("Sensor Selection", LCD_GetXSize() >> 1, 70);
      GUI_SetFont(&GUI_Font24_ASCII);
      sprintf(ac, "Date: %.2d/%.2d/%.2d", _DateDay, _DateMonth, _DateYear % 100);
      GUI_DispStringAt(ac, 10, LCD_GetYSize() - 35);
      sprintf(ac, "Time: %.2d:%.2d", (Time / 3600000) % 24, (Time / 60000) % 60);
      GUI_DispStringAt(ac, LCD_GetXSize() - 120, LCD_GetYSize() - 35);
    }
    break;
  case WM_NOTIFY_PARENT:
    if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) {
      int Id = WM_GetId(pMsg->hWinSrc);
      if (Id == GUI_ID_OK) {
        WM_DeleteWindow(hWin);
        _acUserName[0] = 0;
        _acPassword[0] = 0;
      } else if ((Id >= GUI_ID_BUTTON0) && (Id <= GUI_ID_BUTTON3)) {
        _ActSensor = Id - GUI_ID_BUTTON0;
        hItem = WM_CreateWindowAsChild(0, 0, LCD_GetXSize(), LCD_GetYSize(), hWin, WM_CF_SHOW, &_cbDialogSensor, 0);
        WM_MakeModal(hItem);
      }
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _CreateButton
*/
static void _CreateButton(WM_HWIN hWin, int x, int y, int w, int h, int Id, const char* pText) {
  WM_HWIN hItem;
  hItem = BUTTON_CreateEx(x, y, w, h, hWin, WM_CF_SHOW, 0, Id + GUI_ID_USER);
  BUTTON_SetFocussable(hItem, 0);
  if (pText) {
    BUTTON_SetText(hItem, pText);
  }
  if (Id == 0) {
    WM_DisableWindow(hItem);
  }
}

/*********************************************************************
*
*       _cbKeyWin
*/
static void _cbKeyWin(WM_MESSAGE* pMsg) {
  switch (pMsg->MsgId) {
  case WM_CREATE:
    WM_SetFocus(pMsg->hWin);
    break;
  case WM_PAINT:
    (*WIDGET_Effect_3D2L.pfDrawUp)();
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _CreateKeyboard
*/
static void _CreateKeyboard(WM_HWIN hParent) {
  WM_HWIN hWin;
  int b;
  b = WIDGET_Effect_3D2L.EffectSize;
  hWin = WM_CreateWindowAsChild(145 - b, 258 - b, 349 + 2*b, 146 + 2*b, hParent, WM_CF_SHOW, &_cbKeyWin, 0);
  /* Create buttons for line 1 */
  _CreateButton(hWin, b +   0, b +   0,  30, 30, GUI_ID_USER + '1', "1");
  _CreateButton(hWin, b +  29, b +   0,  30, 30, GUI_ID_USER + '2', "2");
  _CreateButton(hWin, b +  58, b +   0,  30, 30, GUI_ID_USER + '3', "3");
  _CreateButton(hWin, b +  87, b +   0,  30, 30, GUI_ID_USER + '4', "4");
  _CreateButton(hWin, b + 116, b +   0,  30, 30, GUI_ID_USER + '5', "5");
  _CreateButton(hWin, b + 145, b +   0,  30, 30, GUI_ID_USER + '6', "6");
  _CreateButton(hWin, b + 174, b +   0,  30, 30, GUI_ID_USER + '7', "7");
  _CreateButton(hWin, b + 203, b +   0,  30, 30, GUI_ID_USER + '8', "8");
  _CreateButton(hWin, b + 232, b +   0,  30, 30, GUI_ID_USER + '9', "9");
  _CreateButton(hWin, b + 261, b +   0,  30, 30, GUI_ID_USER + '0', "0");
  _CreateButton(hWin, b + 290, b +   0,  59, 30, ID_BACK, "<<");
  /* Create buttons for line 2 */
  _CreateButton(hWin, b +   0, b +  29,  16, 30, 0, 0);
  _CreateButton(hWin, b +  15, b +  29,  30, 30, GUI_ID_USER + 'Q', "Q");
  _CreateButton(hWin, b +  44, b +  29,  30, 30, GUI_ID_USER + 'W', "W");
  _CreateButton(hWin, b +  73, b +  29,  30, 30, GUI_ID_USER + 'E', "E");
  _CreateButton(hWin, b + 102, b +  29,  30, 30, GUI_ID_USER + 'R', "R");
  _CreateButton(hWin, b + 131, b +  29,  30, 30, GUI_ID_USER + 'T', "T");
  _CreateButton(hWin, b + 160, b +  29,  30, 30, GUI_ID_USER + 'Y', "Y");
  _CreateButton(hWin, b + 189, b +  29,  30, 30, GUI_ID_USER + 'U', "U");
  _CreateButton(hWin, b + 218, b +  29,  30, 30, GUI_ID_USER + 'I', "I");
  _CreateButton(hWin, b + 247, b +  29,  30, 30, GUI_ID_USER + 'O', "O");
  _CreateButton(hWin, b + 276, b +  29,  30, 30, GUI_ID_USER + 'P', "P");
  _CreateButton(hWin, b + 305, b +  29,  30, 30, GUI_ID_USER + 'Ü', "Ü");
  _CreateButton(hWin, b + 334, b +  29,  15, 30, 0, 0);
  /* Create buttons for line 3 */
  _CreateButton(hWin, b +   0, b +  58,  30, 30, 0, 0);
  _CreateButton(hWin, b +  29, b +  58,  30, 30, GUI_ID_USER + 'A', "A");
  _CreateButton(hWin, b +  58, b +  58,  30, 30, GUI_ID_USER + 'S', "S");
  _CreateButton(hWin, b +  87, b +  58,  30, 30, GUI_ID_USER + 'D', "D");
  _CreateButton(hWin, b + 116, b +  58,  30, 30, GUI_ID_USER + 'F', "F");
  _CreateButton(hWin, b + 145, b +  58,  30, 30, GUI_ID_USER + 'G', "G");
  _CreateButton(hWin, b + 174, b +  58,  30, 30, GUI_ID_USER + 'H', "H");
  _CreateButton(hWin, b + 203, b +  58,  30, 30, GUI_ID_USER + 'J', "J");
  _CreateButton(hWin, b + 232, b +  58,  30, 30, GUI_ID_USER + 'K', "K");
  _CreateButton(hWin, b + 261, b +  58,  30, 30, GUI_ID_USER + 'L', "L");
  _CreateButton(hWin, b + 290, b +  58,  30, 30, GUI_ID_USER + 'Ö', "Ö");
  _CreateButton(hWin, b + 319, b +  58,  30, 30, GUI_ID_USER + 'Ä', "Ä");
  /* Create buttons for line 4 */
  _CreateButton(hWin, b +   0, b +  87,  45, 30, 0, 0);
  _CreateButton(hWin, b +  44, b +  87,  30, 30, GUI_ID_USER + 'Z', "Z");
  _CreateButton(hWin, b +  73, b +  87,  30, 30, GUI_ID_USER + 'X', "X");
  _CreateButton(hWin, b + 102, b +  87,  30, 30, GUI_ID_USER + 'C', "C");
  _CreateButton(hWin, b + 131, b +  87,  30, 30, GUI_ID_USER + 'V', "V");
  _CreateButton(hWin, b + 160, b +  87,  30, 30, GUI_ID_USER + 'B', "B");
  _CreateButton(hWin, b + 189, b +  87,  30, 30, GUI_ID_USER + 'N', "N");
  _CreateButton(hWin, b + 218, b +  87,  30, 30, GUI_ID_USER + 'M', "M");
  _CreateButton(hWin, b + 247, b +  87,  30, 30, GUI_ID_USER + '@', "@");
  _CreateButton(hWin, b + 276, b +  87,  30, 30, GUI_ID_USER + '.', ".");
  _CreateButton(hWin, b + 305, b +  87,  44, 30, 0, 0);
  /* Create buttons for line 5 */
  _CreateButton(hWin, b +   0, b + 116,  69, 30, ID_ESCAPE, "Esc");
  _CreateButton(hWin, b +  68, b + 116, 214, 30, GUI_ID_USER + ' ', " ");
  _CreateButton(hWin, b + 281, b + 116,  68, 30, ID_ENTER, "Enter");
}

/*********************************************************************
*
*       _cbDialogMain
*/
static void _cbDialogMain(WM_MESSAGE* pMsg) {
  WM_HWIN hWin, hItem;
  hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_CREATE:
    WM_SetFocus(hWin);
    hItem = BUTTON_CreateEx((LCD_GetXSize() >> 1) - 70, LCD_GetYSize() - 42, 140, 36, hWin, WM_CF_SHOW, 0, GUI_ID_OK);
    BUTTON_SetText(hItem, "Enter");
    TEXT_CreateEx(40, 160 + (0 * 45), 300, 30, hWin, WM_CF_SHOW, 0, GUI_ID_TEXT0, "Enter Name:");
    TEXT_CreateEx(40, 160 + (1 * 45), 300, 30, hWin, WM_CF_SHOW, 0, GUI_ID_TEXT1, "Password Name:");
    hItem = EDIT_CreateEx(230, 157 + (0 * 45), 220, 30, hWin, WM_CF_SHOW, 0, GUI_ID_EDIT0, 32);
    EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, &GUI_Font16B_ASCII);
    WM_DisableWindow(hItem);
    hItem = EDIT_CreateEx(230, 157 + (1 * 45), 220, 30, hWin, WM_CF_SHOW, 0, GUI_ID_EDIT1, 32);
    EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, &GUI_Font16B_ASCII);
    WM_DisableWindow(hItem);
    _CreateKeyboard(pMsg->hWin);
    break;
  case WM_PAINT:
    {
      char ac[32];
      int Time;
      Time = GUI_GetTime() + _TimeOffset;
      GUI_SetBkColor(GUI_BLACK);
      GUI_Clear();
      GUI_SetColor(GUI_WHITE);
      GUI_SetFont(&GUI_Font32B_ASCII);
      GUI_DispStringHCenterAt("NEC Building Management Demo", LCD_GetXSize() >> 1, 20);
      GUI_SetFont(&GUI_Font24B_ASCII);
      GUI_DispStringHCenterAt("Touchscreen Menu Control", LCD_GetXSize() >> 1, 70);
      GUI_SetFont(&GUI_Font24_ASCII);
      sprintf(ac, "Date: %.2d/%.2d/%.2d", _DateDay, _DateMonth, _DateYear % 100);
      GUI_DispStringAt(ac, 10, LCD_GetYSize() - 35);
      sprintf(ac, "Time: %.2d:%.2d", (Time / 3600000) % 24, (Time / 60000) % 60);
      GUI_DispStringAt(ac, LCD_GetXSize() - 120, LCD_GetYSize() - 35);
    }
    break;
  case WM_TOUCH_CHILD:
    {
      WM_MESSAGE* pMsg;
    }
    break;
  case WM_NOTIFY_PARENT:
    if (pMsg->Data.v == WM_NOTIFICATION_RELEASED) {
      int Id = WM_GetId(pMsg->hWinSrc);
      if (Id == GUI_ID_OK) {
        hItem = WM_CreateWindowAsChild(0, 0, LCD_GetXSize(), LCD_GetYSize(), hWin, WM_CF_SHOW, &_cbDialogSelect, 0);
        WM_MakeModal(hItem);
      }
    } else if (pMsg->Data.v == WM_NOTIFICATION_CHILD_DELETED) {
      EDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT0), _acUserName);
      EDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_EDIT1), _acPassword);
    }
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void);
void MainTask(void) {
  GUI_Init();
  //WM_SetCreateFlags(WM_CF_MEMDEV);
  BUTTON_SetDefaultFont(&GUI_Font16B_1);
  EDIT_SetDefaultFont(&GUI_Font24B_1);
  TEXT_SetDefaultFont(&GUI_Font24B_ASCII);
  TEXT_SetDefaultTextColor(0xB0FFB0);
  EDIT_SetDefaultTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
  WM_CreateWindowAsChild(0, 0, LCD_GetXSize(), LCD_GetYSize(), WM_HBKWIN, WM_CF_SHOW, &_cbDialogMain, 0);
  while (1) {
    WM_HWIN hWin;
    hWin = WM_Screen2hWin(LCD_GetXSize() - 1, LCD_GetYSize() - 1);
    WM_InvalidateWindow(hWin);
    GUI_Delay(3000);
  }
}

/*************************** End of file ****************************/
	 	 			 		    	 				 	  			   	 	 	 	 	 	  	  	      	   		 	 	 		  		  	 		 	  	  			     			       	   	 			  		    	 	     	 				  	 					 	 			   	  	  			 				 		 	 	 			     			 
