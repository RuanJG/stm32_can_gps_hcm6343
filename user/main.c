#include <stm32f10x.h>	
#include "global.h"	
#include "stdio.h"	
#include  <ctype.h>
#include "stm32f10x_tim.h"

#define byte unsigned char					
#define	DS_SKIP_ROM	0xCC
#define DS_CONVERT	0x44
#define DS_READ_PAD	0xBE
#define WRITE	1
#define READ	0	

char SoftwareVersion[] = "4.5.4";//20141127

int MainControllerType = MAINCONTROLLER_TYPE;


/*****declaration of functions ******************************************/
// local
void ExecuteCommand(u8 *commamnd, int commamnd_len);
void delay_us(u32 Nus);
double BytesToDouble(DtoBsTypeDef data);
FtoBsTypeDef FloatToBytes(float data);
float BytesToFloat(FtoBsTypeDef data);

// extern 
void SetClock(void); 
void NVIC_Configuration(void);
void GPIO_Configuration (void);	
void GPIO_Initialization(void);
void SysTick_Configuration(void);
void UART_Configuration (void);  
void ADC_Configuration (void);
void Timer_Configuration(void);
u8 CAN_Configuration(u8 FilterNumber, u16 ID, u16 ID_Mask);
u8 CAN_TX(CanTxMsg* TxMessage);

void PutUART(char ch);	  
void PutBytes (const char *buffer, int len);

void PutUART3(char ch);
void PutUART2(char ch);

void Read_DS18B20(void);
void ds_writebyte(u8 cmd);
void ds_reset(void);

/*****declaration of variables********************************************/
GlobalVariableTypeDef GlobalVariable;
BoatDataTypeDef BoatData;
//This is to store message received from navigation board

uint32_t ultrasound_timer;
uint8_t ultrasound_flag=0;

u32 sensor;
float tempf;
float voltage_mean;
float voltage_count;
float voltage_max;
extern u16 ADCConvert[50][3];
u8 time_flag = 0;			//count to 10 then measure temperature once
//extern bool RxEndFlag;      //  Ω” ’Ω· ¯±Í÷æ
//int CommandIndex = 0;
//u8 CommandLength = 0;
//uint16_t Command[30];

//chang by zh 
float ADCvoltage_meantable[30]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float ADCvoltage_meansum = 0.0;
float ADCvoltage_meanmean = 0.0;
char ADCvoltage_meancount=0;
char ADCmeanflag=0;


u8 usv_protocol_version = USV_PROTOCOL_VERSION_2;

u8 can_data_updating = 4;

int main(void)														 
{		
	u8 i = 0; 
	int delayCount = 0;
	
	
	SetClock ();				 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); 
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	   
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 

	NVIC_Configuration ();	
	GPIO_Configuration ();
	GPIO_Initialization();
	SysTick_Configuration ();
	ADC_Configuration ();
	UART_Configuration ();
	CAN_Configuration (0, 0x10, 0x1ff);	//0x10CANµÿ÷∑

	for (i = 0; i<PARA_LIST_LEN; ++i)
	{
		GlobalVariable.ParaList[i] = FALSE;
	}
	GPRS_PWR_SET;
	if(MainControllerType != MAINCONTROLLER_ME300)
	{
		//SPEAKER_SET;
		ALERT_LED_SET;//ME300◊¢ ÕµÙ
		delay_us(1000000);//ME300◊¢ ÕµÙ
		//delay_us(1000);
		ALERT_LED_RESET;//ME300◊¢ ÕµÙ
	}
//	Timer_Configuration();
	GlobalVariable.PreviousUltraSonic[0] = 1068;
	GlobalVariable.PreviousUltraSonic[1] = 1068;
	GlobalVariable.PreviousPreviousUltraSonic[0] = 1068;
	GlobalVariable.PreviousPreviousUltraSonic[1] = 1068;
