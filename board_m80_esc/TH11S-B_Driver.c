#include "stm32f10x.h" 	
#include "esc_box.h"


uint8_t th11sb_addr;
rtu_485_ack_t *th11sb_ack;
unsigned short th11sb_wet_value = 0;
unsigned short th11sb_tempture_value = 0;

void Th11sb_Configure(uint8_t addr)
{

	th11sb_addr = addr;

}

uint8_t raw_cmd[8]={0x01,0x03,0x00,0x00,0x00,0x02,0xc4,0x0b};

int _stop_th11sb = 0;
int Th11sb_485_runtime()
{
	// 1 ok , 0 reciveing, -1 error
	static char step=0;
	char data;
	int res=0;
	
	if( _stop_th11sb ) return 1;
	
	if( step == 0){
		
		if( 1 == Rtu_485_send_cmd(th11sb_addr, 3, 0 ,2) )
			step++;
		
		//if( 1 == Rtu_485_send_raw_cmd(raw_cmd,8)){
		//	step++;
		//}
	}else{
		th11sb_ack = Rtu_485_Get_Ack(&res);
		if( res == 1){
			step = 0;
			if( th11sb_addr == th11sb_ack->addr && th11sb_ack->len == 4 ){
						th11sb_tempture_value = ((th11sb_ack->data[0]<<8) | th11sb_ack->data[1]);
						th11sb_wet_value = ((th11sb_ack->data[2]<<8) | th11sb_ack->data[3]);
			}else{
				logd("wrong ack\r\n");
				res = -1;
				_stop_th11sb =1;
			}
		}
		if( res == -1 ){
			logd("ack false\r\n");
			_stop_th11sb = 1;
			step = 0;
		}
	}
	return res;
}

unsigned short Th11sb_get_tempture()
{
	return th11sb_tempture_value;
}
unsigned short Th11sb_get_wet()
{
	return th11sb_wet_value;
}





//End of File
