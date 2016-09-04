#include <stm32f10x.h>		
#include "stdio.h"	
#include  <ctype.h>	
#include <stdlib.h>
#include <math.h>
#include <string.h>	
#include "system.h"
#include "esc_box.h"


#if M80_ESC_BOX
	

systick_time_t report_t;
systick_time_t led_t;
systick_time_t ke4_speed_t;
systick_time_t debug_t;
Uart_t Uart1 ;
Uart_t Uart2 ;
Uart_t Uart3 ;

CanTxMsg TxMessage1;
CanRxMsg RxMessage1;









void uart_echo_test(Uart_t* uartp)
{
	char data;
	if( Uart_GetChar(uartp, &data) > 0 ){
		Uart_PutChar(uartp,data);
	}
}
CanRxMsg rxmsg;
CanTxMsg txmsg;
void test_can_send()
{
	int i;
	uint8_t data[12]={0xff,0x1,0x3,0xfe,0xab,0x12,0xff,0x1,0x3,0xfe,0xab,0x12};
	//Can1_Send(0x10,data,12);
	Can1_Send_Ext(0x10, data, 12, CAN_ID_EXT, CAN_RTR_DATA);
	Esc_Led_toggle(LED_YELLOW_ID);
}
void test_can1()
{
	if( Can1_Get_CanRxMsg(&rxmsg) ){
		//Can1_Send(0x10,rxmsg.Data,rxmsg.DLC);
		Can1_Send_Ext(0x10, rxmsg.Data, rxmsg.DLC, CAN_ID_EXT, CAN_RTR_DATA);
	}
}
void test_uart123_can1()
{
	test_can1();
	uart_echo_test(&Uart1);
	uart_echo_test(&Uart3);
	//uart_echo_test(&Uart2);
}


void print_uint( Uart_t *uart, int n)
{
	int intt,les,count=0;
	int num = n;
	char data[16]={0};
	while(1){
		intt = num/10;
		les = num%10;
		data[count++] = 0x30+les;
		
		if( intt == 0 )
			break;
		
		num = intt;
	}
	while(count-->0){
		Uart_PutChar(uart, data[count]);
	}
	
}

int str_to_int(char *data, int len)
{
	int i;
	int res=0,muli=1;
	for( i=(len-1) ; i>=0; i--)
	{
		res += (data[i]-0x30)*muli;
		muli*=10;
	}
	return res;
}
int read_num_by_uart(Uart_t *uart ,uint32_t *num)
{
	static char data[32];// #200#, num is 200
	static char tag = 0;
	static char index = 0;
	char tmp;
	if( Uart_GetChar(uart, &tmp) > 0 ){
		switch(tag)
		{
			case 0:{
				if( tmp == '#'){
					tag++;
					index = 0;
				}
				break;
			}
			case 1:{
				if( tmp == '#' && index > 0){
					*num = str_to_int(data,index);
					tag = 0;
					index = 0;
					return 1;
				}else{
					data[index++]=tmp;
				}
				break;
			}
		}
	}
	return 0;
}
void yaw_set_yaw_pwm(uint16_t pwm);
void test_yaw_pwm(Uart_t *uarts)
{
	uint32_t pwm=0;

	while(read_num_by_uart(uarts, &pwm ) == 0) delay_us(1000);

		logd_uint("yaw pwm=",pwm);
		yaw_set_yaw_pwm(pwm);
	
}

void Esc_Yaw_Control_SetAngle(uint16_t  angle);
void test_yaw_angle(Uart_t *uarts)
{
	uint32_t pwm=0;

	while(read_num_by_uart(uarts, &pwm ) == 0) delay_us(1000);

		logd_uint("test angle=",pwm);
		Esc_Yaw_Control_SetAngle(pwm);
	
}
void test_pitch(Uart_t *uarts)
{
	char pwm;
	while( Uart_GetChar(uarts, &pwm) <= 0 )delay_us(1000);
	if( pwm == 'm'){
		logd("middle\r\n");
		Esc_Pump_Pitch_Middle();
	}else if( pwm == 'f'){
		logd("forward\r\n");
		Esc_Pump_Pitch_Forward();
	}else if( pwm == 'b'){
		logd("back\r\n");
		Esc_Pump_Pitch_Back();
	}
}
#if 0
#define _yaw_control_shutdown() 	GPIO_ResetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN)
#define _yaw_control_poweron()  GPIO_SetBits(H_BRIDGE_A_PWMB_GPIO_BANK,H_BRIDGE_A_PWMB_GPIO_PIN);
#else
void _yaw_control_shutdown();
void _yaw_control_poweron();
#endif
void _yaw_control_forward();
void _yaw_control_back();
void test_h_bridge(Uart_t *uarts)
{
	unsigned char pwm=0;

	while( Uart_GetChar(uarts, &pwm) <= 0 )delay_us(1000);

		if( pwm == 'l' ){
			Uart_PutString(uarts,"left\r\n");
			_yaw_control_forward();
		}else if( pwm == 's'){
			Uart_PutString(uarts,"stop\r\n");
			_yaw_control_shutdown();
		}else if( pwm == 'r'){
			Uart_PutString(uarts,"right\r\n");
			_yaw_control_back();
		}
	
}