//	TIM1->ARR = delay10ms;
//	TIM_Cmd(TIM1, ENABLE);
//	TIM_Cmd(TIM2, ENABLE);
//	GPIOA->BRR  = GPIO_Pin_8;   //pump active low
//	GPIOA->BSRR  = GPIO_Pin_8;   //pump active low
	US_LEFT_SET;
	US_RIGHT_SET;
	CAM_PWR_SET;
	VALVE12_1RESET;
	VALVE12_2RESET;
	VALVE34_1RESET;
	VALVE34_2RESET;
	
	while(1)
	{
		//PutUART3('3');
		
		//PutUART2('2');
		
		// Voltage convert 
		sensor = 0; 		
		for(i=0;i<50;i++)
		{
			sensor += ADCConvert[i][2];
		}
		//GlobalVariable.Voltage = sensor / 55;
		voltage_count = 0;
		//voltage_mean = sensor / 50;
		voltage_mean = sensor / 55;//2015.11.11monidifed


		//chang by zh
   ADCvoltage_meansum = 0.0;
	 ADCvoltage_meanmean = 0.0;
   ADCvoltage_meantable[ADCvoltage_meancount++]=voltage_mean;
   if(ADCvoltage_meancount==30)
	 {
		  ADCvoltage_meancount=0;
		  ADCmeanflag=1;
	 }
   for(i=0;i<30;i++)
	 {
			ADCvoltage_meansum+=ADCvoltage_meantable[i];
	 }
   ADCvoltage_meanmean = (ADCvoltage_meansum/30);
		
		
		sensor = 0; 
		voltage_max = 0;
		for(i=0;i<50;i++)
		{
			if(ADCConvert[i][2] >= ADCvoltage_meanmean)
			{
				sensor += ADCConvert[i][2];
				voltage_count++;
			}
			if(ADCConvert[i][2] > voltage_max)
			{
				voltage_max = ADCConvert[i][2];
			}
		}

		//if(voltage_count > 0)
		//	GlobalVariable.Voltage = sensor / voltage_count;
		//else
		//	GlobalVariable.Voltage = voltage_mean;
		//2015.11.11deleted
		//if(GlobalVariable.Voltage > 0)
		//	GlobalVariable.Voltage = (voltage_max + GlobalVariable.Voltage)/2;
		//else 
		//	GlobalVariable.Voltage = voltage_max;
		//2015.11.11added
		
		if(ADCmeanflag == 0)
			GlobalVariable.Voltage = ((sensor*100) / (voltage_count*110));
		else
			GlobalVariable.Voltage = ADCvoltage_meanmean;
		 																
		// use ADC1 channel_16 + DMA1 to read temperature sensor
		sensor = 0; 		
		for(i=0;i<50;i++)
		{
			sensor += ADCConvert[i][0];
		}
		tempf = sensor/50.0f;
		tempf = (float)(tempf*3.3f/4096.0f);
		GlobalVariable.tempInternal = (1.43f-tempf)/0.0043f + 25.0f + 40;
		GlobalVariable.tempInternalByte = (u8)((int)GlobalVariable.tempInternal);	

		//Check water leakage
		sensor = 0; 		
		for(i=0;i<50;i++)
		{
			sensor += ADCConvert[i][1];
		}
		GlobalVariable.WaterLeakage = sensor / 50;

		/*
		*/
		//while(can_data_updating < 4 && ++delayCount < 100)
		//{
		//	delay_us(1000);
		//}
		
	  // report via UART to ARM9
		//if (TRUE)
		switch(usv_protocol_version)
		{
			case USV_PROTOCOL_VERSION_1:
				reportToARM9();
				break;
			case USV_PROTOCOL_VERSION_2:
				ReportToARM9Base64();
				break;
			default:
				break;
		}
		
		// read temperature from the sensor, at frequency 1/8 of that of sending to ARM9
		//100000us=100ms=0.1s
		delay_us(100000);
		//delay_us((100-delayCount) * 1000);
		delayCount = 0;
		
		if (time_flag==12)
		{
			//mainCounter++;
			Read_DS18B20();
			time_flag=0;
		}
		time_flag++;
				
		
			if(ultrasound_flag == 0)
			{
				ultrasound_flag++;				
				US_LEFT_RESET;
				US_RIGHT_SET;

			}		
			else if(ultrasound_flag == 1)
			{
				ultrasound_flag++;			
				US_LEFT_RESET;
				US_RIGHT_RESET;

			}			
			else if(ultrasound_flag == 2)
			{
				ultrasound_flag++;				
				US_LEFT_SET;
				US_RIGHT_RESET;
				
			}			
			else
			{
				ultrasound_flag = 0;				
				US_LEFT_RESET;
				US_RIGHT_RESET;
				
			}

	}	//end of while 1
} //end of main



