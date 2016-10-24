/*-------------------------------------------------------------------------
工程名称：参数存储于内部FLASH操作程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150611 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "m_flash.h"


/* Exported function prototypes -----------------------------------------------*/
/**
  * @brief  save_parameter 参数存储函数
  * @param  
  * @retval 
  */
void save_parameter(void)
{
	uint32_t address = SAVE_START_ADDRESS;
	uint16_t temp_i;

	//解锁FLASH后才能向FLASH中写数据
	FLASH_Unlock(); 			
	
	//清状态标记
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);	
	
	//擦除FLASH
	while(FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3) != FLASH_COMPLETE);
	
	//写入RF的8个参数
	for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
	{
		while(FLASH_ProgramWord(address, Xtend_900_para_buf[temp_i]) != FLASH_COMPLETE);
		
		Xtend_900_para[temp_i] = Xtend_900_para_buf[temp_i];
		
		address = address + 4;
	}
	
	//写入遥控器其他参数
	for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
	{
		while(FLASH_ProgramWord(address, local_para_conf_buf[temp_i]) != FLASH_COMPLETE);
		
		local_para_conf[temp_i] = local_para_conf_buf[temp_i];
		
		address = address + 4;
	}
	
	//锁定FLASH
	FLASH_Lock();	
	
	return;
}


/**
  * @brief  load_parameter 参数读出函数
  * @param  
  * @retval 
  */
void load_parameter(void)
{
	uint32_t address = SAVE_START_ADDRESS;
	uint16_t temp_i;

	//判断是否已经存入信道参数，没存入则不从flash中读出，而是用默认参数
	if(*(__IO uint32_t*)address != FIRST_SAVE)
	{
		//读出RF的8个参数
		for(temp_i = 0; temp_i < PARAMETER_QUANTITY; temp_i++)
		{
			Xtend_900_para[temp_i] = *(__IO uint32_t*)address;
			address = address + 4;
		}

		//读出遥控器其他参数
		for(temp_i = 0; temp_i < LOCAL_PARAMETER_QUANTITY; temp_i++)
		{
			local_para_conf[temp_i] = *(__IO uint32_t*)address;
			address = address + 4;
		}		
	}
		
	return;
}