unsigned char char_to_hex(char data)
{
	//'a'=0x61 62 63 64 65 66 'A'=0x41 42 43 44 45 46;  0=0x30 - 0x39
	// ['0' 'f'] = 0x0f 

	if( data >= 0x61)
		return (0x0a+(data-0x61));
	
	else if (data >= 0x41 )
		return (0x0a+(data-0x41));
	
	else 
		return (data-0x30);
	
}
void cmd_uart_485cmd(Uart_t *uarts)
{
	char tmp,data[32];
	unsigned char sendbuff[32];
	int len,i,j;
	unsigned short crc;
	
	
	
	// cmd : #3#3010203 , send  3byte 0x01 0x02 0x03 to uartd 
	while( Uart_GetChar(uarts, &tmp) <= 0 )delay_us(1000);
	//first byte is len
	len = char_to_hex(tmp)*2;
	
	for( i=0 ; i< len; i++){
		while( Uart_GetChar(uarts, &tmp) <= 0 ) delay_us(1000);
		data[i] = tmp;
	}
	
	for( i=0,j=0; i< len;)
	{
		tmp = char_to_hex(data[i])*16 + char_to_hex(data[i+1]);
		sendbuff[j++]=tmp;
		i+=2;
	}
	crc = crc_calculate(sendbuff,j);
	sendbuff[j++]=(crc&0xff);
	sendbuff[j++]=((crc>>8)&0xff);
	
	Rtu_485_Runtime_send_RawCmd(sendbuff,j);
	
}

void dam_control_test(Uart_t *uarts)
{
	char tmp;
	char numid,cmd,addr;
	int ms;
	
	// cmd : #4#410 , control addr=4 ,the 1 path switch, cmd 0 [0 off,1 on,2 flash off,3 flash on] 
	// cmd : #4#41340 , cmd 3 flash on , delay 4s
	while( Uart_GetChar(uarts, &tmp) <= 0 )delay_us(1000);
	addr = tmp-0x30;
	while( Uart_GetChar(uarts, &tmp) <= 0 )delay_us(1000);
	numid = char_to_hex(tmp);
	while( Uart_GetChar(uarts, &tmp) <= 0 )delay_us(1000);
	cmd = tmp-0x30;
	if( cmd >= 2) 
	{// delay time for flash on off
		while( Uart_GetChar(uarts, &tmp) <= 0 )delay_us(1000);
		ms = 10*(tmp-0x30);
		while( Uart_GetChar(uarts, &tmp) <= 0 )delay_us(1000);
		ms = (ms+(tmp-0x30))*100;
	}
	
	Rtu_485_Dam_Cmd(addr,numid,cmd,ms);
}

//#6#id[][][][][][][][]
			//00 00 00   03 7D  7F 01  00 00 
void test_can_send_msg(Uart_t *uarts)
{
	int i;
	CanTxMsg TxMsg;
	char id,tmp;
	unsigned char data[8];
	
	while( Uart_GetChar(uarts, &tmp ) <= 0 )delay_us(1000);
	id = char_to_hex(tmp); id = id << 8;
	while( Uart_GetChar(uarts, &tmp ) <= 0 )delay_us(1000);
	id |= char_to_hex(tmp);
	
	for( i=0; i<8 ; i++)
	{
		while( Uart_GetChar(uarts, &tmp ) <= 0 )delay_us(1000);
		data[i] = char_to_hex(tmp); data[i] = data[i] << 8;
		while( Uart_GetChar(uarts, &tmp ) <= 0 )delay_us(1000);
		data[i] |= char_to_hex(tmp);
	}
	
	//Can1_Send(id, data, 8);
	Can1_Send_Ext(id, data, 8, CAN_ID_EXT, CAN_RTR_DATA);
}




void test_ke4(Uart_t *uarts)
{//#7##500# [500 - 1000] {500-700}
	uint32_t pwm=0;

	while(read_num_by_uart(uarts, &pwm ) == 0) delay_us(1000);

	logd_uint("test ke4=",pwm);
	//_set_ke4_speed(pwm);
	Ke4_Set_Speed(pwm);
}

