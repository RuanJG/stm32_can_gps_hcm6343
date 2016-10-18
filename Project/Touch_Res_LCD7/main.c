/**
  ******************************************************************************
  * @file    USART/USART_Printf/main.c 
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    13-November-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#define Touch_X        0
#define Touch_Y        420
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


extern Matrix matrix ;
extern Coordinate  display ;
 
void TP_Config(void)
{
	  LCD_Clear(LCD_COLOR_BLACK);
    LCD_SetFont(&Font16x24);
		LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
    //LCD_DisplayStringLine(LINE(18), (uint8_t*)"Touch Panel                        Paint Example");
    LCD_DisplayStringLine(LINE(18), (uint8_t*)"   Touch Panel     Clear Adjust    Paint Example");
		
    LCD_SetTextColor(LCD_COLOR_WHITE); 

    LCD_DrawLine(0, Touch_Y, 800, LCD_DIR_HORIZONTAL);
	  LCD_DrawLine(1, Touch_Y, 56, LCD_DIR_VERTICAL);
		LCD_DrawLine(300, Touch_Y, 56, LCD_DIR_VERTICAL);
		LCD_DrawLine(390, Touch_Y, 56, LCD_DIR_VERTICAL);
  	LCD_DrawLine(500, Touch_Y, 56, LCD_DIR_VERTICAL);		
	  LCD_DrawLine(799, Touch_Y, 56, LCD_DIR_VERTICAL);
    LCD_DrawLine(0, Touch_Y+55, 800, LCD_DIR_HORIZONTAL);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/startup_stm32f429_439xx.s)
       before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file.
     */    

  RCC_ClocksTypeDef SYS_Clocks;
	uint16_t linenum = 0;
  uint16_t Current_Color;
	Coordinate  display_p;
  //static TP_STATE* TP_State; 
  /* USART configuration */
  USART_Config();  
  /* Output a message on Hyperterminal using printf function */
  
  RCC_GetClocksFreq(&SYS_Clocks);
	printf("\r\nSYSCLK:%dM\r\n",SYS_Clocks.SYSCLK_Frequency/1000000);
	printf("HCLK:%dM\r\n",SYS_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1:%dM\r\n",SYS_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2:%dM\r\n",SYS_Clocks.PCLK2_Frequency/1000000);	
  /* LCD initialization */
  LCD_Init();
  //TP_IRQ_EXTI_Config();
  /* LCD Layer initialization */
  LCD_LayerInit();
    
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	

  LCD_Clear(LCD_COLOR_BLUE);
  LCD_Clear(LCD_COLOR_WHITE);
	LCD_Clear(LCD_COLOR_YELLOW);
	LCD_SetFont(&Font8x8);
	
  /* Touch Panel configuration */
	TP_Init();
  //TouchPanel_Calibrate(); 
 	TP_Config();
  while (1)
  { 

 	  getDisplayPoint(&display, Read_Ads7846(), &matrix );		
	    if(((display.y < Touch_Y-10) && (display.y >= 2)))
    {
      if((display.x >= 798) || (display.x < 2))
      {}     
      else
      {
				LCD_GetColors(&Current_Color,0);
				LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
				LCD_DrawLine(0, display_p.y, 800, LCD_DIR_HORIZONTAL);
				LCD_DrawLine(display_p.x, 0, 52*8, LCD_DIR_VERTICAL);
				
				LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
				LCD_DrawLine(0, display.y, 800, LCD_DIR_HORIZONTAL);
				LCD_DrawLine(display.x, 0, 52*8, LCD_DIR_VERTICAL);

				Xpos_Ypos(display.x,display.y);			
						
				display_p.x=display.x;
				display_p.y=display.y;					
      }
    }
    else if ((display.y <= Touch_Y+50) && (display.y >= Touch_Y) && (display.x >= 300) && (display.x <= 390) )   //Clear
    {
      LCD_SetFont(&Font8x8);
      for(linenum = 0; linenum < 52; linenum++)
      {
        LCD_ClearLine(LINE(linenum));
      }
    }
		else if ((display.y <= Touch_Y+50) && (display.y >= Touch_Y) && (display.x >= 390) && (display.x <= 500))   //Adjust
		{

			getDisplayPoint(&display, Read_Ads7846(), &matrix );		
			if ((display.y <= Touch_Y+50) && (display.y >= Touch_Y) && (display.x >= 390) && (display.x <= 500))
			{
			TouchPanel_Calibrate(); 
			TP_Config();
			}
			
		}
    else
    {
    }
  }
	
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
