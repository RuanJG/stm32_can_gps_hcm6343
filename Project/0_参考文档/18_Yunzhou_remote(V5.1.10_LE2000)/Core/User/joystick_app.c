/*-------------------------------------------------------------------------
�������ƣ�ҡ�ˡ���ť����ش���Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
-------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "joystick_app.h"


/* Exported variables ---------------------------------------------------------*/
xTaskHandle                   Joystick_Task_Handle;

u16 RightRudderCommand;				//�Ҷ�����ӳ��
u16 RightSpeedCommand;				//���ƽ��ٶ�ӳ��
u16 LeftRudderCommand;				//�������ӳ��
u16 LeftSpeedCommand;					//���ƽ��ٶ�ӳ��

//��ť��ֵӳ��
u16 KnobValue;									//��ť������Ĵ���ֵ	---  ��ˮ
u16 SpeedFactor;								//��ť������Ĵ���ֵ	---	 �ٶ����ӻ�����

int xx;
int yy;
int adcOffset = 100;
double doublespeed;

u8 ControllerType = 2;							//ҡ��ģʽ���Ƿ������ť�ж�


/**
  * @brief  Joystick task
  * @param  pvParameters not used
  * @retval None
  */
void Joystick_Task(void * pvParameters)
{
	uint16_t J_Temp = 0, i_temp = 0;
	int count_temp = 0;
	uint16_t Battery_temp[10];
	uint16_t Battery_pointer = 0;

//	//�ȴ�200�������������Һ������ʼ��
//	vTaskDelay(200);	
//	GPIO_SetBits(GPIOF, GPIO_Pin_6);			//����Һ����
	
//	LCD_PWM_Config();						//Һ����ĻPWM����
//	Buzzer_PWM_Config();
	
  /* Run the Joystick task */
  while (1)
  {
		/*ADC��������-----------------------------------------------------------------------------------*/
		//��ص�����ֵ----------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][0];
//		}		
//		Battery_Power_Avg = count_temp / ADC_BUFFER_SIZE;					//��ص�����ֵ	Bat_ADC

		
		
		//���ҡ�� X �������ֵ----------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][1];
//		}		
//		Left_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//���ҡ�� X �������ֵ		RCKLY_ADC
		Left_Joystick_X_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 1);
		
		
		//��λ����ť��ֵ----------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][2];
//		}		
//		Knob_Avg = count_temp / ADC_BUFFER_SIZE;					//��λ����ť��ֵ		Wheel_ADC
		Knob_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 2);
		
		
		//���ҡ�� Y �������ֵ---------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][3];
//		}		
//		Left_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//���ҡ�� Y �������ֵ		RCKLX_ADC
		Left_Joystick_Y_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 3);
		
		
		//�Ҳ�ҡ�� X �������ֵ---------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][4];
//		}		
//		Right_Joystick_X_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//�Ҳ�ҡ�� X �������ֵ		RCKRY_ADC
		Right_Joystick_X_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 4);

		
		//�Ҳ�ҡ�� Y �������ֵ---------------------------------------------
//		count_temp = 0;		
//		for(i_temp = 0; i_temp < ADC_BUFFER_SIZE; i_temp++)
//		{
//			count_temp += ADCValue[i_temp][5];
//		}		
//		Right_Joystick_Y_axis_Avg = count_temp / ADC_BUFFER_SIZE;					//�Ҳ�ҡ�� X �������ֵ		RCKRY_ADC		
		Right_Joystick_Y_axis_Avg = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 5);
		/*ADC��������-----------------------------------------------------------------------------------*/		
		
		J_Temp++;
				
		if(J_Temp >= 10)
		{
			J_Temp = 0;
		
			//�������ٴ���			
			Battery_temp[Battery_pointer] = LowPassFilter(ADCValue, ADC_BUFFER_SIZE, 0) & 0x0fff;
//			printf("%d\n", Battery_temp[Battery_pointer]);
			
			Battery_pointer++;
			
			if(Battery_pointer >= 10)
			{
				Battery_pointer = 0;
			}
			
			Battery_Power_Avg = FindMax(Battery_temp, 10);
			
			/*
					��ر��10.8 ~ 12.3V��ͨ��1M��270K�����ѹ��Ϊ2.30 ~ 2.61V��ADC����Ϊ 2854 ~ 3238
					��ص������ӵļ��㹫ʽΪ��1-(4.2-x)/(4.2-3.6)
			*/
			Battery_Power_Percent = 100 - (3238 - Battery_Power_Avg) * 100 / (3238 - 2854);
			
			if(Battery_Power_Percent <= 0)
			{
				Battery_Power_Percent = 0;
			}
			else if(Battery_Power_Percent >= 100)
			{
				Battery_Power_Percent = 100;
			}			
			
//			printf("%d\n", Battery_Power_Avg);
		}

		
		//��ť��ֵӳ��
		CountKnobValue();
		//�¶���ƽ�����
		count_angle_speed_right();
		count_angle_speed_left();		
		
		vTaskDelay(10);
	}
}


