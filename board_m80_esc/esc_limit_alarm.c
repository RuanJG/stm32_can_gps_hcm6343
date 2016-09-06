#include "stm32f10x.h" 	
#include "esc_box.h"

void Esc_Limit_Configuration (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
													   
	


	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LIMIT_L_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LIMIT_L_GPIO_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LIMIT_M_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LIMIT_M_GPIO_BANK, &GPIO_InitStructure);
	
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LIMIT_R_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LIMIT_R_GPIO_BANK, &GPIO_InitStructure);

}
//End of File

#include "esc_box.h"
volatile uint8_t limit_status=0;
void esc_check_limit_gpio_loop()
{
	 uint8_t limit_r = GPIO_ReadInputDataBit(LIMIT_R_GPIO_BANK,LIMIT_R_GPIO_PIN);
	 uint8_t limit_l = GPIO_ReadInputDataBit(LIMIT_L_GPIO_BANK,LIMIT_L_GPIO_PIN);
	 uint8_t limit_m = GPIO_ReadInputDataBit(LIMIT_M_GPIO_BANK,LIMIT_M_GPIO_PIN);
	 
	//Ç°²Õ
	if( limit_r == 0 )
		limit_status |= LEAK_HEAD_SENSOR_TAG;
	else
		limit_status &= (~LEAK_HEAD_SENSOR_TAG);
	//ºó²Õ
	if( limit_m == 0 )
		limit_status |= LEAK_BACK_SENSOR_TAG;
	else
		limit_status &= (~LEAK_BACK_SENSOR_TAG);
	//ÖÐ²Õ
	if( limit_l == 0)
		limit_status |= LEAK_MIDDLE_SENSOR_TAG;
	else
		limit_status &= (~LEAK_MIDDLE_SENSOR_TAG);
	
	
	//if( limit_status != 0 ){
	//	logd_uint("Alarm=",limit_status);
	//}
	#if 0
	if( limit_status != 0 )
	{
		Alarm_limit_Position();
	}else{
		DisAlarm_limit_Position();
	}
	#endif
}

uint8_t get_esc_limit_gpio_status()
{
	return limit_status ;
}
extern uint32_t speeker_ms ;
volatile char limit_belled = 0;
void DisAlarm_limit_Position()
{
	if(speeker_ms==0 && limit_belled == 1){
			logd("leak close bell");
			Rtu_485_Dam_Cmd(0x08,3,0,0);
			limit_belled = 0;
	}
}
void Alarm_limit_Position()
{
	//if( limit_belled==0 && (dam16_08.status & 0x4) == 0){
	if( limit_belled==0) {
		logd("leak start bell");
		limit_belled = 1;
		Rtu_485_Dam_Cmd(0x08,3,1,0);
	}
}