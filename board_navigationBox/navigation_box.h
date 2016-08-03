#ifndef _NAVIGATION_BOX_H
#define _NAVIGATION_BOX_H

#include <stm32f10x.h>	
#include "stdio.h"	
#include "system.h"

typedef struct {
unsigned char bArray[8];
}DtoBsTypeDef;

typedef struct {
unsigned char headingBytes[2];	// 0~3600
unsigned char pitchBytes[2];	//-1800~1800
unsigned char rollBytes[2];	//-1800~1800
unsigned short heading;
short pitch;
short roll;
}CompassTypeDef;

typedef struct {
unsigned short X;
unsigned short Y;
unsigned short Z;
}TempCompassTypeDef;

typedef struct {
short X;
short Y;
short Z;
}AccTypeDef;

typedef struct {
bool Location;
char Date[10];	  
char Time[15];	 
char Latitude[20];
char Longitude[20];
char Speed[10];
char Course[10];
}GPSTypeDef;



bool HMC6343_Configuration(void);
bool HMC6343_Read(CompassTypeDef * compass);

void Navi_GPIO_Configuration (void);

void gps_event();
void gps_config(Uart_t* uart, GPSTypeDef* gps);

void navi_report();
bool HMC6343_Configuration(void);
	
extern CompassTypeDef compass_data;
extern GPSTypeDef gps_data;

#define KEY_GPIO_BANK GPIOB
#define KEY_GPIO_PIN GPIO_Pin_8


#endif