/*-------------------------------------------------------------------------
	��������LowPassFilter
	��  �ܣ����ֵ�Ƶ�˲�
	��  ����buffer 	���˲����飨��ά��
					length	���鳤��
					subpara ��ά����ڶ�ά������
	����ֵ���޷���16λ�˲�ֵ
-------------------------------------------------------------------------*/
uint16_t LowPassFilter(uint16_t buffer[ADC_BUFFER_SIZE][6], uint16_t length, uint8_t subpara)
{
	uint8_t t1 = 0;
	uint16_t t, temp;

	do			//��������������
	{	
		t1 = 0;	
	  
		for(t = 0; t < length - 1; t++)
		{
			//��������������
			if((buffer[t][subpara] & 0x0fff) > (buffer[t+1][subpara] & 0x0fff))	//��������
			{
				temp = buffer[t+1][subpara] & 0x0fff;
				buffer[t+1][subpara] = buffer[t][subpara] & 0x0fff;
				buffer[t][subpara] = temp;
				t1 = 1; 
			} 		
		}
	}while(t1); 
	
	return ((buffer[(length >> 1) - 1][subpara] + buffer[(length >> 1)][subpara] + buffer[(length >> 1) - 2][subpara] + buffer[(length >> 1) + 1][subpara]) >> 2) & 0x0fff;		
}


/*-------------------------------------------------------------------------
	��������FindMax
	��  �ܣ��������ֵ
	��  ����buffer 	����������
					length	���鳤��
	����ֵ���޷���16λ���ֵ
-------------------------------------------------------------------------*/
uint16_t FindMax(uint16_t * buffer, uint16_t length)
{
	uint16_t temp;
	uint8_t i;
	
	temp = buffer[0] & 0x0fff;
	
	for(i = 0; i < length; i++)
	{
		if((buffer[i] & 0x0fff) >= temp)
		{
			temp = buffer[i] & 0x0fff;
		}
	}
	
	return temp;
}


/*-------------------------------------------------------------------------
	��������CountKnobValue
	��  �ܣ���ť��ֵӳ��
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void CountKnobValue(void)
{
	int tempvalue = Knob_Avg;
	
	if(tempvalue >= MIN_KNOB_VALUE)
	{
		if(tempvalue > MAX_KNOB_VALUE)
		{
			tempvalue = MAX_KNOB_VALUE;
		}
		
		KnobValue = 2000 + (tempvalue - MIN_KNOB_VALUE) * 2000 / (MAX_KNOB_VALUE - MIN_KNOB_VALUE);
		SpeedFactor = (tempvalue - MIN_KNOB_VALUE) * 100 / (MAX_KNOB_VALUE - MIN_KNOB_VALUE);
	}
	else 
	{
		KnobValue = 2000;//ֹͣ
		SpeedFactor = 0;
	}
}


/*-------------------------------------------------------------------------
	��������count_angle_speed_right
	��  �ܣ���ҡ�˶�����ٶ�ӳ��
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void count_angle_speed_right(void)
{
	u8 debugbuffer[32];
	
	int speed;
	//double doublespeed;
	
	double angle;

	//��ҡ�˶��ӳ��--------------------------------------------------------------------------------------------------		
//	xx = ADC.ADC_2 - ADC.ADC_Middle_2;						//RJX		
	xx = ((int)(Right_Joystick_X_axis_Avg)) - DEFAULT_MIDDLE_VALUE;
	/*
	if(ADC.ADC_2 >= ADC.ADC_Middle_2)
	{
		xx = ADC.ADC_2 - ADC.ADC_Middle_2;//����ҡ��
	}
	else 
	{
		xx = -(ADC.ADC_Middle_2 - ADC.ADC_2);//����ҡ��
	}
	*/
	if(xx > 2000)
		xx = 2000;
	else if(xx < -2000)
		xx = -2000;

	//���ö��
	if(xx > adcOffset || xx < -adcOffset){
		//angle = 1500 + xx/4;//���1000-2000, ����1500��λ
		//angle = GlobalVariable.RudderMiddle*10 + xx/4;//���1000-2000, ����1500��λ
		//angle = GlobalVariable.RudderMiddle*10 + (xx-adcOffset)/4;//���1000-2000, ����1500��λ
		angle = (xx - adcOffset) / 4;
		if(xx > adcOffset)
			angle = angle * angle / 500;
		else
			angle = -angle * angle / 500;
		if(local_para_conf[2] != USV_Boat_Type_ME300)
		{
			angle = local_para_conf[3] * 10 + angle;		//���1000-2000, ����1500��λ
		}
		else
		{
			angle = local_para_conf[3] * 10 - angle;		//���1000-2000, ����1500��λ
		}
	}
	else {
		angle = local_para_conf[3] * 10;
	}
	RightRudderCommand = angle;
	//��ҡ�˶��ӳ��==================================================================================================
	
	
	//��ҡ���ٶ�ӳ��--------------------------------------------------------------------------------------------------
