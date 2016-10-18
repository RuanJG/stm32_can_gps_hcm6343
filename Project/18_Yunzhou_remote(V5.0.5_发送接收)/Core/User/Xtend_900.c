/*-------------------------------------------------------------------------
�������ƣ�Xtend900 ����ģ�������������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150608 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "Xtend_900.h"


/* Exported variables ---------------------------------------------------------*/
uint16_t Xtend_900_para[PARAMETER_QUANTITY];
// ����0 ~ PARAMETER_QUANTITY - 1����Ĳ�������
/*------------------------------------------------------------------------------------------------
������			����										����											ȡֵ��Χ
0 					Hopping_Channel					����ģ���ŵ� (HP) 				0 ~ 9
1 					Modem_VID								����ģ��VID	(ID)					0x11 ~ 0x7fff
2 					Source_Address					���ص�ַ		(MY)					0 ~ 0xffff
3						Destination_Address			Ŀ�ĵ�ַ		(DT)					0 ~ 0xffff   0xffffΪ�㲥��ַ
4						Address_Mask						�����ַ		(MK)					0 ~ 0xffff
5						TX_Power_Level					���书��		(PL)					0 ~ 4    0Ϊ1mW��1Ϊ10mW��2Ϊ100mW��3Ϊ500mW��4Ϊ1W
6						API_Enable							APIģʽ			(AP)					0 ~ 2		0Ϊ��ֹAPI��1Ϊ����escaped��APIģʽ��2Ϊ��escaped��APIģʽ
7						Board_Temperature				����ģ�鵱ǰ�¶� (TP)			0 ~ 0x7f	        
------------------------------------------------------------------------------------------------*/
uint16_t Xtend_900_para_buf[PARAMETER_QUANTITY];					//�����޸Ļ���
uint8_t Xtend_900_para_char[PARAMETER_QUANTITY][4];				//��������16���� ASCII����ʽ


/* Const define ------------------------------------------------------------*/
uint32_t Xtend_900_para_min[PARAMETER_QUANTITY] = {0, 0x11, 0, 0, 0, 0, 0, 0};														//������Сֵ
uint32_t Xtend_900_para_max[PARAMETER_QUANTITY] = {9, 0x7fff, 0xffff, 0xffff, 0xffff, 4, 2, 0x7f};				//�������ֵ


/* Private const ---------------------------------------------------------*/
const uint8_t ATRT[15] = {0x41, 0x54, 0x52, 0x54, 0x31, 0x2c, 0x57, 0x52, 0x2c, 0x43, 0x4e, 0x0d};				//ATRT1,WR,CN<Enter>		�������������ģʽ
const uint8_t RTTP[3] = {0x38, 0x00, 0x00};						//ATTP�Ķ����������ʽ
const uint8_t RTRT[4] = {0x16, 0x00, 0x00, 0x08};						//����AT����ģʽ

//���Ͳ�����ѯ���� ATHP,ID,MY,DT,MK,PL,AP,TP<CR>
//��ȡ�ŵ���VID�����ص�ַ��Ŀ�ĵ�ַ�������ַ�����书�ʣ�APIģʽ�������¶ȵ���Ϣ
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
	
	//����ATģʽ�� +++   
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
	
	//�������� ATHP,ID,MY,DT,MK,PL,AP,TP<CR>  
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
	uint16_t temp_len;							//���ݽṹ�ĳ���
	uint32_t temp_count;						//���ݽṹ���ݴ��ۼ�ֵ
	uint16_t pre_pointer = 0;				//ָ�����ݽṹ��ͷ��ָ��
	uint16_t sep_pointer = 0;				//ָ�����ݽṹ��β��ָ�룬��ָ��ָ���<CR>  ʮ������Ϊ0x0d��ָ��λ��
	
	
	//�����ڻ����������ݷֱ𱣴浽�������У��ָ���Ϊ<CR> ��16����Ϊ0x0d
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		//Step1����ȡ��ǰ�������ַ�������--------------------------------------
		while(USART1_RX_Buffer[sep_pointer] != 0x0d)
		{
			sep_pointer++;
		}
		
		temp_len = sep_pointer - pre_pointer;				
		
		
		//Step2�����ַ�����ASCII����ʽת��Ϊuint32_t��ʽ--------------------------------------
		temp_count = 0;
		
		for(temp_j = 0; temp_j < temp_len; temp_j++)
		{
			temp_count = temp_count << 4;			//����4λ
			
			//��ǰ�ַ�Ϊ '0' ~ '9'
			if((USART1_RX_Buffer[pre_pointer] >= 0x30) && (USART1_RX_Buffer[pre_pointer] <= 0x39))
				temp_count = temp_count + USART1_RX_Buffer[pre_pointer] - 0x30;
			
			//��ǰ�ַ�Ϊ 'A' ~ 'F'
			else if((USART1_RX_Buffer[pre_pointer] >= 0x41) && (USART1_RX_Buffer[pre_pointer] <= 0x46))
				temp_count = temp_count + USART1_RX_Buffer[pre_pointer] - 0x41 + 10;
			
			pre_pointer++;
		}
		
		//Step3��ָ��ָ���¸�����--------------------------------------
		pre_pointer++;
		sep_pointer = pre_pointer;
		
		
		//Step4���洢ת�������ֵ--------------------------------------
		Xtend_900_para[temp_i] = temp_count;
	}
	
	return;
}


