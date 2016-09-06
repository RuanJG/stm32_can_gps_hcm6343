#include "stm32f10x.h" 	
#include "esc_box.h"




/***********************   TH11SB  ************************/
th11sb_t th11sb_head,th11sb_tail;


void _Th11sb_config()
{
	th11sb_head.addr = 0x0a;
	th11sb_head.updated = 0;
	th11sb_tail.addr = 0x0b;
	th11sb_tail.updated = 0;
}
int _Th11sb_485_runtime( th11sb_t *th11sb, int step, int res, rtu_485_ack_t *runtime_ack)
{
	// 1 : this step run ok ; 0: send cmd fail need retry; -1 ack analize failed
	// ack analize ( step 1) cannot return 0, if failed , return -1;
	int ret;
	if( step == 0 ){
		//send cmd
		return Rtu_485_send_cmd(th11sb->addr, 3, 0 ,2);
	}else{
		//recive ack
		if( th11sb->addr == 0x0a )
			logd("rtu485 th11sb front: ");
		else 
			logd("rtu485 th11sb back: ");
		if( res == 1){
			if( th11sb->addr == runtime_ack->addr && runtime_ack->len == 4 ){
						//logd("th11sb updated\r\n");
						th11sb->tempture = ((runtime_ack->data[0]<<8) | runtime_ack->data[1]);
						th11sb->wet = ((runtime_ack->data[2]<<8) | runtime_ack->data[3]);
						th11sb->updated = 1;
						logd(" wet="); logd_num(th11sb_head.wet /10);
						logd_uint(" temp=",th11sb_head.tempture/10);
			}else{
				logd("wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd("timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}


/******************************* DAM  ***********************/

dam_t dam4_02,dam4_04,dam4_05,dam16_08,dam4_09;

void _dam_config()
{
	
	dam4_02.addr = 0x02;
	dam4_02.type = 4;
	dam4_02.status = 0;
	dam4_02.updated = 0;
	memset(dam4_02.input,0,4);
	
	dam4_04.addr = 0x04;
	dam4_04.type = 4;
	dam4_04.status = 0;
	dam4_04.updated = 0;
	
	dam4_05.addr = 0x05;
	dam4_05.type = 4;
	dam4_05.status = 0;
	dam4_05.updated = 0;
	
	dam16_08.addr = 0x08;
	dam16_08.type = 16;
	dam16_08.status = 0;
	dam16_08.updated = 0;
	
	dam4_09.addr = 0x09;
	dam4_09.type = 4;
	dam4_09.status = 0;
	dam4_09.updated = 0;
}
int _dam_485_runtime( dam_t *dam_dev, int step, int res, rtu_485_ack_t *runtime_ack)
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
		logd("rtu485 dam"); logd_num(dam_dev->addr);
		if( res == 1){
			if( dam_dev->addr == runtime_ack->addr ){
				if( runtime_ack->len == 2 ){
					dam_dev->status = ((runtime_ack->data[1]<<8) | runtime_ack->data[0]); 
				}else{
					dam_dev->status = runtime_ack->data[0];
				}
				dam_dev->updated = 1;
				logd_uint(" status=",dam_dev->status);
			}else{
				logd(" wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd(" timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}

int _dam_readInput_485_runtime( dam_t *dam_dev, char input_num, int step, int res, rtu_485_ack_t *runtime_ack)
{
	// 1 : this step run ok ; 0: send cmd fail need retry; -1 ack analize failed
	// ack analize ( step 1) cannot return 0, if failed , return -1;
	int ret,i;
	if( step == 0 ){
		//send cmd
		//查询n路继电器的状态
		return Rtu_485_send_cmd(dam_dev->addr, 2, input_num ,1);
	}else{
		//recive ack
		logd("rtu485 dam"); logd_num(dam_dev->addr);
		if( res == 1){
			if( dam_dev->addr == runtime_ack->addr ){
				if( runtime_ack->len == 1 && input_num<4){
					dam_dev->input[input_num] = runtime_ack->data[0];
					logd(" input");logd_num(input_num);
					logd_uint("=",dam_dev->input[input_num]);
				}else{
					logd(" input bad ack\r\n");
				}
			}else{
				logd(" input wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd(" input timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}

// num_id is from 1-4 or 1-16  enable : 1 or disable 0  , return 0 false 1 ok
int _dam_485_send_on_off_cmd(  dam_t *dam_dev, unsigned char num_id, int enable)
{
	unsigned short on_off;
	
	if( num_id > dam_dev->type ) return 0;
	on_off = (enable==DAM_CMD_ON)? 0xff00:0x0;
	return Rtu_485_Runtime_sendCmd( dam_dev->addr , 0x05, num_id-1 , on_off);
}

/*
// flash enable=1 闪开 enable=0 闪断, delaytime ：时间间隔 单位是0.1s 
int _dam_485_send_flash( dam_t *dam_dev, unsigned char num_id, int enable, int delayms)
{
	//FE    10   00 03    00 02     04       00 04       00 0A        00 D8
	//addr func  num_id  cmd count  cmd len   flash type  delay time   crc
	// flash type: 0004 闪开 0002 闪断 
	//delay time : 0x0a=10 = 10*0.1 s = 1s
	
	unsigned char cmd[13]={0};
	unsigned short crc,ms;
	if( num_id > dam_dev->type)  return 0;
	
	cmd[0] = dam_dev->addr;
	cmd[1] = 0x10;
	cmd[2] = 0x00;cmd[3]=num_id-1;
	cmd[4]= 0x00; cmd [5]= 0x02;
	cmd[6] = 0x04;
	cmd[7] = 0x00; cmd[8]= (enable==DAM_CMD_FLASH_OFF)? 0x02:0x04;
	ms = delayms/100;
	cmd[9] = ((ms>>8)&0xff); cmd[10] = (ms&0xff);
	crc = crc_calculate(cmd,11);
	cmd[11]=(crc&0xff);
	cmd[12]=((crc>>8) & 0xff);
	
	return Rtu_485_Runtime_send_RawCmd(cmd, 13);
}
*/

// 自定义的闪开与闪关的功能
volatile unsigned int _dam_flash_delay_ms = 0;
volatile unsigned int _dam_flash_ms_counter = 0;
unsigned char _dam_flash_numId=0;
unsigned char _dam_flash_on_off=0;
dam_t *_dam_flash_dev = NULL;

void _dam_flash_on_off_cmd(dam_t *dam_dev, unsigned char num_id, int enable, int delayms)
{
	if( _dam_flash_delay_ms > 0 && delayms <= 0 ) return ; // has flash cmd running
	
	_dam_flash_dev = dam_dev;
	_dam_flash_numId = num_id;
	_dam_flash_on_off = (enable==DAM_CMD_FLASH_OFF)? DAM_CMD_ON:DAM_CMD_OFF;
	_dam_flash_delay_ms = delayms;
	_dam_flash_ms_counter = 0;
	_dam_485_send_on_off_cmd(dam_dev,num_id, (enable==DAM_CMD_FLASH_OFF)? DAM_CMD_OFF:DAM_CMD_ON );
	
}
void _dam_flash_on_off_loop(uint32_t loop_ms)
{
	if( _dam_flash_delay_ms > 0 )
	{ // have set flash time
		_dam_flash_ms_counter += loop_ms;
		
		if( _dam_flash_ms_counter >= _dam_flash_delay_ms ){
			//is time to run flash cmd
			_dam_485_send_on_off_cmd( _dam_flash_dev, _dam_flash_numId, _dam_flash_on_off);
			_dam_flash_delay_ms = 0;
			_dam_flash_ms_counter = 0;
		}

	}
}



void Rtu_485_Dam_Cmd(unsigned char addr_id, unsigned char num_id, unsigned int cmd, unsigned int ms)
{
	//addr_id 485_addr
	//num_id the switch number 1-4 or 1-16
	//cmd =1 on ;0 close ; 2 flash off ; 3 flash on
	// ms : if on/close no need , if flash on/off for delay ms for keey on/off status
	 dam_t *dam_dev=NULL;
	switch (addr_id){
		case 2:{ dam_dev = &dam4_02; break;}
		case 4:{ dam_dev = &dam4_04; break;}
		case 5:{ dam_dev = &dam4_05; break;}
		case 8:{ dam_dev = &dam16_08; break;}
		case 9:{ dam_dev = &dam4_09; break;}
	}
	if( dam_dev == NULL ) return;
	if( cmd <= DAM_CMD_ON ){
		_dam_485_send_on_off_cmd( dam_dev, num_id, cmd);
	}else{
		//_dam_485_send_flash(dam_dev, num_id, cmd , ms);
		_dam_flash_on_off_cmd(dam_dev, num_id, cmd , ms);
	}
}



/******************************  Power ADC devices 电压采集板 *******************/
powerAdc6_t powerAdc6_01,powerAdc6_06,powerAdc6_07;

void powerAdc6_config()
{
	powerAdc6_01.addr = 0x03;
	powerAdc6_01.updated = 0;
	powerAdc6_06.addr = 0x06;
	powerAdc6_06.updated = 0;
	powerAdc6_07.addr = 0x07;
	powerAdc6_07.updated = 0;
}

int _powerAdc_485_runtime( powerAdc6_t *padc_dev, int step, int res, rtu_485_ack_t *runtime_ack)
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
		logd("rtu485 powerAdc");logd_num(padc_dev->addr);
		if( res == 1){
			if( padc_dev->addr == runtime_ack->addr ){
				//TODO
				if( runtime_ack->len == 12){
					for(i=0; i< runtime_ack->len; i+=2){
						padc_dev->adc[i/2] = ((runtime_ack->data[i]<<8) | runtime_ack->data[i+1] );
						logd(" ,");logd_num(padc_dev->adc[i/2]);
					}
					padc_dev->updated = 1;
					logd("\r\n");
				}else{
					logd(" error ack\r\n");
					return -1;
				}
			}else{
				logd(" wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd(" timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}



/********************************8   speed pgw636 ******************************/
 pgw636_t pgw636_03;

void pgw636_config()
{
	pgw636_03.addr = 0x01;
	pgw636_03.curren_speed = 0;
	pgw636_03.max_speed = 0;
	pgw636_03.min_speed = 0;
	pgw636_03.updated = 0;
}


int _pgw636_485_runtime( pgw636_t *pgw_dev, int step, int res, rtu_485_ack_t *runtime_ack)
{
	// 1 : this step run ok ; 0: send cmd fail need retry; -1 ack analize failed
	// ack analize ( step 1) cannot return 0, if failed , return -1;
	int ret,i;
	if( step == 0 ){
		//send cmd
		//查询当前速度，最大速度，最小速度  
		return Rtu_485_send_cmd(pgw_dev->addr, 3, 0 ,2);
	}else{
		//recive ack
		logd("rtu485 ");
		if( res == 1){
			if( pgw_dev->addr == runtime_ack->addr ){
				//TODO
				if( runtime_ack->len == 4){
					//current => (reg1<<8 | reg0 )  reg0= data[0](reg0_H)<<8 | data[1](reg0_L)  reg1 = data[2](reg1_H) << 8 | data[3](reg1_L)
					pgw_dev->curren_speed = (int)(runtime_ack->data[2]<<24) | (runtime_ack->data[3]<<16) | (runtime_ack->data[0]<<8) | (runtime_ack->data[1]) ;
					//pgw_dev->max_speed = (int)(runtime_ack->data[6]<<24) | (runtime_ack->data[7]<<16) | (runtime_ack->data[4]<<8) | (runtime_ack->data[5]) ;
					//pgw_dev->min_speed = (int)(runtime_ack->data[10]<<24) | (runtime_ack->data[11]<<16) | (runtime_ack->data[8]<<8) | (runtime_ack->data[9]) ;
					pgw_dev->updated = 1;
					logd_uint("pgw636 =",pgw_dev->curren_speed);
				}else{
					logd("pgw636 error ack\r\n");
					return -1;
				}
			}else{
				logd("pgw636 wrong ack\r\n");
				return -1;
			}
		}else{
			//timeout or failed
			logd("pgw636 timeout ack\r\n");
			return -1;
		}
		return 1;
	}
}

















/***************************************** Rtu 485 Device list config */

#define _485_LOOP_MS 100  // each 50ms enter loop 
unsigned int _rtu_485_devices_report_hz = 0;
unsigned int _rtu_485_devices_report_counter = 0;
int _rtu_485_devices_runtime(int step, int res, rtu_485_ack_t *runtime_ack)
{
	// if step=0; return 0 will reenter step 0;
	static uint8_t _485_device_seq = 0;
	int ret;
	
	switch(_485_device_seq){
#if 1
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
			ret = _powerAdc_485_runtime(&powerAdc6_01,step,res,runtime_ack);
			break;
		}
		case 8:{
			ret = _powerAdc_485_runtime(&powerAdc6_06,step,res,runtime_ack);
			break;
		}
		case 9:{
			ret = _powerAdc_485_runtime(&powerAdc6_07,step,res,runtime_ack);
			break;
		}
		
		case 10:{
			ret = _pgw636_485_runtime(&pgw636_03,step,res,runtime_ack);
			break;
		}
		
		case 11:{
			//low oil warning
			ret = _dam_readInput_485_runtime(&dam4_02, 1, step,res,runtime_ack);
			break;
		}
		
		case 12:{
			// high water tempature
			ret = _dam_readInput_485_runtime(&dam4_02, 2, step,res,runtime_ack);
			break;
		}
		
		default:
		{
			_485_device_seq = 0;
			ret = 0;
			_rtu_485_devices_report_counter ++;
			break;
		}

#else //test
		/*
		case 0:{
			ret = _powerAdc_485_runtime(&powerAdc6_06,step,res,runtime_ack);
			break;
		}
		case 1:{
			ret = _dam_485_runtime(&dam4_02, step,res,runtime_ack);
			break;
		}
		case 2:{
			ret = _Th11sb_485_runtime(&th11sb_head,step,res,runtime_ack);
			break;
		}*/
		case 0:{
			ret = _powerAdc_485_runtime(&powerAdc6_06,step,res,runtime_ack);
			break;
		}		
		case 1:{
			_485_device_seq = 0;
			ret = 0;
			_rtu_485_devices_report_counter ++;
			break;
		}
#endif
	}
	if ( step >= 1){
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
				logd_uint("rtu485 send cmd,addr=",_rtu_485_sendcmd[0]);
				return Rtu_485_send_raw_cmd(_rtu_485_sendcmd,len);
			}
		}
		return 0;
	}else{
		//handle ack
		if( res == 1) logd("rtu485 send cmd ok\r\n");
		else logd("rtu485 send cmd false\r\n");
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
		logd("rtu485 cmd buff full\r\n");
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
	//logd_uint("get cmd addr=",addr);
	return 1;
}
int Rtu_485_Runtime_send_RawCmd(unsigned char *data, unsigned char len)
{
	int free_count,i;

	free_count = fifo_free(&rtu_485_cmd_fifo) ;
	if( free_count < len+1 ){
		logd("rtu485 cmd buff full\r\n");
		return 0;
	}
	if( len > RTU_485_CMD_LEN ){
		logd("rtu485 cmd too long\r\n");
		return 0;
	}
	
	//put len
	fifo_put(&rtu_485_cmd_fifo,len);
	for( i=0; i< len; i++){
		fifo_put(&rtu_485_cmd_fifo,data[i]);
	}
	//logd_uint("get raw cmd addr=",data[0]);
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
		}
	}
	
}






/************************************************ main func *************/

void Rtu_485_Runtime_Configure()
{
	_Th11sb_config();
	_dam_config();
	powerAdc6_config();
	pgw636_config();
	fifo_init(&rtu_485_cmd_fifo, rtu_485_cmd_fifo_buff, RTU_485_CMD_FIFI_BUFF_LEN);
	systick_time_start(&rtu_485_time_t,_485_LOOP_MS);
}

#define _485_CMD_LOOP 1
#define _485_DEVICES_LOOP 2
volatile char _rtu_485_loop_type = _485_DEVICES_LOOP;
unsigned int loop_counter=0;
volatile char _rtu485_device_stop = 0;

void Rtu_485_Runtime_stop(int stop)
{
	_rtu485_device_stop = stop;
}

void Rtu_485_Runtime_loop()
{
	char _to_send_cmd = 0;
	
	if( check_systick_time(&rtu_485_time_t) )
	{
		// check flash cmd 
		_dam_flash_on_off_loop(_485_LOOP_MS);
		
		// check report hz
		loop_counter++;
		if( loop_counter >= (1000/_485_LOOP_MS) )
		{
			loop_counter = 0;
			_rtu_485_devices_report_hz = _rtu_485_devices_report_counter;
			_rtu_485_devices_report_counter = 0;
			logd_uint("rtu485 report hz=",_rtu_485_devices_report_hz);
		}
		
		//check cmd	
		if( 8 < fifo_avail(&rtu_485_cmd_fifo))
			_to_send_cmd = 1;
		
		//check stop tag
		if( _rtu485_device_stop == 1 ){
			if( _rtu_485_runtime_running == 0 )
				return;
		}
		
		// run runtime loop
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
		}else if( _rtu_485_loop_type == _485_CMD_LOOP) {
			if( _to_send_cmd == 0  && _rtu_485_runtime_running == 0)
			{
				//switch device read data loop
				_rtu_485_loop_type = _485_DEVICES_LOOP;
				Rtu_485_Devices_runtime_loop();
			}else{
				Rtu_485_SendCmd_runtime_loop();
			}
		}else{
			//stop runtime
			logd("rtu485 error mode\r\n");
		}
	}
}






//End of File
