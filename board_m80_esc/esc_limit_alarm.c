#include "stm32f10x.h" 	
#include "esc_box.h"

void Esc_Limit_Configuration (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
													   
	
	

	//接近开关，检测是否方向推动杆推动泵的roll方向yaw 是否到达最左，最右，中间，的位置
// 采用中断来做处理

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LIMIT_L_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(LIMIT_L_GPIO_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LIMIT_M_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(LIMIT_M_GPIO_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LIMIT_R_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(LIMIT_R_GPIO_BANK, &GPIO_InitStructure);

}
//End of File

uint8_t limit_status=0;
void esc_check_limit_gpio_loop()
{
	 uint8_t limit_r = GPIO_ReadInputDataBit(LIMIT_R_GPIO_BANK,LIMIT_R_GPIO_PIN);
	 uint8_t limit_l = GPIO_ReadInputDataBit(LIMIT_L_GPIO_BANK,LIMIT_L_GPIO_PIN);
	 uint8_t limit_m = GPIO_ReadInputDataBit(LIMIT_M_GPIO_BANK,LIMIT_M_GPIO_PIN);
	 
	
	if( limit_r == 0 )
		limit_status |= 0x04;
	else
		limit_status &= (~0x04);
	if( limit_m == 0 )
		limit_status |= 0x02;
	else
		limit_status &= (~0x02);
	
	if( limit_l )
		limit_status |= 0x01;
	else
		limit_status &= (~0x01);
	
	
	//if( limit_status != 0 ){
	//	logd_uint("Alarm=",limit_status);
	//}
	
}

uint8_t get_esc_limit_gpio_status()
{
	return limit_status ;
}

void Alarm_limit_Position()
{
	if( (dam16_08.status & 0x4) == 0)
		Rtu_485_Dam_Cmd(0x08,3,1,0);
}