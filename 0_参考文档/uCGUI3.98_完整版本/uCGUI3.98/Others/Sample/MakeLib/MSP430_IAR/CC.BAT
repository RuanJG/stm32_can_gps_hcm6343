@ECHO OFF
GOTO START

******************************************************************************
*
* File      : CC.bat
* Parameters: %1 Name of file to compile (without extension; .c is added)
* Purpose   : Compile one file and add it to the list of files to put in
*             Library
*
* This file as is uses the IAR MSP430 Compiler
*
******************************************************************************

:START

ECHO CC.BAT:              Compiling %1.c with IAR MSP430 compiler
ICC430 -o temp\Output\ -lC .\Output\ --library_module --warnings_affect_exit_code --silent -s9 --no_cse --no_unroll --no_inline --no_code_motion --no_tbaa -e --double=32 temp\Source\%1.c
IF ERRORLEVEL 1 PAUSE
ECHO f-m temp\Output\%1,Lib\GUI_TARGET,,>>temp\LIB.DAT

