# Microsoft Developer Studio Project File - Name="SimulationShip" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SimulationShip - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Simulation.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Simulation.mak" CFG="SimulationShip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SimulationShip - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "SimulationShip - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SimulationShip - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Output\Debug"
# PROP Intermediate_Dir "Output\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /ML /W3 /Gm /GX /ZI /Od /I "..\uCGUI\Config" /I "..\uCGUI\GUI\Core" /I "..\uCGUI\GUI\JPEG" /I "..\uCGUI\GUI\MultiLayer" /I "..\uCGUI\GUI\Widget" /I "..\uCGUI\GUI\WM" /I "..\Simulation" /I "..\Simulation\Res" /I "..\Simulation\GUISim" /D "LCD_USE_WINSIM" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "SimulationShip - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Output\Release"
# PROP Intermediate_Dir "Output\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /O1 /I "..\uCGUI\Config" /I "..\uCGUI\GUI\Core" /I "..\uCGUI\GUI\JPEG" /I "..\uCGUI\GUI\MultiLayer" /I "..\uCGUI\GUI\Widget" /I "..\uCGUI\GUI\WM" /I "..\Simulation" /I "..\Simulation\Res" /I "..\Simulation\GUISim" /D "LCD_USE_WINSIM" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libcmt.lib libc.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:none /map /machine:I386
# SUBTRACT LINK32 /debug /nodefaultlib

!ENDIF 

# Begin Target

# Name "SimulationShip - Win32 Debug"
# Name "SimulationShip - Win32 Release"
# Begin Group "uCGUI"

