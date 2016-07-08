#ifndef _NAVIGATION_BOX_LED_H
#define _NAVIGATION_BOX_LED_H


#define GPS_LED_ID 0
#define COMPASS_LED_ID 1
#define LED_GPS_GPIO_PIN GPIO_Pin_10
#define LED_GPS_GPIO_BANK GPIOB
#define LED_COMPASS_GPIO_PIN GPIO_Pin_11
#define LED_COMPASS_GPIO_BANK GPIOB

void Nbl_Led_on(int id);
void Nbl_Led_off(int id);
//bool is_led_on(int id);
void Nbl_Led_toggle(int id);
void Nbl_Led_Configuration();
					



#endif //_NAVIGATION_BOX_LED_H