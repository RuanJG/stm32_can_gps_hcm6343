




#ifndef MAIN_H            /* Make sure we only include it once */
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "timers.h "
//#include "test_program.h"
#include <stddef.h>
#include "TouchPanel/TouchPanel.h"
//#include "GUIDEMO.h"
#include "stm32f4xx.h"
#include <stdio.h>

#include "GUI.h"
#include "songti16.c"
#include <math.h>
#include <stdlib.h>
//#include "logo.c"
//#include "logo1.c"
//#include "acautomaticship.c"
//#include "png.h"  
//#include "yunzhou_logo.c"

#include "WindowDLG.c"

#include "stm32f429i_discovery_sdram.h"


extern FRAMEWIN_Handle mFrame;
extern int visible_frame;


#endif   /* MAIN_H */
