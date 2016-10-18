/*-------------------------------------------------------------------------
工程名称：Xtend900 数传模块相关驱动程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150608 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "Xtend_900.h"


/* Exported variables ---------------------------------------------------------*/
uint16_t Xtend_900_para[PARAMETER_QUANTITY];
// 索引0 ~ PARAMETER_QUANTITY - 1代表的参数如下
/*------------------------------------------------------------------------------------------------
索引号			名称										描述											取值范围
0 					Hopping_Channel					数传模块信道 (HP) 				0 ~ 9
1 					Modem_VID								数传模块VID	(ID)					0x11 ~ 0x7fff
2 					Source_Address					本地地址		(MY)					0 ~ 0xffff
3						Destination_Address			目的地址		(DT)					0 ~ 0xffff   0xffff为广播地址
4						Address_Mask						掩码地址		(MK)					0 ~ 0xffff
5						TX_Power_Level					发射功率		(PL)					0 ~ 4    0为1mW，1为10mW，2为100mW，3为500mW，4为1W
6						API_Enable							API模式			(AP)					0 ~ 2		0为禁止API，1为不带escaped的API模式，2为带escaped的API模式
7						Board_Temperature				数传模块当前温度 (TP)			0 ~ 0x7f	        
------------------------------------------------------------------------------------------------*/
uint16_t Xtend_900_para_buf[PARAMETER_QUANTITY];					//参数修改缓存
uint8_t Xtend_900_para_char[PARAMETER_QUANTITY][4];				//参数缓存16进制 ASCII码形式


/* Const define ------------------------------------------------------------*/
uint32_t Xtend_900_para_min[PARAMETER_QUANTITY] = {0, 0x11, 0, 0, 0, 0, 0, 0};														//定义最小值
uint32_t Xtend_900_para_max[PARAMETER_QUANTITY] = {9, 0x7fff, 0xffff, 0xffff, 0xffff, 4, 2, 0x7f};				//定义最大值


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

	return;
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

	return;
}


/**
  * @brief  handle_parameter_message
  * @param  None
  * @retval None
  */
void handle_parameter_message(void)
{
	uint16_t temp_i, temp_j;
	uint16_t temp_len;							//数据结构的长度
	uint32_t temp_count;						//数据结构的暂存累加值
	uint16_t pre_pointer = 0;				//指向数据结构的头部指针
	uint16_t sep_pointer = 0;				//指向数据结构的尾部指针，即指向分隔符<CR>  十六进制为0x0d的指针位置
	
	
	//将串口缓冲区的内容分别保存到个参数中，分隔符为<CR> ，16进制为0x0d
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		//Step1：获取当前参数的字符串长度--------------------------------------
		while(USART1_RX_Buffer[sep_pointer] != 0x0d)
		{
			sep_pointer++;
		}
		
		temp_len = sep_pointer - pre_pointer;				
		
		
		//Step2：将字符串的ASCII码形式转换为uint32_t形式--------------------------------------
		temp_count = 0;
		
		for(temp_j = 0; temp_j < temp_len; temp_j++)
		{
			temp_count = temp_count << 4;			//左移4位
			
			//当前字符为 '0' ~ '9'
			if((USART1_RX_Buffer[pre_pointer] >= 0x30) && (USART1_RX_Buffer[pre_pointer] <= 0x39))
				temp_count = temp_count + USART1_RX_Buffer[pre_pointer] - 0x30;
			
			//当前字符为 'A' ~ 'F'
			else if((USART1_RX_Buffer[pre_pointer] >= 0x41) && (USART1_RX_Buffer[pre_pointer] <= 0x46))
				temp_count = temp_count + USART1_RX_Buffer[pre_pointer] - 0x41 + 10;
			
			pre_pointer++;
		}
		
		//Step3：指针指向下个参数--------------------------------------
		pre_pointer++;
		sep_pointer = pre_pointer;
		
		
		//Step4：存储转换后的数值--------------------------------------
		Xtend_900_para[temp_i] = temp_count;
	}
	
	return;
}


/**
  * @brief  RF_Parameter_Transform		数据存储格式转换
  * @param  None
  * @retval None
  */
void RF_Parameter_Transform(void)
{
	uint8_t i, j;
	uint32_t temp1, temp2;
	
	for(i = 0; i < PARAMETER_QUANTITY; i++)
	{
		temp1 = Xtend_900_para[i];				//临时读取当前参数值

		for(j = 0; j < 4; j++)
		{
			temp2 = temp1 % 16;							//提取最低位
			
			//当前字符为 '0' ~ '9'			
			if(temp2 <= 9)
			{
				temp2 = temp2 + 0x30;
			}
			
			//当前字符为 'A' ~ 'F'
			else
			{
				temp2 = temp2 - 10 + 0x41;
			}
			
			//保存数据
			Xtend_900_para_char[i][3 - j] = temp2;
			
			//当前变量除16
			temp1 = temp1 >> 4;
		}
	}
}


/**
  * @brief  save_RF_parameter		保存参数
  * @param  None
  * @retval None
  */
