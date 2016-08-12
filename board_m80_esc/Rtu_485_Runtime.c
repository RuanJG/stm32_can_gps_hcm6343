#include "stm32f10x.h" 	
#include "esc_box.h"




/***********************   TH11SB  ************************/
struct th11sb_t {
	unsigned char addr;
	unsigned short wet ;
	unsigned short tempture;
}th11sb_head,th11sb_tail;
struct th11sb_t *current_th11sb;

void _Th11sb_config()
{
	th11sb_head.addr = 0x0a;
	th11sb_tail.addr = 0x0b;
}
int _Th11sb_485_runtime(struct th11sb_t *th11sb, int step, int res, rtu_485_ack_t *runtime_ack)
{
	// 1 : this step run ok ; 0: send cmd fail need retry; -1 ack analize failed
	// ack analize ( step 1) cannot return 0, if failed , return -1;
	int ret;
	if( step == 0 ){
		//send cmd
		return Rtu_485_send_cmd(th11sb->addr, 3, 0 ,2);
	}else{
		//recive ack
		if( res == 1){
			if( th11sb->addr == runtime_ack->addr && runtime_ack->len == 4 ){
						//logd("th11sb updated\r\n");
						th11sb->tempture = ((runtime_ack->data[0]<<8) | runtime_ack->data[1]);
						th11sb->wet = ((runtime_ack->data[2]<<8) | runtime_ack->data[3]);
			}else{
				logd("th11sb wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd("th11sb timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}


/******************************* DAM  ***********************/

struct dam_t{
	unsigned int status;
	unsigned char addr;
	unsigned char type; // 4 or 16
}dam4_02,dam4_04,dam4_05,dam16_08,dam4_09;

void _dam_config()
{
	dam4_02.addr = 0x02;
	dam4_02.type = 4;
	dam4_02.status = 0;
	
	dam4_04.addr = 0x04;
	dam4_04.type = 4;
	dam4_04.status = 0;
	
	dam4_05.addr = 0x05;
	dam4_05.type = 4;
	dam4_05.status = 0;
	
	dam16_08.addr = 0x08;
	dam16_08.type = 16;
	dam16_08.status = 0;
	
	dam4_09.addr = 0x09;
	dam4_09.type = 4;
	dam4_09.status = 0;
}
int _dam_485_runtime(struct dam_t *dam_dev, int step, int res, rtu_485_ack_t *runtime_ack)
{
	// 1 : this step run ok ; 0: send cmd fail need retry; -1 ack analize failed
	// ack analize ( step 1) cannot return 0, if failed , return -1;
	int ret,i;
	if( step == 0 ){
		//send cmd
		//查询n路继电器的状态
		return Rtu_485_send_cmd(dam_dev->addr, 1, 0 ,dam_dev->type);
	}else{
		//recive ack
		if( res == 1){
			if( dam_dev->addr == runtime_ack->addr ){
				if( runtime_ack->len == 2 ){
					dam_dev->status = ((runtime_ack->data[0]<<8) | runtime_ack->data[1]); 
				}else{
					dam_dev->status = runtime_ack->data[0];
				}
			}else{
				logd("dam wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd("dam timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}
// num_id is from 1-4 or 1-16  enable : 1 or disable 0  , return 0 false 1 ok
int _dam_485_send_on_off_cmd( struct dam_t *dam_dev, unsigned char num_id, int enable)
{
	unsigned short on_off;
	
	if( num_id > dam_dev->type ) return 0;
	on_off = enable? 0xff00:0x0;
	return Rtu_485_Runtime_sendCmd( dam_dev->addr , 0x05, num_id-1 , on_off);
}

// flash enable=1 闪开 enable=0 闪断, delaytime ：时间间隔 单位是0.1s 
int _dam_485_send_flash(struct dam_t *dam_dev, unsigned char num_id, int enable, int delaytime)
{
	//FE    10   00 03    00 02     04       00 04       00 0A        00 D8
	//addr func  num_id  cmd count  cmd len   flash type  delay time   crc
	// flash type: 0004 闪开 0002 闪断 
	//delay time : 0x0a=10 = 10*0.1 s = 1s
	
	unsigned char cmd[13]={0};
	unsigned short crc;
	if( num_id > dam_dev->type)  return 0;
	
	cmd[0] = dam_dev->addr;
	cmd[1] = 0x10;
	cmd[2] = 0x00;cmd[3]=num_id;
	cmd[4]= 0x00; cmd [5]= 0x02;
	cmd[6] = 0x04;
	cmd[7] = 0x00; cmd[8]= enable? 0x04:0x02;
	cmd[9] = ((delaytime>>8)&0xff); cmd[10] = (delaytime&0xff);
	crc = crc_calculate(cmd,11);
	cmd[11]=(crc&0xff);
	cmd[12]=((crc>>8) & 0xff);
	
	return Rtu_485_Runtime_send_RawCmd(cmd, 13);
}





/******************************  Power ADC devices 电压采集板 *******************/
struct powerAdc6_t{
	unsigned char addr;
	unsigned short adc[6];
}powerAdc6_01,powerAdc6_06,powerAdc6_07;

void powerAdc6_config()
{
	powerAdc6_01.addr = 0x01;
	powerAdc6_06.addr = 0x06;
	powerAdc6_07.addr = 0x07;
}

int _powerAdc_485_runtime(struct powerAdc6_t *padc_dev, int step, int res, rtu_485_ack_t *runtime_ack)
{
	// 1 : this step run ok ; 0: send cmd fail need retry; -1 ack analize failed
	// ack analize ( step 1) cannot return 0, if failed , return -1;
	int ret,i;
	if( step == 0 ){
		//send cmd
		//查询6路电压的状态
		return Rtu_485_send_cmd(padc_dev->addr, 4, 0 ,6);
	}else{
		//recive ack
		if( res == 1){
			if( padc_dev->addr == runtime_ack->addr ){
				//TODO
			}else{
				logd("powerAdc wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd("powerAdc timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}









/***************************************** Rtu 485 Device list config */

#define _485_LOOP_MS 10  // each 10ms enter loop 
int _rtu_485_devices_runtime(int step, int res, rtu_485_ack_t *runtime_ack)
{
	// if step=0; return 0 will reenter step 0;
	static uint8_t _485_device_seq = 0;
	int ret;
	
	switch(_485_device_seq){
		case 0:{
			ret = _Th11sb_485_runtime(&th11sb_head,step,res,runtime_ack);
			break;
		}
		case 1:{
			ret = _Th11sb_485_runtime(&th11sb_tail,step,res,runtime_ack);
			break;
		}
		case 2:{
			ret = _dam_485_runtime(&dam4_02, step,res,runtime_ack);
			break;
		}
		case 3:{
			ret = _dam_485_runtime(&dam4_04, step,res,runtime_ack);
			break;
		}
		case 4:{
			ret = _dam_485_runtime(&dam4_05, step,res,runtime_ack);
			break;
		}
		case 5:{
			ret = _dam_485_runtime(&dam4_09, step,res,runtime_ack);
			break;
		}
		case 6:{
			ret = _dam_485_runtime(&dam16_08, step,res,runtime_ack);
			break;
		}		
		
		case 7:{
			_485_device_seq = 0;
			ret = 0;
			break;
		}
		
	}
	if ( step == 1){
		_485_device_seq++;
	}
	return ret;
}





/*******************************************8     Main control func ******************/

volatile unsigned char _rtu_485_runtime_running = 0; //if a runtime is running or not
systick_time_t rtu_485_time_t;
fifo_t rtu_485_cmd_fifo;
#define RTU_485_CMD_FIFI_BUFF_LEN 512
#define RTU_485_CMD_LEN 32
char rtu_485_cmd_fifo_buff[RTU_485_CMD_FIFI_BUFF_LEN];
unsigned char _rtu_485_sendcmd[RTU_485_CMD_LEN];


int _rtu_485_sendcmd_runtime(int step, int res, rtu_485_ack_t *runtime_ack)
{
	unsigned char i,len;
	int vail_len;
	
	if( step == 0 ){
		vail_len = fifo_avail(&rtu_485_cmd_fifo);
		if( 8 < vail_len ){
			fifo_get(&rtu_485_cmd_fifo,&len);
			vail_len--;
			if( vail_len < len){
				//error vail_len should >= len, clean fifo
				for(i=0; i< vail_len; i++){
					fifo_get(&rtu_485_cmd_fifo,&len);
				}
				logd("rtu485 fifo error\r\n");
				return 0;
			}else{
				for(i=0; i< len; i++){
					fifo_get(&rtu_485_cmd_fifo,&_rtu_485_sendcmd[i]);
				}			
				return Rtu_485_send_raw_cmd(_rtu_485_sendcmd,len);
			}
		}
		return 0;
	}else{
		//handle ack
		if( res == 1) logd("send cmd ok\r\n");
		return res;
	}
}
int Rtu_485_Runtime_sendCmd(unsigned char addr, unsigned char func, unsigned short reg_addr , unsigned short len)
{
	int free_count,i;
	unsigned char cmd[8]={0};
	unsigned short crc;

	free_count = fifo_free(&rtu_485_cmd_fifo) ;
	if( free_count < 9 ){
		logd("rtu_485 cmd buff full\r\n");
		return 0;
	}
	cmd[0]=addr;
	cmd[1]=func;
	cmd[2]=((reg_addr>>8) & 0xff);
	cmd[3]=(reg_addr&0xff);
	cmd[4]=((len>>8) & 0xff);
	cmd[5]=(len&0xff);
	crc = crc_calculate(cmd,6);
	cmd[6]=(crc&0xff);
	cmd[7]=((crc>>8) & 0xff);
	
	//put len
	fifo_put(&rtu_485_cmd_fifo,8);
	for( i=0; i< 8; i++){
		fifo_put(&rtu_485_cmd_fifo,cmd[i]);
	}
	return 1;
}
int Rtu_485_Runtime_send_RawCmd(unsigned char *data, unsigned char len)
{
	int free_count,i;

	free_count = fifo_free(&rtu_485_cmd_fifo) ;
	if( free_count < len+1 ){
		logd("rtu_485 cmd buff full\r\n");
		return 0;
	}
	if( len > RTU_485_CMD_LEN ){
		logd("rtu_485 cmd too long\r\n");
		return 0;
	}
	
	//put len
	fifo_put(&rtu_485_cmd_fifo,len);
	for( i=0; i< len; i++){
		fifo_put(&rtu_485_cmd_fifo,data[i]);
	}
	
	return 1;
}
void Rtu_485_SendCmd_runtime_loop()
{
	static char step=0;
	char data;
	int res=0,ret;
	rtu_485_ack_t *g_ack;
	
	if( step == 0){
		res = _rtu_485_sendcmd_runtime(0,0,NULL);
		if( 1 == res ){
			_rtu_485_runtime_running = 1;
			step++;
		}
	}else{
		g_ack = Rtu_485_Get_Ack(&res);
		if( res != 0){
			step = 0;
			_rtu_485_runtime_running = 0;
			ret = _rtu_485_sendcmd_runtime(1,res,g_ack);
			#if 1// debug
			if( ret == -1 )
				logd("_485_sendcmd error");
			#endif
			//step = _rtu_485_sendcmd_runtime(0,0,NULL);
		}
	}
}







/***********************************  Devide runtime *************************/

void Rtu_485_Devices_runtime_loop()
{
	static char step=0;
	char data;
	int res=0,ret;
	rtu_485_ack_t *g_ack;
	
	if( step == 0){
		if( 1 == _rtu_485_devices_runtime(0,0,NULL) ){
			_rtu_485_runtime_running = 1;
			step++;
		}
	}else{
		g_ack = Rtu_485_Get_Ack(&res);
		if( res != 0){
			step = 0;
			_rtu_485_runtime_running = 0;
			ret = _rtu_485_devices_runtime(1,res,g_ack);
			#if 1// debug
			
			if( res == -1 || ret == -1 )
				logd("_485_devices error");
			#endif
		}
	}
	
}






/************************************************ main func *************/

void Rtu_485_Runtime_Configure()
{
	_Th11sb_config();
	_dam_config();
	powerAdc_config();
	fifo_init(&rtu_485_cmd_fifo, rtu_485_cmd_fifo_buff, RTU_485_CMD_FIFI_BUFF_LEN);
	systick_time_start(&rtu_485_time_t,_485_LOOP_MS);
}

#define _485_CMD_LOOP 1
#define _485_DEVICES_LOOP 2
volatile char _rtu_485_loop_type = _485_DEVICES_LOOP;

void Rtu_485_Runtime_loop()
{
	char _to_send_cmd = 0;
	
	if( 8 < fifo_avail(&rtu_485_cmd_fifo) )
		_to_send_cmd = 1;
	
	if( check_systick_time(&rtu_485_time_t) ){
		
		if( _rtu_485_loop_type == _485_DEVICES_LOOP)
		{
			if( _to_send_cmd == 1  && _rtu_485_runtime_running == 0)
			{
				//switch cmd loop
				_rtu_485_loop_type = _485_CMD_LOOP;
				Rtu_485_SendCmd_runtime_loop();
			}else{
				Rtu_485_Devices_runtime_loop();
			}
		}else{
			if( _to_send_cmd == 0  && _rtu_485_runtime_running == 0)
			{
				//switch device read data loop
				_rtu_485_loop_type = _485_DEVICES_LOOP;
				Rtu_485_Devices_runtime_loop();
			}else{
				Rtu_485_SendCmd_runtime_loop();
			}
		}
	}
}






//End of File
