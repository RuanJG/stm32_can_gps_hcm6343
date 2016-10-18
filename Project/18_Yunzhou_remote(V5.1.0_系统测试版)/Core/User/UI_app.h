/*-------------------------------------------------------------------------
工程名称：遥控器界面应用程序
描述说明：
修改说明：<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					150603 赵铭章    5.0.0		新建立
																		
					

					<<-------------------------------------------------------->>
					日期   修改人    版本号		修改内容
					
-------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UI_APP_H
#define __UI_APP_H


/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"
#include "bsp.h"
//#include "UI_element.h"
#include "keyboard_app.h"
#include "m_flash.h"
#include "local_handle.h"
#include "RFSession.h"


/* Exported define ------------------------------------------------------------*/
#define Graphic_Interface_Task_PRIO    ( tskIDLE_PRIORITY  + 10 )				//数值越小优先级越高
#define Graphic_Interface_Task_STACK   ( 2000 )


//显存相关参数
#define BG0N_XAXIS			50					//定义bg0n_memdev的宽度
#define BG0N_YAXIS			30					//定义bg0n_memdev的高度

#define BG1N_XAXIS			50					//定义bg1n_memdev的宽度
#define BG1N_YAXIS			30					//定义bg1n_memdev的高度

#define DEBUG_BG1_XAXIS			250					//定义调试模式下的bg1_memdev的宽度
#define DEBUG_BG1_YAXIS			100					//定义调试模式下的bg1_memdev的高度
#define DEBUG_BG1W_XAXIS		120					//定义调试模式下的bg1w_memdev的宽度
#define DEBUG_BG1W_YAXIS		30					//定义调试模式下的bg1w_memdev的高度

#define DEBUG_BG0_XAXIS			250					//定义调试模式下的bg0_memdev的宽度
#define DEBUG_BG0_YAXIS			100					//定义调试模式下的bg0_memdev的高度
#define DEBUG_BG0S_XAXIS		50					//定义调试模式下的bg0s_memdev的宽度
#define DEBUG_BG0S_YAXIS		30					//定义调试模式下的bg0s_memdev的高度


//调试界面相关参数
#define DEBUG_MAIN_DIVISION			10						//调试主界面显示分频         10毫秒 * 10 = 100毫秒	
#define DEBUG_PARAMETER_DIVISION	50					//调试参数显示分频         10毫秒 * 50 = 500毫秒
#define ATCOMMAND_DELAY		6										//进入AT命令模式等待时间  6 * 500毫秒 = 3秒

#define DEBUG_MAIN_INTERFACE_INDEX			1					//调试模式主界面
#define DEBUG_PARAM_INTERFACE_INDEX			2					//调试模式参数配置界面
#define DEBUG_SAMPLE_INTERFACE_INDEX		3					//调试模式采样参数设置界面
#define DEBUG_LCD_TEST_INDEX						4					//调试模式LCD测试界面


//主界面相关参数
#define MAIN_GRAPHIC_DIVISION			10							//主界面显示分频         10毫秒 * 10 = 100毫秒	
#define MAIN_PARAM1_DIVISION			10							//参数配置界面显示分频         10毫秒 * 10 = 100毫秒	

#define MAIN_GRAPHIC_INDEX							5					//主界面
#define MAIN_PARAM1_INDEX								6					//一号参数配置界面
#define MAIN_PARAM2_INDEX								7					//二号参数配置界面
#define MAIN_PARAM3_INDEX								8					//三号参数配置界面
#define MAIN_SAMPLE_INDEX								9					//采水配置界面

#define SHOW_DEPTH_TIME		3 						//300毫秒 / pixel
#define SHOW_MAX_DEPTH	75							// 75米为最大显示深度


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Graphic_Interface_Task_Handle;

extern uint8_t interface_index;			//界面索引：1为主界面
																		//					2为参数配置界面
																		//					3为采样配置界面
extern uint8_t modify_index;					//参数修改索引
extern uint8_t EngineTimerCount;			//禁止连续打火计时器

extern GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg1a_memdev, bg0a_memdev;								//显示存储变量	


/* Const ---------------------------------------------------------*/
static const char * _dantui[] = {
	"\xe5\x8d\x95\xe6\x8e\xa8"									//单推
};

static const char * _shuangtui[] = {
	"\xe5\x8f\x8c\xe6\x8e\xa8"									//双推
};

static const char * _zidong[] = {
	"\xe8\x87\xaa\xe5\x8a\xa8"									//自动
};

static const char * _weishezhidanghangrenwu[] = {
	"\xe6\x9c\xaa\xe8\xae\xbe\xe7\xbd\xae\xe5\xaf\xbc\xe8\x88\xaa\xe4\xbb\xbb\xe5\x8a\xa1"					//未设置导航任务
};

static const char * _daohangrenwuyijieshu[] = {
	"\xe5\xaf\xbc\xe8\x88\xaa\xe4\xbb\xbb\xe5\x8a\xa1\xe5\xb7\xb2\xe7\xbb\x93\xe6\x9d\x9f"					//导航任务已结束
};

static const char * _zhinengrenwu[] = {
	"\xe6\x99\xba\xe8\x83\xbd\xe4\xbb\xbb\xe5\x8a\xa1"						//智能任务
};

static const char * _zhengzaicaiyangzhi[] = {
	"\xe6\xad\xa3\xe5\x9c\xa8\xe9\x87\x87\xe6\xa0\xb7\xe8\x87\xb3"						//正在采样至
};

static const char * _haoping[] = {
	"\xe5\x8f\xb7\xe7\x93\xb6"																		//号瓶
};

static const char * _daohangquxiao[] = {
	"\xe5\xaf\xbc\xe8\x88\xaa\xe5\x8f\x96\xe6\xb6\x88"													//导航取消
};