/**
  * @brief  RF_Parameter_Transform		���ݴ洢��ʽת��
  * @param  None
  * @retval None
  */
void RF_Parameter_Transform(void)
{
	uint8_t i, j;
	uint32_t temp1, temp2;
	
	for(i = 0; i < PARAMETER_QUANTITY; i++)
	{
		temp1 = Xtend_900_para[i];				//��ʱ��ȡ��ǰ����ֵ

		for(j = 0; j < 4; j++)
		{
			temp2 = temp1 % 16;							//��ȡ���λ
			
			//��ǰ�ַ�Ϊ '0' ~ '9'			
			if(temp2 <= 9)
			{
				temp2 = temp2 + 0x30;
			}
			
			//��ǰ�ַ�Ϊ 'A' ~ 'F'
			else
			{
				temp2 = temp2 - 10 + 0x41;
			}
			
			//��������
			Xtend_900_para_char[i][3 - j] = temp2;
			
			//��ǰ������16
			temp1 = temp1 >> 4;
		}
	}
}


/**
  * @brief  save_RF_parameter		�������
  * @param  None
  * @retval None
  */
void save_RF_parameter(void)
{
//	uint16_t send_len = 0;

	//Step1������AT����ģʽ---------------------------------------------------------
	enter_AT_Command();					//����AT����ģʽ������+++  

	
	//Step2����ʾ�ȴ���Ϣ-----------------------------------------------------------
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(bg1_memdev);	
	GUI_MEMDEV_Clear(bg1_memdev);					
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_FillRect(0, 0, 250, 100);	
	
	GUI_SetFont(&GUI_Font10_1);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(GUI_TRANSPARENT);	
	
	GUI_DispStringAt("Saving RF parameter, please wait...", 5, 22);										//��ʾ�ȴ���Ϣ	
	GUI_MEMDEV_CopyToLCDAt(bg1_memdev, 200, 10 + 0 + 100);


	//Step3���ȴ�2��----------------------------------------------------------------
	vTaskDelay(3000);


	//Step4�����洢��16������ʽת����ASCII������------------------------------------
	RF_Parameter_Transform();
	
	
	//Step5����װ���͵����ݰ�-------------------------------------------------------	
	USART1_TX_Buffer[0] = 'A';										//ATHP  �����ŵ�
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
	
	//Step6�������޸����ݰ�
	transmit_data(USART1_TX_Buffer, 57);	
}


/**
  * @brief  not_save_RF_parameter		���������
  * @param  None
  * @retval None
  */
void not_save_RF_parameter(void)
{
	USART1_TX_Buffer[0] = 'A';										//ATCN  �˳�����
	USART1_TX_Buffer[1] = 'T';
	USART1_TX_Buffer[2] = 'C';									
	USART1_TX_Buffer[3] = 'N';
	USART1_TX_Buffer[4] = 0x0d;
	
	transmit_data(USART1_TX_Buffer, 5);		
}
