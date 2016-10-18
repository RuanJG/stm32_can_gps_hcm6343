/*-------------------------------------------------------------------------
�������ƣ�ң��������Ӧ�ó���
����˵����
�޸�˵����<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					150603 ������    5.0.0		�½���
																		
					

					<<-------------------------------------------------------->>
					����   �޸���    �汾��		�޸�����
					
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
#include "set_timer.h"
#include "math.h"


/* Exported define ------------------------------------------------------------*/
#define Graphic_Interface_Task_PRIO    ( tskIDLE_PRIORITY  + 10 )				//��ֵԽС���ȼ�Խ��
#define Graphic_Interface_Task_STACK   ( 2000 )


//�Դ���ز���
#define BG0N_XAXIS			50					//����bg0n_memdev�Ŀ��
#define BG0N_YAXIS			30					//����bg0n_memdev�ĸ߶�

#define BG1N_XAXIS			50					//����bg1n_memdev�Ŀ��
#define BG1N_YAXIS			30					//����bg1n_memdev�ĸ߶�

#define DEBUG_BG1_XAXIS			250					//�������ģʽ�µ�bg1_memdev�Ŀ��
#define DEBUG_BG1_YAXIS			100					//�������ģʽ�µ�bg1_memdev�ĸ߶�
#define DEBUG_BG1W_XAXIS		120					//�������ģʽ�µ�bg1w_memdev�Ŀ��
#define DEBUG_BG1W_YAXIS		30					//�������ģʽ�µ�bg1w_memdev�ĸ߶�

#define DEBUG_BG0_XAXIS			250					//�������ģʽ�µ�bg0_memdev�Ŀ��
#define DEBUG_BG0_YAXIS			100					//�������ģʽ�µ�bg0_memdev�ĸ߶�
#define DEBUG_BG0S_XAXIS		50					//�������ģʽ�µ�bg0s_memdev�Ŀ��
#define DEBUG_BG0S_YAXIS		30					//�������ģʽ�µ�bg0s_memdev�ĸ߶�


//���Խ�����ز���
#define DEBUG_MAIN_DIVISION			10						//������������ʾ��Ƶ         10���� * 10 = 100����	
#define DEBUG_PARAMETER_DIVISION	50					//���Բ�����ʾ��Ƶ         10���� * 50 = 500����
#define ATCOMMAND_DELAY		6										//����AT����ģʽ�ȴ�ʱ��  4 * 750���� = 3��		��СΪ2��

#define DEBUG_MAIN_INTERFACE_INDEX			1					//����ģʽ������
#define DEBUG_PARAM_INTERFACE_INDEX			2					//����ģʽ�������ý���
#define DEBUG_SAMPLE_INTERFACE_INDEX		3					//����ģʽ�����������ý���
#define DEBUG_LCD_TEST_INDEX						4					//����ģʽLCD���Խ���


//��������ز���
#define MAIN_GRAPHIC_DIVISION			10							//��������ʾ��Ƶ         10���� * 10 = 100����	
#define MAIN_PARAM1_DIVISION			10							//�������ý�����ʾ��Ƶ         10���� * 10 = 100����	

#define MAIN_GRAPHIC_INDEX							5					//������
#define MAIN_PARAM1_INDEX								6					//һ�Ų������ý���
#define MAIN_PARAM2_INDEX								7					//���Ų������ý��棨һ��Ϊ�������ý��棩
#define MAIN_PARAM3_INDEX								8					//���Ų������ý���
#define MAIN_SAMPLE_INDEX								9					//��ˮ���ý���
#define MAIN_CONTACT_INDEX							10				//��ϵ��ʽ����

#define SHOW_DEPTH_TIME		3 						//300���� / pixel
#define SHOW_MAX_DEPTH	75							// 75��Ϊ�����ʾ���

#define KEY_PROTECT_TIME	50						//����ճ������ʱ�䣬Լ500����


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Graphic_Interface_Task_Handle;

extern uint8_t interface_index;			//����������1Ϊ������
																		//					2Ϊ�������ý���
																		//					3Ϊ�������ý���
extern uint8_t modify_index;					//�����޸�����
extern uint8_t EngineTimerCount;			//��ֹ��������ʱ��

extern GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg1a_memdev, bg0a_memdev;								//��ʾ�洢����	

extern uint32_t RSSI_Timer;


