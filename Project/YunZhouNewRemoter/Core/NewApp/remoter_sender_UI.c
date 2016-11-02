#include "stm32f4xx.h"
#include "remoter_sender_UI.h"
#include "GUI.h"
#include "remoter_sender_RF.h"
#include "remoter_sender_jostick.h"
#include "bsp.h"
#include "PictureResource.h"
#include "bsp_lcd_backlight_pwm_timer10.h"
#include "bsp_buzzer_pwm.h"
#include "cpu_utils.h"


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

#define LAYER1_SMALL_DIALOG_X 100
#define LAYER1_SMALL_DIALOG_Y 100


xTaskHandle remoter_sender_UI_Task_Handle;
GUI_MEMDEV_Handle layer0_textbox_memdev, layer1_dialog_memdev;
char _log_buffer[128];

remoter_sender_jostick_t *jostick;
xtend900_config_t * rf_config;


char _ui_last_layer = 0; 




void _control_backlight( unsigned char on1off0)
{
	if( on1off0 == 1)
		bsp_lcd_backlight_pwm_set_level(10);//10%
	else
		bsp_lcd_backlight_pwm_set_level(0);//0%
}









//*****************   user app

void show_warnning_setting(void)
{

	//clear 
	if( _ui_last_layer != 1 )
	{
		GUI_SelectLayer(_ui_last_layer);
		GUI_Clear();	
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_SetBkColor(GUI_TRANSPARENT);
	}
	_ui_last_layer = 1;
		
	GUI_SelectLayer(1);
	GUI_MEMDEV_Select(layer1_dialog_memdev);	
	GUI_MEMDEV_Clear(layer1_dialog_memdev);	
	GUI_SetColor(GUI_TRANSPARENT);		
	GUI_FillRect(0, 0, LAYER1_SMALL_DIALOG_X, LAYER1_SMALL_DIALOG_Y);	
			
	GUI_BMP_Draw(_warningPic, 20, 0);											//感叹号图标
		
	GUI_SetColor(GUI_YELLOW);
	GUI_SetBkColor(GUI_TRANSPARENT);	
								
	GUI_DispStringAt("Setting", 20, 60);	
																			
		
	GUI_MEMDEV_CopyToLCDAt(layer1_dialog_memdev, 200, 120);
}


void show_jostick_status()
{
	
	//clear 
	if( _ui_last_layer != 0 )
	{
		GUI_SelectLayer(_ui_last_layer);
		GUI_Clear();	
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_SetBkColor(GUI_TRANSPARENT);
	}
	_ui_last_layer = 0;
	
	GUI_SelectLayer(0);
	GUI_MEMDEV_Select(layer0_textbox_memdev);	
			
// start draw 
	GUI_SetBkColor(GUI_TRANSPARENT);		
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetFont(&GUI_Font8x16_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	GUI_DispStringAt("mode  ", 0, 20);GUI_DispDec(jostick->key_mode,1);
	GUI_DispStringAt("sample  ", 0, 40);GUI_DispDec(jostick->key_sample,1);
	GUI_DispStringAt("alarm  ", 0, 60);GUI_DispDec(jostick->key_alarm,1);
	GUI_DispStringAt("Left-x  ", 0, 80);GUI_DispDec(jostick->left_x,4);
	GUI_DispStringAt("Left-y  ", 0, 100);GUI_DispDec(jostick->left_y,4);
	GUI_DispStringAt("Right-x  ", 0, 120);GUI_DispDec(jostick->right_x,4);
	GUI_DispStringAt("Right-y  ", 0, 140);GUI_DispDec(jostick->right_y,4);
	GUI_DispStringAt("Knob  ", 0, 160);GUI_DispDec(jostick->knob,4);
	GUI_DispStringAt("Power  ", 0, 180);GUI_DispDec(bsp_Get_Battery_Level(),4);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 250, 0);
	
	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	GUI_DispStringAt("boat_power   ", 0, 20);GUI_DispDec(remoter_sender_RF_get_boat_powerLevel(),4);
	GUI_DispStringAt("channel  ",0 , 50);GUI_DispDec(rf_config->hp,4);
	GUI_DispStringAt("xtend_id  ",0 , 65);GUI_DispDec(rf_config->id,8);
	GUI_DispStringAt("RSSI  ",0 , 80);GUI_DispDec(rf_config->id,8);
	GUI_DispStringAt("decode_rate  ",0 , 95);GUI_DispDec(rf_config->id,8);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 50, 0);
	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	GUI_DispStringAt("log:",0 , 10);
	sprintf(_log_buffer,"xtend_id=%04x\n",rf_config->id);
	GUI_DispStringAt(_log_buffer,0,30);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 20, 150);
	
}

