#ifndef _global_h_
#define _global_h_ 

#include <stm32f10x.h>	

#define BIT0 1
#define BIT1 (1<<1)	//1
#define BIT2 (1<<2)	//2
#define BIT3 (1<<3)	//4
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT7 (1<<7)

#define VALVE12_1SET   	GPIOB->BSRR = GPIO_Pin_12
#define VALVE12_1RESET  GPIOB->BRR  = GPIO_Pin_12
#define VALVE12_2SET   	GPIOB->BSRR = GPIO_Pin_13
#define VALVE12_2RESET 	GPIOB->BRR  = GPIO_Pin_13
#define VALVE34_1SET   	GPIOB->BSRR = GPIO_Pin_14
#define VALVE34_1RESET 	GPIOB->BRR  = GPIO_Pin_14
#define VALVE34_2SET   	GPIOB->BSRR = GPIO_Pin_15
#define VALVE34_2RESET 	GPIOB->BRR  = GPIO_Pin_15
#define ROD_DOWN_SET  	GPIOB->BSRR = GPIO_Pin_0
#define ROD_DOWN_RESET	GPIOB->BRR  = GPIO_Pin_0
#define ROD_UP_SET 		GPIOB->BSRR = GPIO_Pin_1
#define ROD_UP_RESET	GPIOB->BRR  = GPIO_Pin_1
#define CAM_PWR_SET		GPIOA->BSRR = GPIO_Pin_6
#define CAM_PWR_RESET	GPIOA->BRR  = GPIO_Pin_6
#define GPRS_PWR_SET	GPIOB->BSRR = GPIO_Pin_5
#define GPRS_PWR_RESET	GPIOB->BRR  = GPIO_Pin_5
#define SPEAKER_SET		GPIOA->BSRR = GPIO_Pin_4
#define SPEAKER_RESET	GPIOA->BRR  = GPIO_Pin_4
#define ALERT_LED_SET	GPIOA->BSRR = GPIO_Pin_5
#define ALERT_LED_RESET	GPIOA->BRR  = GPIO_Pin_5
#define US_LEFT_SET		GPIOA->BSRR = GPIO_Pin_9
#define US_LEFT_RESET	GPIOA->BRR  = GPIO_Pin_9
#define US_RIGHT_SET	GPIOB->BSRR = GPIO_Pin_9
#define US_RIGHT_RESET	GPIOB->BRR  = GPIO_Pin_9

#define USV_PROTOCOL_VERSION_1 1
#define USV_PROTOCOL_VERSION_2 2

typedef struct {
u8 bArray[8];
}DtoBsTypeDef;

typedef struct {
u8 bArray[4];
}FtoBsTypeDef;

typedef struct {
u8 heading[2];		// 0~3600
u8 pitch[2];		//-1800~1800
u8 roll[2];			//-1800~1800
}CompassTypeDef;

typedef struct {
s16 X;
s16 Y;
s16 Z;
}AccTypeDef;

typedef struct {
bool Location;
char Course[10];
u8 Date[3];			// year, month, day
u8 Time[3];			// hour, minute, second
u8 Latitude[8];		// bytes array of double
u8 Longitude[8];	// bytes array of double
u8 Speed[2];		// integer, 10 times of speed m/s
}GPSTypeDef;

typedef struct {
bool Location;
char Course[10];
u8 Date[3];			// year, month, day
u8 Time[3];			// hour, minute, second
u8 BD_Latitude[8];		// bytes array of double
u8 BD_Longitude[8];	// bytes array of double
u8 Speed[2];		// integer, 10 times of speed m/s
}BDTypeDef;

#define PARA_BASE_INDEX 	63//79
#define PARA_LIST_LEN 	30//15

typedef struct {
	u16 Voltage;
	u16 WaterLeakage;
	CompassTypeDef Compass;
	CompassTypeDef TempCompass;
	GPSTypeDef GPS;
	BDTypeDef BD;
	
	AccTypeDef Acc;
	s16 sensor[2];
	bool Test1;
	u8 speed;
	float tempInternal;
	u8 tempInternalByte;		//0~255, represent -40~215 degrees
	float tempWater;
	u8 tempWaterBytes[4];
	u16 UltraSonic[2];
	u16 PreviousUltraSonic[2];
	u16 PreviousPreviousUltraSonic[2];
	u16 US_After_Process[2];
	bool ParaList[PARA_LIST_LEN];//79-93:15
	u8 ParaListData[PARA_LIST_LEN*4];
	u16 SamplePipeLen;
}GlobalVariableTypeDef;

extern GlobalVariableTypeDef GlobalVariable;

typedef struct {
	u8 Latitude[8];
	u8 Longitude[8];
	u8 Pitch[2];
	u8 Roll[2];
	u16 Yaw;
	u8 Mode;
	u16 Status;
	u8 BatLife;
	u16 Ultra1;
	u16 Ultra2;
	u8 InternalTemp;
	u32 ErrorMessage;
	float WaterTemp;
	u8 IfLeaked;
}BoatDataTypeDef;
extern BoatDataTypeDef BoatData;

extern u8 Arm9Started;
extern u8 usv_protocol_version;

void reportToARM9(void);
void ReportToARM9Base64(void);


#define MAINCONTROLLER_COMMON 0
#define MAINCONTROLLER_ME300  1

extern int MainControllerType;

#endif
//End of File
