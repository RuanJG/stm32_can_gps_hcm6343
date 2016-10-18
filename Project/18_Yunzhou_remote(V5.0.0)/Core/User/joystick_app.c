/*-------------------------------------------------------------------------
�������ƣ�ҡ�ˡ���ť����ش���Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/


#include "joystick_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Joystick_Task_Handle;


/**
  * @brief  Joystick task
  * @param  pvParameters not used
  * @retval None
  */
void Joystick_Task(void * pvParameters)
{
	uint16_t J_Temp = 0, i_temp = 0;
	int count_temp = 0;

	//�ȴ�200�������������Һ������ʼ��
	vTaskDelay(200);	
	GPIO_SetBits(GPIOF, GPIO_Pin_6);			//����Һ����
	
  /* Run the Joystick task */
  while (1)
  {
		/*ADC��������-----------------------------------------------------------------------------------*/
		//��ص�����ֵ----------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][0];
		}		
		Battery_Power_Avg = count_temp / ADC_BUFFER_SIZE;					//��ص�����ֵ	Bat_ADC
		
		//���ҡ�� X �������ֵ----------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][1];
		}		
		Left_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//���ҡ�� X �������ֵ		RCKLY_ADC
		
		//��λ����ť��ֵ----------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][2];
		}		
		Knob_Avg = count_temp / ADC_BUFFER_SIZE;					//��λ����ť��ֵ		Wheel_ADC
		
		//���ҡ�� Y �������ֵ---------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][3];
		}		
		Left_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//���ҡ�� Y �������ֵ		RCKLX_ADC
		
		//�Ҳ�ҡ�� X �������ֵ---------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][4];
		}		
		Right_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//�Ҳ�ҡ�� X �������ֵ		RCKRY_ADC
		
		//�Ҳ�ҡ�� Y �������ֵ---------------------------------------------
		count_temp = 0;		
		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
		{
			count_temp += ADCValue[i_temp][5];
		}		
		Right_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//�Ҳ�ҡ�� X �������ֵ		RCKRY_ADC		
		/*ADC��������-----------------------------------------------------------------------------------*/		
		
		J_Temp++;
				
		if(J_Temp >= 10)
		{
			J_Temp = 0;
			
			printf("bbbb\n");
		}
		
		vTaskDelay(10);
	}
}