void _ui_check_layer()
{
	if( _ui_last_layer != 0 )
	{
		GUI_SelectLayer(_ui_last_layer);
		GUI_Clear();	
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_SetBkColor(GUI_TRANSPARENT);
	}
	_ui_last_layer = 0;
	GUI_SelectLayer(0);
}

void show_jostick_status_text()
{
	int x, y;
	char strBuffer[128];
	
	//clear 
	_ui_check_layer();
			
// start draw 
	GUI_SetBkColor(GUI_TRANSPARENT);		
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetFont(&GUI_Font8x16_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_MEMDEV_Select(layer0_textbox_memdev);	
	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	x= 0; y=20;
	GUI_DispStringAt("mode     ", x, y);GUI_DispDec(jostick->key_mode,1);
	GUI_DispStringAt("sample   ", x, y+20);GUI_DispDec(jostick->key_sample,1);
	GUI_DispStringAt("alarm    ", x, y+40);GUI_DispDec(jostick->key_alarm,1);
	GUI_DispStringAt("Left-x   ", x, y+60);GUI_DispDec(jostick->left_x,4);
	GUI_DispStringAt("Left-y   ", x, y+80);GUI_DispDec(jostick->left_y,4);
	GUI_DispStringAt("Right-x  ", x, y+100);GUI_DispDec(jostick->right_x,4);
	GUI_DispStringAt("Right-y  ", x, y+120);GUI_DispDec(jostick->right_y,4);
	GUI_DispStringAt("Knob     ", x, y+140);GUI_DispDec(jostick->knob,4);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 40, 20);
	

	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	x = 0; y = 20;
	GUI_DispStringAt("------ Wireless ---------", x, y);
	sprintf(strBuffer,"Channel      %d",rf_config->hp);
	GUI_DispStringAt(strBuffer ,x , y+15);
	sprintf(strBuffer,"xtend_id     %d",rf_config->id);
	GUI_DispStringAt(strBuffer ,x , y+30);
	sprintf(strBuffer,"RSSI         %d -- %d ",xtend900_get_rssi(),xtend900_get_rssi_level() );
	GUI_DispStringAt(strBuffer,x , y+45);
	sprintf(strBuffer,"decode_rate  %d",remoter_sender_RF_getDecodeRate() );
	GUI_DispStringAt(strBuffer,x , y+60);
	GUI_DispStringAt("Status       ",x , y+75);
	switch(remoter_sender_RF_getMode())
	{
		case _RF_NORMAL_MODE:
		{
			if( _RF_STATUS_OK == remoter_sender_RF_getStatus() ) GUI_DispString("OK               ");
			else if( _RF_STATUS_LOAD_PARAM_ERROR == remoter_sender_RF_getStatus() ) GUI_DispString("Load Param Error");
			else if( _RF_STATUS_SAVE_PARAM_ERROR == remoter_sender_RF_getStatus() ) GUI_DispString("Save Param Error");
			break;
		}
		case _RF_SETTING_MODE:
		{
			GUI_DispString("Saving Param ...");
			break;
		}
		case _RF_LOADING_MODE:
		{
			GUI_DispString("Loading Param ...");
			break;
		}
		default:
			break;
	}
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 200, 20);

	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	x = 0; y = 20;
	GUI_DispStringAt("------ Power --------", x, y);
	sprintf(strBuffer,"Remoter   %d",bsp_Get_Battery_Level());
	GUI_DispStringAt(strBuffer, x, y+20);
	sprintf(strBuffer,"Boat      %d",remoter_sender_RF_get_boat_powerLevel());
	GUI_DispStringAt(strBuffer, x, y+40);
	sprintf(strBuffer,"Cpu       %d",FreeRTOS_GetCPUUsage());
	GUI_DispStringAt(strBuffer, x, y+60);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 200, 120);
	
	x = 0; y= 20;
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	GUI_DispStringAt("------ Log   ---------",x , y);
	GUI_DispStringAt(" ",x, y+15);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 200, 200);
}


