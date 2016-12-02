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

void _ui_check_layer(int num)
{
	if( _ui_last_layer != num )
	{
		GUI_SelectLayer(_ui_last_layer);
		GUI_Clear();	
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_SetBkColor(GUI_TRANSPARENT);
	}
	_ui_last_layer = num;
	GUI_SelectLayer(num);
}







//*****************   user app

void show_warnning_setting(void)
{

	//clear 
	_ui_check_layer(1);
		
	
	//start draw
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


void show_logo_and_initing(void)
{

	static int counter = 0;
	int logo_display_Ms = 0;
	GUI_MEMDEV_Handle logo_memdev;
	
	//clear 
	_ui_check_layer(1);
		
	logo_memdev = GUI_MEMDEV_Create(0, 0, 200, 200);

	GUI_MEMDEV_Clear(logo_memdev);		
	GUI_MEMDEV_Select(logo_memdev );
	GUI_SetBkColor(GUI_TRANSPARENT);
	GUI_SetColor(GUI_TRANSPARENT);	
	GUI_BMP_DrawScaled(_logo_big,180,100,2,1);
	GUI_MEMDEV_CopyToLCDAt(logo_memdev, 0, 0);

	
	while( remoter_sender_RF_getMode() != _RF_NORMAL_MODE  && logo_display_Ms< 4000 )
	{
		vTaskDelay(20);
		logo_display_Ms += 20;
	}
	GUI_MEMDEV_Delete(logo_memdev);
}



void show_jostick_status_text()
{
	int x, y;
	char strBuffer[128];
	
	//clear 
	_ui_check_layer(0);
			
// start draw 
	GUI_SetBkColor(GUI_TRANSPARENT);		
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_SetFont(&GUI_Font8x16_ASCII);
	GUI_SetColor(GUI_WHITE);
	
	GUI_MEMDEV_Select(layer0_textbox_memdev);	
	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	x= 0; y=10;
	GUI_DispStringAt("------ Jostick -----",x , y); 
	
	y+=15; 
	sprintf(strBuffer,"mode-switch      %d   ",jostick->key_mode);  GUI_DispStringAt(strBuffer, x, y);
	sprintf(strBuffer,"sample-switch    %d   ",jostick->key_sample);  GUI_DispStringAt(strBuffer, x, y+20);
	sprintf(strBuffer,"alarm-switch     %d   ",jostick->key_alarm);  GUI_DispStringAt(strBuffer, x, y+40);
	
	y+=10;
	sprintf(strBuffer,"Left-x   	%d   ",(jostick->left_x/100-20)*5 );  GUI_DispStringAt(strBuffer, x, y+60);
	sprintf(strBuffer,"Left-y   	%d   ",(jostick->left_y/100-20)*5 );  GUI_DispStringAt(strBuffer, x, y+80);
	sprintf(strBuffer,"Right-x  	%d   ",(jostick->right_x/100-20)*5 );  GUI_DispStringAt(strBuffer, x, y+100);
	sprintf(strBuffer,"Right-y  	%d   ",(jostick->right_y/100-20)*5 );  GUI_DispStringAt(strBuffer, x, y+120);
	sprintf(strBuffer,"Knob     	%d   ",(jostick->knob/10)*100/409 );  GUI_DispStringAt(strBuffer, x, y+140);
	
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 40, 10);
	
	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	x = 0; y = 10;
	GUI_DispStringAt("------- Power ------", x, y);
	sprintf(strBuffer,"Battery     %d    ",bsp_Get_Battery_Level());
	GUI_DispStringAt(strBuffer, x, y+15);
	sprintf(strBuffer,"Boat        %d    ",remoter_sender_RF_get_boat_powerLevel());
	GUI_DispStringAt(strBuffer, x, y+30);

	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 40, 200);
	

	x = 0; y= 0;
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	GUI_BMP_Draw(_logo_big, x, y+5);
	//GUI_BMP_DrawScaled(_logo_big, x, y+10, 3, 2);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 300, 10);
	
	
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	x = 0; y = 20;
	GUI_DispStringAt("------ Radio  -------", x, y);
	sprintf(strBuffer,"Channel      %d    ",rf_config->hp);
	GUI_DispStringAt(strBuffer ,x , y+15);
	sprintf(strBuffer,"xtend_id     %d    ",rf_config->id);
	GUI_DispStringAt(strBuffer ,x , y+30);
	//sprintf(strBuffer,"RSSI         %d -- %d     ",xtend900_get_rssi(),xtend900_get_rssi_level() );
	sprintf(strBuffer,"RSSI         %d     ",xtend900_get_rssi_level() );
	GUI_DispStringAt(strBuffer,x , y+45);
	sprintf(strBuffer,"decode_rate  %d    ",remoter_sender_RF_getDecodeRate() );
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
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 250, 80);

	

	
	x = 0; y= 10;
	GUI_MEMDEV_Clear(layer0_textbox_memdev);
	GUI_DispStringAt("------ Log    -------",x , y);
	sprintf(strBuffer,"Cpu Usage   %d   ",FreeRTOS_GetCPUUsage());
	GUI_DispStringAt(strBuffer, x, y+15);
	GUI_DispStringAt(_log_buffer,x, y+30);
	GUI_MEMDEV_CopyToLCDAt(layer0_textbox_memdev, 250, 200);
	

}








//just for test
void remoter_sender_UI_setLog(char *str)
{
	sprintf(_log_buffer,"%s",str);
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
	
	//loading param , so display logo now 
	//show_logo_and_initing();
	
	while(1)
	{	
		switch( current_win_id )
		{
			case 0:
			{
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
		
	}
	
}