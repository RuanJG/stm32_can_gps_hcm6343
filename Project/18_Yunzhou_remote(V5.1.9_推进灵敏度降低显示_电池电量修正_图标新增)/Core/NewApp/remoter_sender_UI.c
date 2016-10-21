#include "stm32f4xx.h"
#include "remoter_sender_UI.h"
#include "GUI.h"
#include "remoter_sender_RF.h"
#include "remoter_sender_jostick.h"
#include "bsp.h"



//***********  maybe need to port
#include "FreeRTOS.h"
#include "task.h"

//LCD x y 参数
#define LCD_SCREEN_X 480
#define LCD_SCREEN_Y 320
//layer0 memdev
#define LAYER0_SCREEN_X LCD_SCREEN_X  
#define LAYER0_SCREEN_Y LCD_SCREEN_Y
//layer1 debug text line memdev
#define LAYER0_TEXT_BOX_X 200  
#define LAYER0_TEXT_BOX_Y 200



xTaskHandle remoter_sender_UI_Task_Handle;
GUI_MEMDEV_Handle layer0_textbox_memdev, layer0_memdev;
char _log_buffer[128];








void LCD_PWM_Config(uint8_t status);
void _control_backlight( unsigned char on1off0)
{
	LCD_PWM_Config(on1off0);
}









//*****************   user app


void remoter_sender_UI_Task(void * param)
{
	remoter_sender_jostick_t *jostick;
	xtend900_config_t * rf_config;
	
	int x=0,y=0;
	
		/* Initialize the BSP layer */
  bsp_Gui_lcd_init();
	

	
	//init 
	GUI_Init();
	GUI_SetBkColor(GUI_TRANSPARENT);
	
  GUI_SelectLayer(1);
  GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT);
	
	GUI_SelectLayer(0);	
	GUI_Clear();
  GUI_SetBkColor(GUI_TRANSPARENT);
	
	layer0_textbox_memdev = GUI_MEMDEV_Create(0, 0, LAYER0_TEXT_BOX_X, LAYER0_TEXT_BOX_Y);
	//layer0_memdev = GUI_MEMDEV_Create(0, 0, LAYER0_SCREEN_X, LAYER0_SCREEN_Y);
	
	//防止闪屏，廷时再打开背光
	vTaskDelay(300);
	_control_backlight(ENABLE);	
	
	
	jostick = remoter_sender_jostick_get_data();
	rf_config = remoter_sender_get_rf_config();
	while(1)
	{
		//clear
		GUI_SelectLayer(0);
		//GUI_Clear();
		GUI_MEMDEV_Select(layer0_textbox_memdev);	
		GUI_MEMDEV_Clear(layer0_textbox_memdev);		
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_SetBkColor(GUI_TRANSPARENT);		

		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_SetFont(&GUI_Font8x16_ASCII);
		GUI_SetColor(GUI_WHITE);
		

		GUI_DispStringAt("mode  ", 0, 20);GUI_DispDec(jostick->key_mode,4);
		GUI_DispStringAt("sample  ", 0, 40);GUI_DispDec(jostick->key_sample,4);
		GUI_DispStringAt("alarm  ", 0, 60);GUI_DispDec(jostick->key_alarm,4);
		GUI_DispStringAt("Left-x  ", 0, 80);GUI_DispDec(jostick->left_x,4);
		GUI_DispStringAt("Left-y  ", 0, 100);GUI_DispDec(jostick->left_y,4);
		GUI_DispStringAt("Right-x  ", 0, 120);GUI_DispDec(jostick->right_x,4);
		GUI_DispStringAt("Right-y  ", 0, 140);GUI_DispDec(jostick->right_y,4);
		GUI_DispStringAt("Knob  ", 0, 160);GUI_DispDec(jostick->knob,4);
		GUI_DispStringAt("Power  ", 0, 180);GUI_DispDec(bsp_Get_Battery_Level(),4);
 
		GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 250, 0);
		GUI_MEMDEV_Clear(layer0_textbox_memdev);
		GUI_DispStringAt("power   ", 0, 20);GUI_DispDec(remoter_sender_get_boat_powerLevel(),4);
		GUI_DispStringAt("channel  ",0 , 40);GUI_DispDec(rf_config->hp,4);
		GUI_DispStringAt("xtend_id  ",0 , 60);GUI_DispDec(rf_config->id,8);
		GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 50, 0);
		
		GUI_MEMDEV_Clear(layer0_textbox_memdev);
		GUI_DispStringAt("log:",0 , 10);
		sprintf(_log_buffer,"xtend_id=%04x\n",rf_config->id);
		GUI_DispStringAt(_log_buffer,0,30);
		GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 20, 100);

		
		vTaskDelay(50);
		
	}
	
}