extern volatile char main_control_stop_listen_dma;
void listen_cmd(Uart_t *uart)
{
	uint32_t cmd;
	char tmp;
	
	if(read_num_by_uart(uart, &cmd ) == 1)
	{
		if(cmd == 0  ){
			Uart_PutString(uart,"jump \r\n");
			Iap_Jump();
		}
		
		if( cmd == 1 ){
			//#1##1800# 0-2000
			test_yaw_pwm(&Uart1);
		}
		if( cmd == 2 ){
			//#2##1000# 1000 - 2000
			test_yaw_angle(&Uart1);
		}
		if( cmd == 3 ){
			//#3#s[s,l,r]
			test_h_bridge(&Uart1);
		}
		if(cmd==4){
			//uart ×ª·¢ #4#[len][byte][][][][] : len=byte's count; byte= hex( 0= 00 1= 01 10= 0a ...)
			// #4#6010400010002 addr=0x01 func=0x04 reg=0x0001 len=0x0002
			cmd_uart_485cmd(&Uart1);
		}
		if(cmd==5){
			//#5#[485addr][1,2,3,4...][0:off, 1:on, 2:flash off: 3: flash on]{[s]}
			//#5#210   off 1 path
			//#5#21330  flash on 1 path , delay 3s
			dam_control_test(&Uart1);
		}
		if(cmd==6){
			//#6#id[][][][][][][][]
			//00 00 00   03 7D  7F 01  00 00 
			test_can_send_msg(&Uart1);
		}
		if( cmd == 7 ){
			//#7##0# [0-1000]
			test_ke4(&Uart1);
		}
		if( cmd == 8 ){
			//#8#m [m,f,b]
			test_pitch(&Uart1);
		}
		if( cmd == 9 ){
			//#9#1 [1 0] //1:stop 485 runtime 0 : continue 485 runtime
			while( Uart_GetChar(&Uart1, &tmp) <= 0 ) delay_us(1000);
			if( tmp == '0' )
				Rtu_485_Runtime_stop(0);
			else if( tmp == '1')
				Rtu_485_Runtime_stop(1);
		}
	}
}











uint32_t speeker_ms = 1;
void dam_devices_second_init()
{
		
	//TODO : make sure these cmd ok
	static int inited = 0;
	
	if( inited == 1 )
		return;
	
	Rtu_485_Dam_Cmd(0x08,9,1,0);
	Rtu_485_Dam_Cmd(0x08,7,1,0);
	Rtu_485_Dam_Cmd(0x08,8,1,0);
	Rtu_485_Dam_Cmd(0x08,3,1,0);
	Rtu_485_Dam_Cmd(0x08,4,1,0);
	Rtu_485_Dam_Cmd(0x08,5,1,0);
	Rtu_485_Dam_Cmd(0x08,6,1,0);
	Rtu_485_Dam_Cmd(0x08,15,1,0);
	
	inited = 1;
	//Rtu_485_Dam_Cmd(0x08,3,DAM_CMD_FLASH_ON,10000);
}







void main_setup()
{
	SetupPllClock(HSE_CLOCK_6MHZ);
	Esc_GPIO_Configuration();
	//Can1_Configuration (0x12);
	Can1_Configuration_withRate(0x12,CAN_ID_EXT,CAN_SJW_1tq,CAN_BS1_5tq,CAN_BS2_2tq,9);
	Uart_Configuration (&Uart1, USART1, IAP_UART_BAUDRATE, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart2, USART2, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Uart_Configuration (&Uart3, USART3, 9600, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
	Esc_Pump_Pitch_Config();
	Esc_Yaw_Control_Configure(); 
	//Esc_Led_Configuration();
	
	//time_t init 
	systick_time_start(&report_t,CAN1_LISTENER_REPORT_STATUS_MS);//REPORT_STATUS_MS);
	systick_time_start(&led_t,100);
	systick_time_start(&ke4_speed_t,500);
	systick_time_start(&debug_t,500);
	
	
	//system error 
	//system_error = system_error_get();
	
	Iap_Configure(&Uart1);
	
	Rtu_485_Configure(&Uart2);
	Rtu_485_Runtime_Configure(); //  base on rtu_485
	
	Esc_Limit_Configuration();

}


void main_loop()
{
	Listen_Can1();
	
	Esc_Yaw_Control_Event();

	Rtu_485_Event();
	Rtu_485_Runtime_loop(); // base on rtu_485

#if 0  // control by remoter
	if( check_systick_time(&ke4_speed_t) ){
		Ke4_Speed_Control_Loop();
	}
#endif 
	
	if( check_systick_time(&report_t) ){
		Can1_Listener_Report_Event();
		Can1_Listener_Check_connect_event();
	}
	
	if( check_systick_time(&led_t) ){
		//Esc_Led_Event();
		
		//check leak water gpio 
		esc_check_limit_gpio_loop();
		
		//delay 1s to turn on dam 
		if( speeker_ms > 0 )
		{
			speeker_ms += 100;
			
			if( speeker_ms > 11000){
				Rtu_485_Dam_Cmd(0x08,3,0,0);
				speeker_ms = 0;
			}else if( speeker_ms >= 1000){
				dam_devices_second_init();
				//Rtu_485_Dam_Cmd(0x08,3,1,0);
			}
		}
		
	}
	
	if( check_systick_time(&debug_t) )
	{
		logd_uint("current: ",Esc_Yaw_Control_GetCurrentAdc());
		//logd_uint("angle:   ",Esc_Yaw_Control_GetAngleAdc());
		logd_uint("oil mass:",Esc_Yaw_Control_GetOilMassAdc());
		Esc_Yaw_Control_print_status();
		logd_uint("leak status=",get_esc_limit_gpio_status());
	}
	
	listen_cmd(&Uart1);
}





#endif 






//End of file
