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
#include "sdcard.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_Printf
  * @{
  */ 

  
/*******************************************************************************
* Function Name  : Delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void  Delay (uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


unsigned char Start_Flag=0;

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define BlockSize            512 /* Block Size in Bytes */
#define BufferWordsSize      (BlockSize >> 2)

#define NumberOfBlocks       2  /* For Multi Blocks operation (Read/Write) */
#define MultiBufferWordsSize ((BlockSize * NumberOfBlocks) >> 2)

#define Operate_Block 0

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SD_CardInfo SDCardInfo;
u32 Buffer_Block_Tx[BufferWordsSize], Buffer_Block_Rx[BufferWordsSize];
u32 Buffer_MultiBlock_Tx[MultiBufferWordsSize], Buffer_MultiBlock_Rx[MultiBufferWordsSize];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;
ErrorStatus HSEStartUpStatus;
/* Private functions ---------------------------------------------------------*/


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
	  u32 i;
  RCC_ClocksTypeDef SYS_Clocks;
  /* USART configuration */
  USART_Config();
	NVIC_Configuration();
   USART_SendData(EVAL_COM2,0x55);    
  /* Output a message on Hyperterminal using printf function */
  printf("\n\rUSART Printf Example: retarget the C library printf function to the USART\n\r");
  RCC_GetClocksFreq(&SYS_Clocks);
	printf("\r\nSYSCLK:%dM\r\n",SYS_Clocks.SYSCLK_Frequency/1000000);
	printf("HCLK:%dM\r\n",SYS_Clocks.HCLK_Frequency/1000000);
	printf("PCLK1:%dM\r\n",SYS_Clocks.PCLK1_Frequency/1000000);
	printf("PCLK2:%dM\r\n",SYS_Clocks.PCLK2_Frequency/1000000);	

  printf("\r\nWarning: the test program will lead to TF card is specified in the sector data is rewrite, the original file can be damaged (not damage the TF card hardware),Continue to please click 'y' key!\r\n");		
  printf("\r\n警告：接下来的操作能破坏您在SD卡上的数据，请备份好SD卡的内容后，在按“Y”继续操作！\r\n");		
  while(USART_ReceiveData(EVAL_COM1)!= 'y');
  /*-------------------------- SD Init ----------------------------- */
  Status = SD_Init();
  printf("    \r\n\r\n01. ----- SD_Init Status:%d\r\n",Status);

  if (Status == SD_OK)
  {				                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 +                    
    printf("          Initialize SD card successfully!\r\n\r\n");
    /*----------------- Read CSD/CID MSD registers ------------------*/
    Status = SD_GetCardInfo(&SDCardInfo);
    printf("02. ----- SD_GetCardInfo Status:%d\r\n",Status);
  }
  
  if (Status == SD_OK)
  {
    printf("          Get SD card infomation successfully!\r\n          Block size:%x, Card type:%x\r\n\r\n",SDCardInfo.CardBlockSize,SDCardInfo.CardType);  
    /*----------------- Select Card --------------------------------*/
    Status = SD_SelectDeselect((u32) (SDCardInfo.RCA << 16));
    printf("03. ----- SD_SelectDeselect Status:%d\r\n",Status);
  }
  
  if (Status == SD_OK)
  {
    printf("          Select SD card successfully!\r\n\r\n");
    Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
    printf("04. ----- SD_EnableWideBusOperation Status:%d\r\n",Status);
  }
  
  /*------------------- Block Erase -------------------------------*/
  if (Status == SD_OK)
  {
    printf("          Enable wide bus operation successfully!\r\n\r\n");
    /* Erase NumberOfBlocks Blocks of WRITE_BL_LEN(512 Bytes) */
    Status = SD_Erase(Operate_Block*BlockSize, (Operate_Block+1)*BlockSize);
    printf("05. ----- SD_Erase Status:%d\r\n",Status);
  }
 
  /* Set Device Transfer Mode to DMA */
  if (Status == SD_OK)
  {  
    printf("          Erase block %d successfully!\r\n          All the data is 0x00\r\n\r\n",Operate_Block);
    Status = SD_SetDeviceMode(SD_DMA_MODE);
    printf("06. ----- SD_SetDeviceMode Status:%d\r\n",Status);
  }

  if (Status == SD_OK)
  {
    printf("          Set SD card mode successfully!\r\n\r\n");
    memset(Buffer_MultiBlock_Rx,0xfe,sizeof(Buffer_MultiBlock_Rx));

    Status = SD_ReadMultiBlocks(Operate_Block*BlockSize, Buffer_MultiBlock_Rx, BlockSize, NumberOfBlocks);

    printf("07. ----- SD_ReadMultiBlocks Status:%d\r\n",Status);
  }
  
  if (Status == SD_OK)
  {
	  printf("          Read 2 blocks from block %d sucessfully!\r\n          All the data is:\r\n",Operate_Block);  //karlno add:20100505 for debug
	  for(i=0;i<sizeof(Buffer_MultiBlock_Rx)>>2;i++)
	  {
		  printf("%02x:0x%08x ",i,Buffer_MultiBlock_Rx[i]);
	  }
	  printf("\r\n\r\n");
  }

  /*------------------- Block Read/Write --------------------------*/
  /* Fill the buffer to send */
  memset(Buffer_Block_Tx, 0x88,sizeof(Buffer_Block_Tx));


  if (Status == SD_OK)
  {
    /* Write block of 512 bytes on address 0 */
    Status = SD_WriteBlock(Operate_Block*BlockSize, Buffer_Block_Tx, BlockSize);
    printf("08. ----- SD_WriteBlock Status:%d\r\n",Status);
  }
  
  if (Status == SD_OK)
  {
    printf("          Write block %d successfully!\r\n          All the data is 0x88\r\n\r\n",Operate_Block);  //karlno add:20100505 for debug
    /* Read block of 512 bytes from address 0 */
    Status = SD_ReadBlock(Operate_Block*BlockSize, Buffer_Block_Rx, BlockSize);
    printf("09. ----- SD_ReadBlock Status:%d\r\n",Status);
  }

  if (Status == SD_OK)
  {
	  printf("          Read block %d successfully!\r\n          All the data is:\r\n",Operate_Block);  //karlno add:20100505 for debug
	  for(i=0;i<sizeof(Buffer_Block_Rx)>>2;i++)
	  {
		  printf("%02x:0x%08x ",i,Buffer_Block_Rx[i]);
	  }
	  printf("\r\n\r\n");
  }
  
  /*--------------- Multiple Block Read/Write ---------------------*/
  /* Fill the buffer to send */
  memset(Buffer_MultiBlock_Tx, 0x66, sizeof(Buffer_MultiBlock_Tx));

  if (Status == SD_OK)
  {
    /* Write multiple block of many bytes on address 0 */
    Status = SD_WriteMultiBlocks((Operate_Block+2)*BlockSize, Buffer_MultiBlock_Tx, BlockSize, NumberOfBlocks);
    printf("10. ----- SD_WriteMultiBlocks Status:%d\r\n",Status);
  }
  
  if (Status == SD_OK)
  {
    printf("          Write 2 blocks from block %d successfully!\r\n          All the data is 0x66\r\n\r\n",Operate_Block+2);  //karlno add:20100505 for debug
    /* Read block of many bytes from address 0 */
    Status = SD_ReadMultiBlocks((Operate_Block+2)*BlockSize, Buffer_MultiBlock_Rx, BlockSize, NumberOfBlocks);
    printf("11. ----- SD_ReadMultiBlocks Status:%d\r\n",Status);
  }

  
  if (Status == SD_OK)
  {
    printf("          Read 2 blocks from block %d successfully\r\n          All the data is:\r\n",Operate_Block+2);  //karlno add:20100505 for debug
    for(i=0;i<sizeof(Buffer_MultiBlock_Rx)>>2;i++)
    {
      printf("%02x:0x%08x ",i,Buffer_MultiBlock_Rx[i]);
    }
    printf("\r\n\r\n");
	printf("----- SD Succeed!\r\n");
  }
  while (1)
  {
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
