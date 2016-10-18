/*********************************************************************************************************
*
* File                : TouchPanel.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  : 
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#ifndef _TOUCHPANEL_H_
#define _TOUCHPANEL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


#define Open_TP_CS_PIN					GPIO_Pin_3
#define Open_TP_CS_PORT					GPIOI
#define Open_TP_CS_CLK					RCC_AHB1Periph_GPIOI

#define Open_TP_IRQ_PIN					GPIO_Pin_3
#define Open_TP_IRQ_PORT				GPIOE
#define Open_TP_IRQ_CLK					RCC_AHB1Periph_GPIOE

#define TP_CS(x)	x ? GPIO_SetBits(Open_TP_CS_PORT,Open_TP_CS_PIN): GPIO_ResetBits(Open_TP_CS_PORT,Open_TP_CS_PIN)
#define TP_INT_IN   GPIO_ReadInputDataBit(Open_TP_IRQ_PORT,Open_TP_IRQ_PIN)

/**
 * @brief Definition for TouchPanel  SPI
 */
 /* Configure TouchPanel pins:   TP_CLK-> PB13 and TP_MISO-> PB14 and TP_MOSI-> PB15 */
#define Open_RCC_SPI   	        		RCC_APB1Periph_SPI2
#define Open_GPIO_AF_SPI 				    GPIO_AF_SPI2

#define Open_SPI                        SPI2
#define Open_SPI_CLK_INIT               RCC_APB1PeriphClockCmd
#define Open_SPI_IRQn                   SPI2_IRQn
#define Open_SPI_IRQHANDLER             SPI2_IRQHandler

#define Open_SPI_SCK_PIN                GPIO_Pin_13
#define Open_SPI_SCK_GPIO_PORT          GPIOB
#define Open_SPI_SCK_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define Open_SPI_SCK_SOURCE             GPIO_PinSource13

#define Open_SPI_MISO_PIN               GPIO_Pin_14
#define Open_SPI_MISO_GPIO_PORT         GPIOB
#define Open_SPI_MISO_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define Open_SPI_MISO_SOURCE            GPIO_PinSource14

#define Open_SPI_MOSI_PIN               GPIO_Pin_15
#define Open_SPI_MOSI_GPIO_PORT         GPIOB
#define Open_SPI_MOSI_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define Open_SPI_MOSI_SOURCE            GPIO_PinSource15		




/* Private typedef -----------------------------------------------------------*/
typedef	struct POINT 
{
   uint16_t x;
   uint16_t y;
}Coordinate;


typedef struct Matrix 
{						
long double An,  
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,
//             Gn,     
//             Hn,   
//             In,    
//             Jn,    
//             Kn,	
// 	          Ln,	
            Divider ;
} Matrix; //

/* Private variables ---------------------------------------------------------*/
extern Coordinate ScreenSample[5];
extern Coordinate DisplaySample[5];
extern Matrix matrix ;
extern Coordinate  display ;

/* Private define ------------------------------------------------------------*/

#define	CHX 	0x90
#define	CHY 	0xd0

/* Private function prototypes -----------------------------------------------*/				
void TP_Init(void);	
Coordinate *Read_Ads7846(void);
void TouchPanel_Calibrate(void);
void TP_IRQ_EXTI_Config(void) ;
void DrawCross(uint16_t Xpos,uint16_t Ypos);
void TP_DrawPoint(uint16_t Xpos,uint16_t Ypos);
FunctionalState setCalibrationMatrix( Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr);
FunctionalState getDisplayPoint(Coordinate * displayPtr,Coordinate * screenPtr,Matrix * matrixPtr );
void delay_ms(uint16_t ms);    
void Xpos_Ypos(uint16_t Xpos, uint16_t Ypos);
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


