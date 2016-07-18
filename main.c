#include "STM32F10x.h"
#include <cmsis_os.h>
#include "GPIO_STM32F10x.h"

extern void myUART_Thread(void const *argument);
extern osThreadId tid_myUART_Thread;
osThreadDef(myUART_Thread, osPriorityNormal, 1, 0);

extern void can1_thread (void const *argument);
extern osThreadId tid_can1_Thread;
osThreadDef(can1_thread, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 Flash LED 2
 *---------------------------------------------------------------------------*/
#define LED_On() GPIO_PinWrite(GPIOB,10,0);//  (10 << 1);
#define LED_Off() GPIO_PinWrite(GPIOB,10,1);
void led_thread1 (void const *argument) 
{
	for (;;) 
	{
		LED_On();                          
		osDelay(1000);
		LED_Off();
		osDelay(1000);
	}
}

/*----------------------------------------------------------------------------
 Define the thread handles and thread parameters
 *---------------------------------------------------------------------------*/

osThreadId main_ID,led_ID1,led_ID2;	
osThreadDef(led_thread1, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 Initilise the LED's, get a handle for main, start the threads and terminate main
 *---------------------------------------------------------------------------*/

int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	GPIO_PinConfigure(GPIOB, 10,  GPIO_OUT_PUSH_PULL , GPIO_MODE_OUT50MHZ);
	led_ID1 = osThreadCreate(osThread(led_thread1), NULL);
	tid_myUART_Thread = osThreadCreate(osThread(myUART_Thread), NULL);
	tid_can1_Thread = osThreadCreate(osThread(can1_thread), NULL);
	osKernelStart ();                         // start thread execution 
	while(1)
	{
		;
	}
}