# PROP Default_Filter ""
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\Application\AppTask.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\Application\MicriumLogo_1bpp.c
# End Source File
# End Group
# Begin Group "GUIDemo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\DemoTask.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Automotive.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Bitmap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Bitmap4bpp.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Circle.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_ColorBar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_ColorList.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Cursor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Dialog.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Font.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_FrameWin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Graph.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_HardwareInfo.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Intro.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_LUT.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_MemDevB.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Messagebox.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Navi.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Polygon.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_ProgBar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Speed.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_Touch.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\GUIDEMO_WM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\Map.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\MicriumLogo.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUIDemo\MicriumLogoWRed.c
# End Source File
# End Group
# Begin Group "Config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\Config\GUIConf.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\Config\GUITouchConf.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\Config\LCDConf.h
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Group "AntiAlias"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAAArc.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAAChar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAAChar2.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAAChar4.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAACircle.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAALib.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAALine.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAAPoly.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\AntiAlias\GUIAAPolyOut.c
# End Source File
# End Group
# Begin Group "ConvertColor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP111.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP222.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP233.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP323.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP332.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP444_12.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP444_12_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP444_16.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP555.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP556.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP565.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP655.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP666.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP8666.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP8666_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDP888.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM233.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM323.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM332.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM444_12.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM444_16.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM555.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM556.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM565.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM655.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM666.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertColor\LCDPM888.c
# End Source File
# End Group
# Begin Group "ConvertMono"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertMono\LCDP0.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertMono\LCDP2.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\ConvertMono\LCDP4.c
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI2DLib.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__AddSpaceHex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__Arabic.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__CalcTextRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__DivideRound.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__DivideRound32.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__DrawBitmap16bpp.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__GetCursorPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__GetFontSizeY.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__GetNumChars.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__HandleEOLine.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__IntersectRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__IntersectRects.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__memset.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__memset16.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__Read.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__ReduceRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__SetText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__strcmp.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__strlen.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI__Wrap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_AddBin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_AddDec.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_AddDecMin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_AddDecShift.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_AddHex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_AddKeyMsgHook.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_ALLOC_AllocInit.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_ALLOC_AllocZero.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_BMP.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_BMP_Serialize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CalcColorDist.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_ClearRectEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Color2VisColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_ConfDefaults.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowL.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowLI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowLPx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowMI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowMPx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowS.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowSI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorArrowSPx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossL.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossLI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossLPx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossMI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossMPx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossS.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossSI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorCrossSPx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorHeaderM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorHeaderMI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorHeaderMPx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorPal.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_CursorPalI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispBin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispCEOL.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispChar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispChars.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispHex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispStringAt.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispStringAtCEOL.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispStringHCenter.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispStringInRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispStringInRectEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispStringInRectWrap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DispStringLen.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawBitmap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawBitmap_555.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawBitmap_565.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawBitmap_888.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawBitmapEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawBitmapExp.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawBitmapMag.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawFocusRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawGraph.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawGraphEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawHLine.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawLine.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawPie.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawPixel.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawPoint.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawPolygon.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawPolyline.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawRectEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_DrawVLine.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_ErrorOut.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Exec.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_FillPolygon.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_FillRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_FillRectEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_FontIntern.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetBitmapPixelColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetBitmapPixelIndex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetClientRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetDispPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetDrawMode.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetFontInfo.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetFontSizeY.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetLineStyle.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetOrg.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetStringDistX.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetTextExtend.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetTextMode.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetVersionString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GetYSizeOfFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_GIF.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Goto.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_InitLUT.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_InvertRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_IsInFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Log.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_MergeRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_MOUSE.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_MOUSE_DriverPS2.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_MoveRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_OnKey.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Pen.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_PID.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Protected.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_RectsIntersect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SaveContext.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_ScreenSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SelectLayer.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SelectLCD.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetClipRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetColorIndex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetDecChar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetDefault.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetDrawMode.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetLBorder.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetLineStyle.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetLUTColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetLUTColorEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetLUTEntry.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetOrg.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetPixelIndex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetTextMode.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SetTextStyle.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SIF.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_SIF_Prop.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_TOUCH.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_TOUCH_DriverAnalog.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_TOUCH_StoreState.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_TOUCH_StoreUnstable.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_UC.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_UC_ConvertUC2UTF8.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_UC_ConvertUTF82UC.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_UC_DispString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_UC_EncodeNone.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_UC_EncodeUTF8.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_VNC.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_WaitEvent.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_WaitKey.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_Warn.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUI_X.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIAlloc.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIArc.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIChar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUICharLine.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUICharM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUICharP.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUICirc.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIColor2Index.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUICore.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUICurs.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIDebug.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIEncJS.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIIndex2Color.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIPolyE.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIPolyM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIPolyR.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIRealloc.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIStream.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUITask.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUITime.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUITimer.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIType.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIUC0.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIVal.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIValf.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\GUIVersion.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_API.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_ConfDefaults.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_DrawVLine.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_GetColorIndex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_GetEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_GetNumDisplays.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_GetPixelColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_Index2ColorEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_L0_Generic.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_Mirror.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_MixColors256.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_Protected.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_ReadRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_Rotate180.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_RotateCCW.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_RotateCW.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_SelectLCD.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_SetAPI.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_SetClipRectEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCD_UpdateColorIndices.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDAA.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDGetP.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDInfo.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDInfo0.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDInfo1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDL0Delta.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDL0Mag.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDP1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDP565_Index2Color.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDP888_Index2Color.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDPM565_Index2Color.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDPM888_Index2Color.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDRLE16.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDRLE4.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDRLE8.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Core\LCDSIM.h
# End Source File
# End Group
# Begin Group "Font"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F08_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F08_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F10_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F10_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F10S_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F10S_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13B_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13B_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13H_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13H_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13HB_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F13HB_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F16_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F16_1HK.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F16_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F16_HK.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F16B_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F16B_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F24_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F24_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F24B_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F24B_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F32_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F32_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F32B_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F32B_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F4x6.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F6x8.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x10_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x12_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x13_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x13_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x15B_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x15B_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x16.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\F8x8.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FComic18B_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FComic18B_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FComic24B_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FComic24B_ASCII.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD24x32.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD32.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD36x48.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD48.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD48x64.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD60x80.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD64.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Font\FD80.c
# End Source File
# End Group
# Begin Group "JPEG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\GUI_JPEG.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jcomapi.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jconfig.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdapimin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdapistd.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdcoefct.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdcolor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdct.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jddctmgr.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdhuff.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdhuff.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdinput.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdmainct.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdmarker.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdmaster.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdmerge.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdphuff.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdpostct.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdsample.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jdtrans.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jerror.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jerror.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jfdctint.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jidctflt.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jidctfst.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jidctint.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jidctred.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jinclude.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jmemsys.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jpegint.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jpeglib.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jquant1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jquant2.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jutils.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\JPEG\jversion.h
# End Source File
# End Group
# Begin Group "LCDDriver"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\LCDDriver\LCDLin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\LCDDriver\LCDLin32.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\LCDDriver\LCDMem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\LCDDriver\LCDNull.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\LCDDriver\LCDTemplate.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\LCDDriver\LCDWin.c
# End Source File
# End Group
# Begin Group "MemDev"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_16.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_8.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_AA.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_Auto.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_Banding.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_Clear.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_CmpWithLCD.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_CopyFromLCD.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_CreateFixed.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_GetDataPtr.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_GetXSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_GetYSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_Measure.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_ReduceYSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_SetOrg.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_Usage.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_UsageBM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_Write.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_WriteAlpha.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_WriteEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MemDev\GUIDEV_XY2PTR.c
# End Source File
# End Group
# Begin Group "MultiLayer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\MultiLayer\LCD_1.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MultiLayer\LCD_2.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MultiLayer\LCD_3.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MultiLayer\LCD_4.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\MultiLayer\LCD_IncludeDriver.h
# End Source File
# End Group
# Begin Group "Touch"