//	yy = ADC.ADC_3 - ADC.ADC_Middle_3;//ǰ��ҡ��
	yy = ((int)(Right_Joystick_Y_axis_Avg)) - DEFAULT_MIDDLE_VALUE;	
	
	if((yy > adcOffset || yy < -adcOffset)|| (xx > adcOffset || xx < -adcOffset)){
		//�����ٶ�ֵ,ͨ��ƽ��������,���ӵ��ٵļ��ٶ�,�����ٵļ��ٶ�//2000��ƽ����Ϊ44.7; �����45��������2000�����
		//doublespeed = sqrt(sqrt(yy))*303.03;
		//doublespeed = sqrt(sqrt(sqrt(yy*yy+xx*xx)))*303;
		//doublespeed = sqrt(yy)*44;
		doublespeed = sqrt(sqrt(yy*yy+xx*xx))*44;

		//��������ֵ���ɺܴ���޷�������,����ȷ��С��2000
		if(doublespeed > 2000)
		{
			doublespeed = 2000;
		}
		
		if(doublespeed >= adcOffset)
		{
			//doublespeed = (doublespeed - adcOffset) / 2;
				
			if(ControllerType == CONTROLLER_TYPE_2)
			{
				switch(local_para_conf[2])
				{
					case USV_Boat_Type_MC70://��ť������ˮ
					case USV_Boat_Type_MC120://��ť������ˮ
					case USV_Boat_Type_ME300://��ť��������
						if(yy >= 0){
							//speed = 3000 + doublespeed/2;//50%���ϲſ���ת��
							speed = 2000 + doublespeed;
						}
						else {
							//speed = 1000 - doublespeed/2;//50%���ϲſ���ת��
							speed = 2000 - doublespeed;
						}
						break;
					default://20141114��ť��������
						if(SpeedFactor > 0)
						{
							doublespeed = doublespeed * SpeedFactor / 100;
							if(yy >= 0){
								//speed = 3000 + doublespeed;//50%���ϲſ���ת��
								speed = 2000 + doublespeed;
							}
							else {
								//speed = 1000 - doublespeed;//50%���ϲſ���ת��
								speed = 2000 - doublespeed;
							}
						}
						else {
							speed = 2000;
						}
						break;
				}
			}
			else 
			{
				if(yy >= 0){
					//speed = 3000 + doublespeed/2;//50%���ϲſ���ת��
					speed = 2000 + doublespeed;
				}
				else {
					//speed = 1000 - doublespeed/2;//50%���ϲſ���ת��
					speed = 2000 - doublespeed;
				}
			}
		}
		else {
			speed = 2000;
		}
	}
	else {
			speed = 2000;
	}

	//�����ٶ�ֵ
	RightSpeedCommand = speed;//�ƽ���0-2000,ͣ2000,��2000-4000, ��ת���0, ��ת���4000
	//��ҡ���ٶ�ӳ��==================================================================================================
}


