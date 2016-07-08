#include "stm32f10x.h"
#include "system.h"
u16 ADCConvert[50][3];



void ADC_Configuration (void)
{										 
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_239Cycles5);	  
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	
 
	// set ADC channel for temperature sensor
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_DMACmd(ADC1, ENABLE);  
	ADC_Cmd(ADC1, ENABLE);
 
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
 	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(ADC1->DR);	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 150;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADCConvert;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);

}

u16 Get_Temperature_Adc_value()
{
	int i;
	u32 sensor = 0; 		
	for(i=0;i<50;i++)
	{
		sensor += ADCConvert[i][0];
	}
	sensor = sensor/50;
	return sensor;
}
u16 Get_Voltage_Adc_value()
{
	int i;
	u32 sensor = 0; 		
	for(i=0;i<50;i++)
	{
		sensor += ADCConvert[i][2];
	}
	sensor = sensor/50;
	return sensor;
}
u16 Get_WaterLeakage_Adc_value()
{
	int i;
	u32 sensor = 0; 		
	for(i=0;i<50;i++)
	{
		sensor += ADCConvert[i][1];
	}
	sensor = sensor/50;
	return sensor;
}

//End of File
