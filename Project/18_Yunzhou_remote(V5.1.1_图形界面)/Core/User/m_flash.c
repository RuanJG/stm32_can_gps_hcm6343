/*-------------------------------------------------------------------------
�������ƣ������洢���ڲ�FLASH��������
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150611 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "m_flash.h"


/* Exported function prototypes -----------------------------------------------*/
/**
  * @brief  save_parameter �����洢����
  * @param  
  * @retval 
  */
void save_parameter(void)
{
	uint32_t address = SAVE_START_ADDRESS;
	uint16_t temp_i;

	//����FLASH�������FLASH��д����
	FLASH_Unlock(); 			
	
	//��״̬���
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);	
	
	//����FLASH
	while(FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3) != FLASH_COMPLETE);
	
	//д��RF��8������
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		while(FLASH_ProgramWord(address, Xtend_900_para_buf[temp_i]) != FLASH_COMPLETE);
		
		Xtend_900_para[temp_i] = Xtend_900_para_buf[temp_i];
		
		address = address + 4;
	}
	
	//д��ң������������
	for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
	{
		while(FLASH_ProgramWord(address, local_para_conf_buf[temp_i]) != FLASH_COMPLETE);
		
		local_para_conf[temp_i] = local_para_conf_buf[temp_i];
		
		address = address + 4;
	}
	
	//����FLASH
	FLASH_Lock();	
	
	return;
}


/**
  * @brief  load_parameter ������������
  * @param  
  * @retval 
  */
void load_parameter(void)
{
	uint32_t address = SAVE_START_ADDRESS;
	uint16_t temp_i;

	//�ж��Ƿ��Ѿ������ŵ�������û�����򲻴�flash�ж�����������Ĭ�ϲ���
	if(*(__IO uint32_t*)address != FIRST_SAVE)
	{
		//����RF��8������
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			Xtend_900_para[temp_i] = *(__IO uint32_t*)address;
			address = address + 4;
		}

		//����ң������������
		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			local_para_conf[temp_i] = *(__IO uint32_t*)address;
			address = address + 4;
		}		
	}
		
	return;
}




