/*-------------------------------------------------------------------------
工程名称：Xtend900 数传模块相关驱动程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150608 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


#include "Xtend_900.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle		Xtend_900_Task_Handle;

uint8_t board_temperature = 0;				//数传模块当前温度


/* Private const ---------------------------------------------------------*/
const uint8_t ATRT[15] = {0x41, 0x54, 0x52, 0x54, 0x31, 0x2c, 0x57, 0x52, 0x2c, 0x43, 0x4e, 0x0d};				//ATRT1,WR,CN<Enter>		允许二进制命令模式
const uint8_t RTTP[3] = {0x38, 0x00, 0x00};						//ATTP的二进制命令格式

/**
  * @brief  Xtend_900 handling task
  * @param  pvParameters not used
  * @retval None
  */
void Xtend_900_Task(void * pvParameters)
{
	uint16_t xt900_i = 0;
	
	/* Run the Xtend_900 handling task */
  while (1)
  {
		//判断是否为调试模式
		if(debug_Mode == 1)
		{		
			//判断是否进入Binary Commands模式
			if(CANCEL_Button && OK_Button)
			{
				//进入AT模式： +++   
				for(xt900_i = 0; xt900_i < 3; xt900_i++)
				{
					USART_SendData(USART1, '+');
					
					/* Loop until the end of transmission */
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				}	

				vTaskDelay(2000);
				
				//ATRT1,WR,CN<Enter>
				for(xt900_i = 0; xt900_i < 15; xt900_i++)
				{
					USART_SendData(USART1, ATRT[xt900_i]);

					/* Loop until the end of transmission */
					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);					
				}
			}
			
//			GPIO_SetBits(GPIOI, GPIO_Pin_7);			//启动Xtend 900 的CMD
//			//读取数传模块当前温度
//			for(xt900_i = 0; xt900_i < 3; xt900_i++)
//			{
//				USART_SendData(USART1, RTTP[xt900_i]);

//				/* Loop until the end of transmission */
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//				{
//					vTaskDelay(1);
//				}					
//			}		
//			GPIO_ResetBits(GPIOI, GPIO_Pin_7);			//关闭Xtend 900 的CMD
//			vTaskDelay(10);		
		}
		
		vTaskDelay(1000);							//读出时间间隔为1秒，过快会导致数据读出失败
	}		
}