/*****definition of local functions ******************************************/
/**
 * @brief  decode and execute the commands sent by ARM9 via UART
 * @param  None
 * @return None
 */
 
uint16_t seroCommand;
uint16_t properCommand;

int speekerstatus = 0;
int alertledstatus = 0;

void DriverLED()
{
	if(alertledstatus ==0)
	{
		alertledstatus = 1;
		ALERT_LED_SET;
	}
	else {
		alertledstatus = 0;
		ALERT_LED_RESET;
	}
}

/**
 * @brief  Use systick to delay for microseconds
 *
 * @param  number of microseconds to delay u32
 * @return None
 *
 */
//void delay_us(u32 Nus){  
//	SysTick->LOAD=Nus*9;          
//	SysTick->CTRL|=0x01;          
//	while(!(SysTick->CTRL&(1<<16)));
//	SysTick->CTRL=0X00000000;       
//	SysTick->VAL=0X00000000;        
//}
//void SysTick_Handler (void)
//{	
//}

/**
 * @brief  convert byte array to double
 *
 * @param  name of the byte array
 * @return a double data
 *
 */
double BytesToDouble(DtoBsTypeDef data){ 
    double a = 0; 
    u8 ii; 
    void *pf; 
    byte* px = data.bArray; 
    pf = &a; 
    for(ii=0;ii<8;ii++) 
    { 
        *((byte *)pf+ii)=*(px+ii); 
    } 
    return a; 
}
 

/**
 * @brief convert float to byte array[4]
 *
 * @param  float data
 * @return byte array
 *
 */
FtoBsTypeDef FloatToBytes(float data){ 
    byte *pdata = (byte*)&data; 
    FtoBsTypeDef byteArray;
	int ii;
    for (ii = 0; ii < 4; ++ii) 
        byteArray.bArray[ii] = *pdata++; 
    return byteArray; 
}

/**
 * @brief  convert byte array [4] to float
 *
 * @param  name of the byte array
 * @return a float data
 *
 */
float BytesToFloat(FtoBsTypeDef data){ 
    float a = 0; 
    u8 ii; 
    void *pf; 
    byte* px = data.bArray; 
    pf = &a; 
    for(ii=0;ii<4;ii++) 
    { 
        *((byte *)pf+ii)=*(px+ii); 
    } 
    return a; 
} 