/* Const ---------------------------------------------------------*/
static const char * _dantui[] = {
	"\xe5\x8d\x95\xe6\x8e\xa8"									//����
};

static const char * _shuangtui[] = {
	"\xe5\x8f\x8c\xe6\x8e\xa8"									//˫��
};

static const char * _zidong[] = {
	"\xe8\x87\xaa\xe5\x8a\xa8"									//�Զ�
};

static const char * _weishezhidanghangrenwu[] = {
	"\xe6\x9c\xaa\xe8\xae\xbe\xe7\xbd\xae\xe5\xaf\xbc\xe8\x88\xaa\xe4\xbb\xbb\xe5\x8a\xa1"					//δ���õ�������
};

static const char * _daohangrenwuyijieshu[] = {
	"\xe5\xaf\xbc\xe8\x88\xaa\xe4\xbb\xbb\xe5\x8a\xa1\xe5\xb7\xb2\xe7\xbb\x93\xe6\x9d\x9f"					//���������ѽ���
};

static const char * _zhinengrenwu[] = {
	"\xe6\x99\xba\xe8\x83\xbd\xe4\xbb\xbb\xe5\x8a\xa1"						//��������
};

static const char * _zhengzaicaiyangzhi[] = {
	"\xe6\xad\xa3\xe5\x9c\xa8\xe9\x87\x87\xe6\xa0\xb7\xe8\x87\xb3"						//���ڲ�����
};

static const char * _haoping[] = {
	"\xe5\x8f\xb7\xe7\x93\xb6"																		//��ƿ
};

static const char * _daohangquxiao[] = {
	"\xe5\xaf\xbc\xe8\x88\xaa\xe5\x8f\x96\xe6\xb6\x88"													//����ȡ��
};

static const char * _daohangzanting[] = {
	"\xe5\xaf\xbc\xe8\x88\xaa\xe6\x9a\x82\xe5\x81\x9c"													//������ͣ
};

static const char * _zhengzaidaohangzhi[] = {
	"\xe6\xad\xa3\xe5\x9c\xa8\xe5\xaf\xbc\xe8\x88\xaa\xe8\x87\xb3"							//���ڵ�����
};

static const char * _haodian[] = {
	"\xe5\x8f\xb7\xe7\x82\xb9"																		//�ŵ�
};

static const char * _zhengzaijiancezhi[] = {
	"\xe6\xad\xa3\xe5\x9c\xa8\xe7\x9b\x91\xe6\xb5\x8b\xe8\x87\xb3"							//���ڼ����
};

static const char * _caiyangjiance[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe7\x9b\x91\xe6\xb5\x8b"													//�������
};

static const char * _caiyangzhi[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe8\x87\xb3"																			//������
};

static const char * _youshoukongzhi[] = {
	"\xe5\x8f\xb3\xe6\x89\x8b\xe6\x8e\xa7\xe5\x88\xb6"													//���ֿ���
};

static const char * _shuibeng[] = {
	"\xe6\xb0\xb4\xe6\xb3\xb5"																		//ˮ��
};

static const char * _youmen[] = {
	"\xe6\xb2\xb9\xe9\x97\xa8"																		//����
};

static const char * _dangwei[] = {
	"\xe6\xa1\xa3\xe4\xbd\x8d"																		//��λ
};

static const char * _tuijin[] = {
	"\xe6\x8e\xa8\xe8\xbf\x9b"																		//�ƽ�
};

static const char * _duojiao[] = {
	"\xe8\x88\xb5\xe8\xa7\x92"																		//���
};

static const char * _qianjin[] = {
	"\xe2\x86\x91"																								//��
};

static const char * _houtui[] = {
	"\xe2\x86\x93"																								//��
};

static const char * _xiangzuo[] = {
	"\xe2\x86\x90"																								//��
};

static const char * _xiangyou[] = {
	"\xe2\x86\x92"																								//��
};

static const char * _zuosuyouduo[] = {
	"\xe5\xb7\xa6\xe9\x80\x9f\xe5\x8f\xb3\xe8\x88\xb5"						//�����Ҷ�
};

static const char * _zuoyouchasu[] = {
	"\xe5\xb7\xa6\xe5\x8f\xb3\xe5\xb7\xae\xe9\x80\x9f"						//���Ҳ���
};