void save_RF_parameter(void)
{
//	uint16_t send_len = 0;

	//Step1：进入AT命令模式---------------------------------------------------------
	enter_AT_Command();					//进入AT命令模式，发送+++  

	
	//Step2：显示等待信息-----------------------------------------------------------
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 100);	
	
	GUI_SetFont(&GUI_Font10_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);	
	
	GUI_DispStringAt("Saving RF parameter, please wait...", 5, 22);										//显示等待信息	
	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 200, 10 + 0 + 100);


	//Step3：等待2秒----------------------------------------------------------------
	vTaskDelay(3000);


	//Step4：将存储的16进制形式转换成ASCII码数组------------------------------------
	RF_Parameter_Transform();
	
	
	//Step5：组装发送的数据包-------------------------------------------------------	
	USART1_TX_Buffer[0] = 'A';										//ATHP  设置信道
	USART1_TX_Buffer[1] = 'T';
	USART1_TX_Buffer[2] = 'H';
	USART1_TX_Buffer[3] = 'P';
	USART1_TX_Buffer[4] = Xtend_900_para_char[0][0];
	USART1_TX_Buffer[5] = Xtend_900_para_char[0][1];
	USART1_TX_Buffer[6] = Xtend_900_para_char[0][2];
	USART1_TX_Buffer[7] = Xtend_900_para_char[0][3];
	USART1_TX_Buffer[8] = ',';	
	USART1_TX_Buffer[9] = 'I';										//ID
	USART1_TX_Buffer[10] = 'D';
	USART1_TX_Buffer[11] = Xtend_900_para_char[1][0];
	USART1_TX_Buffer[12] = Xtend_900_para_char[1][1];
	USART1_TX_Buffer[13] = Xtend_900_para_char[1][2];
	USART1_TX_Buffer[14] = Xtend_900_para_char[1][3];
	USART1_TX_Buffer[15] = ',';	
	USART1_TX_Buffer[16] = 'M';										//MY
	USART1_TX_Buffer[17] = 'Y';
	USART1_TX_Buffer[18] = Xtend_900_para_char[2][0];
	USART1_TX_Buffer[19] = Xtend_900_para_char[2][1];
	USART1_TX_Buffer[20] = Xtend_900_para_char[2][2];
	USART1_TX_Buffer[21] = Xtend_900_para_char[2][3];
	USART1_TX_Buffer[22] = ',';	
	USART1_TX_Buffer[23] = 'D';										//DT
	USART1_TX_Buffer[24] = 'T';
	USART1_TX_Buffer[25] = Xtend_900_para_char[3][0];
	USART1_TX_Buffer[26] = Xtend_900_para_char[3][1];
	USART1_TX_Buffer[27] = Xtend_900_para_char[3][2];
	USART1_TX_Buffer[28] = Xtend_900_para_char[3][3];
	USART1_TX_Buffer[29] = ',';	
	USART1_TX_Buffer[30] = 'M';										//MK
	USART1_TX_Buffer[31] = 'K';
	USART1_TX_Buffer[32] = Xtend_900_para_char[4][0];
	USART1_TX_Buffer[33] = Xtend_900_para_char[4][1];
	USART1_TX_Buffer[34] = Xtend_900_para_char[4][2];
	USART1_TX_Buffer[35] = Xtend_900_para_char[4][3];
	USART1_TX_Buffer[36] = ',';		
	USART1_TX_Buffer[37] = 'P';										//PL
	USART1_TX_Buffer[38] = 'L';
	USART1_TX_Buffer[39] = Xtend_900_para_char[5][0];
	USART1_TX_Buffer[40] = Xtend_900_para_char[5][1];
	USART1_TX_Buffer[41] = Xtend_900_para_char[5][2];
	USART1_TX_Buffer[42] = Xtend_900_para_char[5][3];
	USART1_TX_Buffer[43] = ',';		
	USART1_TX_Buffer[44] = 'A';										//AP
	USART1_TX_Buffer[45] = 'P';
	USART1_TX_Buffer[46] = Xtend_900_para_char[6][0];
	USART1_TX_Buffer[47] = Xtend_900_para_char[6][1];
	USART1_TX_Buffer[48] = Xtend_900_para_char[6][2];
	USART1_TX_Buffer[49] = Xtend_900_para_char[6][3];
	USART1_TX_Buffer[50] = ',';	
	USART1_TX_Buffer[51] = 'W';										//WR
	USART1_TX_Buffer[52] = 'R';
	USART1_TX_Buffer[53] = ',';	
	USART1_TX_Buffer[54] = 'C';										//CN
	USART1_TX_Buffer[55] = 'N';
	USART1_TX_Buffer[56] = 0x0d;
	USART1_TX_Buffer[57] = '\0';
	
//	printf("%s", USART1_TX_Buffer);
	
	//Step6：发送修改数据包
	transmit_data(USART1_TX_Buffer, 57);	
}


/**
  * @brief  not_save_RF_parameter		不保存参数
  * @param  None
  * @retval None
  */
void not_save_RF_parameter(void)
{
	USART1_TX_Buffer[0] = 'A';										//ATCN  退出设置
	USART1_TX_Buffer[1] = 'T';
	USART1_TX_Buffer[2] = 'C';									
	USART1_TX_Buffer[3] = 'N';
	USART1_TX_Buffer[4] = 0x0d;
	
	transmit_data(USART1_TX_Buffer, 5);		
}