//void FromByteArrayToBoatData(u8* bytearray)
//{
//   u8 count = 0;
//   u16 startindex = 0;
//   while (count<=30)
//   {
//   		switch (bytearray[startindex])
//		{
//			case 1:
//	            {
//	                //result.GpsData.Latitude = BitConverter.ToDouble(bytearray, startindex + 1);
//	                startindex += 8 + 1;
//	                break;
//	            }
//	        //GPS Longitude
//	        case 2:
//	            {
//	                //result.GpsData.Longitude = BitConverter.ToDouble(bytearray, startindex + 1);
//	                startindex += 8 + 1;
//	                break;
//	            }
//	        case 3:
//	            {
//	                //result.GpsData.Speed = (float)(((int)bytearray[startindex + 1] * 256 + (int)bytearray[startindex + 2]) / 10.0);
//	                startindex += 2 + 1;
//	                break;
//	            }
//	        case 4:      //GPS time 3 byte
////	            result.USVTimeData.Hour = bytearray[startindex + 1];
////	            result.USVTimeData.Minute = bytearray[startindex + 2];
////	            result.USVTimeData.Second = bytearray[startindex + 3];
//	            startindex += 3 + 1;
//	            break;
//	        case 5:      //GPS date 3 byte
////	            result.USVDateData.Year = bytearray[startindex + 1];
////	            result.USVDateData.Month = bytearray[startindex + 2];
////	            result.USVDateData.Day = bytearray[startindex + 3];
//	            startindex += 3 + 1;
//	            break;
//	        case 6:      //remaining battery 1 byte
//	            BoatData.BatLife =  bytearray[startindex + 1];
//	            startindex += 1 + 1;
//	            break;
//	        case 10:    // pitch 2 byte
//	            //result.TiltData.Pitch = (float)(((Int16)(bytearray[startindex + 1] * 256 + (int)bytearray[startindex + 2])) / 10.0f);
//	            startindex += 2 + 1;
//	            break;
//	        case 11:    //roll 2 byte
//	            //result.TiltData.Roll = (float)(((Int16)(bytearray[startindex + 1] * 256 + (int)bytearray[startindex + 2])) / 10.0f);
//	            startindex += 2 + 1;
//	            break;
//	        case 12:    //yaw 2 byte
//	            //float tempfloat = (float)(((Int16)(bytearray[startindex + 1] * 256 + (int)bytearray[startindex + 2])) / 10.0f);                    
//	            BoatData.Yaw = (u16)((u16)bytearray[startindex + 1] * 256 + (u16)bytearray[startindex + 2]);                        
//	            startindex += 2 + 1;
//	            break;
//	        case 14:
//	            {
//	                if (bytearray[startindex + 1] == 65)
//					{
//						BoatData.Mode = 'A';
//					}
//	                    //result.RobotInfoData.CurrentMode = Mode.Auto;
//	                else if (bytearray[startindex + 1] == 77)
//					{
//						BoatData.Mode = 'M';
//					}
//	                    //result.RobotInfoData.CurrentMode = Mode.ManualRF;
//	                startindex += 1 + 1;
//	                break;
//	            }
//	        case 15:
//	            {
//	                BoatData.Status = (u32)((u32)bytearray[startindex + 1] * 256 + (u32)bytearray[startindex + 2]);
//	                startindex += 2 + 1;
//	                break;
//	            }
//	        case 17:
//	            {
//	                //result.UltraSonicData.Sensor1Distance = ((int)(bytearray[startindex + 1]) * 256) + ((int)bytearray[startindex + 2]);
//	                startindex += 2 + 1;
//	                break;
//	            }
//	        case 18:
//	            {
//	                //result.UltraSonicData.Sensor2Distance = ((int)(bytearray[startindex + 1]) * 256) + ((int)bytearray[startindex + 2]);
//	                startindex += 2 + 1;
//	                break;
//	            }
//	        case 19:    //board temperature 1byte
//	            //result.RobotInfoData.Temperature = (int)(bytearray[startindex + 1]) - 40;
//				BoatData.InternalTemp = (bytearray[startindex + 1]) - 40;
//	            startindex += 1 + 1;
//	            break;
//	        case 20:
//	            {
//	                //result.RobotInfoData.ErrorMessage = (BoatError)BitConverter.ToInt32(bytearray, startindex + 1);
//	                startindex += 1 + 4;
//	                break;
//	            }
//	        case 81:
//	            {
//	                //result.WaterParaData.Temperature = (float)Math.Round(BitConverter.ToSingle(bytearray, startindex + 1), 2);
//	                //BoatData.WaterTemp
//					startindex += 4 + 1;
//	                break;
//	            }
//	        case 35:
//	            {
//	                return;
//	            }
//	        default:
//	            break;
//	    }
//	    count++;
//
//   }
//}	
//End of file


 			  