static const char * _zuojin[] = {
	"\xe5\xb7\xa6\xe8\xbf\x9b"																		//���
};

static const char * _zuoduo[] = {
	"\xe5\xb7\xa6\xe8\x88\xb5"																		//���
};

static const char * _youduo[] = {
	"\xe5\x8f\xb3\xe8\x88\xb5"																		//�Ҷ�
};

static const char * _youjin[] = {
	"\xe5\x8f\xb3\xe8\xbf\x9b"																		//�ҽ�
};

static const char * _zhuangtaijiance[] = {
	"\xe7\x8a\xb6\xe6\x80\x81\xe7\x9b\x91\xe6\xb5\x8b"						//״̬���
};

static const char * _weixingdingwei[] = {
	"\xe5\x8d\xab\xe6\x98\x9f\xe5\xae\x9a\xe4\xbd\x8d"						//���Ƕ�λ
};

static const char * _weizhi[] = {
	"\xe4\xbd\x8d\xe7\xbd\xae"																		//λ��
};

static const char * _du[] = {
	"\xc2\xb0"																										//�� ��
};

static const char * _sudu[] = {
	"\xe9\x80\x9f\xe5\xba\xa6"																		//�ٶ�
};

static const char * _shouyao[] = {
	"\xe8\x89\x8f\xe6\x91\x87"																		//��ҡ
};

static const char * _hengyao[] = {
	"\xe6\xa8\xaa\xe6\x91\x87"																		//��ҡ
};

static const char * _zongyao[] = {
	"\xe7\xba\xb5\xe6\x91\x87"																		//��ҡ
};

static const char * _shuiwen[] = {
	"\xe6\xb0\xb4\xe6\xb8\xa9"																		//ˮ��
};

static const char * _sheshidu[] = {
	"\xe2\x84\x83"																								//��
};

static const char * _chuanwen[] = {
	"\xe8\x88\xb9\xe6\xb8\xa9"																							//����
};

static const char * _caiyangyujiance[] = {
	"\xe9\x87\x87\xe6\xa0\xb7 & \xe7\x9b\x91\xe6\xb5\x8b"										//���� & ���
};

static const char * _shendu[] = {
	"\xe6\xb7\xb1\xe5\xba\xa6"																							//���
};

static const char * _diantaishezhi[] = {
	"\xe7\x94\xb5\xe5\x8f\xb0\xe8\xae\xbe\xe7\xbd\xae"											//��̨����
};

static const char * _tongxinpindao[] = {
	"\xe9\x80\x9a\xe4\xbf\xa1\xe9\xa2\x91\xe9\x81\x93"											//ͨ��Ƶ��
};

static const char * _tongxinbianhao[] = {
	"\xe9\x80\x9a\xe4\xbf\xa1\xe7\xbc\x96\xe5\x8f\xb7"											//ͨ�ű��
};

static const char * _bendidizhi[] = {
	"\xe6\x9c\xac\xe5\x9c\xb0\xe5\x9c\xb0\xe5\x9d\x80"											//���ص�ַ
};

static const char * _mudidizhi[] = {
	"\xe7\x9b\xae\xe7\x9a\x84\xe5\x9c\xb0\xe5\x9d\x80"											//Ŀ�ĵ�ַ
};

static const char * _dizhiyanma[] = {
	"\xe5\x9c\xb0\xe5\x9d\x80\xe6\x8e\xa9\xe7\xa0\x81"											//��ַ����
};

static const char * _fashegonglv[] = {
	"\xe5\x8f\x91\xe5\xb0\x84\xe5\x8a\x9f\xe7\x8e\x87"											//���书��
};

static const char * _diantaimoshi[] = {
	"\xe7\x94\xb5\xe5\x8f\xb0\xe6\xa8\xa1\xe5\xbc\x8f"											//��̨ģʽ
};

static const char * _diantaiwendu[] = {
	"\xe7\x94\xb5\xe5\x8f\xb0\xe6\xb8\xa9\xe5\xba\xa6"											//��̨�¶�
};

static const char * _canshushezhi[] = {
	"\xe5\x8f\x82\xe6\x95\xb0\xe8\xae\xbe\xe7\xbd\xae"											//��������
};

static const char * _pingmuliangdu[] = {
	"\xe5\xb1\x8f\xe5\xb9\x95\xe4\xba\xae\xe5\xba\xa6"											//��Ļ����
};

