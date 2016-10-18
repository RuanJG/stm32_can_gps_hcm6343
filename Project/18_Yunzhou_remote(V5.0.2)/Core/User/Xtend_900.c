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

//uint8_t RT_buffer[50];								//二进制命令模式回读数据缓冲区
//uint8_t RT_pointer = 0;								//二进制命令模式缓冲指针

char Xtend_900_para_buf[PARAMETER_QUANTITY][4];						//Xtend 900 参数存储字符格式
char Xtend_900_para_buf_tmp[PARAMETER_QUANTITY][4];				//Xtend 900 参数存储字符格式，显示缓存
uint32_t Hopping_Channel = 0;					//数传模块信道 (HP) 			取值范围为 0 ~ 9
uint32_t Modem_VID = 0;								//数传模块VID	(ID)				取值范围为 0x11 ~ 0x7fff
uint32_t Source_Address = 0;					//本地地址		(MY)				取值范围为 0 ~ 0xffff
uint32_t Destination_Address = 0;			//目的地址		(DT)				取值范围为 0 ~ 0xffff   0xffff为广播地址
uint32_t Address_Mask = 0;						//掩码地址		(MK)				取值范围为 0 ~ 0xffff
uint32_t TX_Power_Level = 0;					//发射功率		(PL)				取值范围为 0 ~ 4    0为1mW，1为10mW，2为100mW，3为500mW，4为1W
uint32_t API_Enable = 0;							//API模式			(AP)				取值范围为 0 ~ 2		0为禁止API，1为不带escaped的API模式，2为带escaped的API模式
uint32_t Board_Temperature = 0;				//数传模块当前温度				取值范围为 0 ~ 0x7f	


/* Private const ---------------------------------------------------------*/
const uint8_t ATRT[15] = {0x41, 0x54, 0x52, 0x54, 0x31, 0x2c, 0x57, 0x52, 0x2c, 0x43, 0x4e, 0x0d};				//ATRT1,WR,CN<Enter>		允许二进制命令模式
const uint8_t RTTP[3] = {0x38, 0x00, 0x00};						//ATTP的二进制命令格式
const uint8_t RTRT[4] = {0x16, 0x00, 0x00, 0x08};						//返回AT命令模式

//发送参数查询命令 ATHP,ID,MY,DT,MK,PL,AP,TP<CR>
//读取信道，VID，本地地址，目的地址，掩码地址，发射功率，API模式，板载温度等信息
const uint8_t inquiry_para[26] = {0x41, 0x54, 0x48, 0x50, 0x2c, 0x49, 0x44, 0x2c, 0x4d, \
																	0x59, 0x2c, 0x44, 0x54, 0x2c, 0x4d, 0x4b, 0x2c, 0x50, \
																	0x4c, 0x2c, 0x41, 0x50, 0x2c, 0x54, 0x50, 0x0d};																									

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
				
//				//ATRT1,WR,CN<Enter>
//				for(xt900_i = 0; xt900_i < 15; xt900_i++)
//				{
//					USART_SendData(USART1, ATRT[xt900_i]);

//					/* Loop until the end of transmission */
//					while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);					
//				}
			}
			
//			//进入Binary Commands模式后读取数传模块状态信息----------------------------
//			GPIO_SetBits(GPIOI, GPIO_Pin_7);			//启动Xtend 900 的CMD
//			RT_pointer = 0;
//			
////			//读取数传模块当前温度
////			for(xt900_i = 0; xt900_i < 3; xt900_i++)
////			{
////				USART_SendData(USART1, RTTP[xt900_i]);

////				/* Loop until the end of transmission */
////				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
////				{
////					vTaskDelay(1);
////				}					
////			}

//			//读取数传模块当前温度
//			for(xt900_i = 0; xt900_i < 4; xt900_i++)
//			{
//				USART_SendData(USART1, RTRT[xt900_i]);

//				/* Loop until the end of transmission */
//				while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
//				{
//					vTaskDelay(1);
//				}					
//			}				
//			
//			GPIO_ResetBits(GPIOI, GPIO_Pin_7);			//关闭Xtend 900 的CMD	
//			//进入Binary Commands模式后读取数传模块状态信息----------------------------



		}
		
		vTaskDelay(100);							//读出时间间隔为1秒，过快会导致数据读出失败
	}		
}


/**
  * @brief  enter_AT_Command
  * @param  None
  * @retval None
  */
void enter_AT_Command(void)
{
	uint16_t xt900_i = 0;
	
	//进入AT模式： +++   
	for(xt900_i = 0; xt900_i < 3; xt900_i++)
	{
		USART_SendData(USART1, '+');
		
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}		
}


/**
  * @brief  send_AT_inquiry
  * @param  None
  * @retval None
  */
void send_AT_inquiry(void)
{
	uint16_t xt900_i = 0;
	
	//发送命令 ATHP,ID,MY,DT,MK,PL,AP,TP<CR>  
	for(xt900_i = 0; xt900_i < 26; xt900_i++)
	{
		USART_SendData(USART1, inquiry_para[xt900_i]);
		
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}		
}


/**
  * @brief  handle_parameter_message
  * @param  None
  * @retval None
  */
void handle_parameter_message(void)
{
	uint16_t temp_i;
	uint16_t temp_len;
	
	//将串口缓冲区的内容分别保存到个参数中，分隔符为<CR> ，16进制为0x0d
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		
	}
}

