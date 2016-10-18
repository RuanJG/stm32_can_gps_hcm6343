@ECHO OFF
GOTO START

******************************************************************************
*
* File      : Prep.bat
* Parameters: None
* Purpose   : Sets path and other environment variables as required by tool chain
*
* This file is written for the IAR MSP430 toolchain
*
* It needs to be modified if the compiler is installed in a different location.
*
******************************************************************************

:START
ECHO PREP.BAT:            Preparing environment for MSP430 IAR

if "%_PREP_MSP430_IAR_%" == "_PREP_MSP430_IAR_" goto cont
set _PREP_MSP430_IAR_=_PREP_MSP430_IAR_

SET TOOLPATH=C:\Tool\C\IAR\MSP430_V310A
SET PATH=%TOOLPATH%\430\bin;%TOOLPATH%\common\bin;%PATH%
SET C_INCLUDE=%TOOLPATH%\430\INC\CLIB

:cont