static const char * _tuijinmoshi[] = {
	"\xe6\x8e\xa8\xe8\xbf\x9b\xe6\xa8\xa1\xe5\xbc\x8f"											//�ƽ�ģʽ
};

static const char * _chuanxingxuanze[] = {
	"\xe8\x88\xb9\xe5\x9e\x8b\xe9\x80\x89\xe6\x8b\xa9"											//����ѡ��
};

static const char * _duojizhongwei[] = {
	"\xe8\x88\xb5\xe6\x9c\xba\xe4\xb8\xad\xe4\xbd\x8d"											//�����λ
};

static const char * _tiaoshimoshi[] = {
	"\xe8\xb0\x83\xe8\xaf\x95\xe6\xa8\xa1\xe5\xbc\x8f"											//����ģʽ
};

static const char * _shengyinshezhi[] = {
	"\xe5\xa3\xb0\xe9\x9f\xb3\xe8\xae\xbe\xe7\xbd\xae"											//��������
};

static const char * _yuyanshezhi[] = {
	"\xe8\xaf\xad\xe8\xa8\x80\xe8\xae\xbe\xe7\xbd\xae"											//��������
};

static const char * _banbenxinxi[] = {
	"\xe7\x89\x88\xe6\x9c\xac\xe4\xbf\xa1\xe6\x81\xaf"											//�汾��Ϣ
};

static const char * _zhongwen[] = {
	"\xe4\xb8\xad\xe6\x96\x87"																							//����
};

static const char * _canshuduquzhong[] = {
	"\xe5\x8f\x82\xe6\x95\xb0\xe8\xaf\xbb\xe5\x8f\x96\xe4\xb8\xad"					//������ȡ��
};

static const char * _qingshaohou[] = {
	"\xe8\xaf\xb7\xe7\xa8\x8d\xe5\x80\x99"																	//���Ժ�
};

static const char * _canshucunchuzhong[] = {
	"\xe5\x8f\x82\xe6\x95\xb0\xe5\xad\x98\xe5\x82\xa8\xe4\xb8\xad"					//�����洢��
};

static const char * _qingwuduandian[] = {
	"\xe8\xaf\xb7\xe5\x8b\xbf\xe6\x96\xad\xe7\x94\xb5"											//����ϵ�
};

static const char * _tongxinxieyi[] = {
	"\xe9\x80\x9a\xe4\xbf\xa1\xe5\x8d\x8f\xe8\xae\xae"											//ͨ��Э��
};

static const char * _caiyangshezhi[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe8\xae\xbe\xe7\xbd\xae"											//��������
};

static const char * _caiyangpinghao[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe7\x93\xb6\xe5\x8f\xb7"											//����ƿ��
};

static const char * _caiyangshendu[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe6\xb7\xb1\xe5\xba\xa6"											//�������
};

static const char * _caiyangmoshi[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe6\xa8\xa1\xe5\xbc\x8f"											//����ģʽ
};

static const char * _caiyangrongliang[] = {
	"\xe9\x87\x87\xe6\xa0\xb7\xe5\xae\xb9\xe9\x87\x8f"											//��������
};

static const char * _qingximoshi[] = {
	"\xe6\xb8\x85\xe6\xb4\x97\xe6\xa8\xa1\xe5\xbc\x8f"											//��ϴģʽ
};

static const char * _famenshezhi[] = {
	"\xe9\x98\x80\xe9\x97\xa8\xe8\xae\xbe\xe7\xbd\xae"											//��������
};

static const char * _lianxifangshi[] = {
	"\xe8\x81\x94\xe7\xb3\xbb\xe6\x96\xb9\xe5\xbc\x8f"											//��ϵ��ʽ
};

static const char * _yihaofamen[] = {
	"1\xe5\x8f\xb7\xe9\x98\x80\xe9\x97\xa8"																	//1�ŷ���
};

static const char * _erhaofamen[] = {
	"2\xe5\x8f\xb7\xe9\x98\x80\xe9\x97\xa8"																	//2�ŷ���
};

static const char * _sanhaofamen[] = {
	"3\xe5\x8f\xb7\xe9\x98\x80\xe9\x97\xa8"																	//3�ŷ���
};

static const char * _sihaofamen[] = {
	"4\xe5\x8f\xb7\xe9\x98\x80\xe9\x97\xa8"																	//4�ŷ���
};