# PROP Default_Filter ""
# End Group
# Begin Group "VNC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\VNC\GUI_ProcessVNC.c
# End Source File
# End Group
# Begin Group "Widget"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON__SetBitmapObj.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_Bitmap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_BMP.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_Get.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_GetBitmap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_IsPressed.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_SelfDraw.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_SetFocusColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\BUTTON_StreamedBitmap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_GetState.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_GetText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_Image.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_IsChecked.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetBoxBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetDefaultImage.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetFocusColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetImage.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetNumStates.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetSpacing.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetState.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\CHECKBOX_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DIALOG.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DIALOG.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DIALOG_Intern.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_AddString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_DeleteItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_GetNumItems.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_InsertString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_ItemSpacing.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetAutoScroll.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetScrollbarColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetScrollbarWidth.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\DROPDOWN_SetTextHeight.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_GetCursorPixelPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_GetCursorPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_GetNumChars.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_SetCursorAtChar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_SetFocussable.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_SetInsertMode.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_SetpfAddKeyEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_SetpfUpdateBuffer.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_SetSel.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDIT_SetTextMode.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDITBin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDITDec.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDITFloat.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDITHex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\EDITUlong.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN__UpdateButtons.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_AddMenu.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_Button.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_ButtonClose.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_ButtonMax.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_ButtonMin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_Get.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_IsMinMax.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_MinMaxRest.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_SetBorderSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_SetColors.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_SetResizeable.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_SetTitleHeight.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\FRAMEWIN_SetTitleVis.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GRAPH.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GRAPH.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GRAPH_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GRAPH_DATA_XY.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GRAPH_DATA_YT.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GRAPH_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GRAPH_SCALE.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_ARRAY.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_ARRAY.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_ARRAY_DeleteItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_ARRAY_InsertItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_ARRAY_ResizeItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_DRAW.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_DRAW_BITMAP.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_DRAW_BMP.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_DRAW_Self.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_DRAW_STREAMED.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_EditBin.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_EditDec.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_EditFloat.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_EditHex.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_EditString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_HOOK.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\GUI_HOOK.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER__SetDrawObj.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_Bitmap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_BMP.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_GetSel.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_SetDragLimit.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\HEADER_StreamedBitmap.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_DeleteItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_Font.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_GetItemText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_GetNumItems.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_GetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_InsertString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_ItemDisabled.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_ItemSpacing.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_MultiSel.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_ScrollStep.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetAutoScroll.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetOwner.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetOwnerDraw.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetScrollbarColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetScrollbarWidth.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetString.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTBOX_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_DeleteAllRows.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_DeleteColumn.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_DeleteRow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_DisableRow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetHeader.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetItemText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetNumColumns.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetNumRows.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetSel.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_GetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_InsertRow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetAutoScroll.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetColumnWidth.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetGridVis.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetItemColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetItemText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetLBorder.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetRBorder.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetRowHeight.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetSel.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetSort.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\LISTVIEW_UserData.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU__FindItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_Attach.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_DeleteItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_DisableItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_EnableItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_GetItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_GetItemText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_GetNumItems.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_InsertItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_Popup.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_SetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_SetBorderSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_SetItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MENU_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MESSAGEBOX.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MESSAGEBOX.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIEDIT.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIEDIT.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIEDIT_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIEDIT_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIPAGE.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIPAGE.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIPAGE_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIPAGE_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIPAGE_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\MULTIPAGE_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_SetBarColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_SetMinMax.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_SetText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\PROGBAR_SetTextPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_GetText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_Image.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetDefaultImage.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetFocusColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetGroupId.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetImage.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\RADIO_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR_Defaults.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR_GetValue.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR_SetColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SCROLLBAR_SetWidth.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SLIDER.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SLIDER.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SLIDER_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SLIDER_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SLIDER_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SLIDER_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\SLIDER_SetFocusColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_Create.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_CreateIndirect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_Private.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_SetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_SetFont.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_SetText.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_SetTextAlign.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_SetTextColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\TEXT_SetWrapMode.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_Effect_3D.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_Effect_3D1L.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_Effect_3D2L.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_Effect_None.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_Effect_Simple.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_FillStringInRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_SetEffect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WIDGET_SetWidth.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WINDOW.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WINDOW_Default.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\Widget\WINDOW_Private.h
# End Source File
# End Group
# Begin Group "WM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__ForEachDesc.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__GetFirstSibling.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__GetFocussedChild.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__GetLastSibling.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__GetOrg_AA.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__GetPrevSibling.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__IsAncestor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__IsChild.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__IsEnabled.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__NotifyVisChanged.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__Screen2Client.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__SendMessage.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__SendMessageIfEnabled.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__SendMessageNoPara.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM__UpdateChildPositions.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_AttachWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_BringToBottom.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_BringToTop.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Broadcast.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_CheckScrollPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_CriticalHandle.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_DIAG.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_EnableWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_ForEachDesc.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetBkColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetCallback.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetClientRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetClientWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetDesktopWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetDesktopWindowEx.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetDiagInfo.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetDialogItem.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetFirstChild.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetFlags.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetFocussedWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetId.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetInsideRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetInsideRectExScrollbar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetInvalidRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetNextSibling.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetOrg.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetParent.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetPrevSibling.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetScrollbar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetScrollPartner.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetScrollPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetScrollState.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetWindowRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GetWindowSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_GUI.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_HasCaptured.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_HasFocus.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Hide.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Intern.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Intern_ConfDep.h
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_InvalidateArea.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_IsCompletelyVisible.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_IsEnabled.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_IsFocussable.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_IsVisible.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_IsWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_MakeModal.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Move.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_MoveChildTo.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_NotifyParent.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_OnKey.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Paint.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_PaintWindowAndDescs.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_PID__GetPrevState.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_ResizeWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Screen2Win.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SendMessageNoPara.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SendToParent.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetAnchor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetCallback.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetCapture.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetCaptureMove.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetCreateFlags.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetDesktopColor.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetFocus.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetFocusOnNextChild.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetFocusOnPrevChild.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetId.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetpfPollPID.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetScrollbar.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetScrollPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetScrollState.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetTrans.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetTransState.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetUserClipRect.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetWindowPos.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetXSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SetYSize.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Show.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_SIM.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_StayOnTop.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Timer.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_TimerExternal.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_UpdateWindowAndDescs.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_UserData.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_Validate.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WM_ValidateWindow.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WMMemDev.c
# End Source File
# Begin Source File