/*-------------------------------------------------------------------------
	��������count_angle_speed_left
	��  �ܣ���ҡ�˶�����ٶ�ӳ��
	��  ����
	����ֵ��
-------------------------------------------------------------------------*/
void count_angle_speed_left(void)
{
	u8 debugbuffer[32];
	
	int speed;
	//double doublespeed;
	
	double angle;

	//��ҡ�˶��ӳ��--------------------------------------------------------------------------------------------------	
//	xx = ADC.ADC_0 - ADC.ADC_Middle_0;
	xx = ((int)(Left_Joystick_X_axis_Avg)) - DEFAULT_MIDDLE_VALUE;
	/*
	if(ADC.ADC_0 >= ADC.ADC_Middle_0)
	{
		xx = ADC.ADC_0 - ADC.ADC_Middle_0;//����ҡ��
	}
	else 
	{
		xx = -(ADC.ADC_Middle_0 - ADC.ADC_0);//����ҡ��
	}
	*/
	
	if(xx>2000)
		xx = 2000;
	else if(xx<-2000)
		xx = -2000;

	//���ö��
	if(xx > adcOffset || xx < -adcOffset){
		//angle = 1500 + xx/4;//���1000-2000, ����1500��λ
		//angle = GlobalVariable.RudderMiddle*10 + xx/4;//���1000-2000, ����1500��λ
		//angle = GlobalVariable.RudderMiddle*10 + (xx-adcOffset)/4;//���1000-2000, ����1500��λ
		angle = (xx-adcOffset)/4;
		if(xx > adcOffset)
			angle = angle*angle/500;
		else
			angle = -angle*angle/500;
		if(local_para_conf[2] != USV_Boat_Type_ME300)
		{
			angle = local_para_conf[3] * 10 + angle;					//���1000-2000, ����1500��λ
		}
		else
		{
			angle = local_para_conf[3] * 10 - angle;			//���1000-2000, ����1500��λ
		}
	}
	else {
		angle = local_para_conf[3] * 10;
	}
	LeftRudderCommand = angle;
	//��ҡ�˶��ӳ��==================================================================================================
	
	
	//��ҡ���ٶ�ӳ��--------------------------------------------------------------------------------------------------
//	yy = ADC.ADC_1 - ADC.ADC_Middle_1;//ǰ��ҡ��
	yy = ((int)(Left_Joystick_Y_axis_Avg)) - DEFAULT_MIDDLE_VALUE;
	if((yy > adcOffset || yy < -adcOffset) || (xx > adcOffset || xx < -adcOffset)){
		//�����ٶ�ֵ,ͨ��ƽ��������,���ӵ��ٵļ��ٶ�,�����ٵļ��ٶ�//2000��ƽ����Ϊ44.7; �����45��������2000�����
		//doublespeed = sqrt(sqrt(yy))*303.03;
		//doublespeed = sqrt(sqrt(sqrt(yy*yy+xx*xx)))*303;
		//doublespeed = sqrt(yy)*44;
		doublespeed = sqrt(sqrt(yy*yy+xx*xx))*44;

		//��������ֵ���ɺܴ���޷�������,����ȷ��С��2000
		if(doublespeed>2000)
		{
			doublespeed = 2000;
		}
		
		if(doublespeed >= adcOffset)
		{
			//doublespeed = (doublespeed - adcOffset) / 2;
				
			if(ControllerType == CONTROLLER_TYPE_2)
			{
				switch(local_para_conf[2])
				{
					case USV_Boat_Type_MC70://��ť������ˮ
					case USV_Boat_Type_MC120://��ť������ˮ
					case USV_Boat_Type_ME300://��ť��������
						if(yy >= 0){
							//speed = 3000 + doublespeed/2;//50%���ϲſ���ת��
							speed = 2000 + doublespeed;
						}
						else {
							//speed = 1000 - doublespeed/2;//50%���ϲſ���ת��
							speed = 2000 - doublespeed;
						}
						break;
					default://20141114��ť��������
						if(SpeedFactor > 0)
						{
							doublespeed = doublespeed * SpeedFactor / 100;
							if(yy >= 0){
								//speed = 3000 + doublespeed;//50%���ϲſ���ת��
								speed = 2000 + doublespeed;
							}
							else {
								//speed = 1000 - doublespeed;//50%���ϲſ���ת��
								speed = 2000 - doublespeed;
							}
						}
						else {
							speed = 2000;
						}
						break;
				}
			}
			else 
			{
				if(yy >= 0){
					//speed = 3000 + doublespeed/2;//50%���ϲſ���ת��
					speed = 2000 + doublespeed;
				}
				else {
					//speed = 1000 - doublespeed/2;//50%���ϲſ���ת��
					speed = 2000 - doublespeed;
				}
			}
		}
		else {
			speed = 2000;
		}
	}
	else {
			speed = 2000;
	}
	//�����ٶ�ֵ
	LeftSpeedCommand = speed;			//�ƽ���0-2000,ͣ2000,��2000-4000, ��ת���0, ��ת���4000
	//��ҡ���ٶ�ӳ��==================================================================================================
}
