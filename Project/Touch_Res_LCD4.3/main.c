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
	LCD_Clear(LCD_COLOR_WHITE);
    LCD_SetFont(&Font8x8);
	  LCD_SetTextColor(LCD_COLOR_BLACK); 
    LCD_DisplayStringLine(LINE(30), (uint8_t*)"                                          Touch Panel Paint");
    LCD_DisplayStringLine(LINE(32), (uint8_t*)"                                          Example          ");

    LCD_SetTextColor(LCD_COLOR_RED); 
    LCD_DrawFullRect(5, 238, 30, 30);
    LCD_SetTextColor(LCD_COLOR_BLUE); 
    LCD_DrawFullRect(40, 238, 30, 30);
    LCD_SetTextColor(LCD_COLOR_GREEN); 
    LCD_DrawFullRect(75, 238, 30, 30);
    LCD_SetTextColor(LCD_COLOR_BLACK); 
    LCD_DrawFullRect(110, 238, 30, 30);	
    LCD_SetTextColor(LCD_COLOR_MAGENTA); 
    LCD_DrawFullRect(145, 238, 30, 30);
    LCD_SetTextColor(LCD_COLOR_BLUE2); 
    LCD_DrawFullRect(180, 238, 30, 30);
    LCD_SetTextColor(LCD_COLOR_CYAN); 
    LCD_DrawFullRect(215, 238, 30, 30);
    LCD_SetTextColor(LCD_COLOR_YELLOW); 
    LCD_DrawFullRect(250, 238, 30, 30);
		
    LCD_SetTextColor(LCD_COLOR_BLACK); 
    
		
    LCD_SetFont(&Font16x24);
    LCD_DisplayChar(LCD_LINE_10, 292, 0x43);
    LCD_DrawLine(0, 232, 480, LCD_DIR_HORIZONTAL);
   
    LCD_DrawLine(1, 232, 36, LCD_DIR_VERTICAL);
    LCD_DrawLine(37, 232, 36, LCD_DIR_VERTICAL);
    LCD_DrawLine(72, 232, 36, LCD_DIR_VERTICAL);
    LCD_DrawLine(107, 232, 36, LCD_DIR_VERTICAL);
    LCD_DrawLine(142, 232, 36, LCD_DIR_VERTICAL);
		
		LCD_DrawLine(177, 232, 36, LCD_DIR_VERTICAL);
		LCD_DrawLine(212, 232, 36, LCD_DIR_VERTICAL);
		LCD_DrawLine(247, 232, 36, LCD_DIR_VERTICAL);
		LCD_DrawLine(282, 232, 36, LCD_DIR_VERTICAL);
		LCD_DrawLine(317, 232, 36, LCD_DIR_VERTICAL);
		
    LCD_DrawLine(1, 269, 480, LCD_DIR_HORIZONTAL);
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
  //static TP_STATE* TP_State; 
  /* USART configuration */
  USART_Config();
 //  USART_SendData(USART2,0x55);    
  /* Output a message on Hyperterminal using printf function */
  
  RCC_GetClocksFreq(&SYS_Clocks);
	printf("\r\nSYSCLK:%dM\r\n",SYS_Clocks.SYSCLK_Frequency/1000000);
	printf("HCLK:%dM\r\n",SYS_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1:%dM\r\n",SYS_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2:%dM\r\n",SYS_Clocks.PCLK2_Frequency/1000000);	
  /* LCD initialization */
  LCD_Init();
  
  /* LCD Layer initialization */
  LCD_LayerInit();
    
  /* Enable the LTDC */
  LTDC_Cmd(ENABLE);
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	
	LCD_Clear(LCD_COLOR_YELLOW);
	LCD_SetFont(&Font8x8);
	LCD_DisplayStringLine(LINE(6), (uint8_t*)"Touch Panel Paint     ");
  /* Touch Panel configuration */
	TP_Init();
  TouchPanel_Calibrate(); 
 	TP_Config();
  while (1)
  {
 	  getDisplayPoint(&display, Read_Ads7846(), &matrix );		
	    if(((display.y < 230) && (display.y >= 2)))
    {
      if((display.x >= 478) || (display.x < 2))
      {}     
      else
      {
        LCD_DrawFullCircle(display.x, display.y, 2);
      }
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 180) && (display.x <= 210))
    {
      LCD_SetTextColor(LCD_COLOR_BLUE2);
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 215) && (display.x <= 245))
    {
      LCD_SetTextColor(LCD_COLOR_CYAN); 
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 250) && (display.x <= 280))
    {
      LCD_SetTextColor(LCD_COLOR_YELLOW); 
    }      
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 5) && (display.x <= 35))
    {
      LCD_SetTextColor(LCD_COLOR_RED);
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 40) && (display.x <= 70))
    {
      LCD_SetTextColor(LCD_COLOR_BLUE); 
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 75) && (display.x <= 105))
    {
      LCD_SetTextColor(LCD_COLOR_GREEN); 
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 110) && (display.x <= 140))
    {
      LCD_SetTextColor(LCD_COLOR_BLACK); 
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 145) && (display.x <= 175))
    {
      LCD_SetTextColor(LCD_COLOR_MAGENTA); 
    }
    else if ((display.y <= 270) && (display.y >= 230) && (display.x >= 285) && (display.x <= 315))
    {
      LCD_SetFont(&Font8x8);
      for(linenum = 0; linenum < 29; linenum++)
      {
        LCD_ClearLine(LINE(linenum));
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