SOURCE=..\uCGUI\GUI\WM\WMTouch.c
# End Source File
# End Group
# End Group
# End Group
# Begin Group "Simulation"

# PROP Default_Filter ""
# Begin Group "GUISim"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Simulation\GUISim\GUI_SIM_NoWin32.h
# End Source File
# Begin Source File

SOURCE=..\Simulation\GUISim\GUI_SIM_Win32.h
# End Source File
# Begin Source File

SOURCE=..\Simulation\GUISim\SIM.h
# End Source File
# Begin Source File

SOURCE=..\Simulation\GUISim\SIM_X.c
# End Source File
# Begin Source File

SOURCE=..\Simulation\GUISim\GUISim.lib
# End Source File
# End Group
# Begin Group "Res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Simulation\Res\Branding.rh
# End Source File
# Begin Source File

SOURCE=..\Simulation\Res\Device.bmp
# End Source File
# Begin Source File

SOURCE=..\Simulation\Res\Device1.bmp
# End Source File
# Begin Source File

SOURCE=..\Simulation\Res\Logo.bmp
# End Source File
# Begin Source File

SOURCE=..\Simulation\Res\ResourceSim.h
# End Source File
# Begin Source File

SOURCE=..\Simulation\Res\Simulation.aps
# End Source File
# Begin Source File

SOURCE=..\Simulation\Res\Simulation.ico
# End Source File
# Begin Source File

SOURCE=..\Simulation\Res\Simulation.rc
# End Source File
# End Group
# Begin Group "WinMain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Simulation\WinMain\WinMain.c
# End Source File
# End Group
# Begin Source File

SOURCE=..\Simulation\BASETSD.H
# End Source File
# End Group
# End Target
# End Project
