#ifndef _DRIVER_XTEND900_H
#define _DRIVER_XTEND900_H


typedef struct _xtend_900_config_t{
	unsigned char updated;
	unsigned char hp;
	unsigned char tp;
	unsigned short id;
	unsigned short my;
	unsigned short dt;
	unsigned short mk;
	unsigned short pl;
	unsigned short ap;
}xtend900_config_t;

typedef void (*xtend900_Recive_Handler_t)(char c);




char * xtend900_get_TX_Power_string(int power_select);
void xtend900_putchar(unsigned char c);
void xtend900_parase(unsigned char c);
int xtend900_save_param(xtend900_config_t * config);
int xtend900_load_param(xtend900_config_t * config);
void xtend900_set_reciver_handler( xtend900_Recive_Handler_t cb);




#endif