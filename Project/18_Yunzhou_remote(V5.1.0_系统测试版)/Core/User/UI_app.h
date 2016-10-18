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
#define ATCOMMAND_DELAY		6										//����AT����ģʽ�ȴ�ʱ��  6 * 500���� = 3��

#define DEBUG_MAIN_INTERFACE_INDEX			1					//����ģʽ������
#define DEBUG_PARAM_INTERFACE_INDEX			2					//����ģʽ�������ý���
#define DEBUG_SAMPLE_INTERFACE_INDEX		3					//����ģʽ�����������ý���
#define DEBUG_LCD_TEST_INDEX						4					//����ģʽLCD���Խ���


//��������ز���
#define MAIN_GRAPHIC_DIVISION			10							//��������ʾ��Ƶ         10���� * 10 = 100����	
#define MAIN_PARAM1_DIVISION			10							//�������ý�����ʾ��Ƶ         10���� * 10 = 100����	

#define MAIN_GRAPHIC_INDEX							5					//������
#define MAIN_PARAM1_INDEX								6					//һ�Ų������ý���
#define MAIN_PARAM2_INDEX								7					//���Ų������ý���
#define MAIN_PARAM3_INDEX								8					//���Ų������ý���
#define MAIN_SAMPLE_INDEX								9					//��ˮ���ý���

#define SHOW_DEPTH_TIME		3 						//300���� / pixel
#define SHOW_MAX_DEPTH	75							// 75��Ϊ�����ʾ���


/* Exported variables ---------------------------------------------------------*/
extern	xTaskHandle		Graphic_Interface_Task_Handle;

extern uint8_t interface_index;			//����������1Ϊ������
																		//					2Ϊ�������ý���
																		//					3Ϊ�������ý���
extern uint8_t modify_index;					//�����޸�����
extern uint8_t EngineTimerCount;			//��ֹ��������ʱ��

extern GUI_MEMDEV_Handle bg1_memdev, bg0_memdev, bg0s_memdev, bg1w_memdev, bg1a_memdev, bg0a_memdev;								//��ʾ�洢����	


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

static const char * _souyao[] = {
	"\xe8\x89\x98\xe6\x91\x87"																		//��ҡ
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