static const char * _daohangzanting[] = {
	"\xe5\xaf\xbc\xe8\x88\xaa\xe6\x9a\x82\xe5\x81\x9c"													//导航暂停
};

static const char * _zhengzaidaohangzhi[] = {
	"\xe6\xad\xa3\xe5\x9c\xa8\xe5\xaf\xbc\xe8\x88\xaa\xe8\x87\xb3"							//正在导航至
};

static const char * _haodian[] = {
	"\xe5\x8f\xb7\xe7\x82\xb9"																		//号点
};

static const char * _zhengzaijiancezhi[] = {
	"\xe6\xad\xa3\xe5\x9c\xa8\xe7\x9b\x91\xe6\xb5\x8b\xe8\x87\xb3"							//正在监测至
};

static const char * _caiyangjiance[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe7\x9b\x91\xe6\xb5\x8b"													//采样监测
};

static const char * _caiyangzhi[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe8\x87\xb3"																			//采样至
};

static const char * _youshoukongzhi[] = {
	"\xe5\x8f\xb3\xe6\x89\x8b\xe6\x8e\xa7\xe5\x88\xb6"													//右手控制
};

static const char * _shuibeng[] = {
	"\xe6\xb0\xb4\xe6\xb3\xb5"																		//水泵
};

static const char * _youmen[] = {
	"\xe6\xb2\xb9\xe9\x97\xa8"																		//油门
};

static const char * _dangwei[] = {
	"\xe6\xa1\xa3\xe4\xbd\x8d"																		//档位
};

static const char * _tuijin[] = {
	"\xe6\x8e\xa8\xe8\xbf\x9b"																		//推进
};

static const char * _duojiao[] = {
	"\xe8\x88\xb5\xe8\xa7\x92"																		//舵角
};

static const char * _qianjin[] = {
	"\xe2\x86\x91"																								//↑
};

static const char * _houtui[] = {
	"\xe2\x86\x93"																								//↓
};

static const char * _xiangzuo[] = {
	"\xe2\x86\x90"																								//←
};

static const char * _xiangyou[] = {
	"\xe2\x86\x92"																								//→
};

static const char * _zuosuyouduo[] = {
	"\xe5\xb7\xa6\xe9\x80\x9f\xe5\x8f\xb3\xe8\x88\xb5"						//左速右舵
};

static const char * _zuoyouchasu[] = {
	"\xe5\xb7\xa6\xe5\x8f\xb3\xe5\xb7\xae\xe9\x80\x9f"						//左右差速
};

static const char * _zuojin[] = {
	"\xe5\xb7\xa6\xe8\xbf\x9b"																		//左进
};

static const char * _zuoduo[] = {
	"\xe5\xb7\xa6\xe8\x88\xb5"																		//左舵
};

static const char * _youduo[] = {
	"\xe5\x8f\xb3\xe8\x88\xb5"																		//右舵
};

static const char * _youjin[] = {
	"\xe5\x8f\xb3\xe8\xbf\x9b"																		//右进
};

static const char * _zhuangtaijiance[] = {
	"\xe7\x8a\xb6\xe6\x80\x81\xe7\x9b\x91\xe6\xb5\x8b"						//状态监测
};

static const char * _weixingdingwei[] = {
	"\xe5\x8d\xab\xe6\x98\x9f\xe5\xae\x9a\xe4\xbd\x8d"						//卫星定位
};

static const char * _weizhi[] = {
	"\xe4\xbd\x8d\xe7\xbd\xae"																		//位置
};

static const char * _du[] = {
	"\xc2\xb0"																										//度 °
};

static const char * _sudu[] = {
	"\xe9\x80\x9f\xe5\xba\xa6"																		//速度
};

static const char * _souyao[] = {
	"\xe8\x89\x98\xe6\x91\x87"																		//艘摇
};

static const char * _hengyao[] = {
	"\xe6\xa8\xaa\xe6\x91\x87"																		//横摇
};

static const char * _zongyao[] = {
	"\xe7\xba\xb5\xe6\x91\x87"																		//纵摇
};

static const char * _shuiwen[] = {
	"\xe6\xb0\xb4\xe6\xb8\xa9"																		//水温
};

static const char * _sheshidu[] = {
	"\xe2\x84\x83"																								//℃
};

static const char * _chuanwen[] = {
	"\xe8\x88\xb9\xe6\xb8\xa9"																							//船温
};

static const char * _caiyangyujiance[] = {
	"\xe9\x87\x87\xe6\xa0\xb7 & \xe7\x9b\x91\xe6\xb5\x8b"										//采样 & 监测
};

static const char * _shendu[] = {
	"\xe6\xb7\xb1\xe5\xba\xa6"																							//深度
};

static const char * _diantaishezhi[] = {
	"\xe7\x94\xb5\xe5\x8f\xb0\xe8\xae\xbe\xe7\xbd\xae"											//电台设置
};

static const char * _tongxinpindao[] = {
	"\xe9\x80\x9a\xe4\xbf\xa1\xe9\xa2\x91\xe9\x81\x93"											//通信频道
};




/* Exported function prototypes -----------------------------------------------*/
void Graphic_Interface_Task(void * pvParameters);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);
void main_craphic_show(void);
void main_param_show(void);
void debug_mode_main(void);
void debug_mode_parameter(void);
void sample_mode_config(void);
void save_sample_config(void);
void load_sample_config(void);
void raw_message_handling(void);
uint16_t LCD_Test(uint16_t timer);
void Draw_Empty_Bottle(uint8_t start_pix);
void LCD_clear(void);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);
void mdelay(volatile uint32_t timer);


#endif