static const char * _bengshuifamen[] = {
	"\xe6\xb3\xb5\xe6\xb0\xb4\xe9\x98\x80\xe9\x97\xa8"											//��ˮ����
};

static const char * _quanqiuzongbu[] = {
	"\xe5\x85\xa8\xe7\x90\x83\xe6\x80\xbb\xe9\x83\xa8"											//ȫ���ܲ�
};

static const char * _lianxidizhi[] = {
	"\xe8\x81\x94\xe7\xb3\xbb\xe5\x9c\xb0\xe5\x9d\x80"											//��ϵ��ַ
};

static const char * _zhongguoguangdongshengzhuhaishi[] = {
	"\xe4\xb8\xad\xe5\x9b\xbd\xe5\xb9\xbf\xe4\xb8\x9c\xe7\x9c\x81\xe7\x8f\xa0\xe6\xb5\xb7\xe5\xb8\x82"											//�й��㶫ʡ�麣��
	"\xe9\xab\x98\xe6\x96\xb0\xe5\x8c\xba"																																									//������
};

static const char * _nanfangruanjianyuan[] = {
	"\xe5\x8d\x97\xe6\x96\xb9\xe8\xbd\xaf\xe4\xbb\xb6\xe5\x9b\xad""D2-214"							//�Ϸ����԰D2-214
};

static const char * _youzhengbianma[] = {
	"\xe9\x82\xae\xe6\x94\xbf\xe7\xbc\x96\xe7\xa0\x81"							//��������
};

static const char * _dianhuahaoma[] = {
	"\xe7\x94\xb5\xe8\xaf\x9d\xe5\x8f\xb7\xe7\xa0\x81"							//�绰����
};

static const char * _chuanzhenhaoma[] = {
	"\xe4\xbc\xa0\xe7\x9c\x9f\xe5\x8f\xb7\xe7\xa0\x81"							//�������
};

static const char * _dianziyoujian[] = {
	"\xe7\x94\xb5\xe5\xad\x90\xe9\x82\xae\xe4\xbb\xb6"							//�����ʼ�
};

static const char * _guanfangwangzhi[] = {
	"\xe5\xae\x98\xe6\x96\xb9\xe7\xbd\x91\xe5\x9d\x80"							//�ٷ���ַ
};

static const char * _shishijiance[] = {
	"\xe5\xae\x9e\xe6\x97\xb6\xe7\x9b\x91\xe6\xb5\x8b"							//ʵʱ���
};

static const char * _dixing[] = {
	"\xe5\x9c\xb0\xe5\xbd\xa2"																			//����
};

static const char * _zhongguoguangdongshengguangzhoushikeyunlu[] = {
	"\xe4\xb8\xad\xe5\x9b\xbd\xe5\xb9\xbf\xe4\xb8\x9c\xe7\x9c\x81\xe5\xb9\xbf\xe5\xb7\x9e\xe5\xb8\x82"											//�й��㶫ʡ������
	"\xe7\xa7\x91\xe9\x9f\xb5\xe8\xb7\xaf"																																									//����·
};

static const char * _ruanjianyuanjianzhonglu52haodaohangdasha[] = {
	"\xe8\xbd\xaf\xe4\xbb\xb6\xe5\x9b\xad\xe5\xbb\xba\xe4\xb8\xad\xe8\xb7\xaf"														//���԰����·
	"52\xe5\x8f\xb7"																																											//52��
	"\xe5\xaf\xbc\xe8\x88\xaa\xe5\xa4\xa7\xe5\x8e\xa6"																										//��������												
};




/* Exported function prototypes -----------------------------------------------*/
void Graphic_Interface_Task(void * pvParameters);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);

void main_graphic_show(void);
void main_param_show(void);
void main_sample_show(void);
void main_valve_show(void);
void main_contact_show(void);

void debug_mode_main(void);
void debug_mode_parameter(void);
void sample_mode_config(void);
void save_sample_config(void);
void load_sample_config(void);
void raw_message_handling(void);
uint16_t LCD_Test(uint16_t timer);
void Draw_Empty_Bottle(uint8_t start_pix);
void LCD_clear(void);
void show_save_wait(void);
int float_to_string(double data, char *str, u8 length, u8 unit, u8 * prechar, u8 prelength);
void mdelay(volatile uint32_t timer);


#endif