void remoter_sender_UI_setLog(char *str)
{
	int	x = 0, y= 20;
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	GUI_DispStringAt(_log_buffer,x, y+15);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 200, 200);
}


void do_test()
{
	static int count=50;
	static int last_status=0;
	
#if 0
	if(jostick->left_y>3000)count=916;           //do
	else if(jostick->left_y<1000)count=946;      //re
	else if(jostick->left_x<1000)count=976;      //mi
	else if(jostick->right_y>3000)count=1006;    //fa
	else if(jostick->right_y<1000)count=1036;    //so
	else if(jostick->right_x>3000)count=1076;    //la
	else if(jostick->right_x<1000)count=1106;    //xi
	last_status = jostick->left_y>3000 | jostick->left_y<1000 | jostick->left_x<1000 
             | jostick->right_y>3000 | jostick->right_y<1000 | jostick->right_x>3000 
						 | jostick->right_x<1000;
	 if(last_status==1)	bsp_buzzer_pwm_set_voice(count,50);
	else bsp_buzzer_pwm_set_voice(count,0);
#endif
	
	if( last_status== 0 &&jostick->button_ok == 1  )
	{
		count+=10;
		//bsp_buzzer_pwm_set_level(count);
		//bsp_buzzer_pwm_set_voice(count,50);
		bsp_lcd_backlight_pwm_set_level(count);
		printf("light level=%d\r\n",count);
	}
	if( last_status== 0 &&jostick->button_cancel == 1  )
	{
		count-=10;
		//bsp_buzzer_pwm_set_level(count);
		//bsp_buzzer_pwm_set_voice(count,50);
		bsp_lcd_backlight_pwm_set_level(count);
		printf("light level=%d\r\n",count);
	}
  last_status = jostick->button_ok | jostick->button_cancel ;

}

void remoter_sender_UI_Task(void * param)
{

	int x=0,y=0;
	int current_win_id = 0;
	
	
	/* Initialize the BSP layer */
  bsp_Gui_lcd_init();
 
	//init 
	GUI_Init();
	
	
	GUI_SelectLayer(1);
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_Clear();	
	layer1_dialog_memdev = GUI_MEMDEV_Create(0, 0, LAYER1_SMALL_DIALOG_X, LAYER1_SMALL_DIALOG_Y);
	
	GUI_SelectLayer(0);
	GUI_SetColor(GUI_TRANSPARENT);
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_Clear();	
	layer0_textbox_memdev = GUI_MEMDEV_Create(0, 0, LAYER0_TEXT_BOX_X, LAYER0_TEXT_BOX_Y);
	
	//防止闪屏，廷时再打开背光
	vTaskDelay(300);
	_control_backlight(ENABLE);	
	

	jostick = remoter_sender_jostick_get_data();
	rf_config = remoter_sender_RF_get_config();
	
	while(1)
	{	
		/*
		if( _RF_SETTING_MODE == remoter_sender_RF_getMode() )
		{
			current_win_id = 1;
		}else{
			current_win_id = 0;
		}*/
		
		
		switch( current_win_id )
		{
			case 0:
			{
				
				//show_jostick_status();
				show_jostick_status_text();
				break;
			}
			case 1:
			{
				show_warnning_setting();
				break;
			}
		}

		vTaskDelay(20);
		
		//do_test();
		
	}
	
}