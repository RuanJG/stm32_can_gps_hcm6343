@ECHO OFF
GOTO START

******************************************************************************
*
* File      : Lib.bat
* Parameters: None
* Purpose   : Put all (object) files in linklist into the library
*
* This file is written for IAR MSP430 tool chain
*
******************************************************************************
:START

ECHO MAKELIB.BAT:         Creating GUI target library using IAR MSP430 tool-chain

REM ****************************************
REM   Create library
REM ****************************************

IF EXIST Lib\GUI_TARGET.LIB DEL Lib\GUI_TARGET.LIB

ECHO d-c MSP430>temp\PARA.DAT
COPY temp\PARA.DAT+temp\LIB.DAT temp\LIB.PAR
ECHO Q>>temp\LIB.PAR
XLIB temp\LIB.PAR

IF ERRORLEVEL 1 PAUSE
