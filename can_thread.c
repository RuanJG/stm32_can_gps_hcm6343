#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
 
#include "Driver_CAN.h"
 
#include "Driver_USART.h"
extern ARM_DRIVER_USART Driver_USART1;
void putChar(unsigned char data){
	ARM_DRIVER_USART * uart = &Driver_USART1;
	uart->Send(&data,1);
}
void putBytes(unsigned *data, int len)
{
		ARM_DRIVER_USART * uart = &Driver_USART1;
	uart->Send(&data,len);
}

void can1_thread (void const *argument);
 osThreadId tid_can1_Thread;
 
 
// CAN Driver Controller selector
#define  CAN_CONTROLLER         1       // CAN Controller number
 
#define _CAN_Driver_(n)         Driver_CAN##n
#define  CAN_Driver_(n)        _CAN_Driver_(n)
extern   ARM_DRIVER_CAN         CAN_Driver_(CAN_CONTROLLER);
#define  ptrCAN               (&CAN_Driver_(CAN_CONTROLLER))
 
uint32_t                        rx_obj_idx  = 0xFFFFFFFFU;
uint8_t                         rx_data[8];
ARM_CAN_MSG_INFO                rx_msg_info;
uint32_t                        tx_obj_idx  = 0xFFFFFFFFU;
uint8_t                         tx_data[8];
ARM_CAN_MSG_INFO                tx_msg_info;



#define CAN1_SEND_OK_SIGNAL 0x1
#define CAN1_RECIVE_OK_SIGNAL 0x2
static void Error_Handler (void) 
{ 
	Driver_USART1.Send("can1 error\r\n",12);
//while (1); 
}
 
void CAN_SignalUnitEvent (uint32_t event) {}
 
void CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event) {
	int i;
  if (obj_idx == rx_obj_idx) {                  // If receive object event
    if (event == ARM_CAN_EVENT_RECEIVE) {       // If message was received successfully
      if (ptrCAN->MessageRead(rx_obj_idx, &rx_msg_info, rx_data, 8U) > 0U) {
                                                // Read received message
        Driver_USART1.Send("get can msg \r\n",14);
				osSignalSet(tid_can1_Thread, CAN1_RECIVE_OK_SIGNAL);
      }
    }
  }
  if (obj_idx == tx_obj_idx) {                  // If transmit object event
    if (event == ARM_CAN_EVENT_SEND_COMPLETE) { // If message was sent successfully
      ;//Driver_USART1.Send("send can msg\r\n",14);
			osSignalSet(tid_can1_Thread, CAN1_SEND_OK_SIGNAL);
    }
  }
}
 
int can1_send( uint32_t id, uint8_t *data, int len)
{
	int32_t   status;
	int leave_len;
	leave_len = len;
	
	do{
	osSignalWait(CAN1_SEND_OK_SIGNAL, osWaitForever);
	memset(&tx_msg_info, 0U, sizeof(ARM_CAN_MSG_INFO));
	tx_msg_info.id = ARM_CAN_EXTENDED_ID(id);
	status = ptrCAN->MessageSend(tx_obj_idx, &tx_msg_info, &data[len-leave_len], leave_len);
	if( status > 0)
		leave_len -= status;
	else
		return 0;
	}while( leave_len > 0 );
	
	return 1;
}

void can1_init()
{
	ARM_CAN_CAPABILITIES     can_cap;
  ARM_CAN_OBJ_CAPABILITIES can_obj_cap;
  int32_t                  status;
  uint32_t                 i, num_objects;
 
  can_cap = ptrCAN->GetCapabilities (); // Get CAN driver capabilities
  num_objects = can_cap.num_objects;    // Number of receive/transmit objects
 
  status = ptrCAN->Initialize    (CAN_SignalUnitEvent, CAN_SignalObjectEvent);  // Initialize CAN driver
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->PowerControl  (ARM_POWER_FULL);                              // Power-up CAN controller
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->SetMode       (ARM_CAN_MODE_INITIALIZATION);                 // Activate initialization mode
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->SetBitrate    (ARM_CAN_BITRATE_NOMINAL,              // Set nominal bitrate
                                  1000000U,                      //1M        // Set bitrate to 100 kbit/s
                                  ARM_CAN_BIT_PROP_SEG(2U)   |          // Set propagation segment to 5 time quanta
                                  ARM_CAN_BIT_PHASE_SEG1(1U) |          // Set phase segment 1 to 1 time quantum (sample point at 87.5% of bit time)
                                  ARM_CAN_BIT_PHASE_SEG2(5U) |          // Set phase segment 2 to 1 time quantum (total bit is 8 time quanta long)
                                  ARM_CAN_BIT_SJW(1U));                 // Resynchronization jump width is same as phase segment 2
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  for (i = 0U; i < num_objects; i++) {                                          // Find first available object for receive and transmit
    can_obj_cap = ptrCAN->ObjectGetCapabilities (i);                            // Get object capabilities
    if      ((rx_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.rx == 1U)) { rx_obj_idx = i; }
    else if ((tx_obj_idx == 0xFFFFFFFFU) && (can_obj_cap.tx == 1U)) { tx_obj_idx = i; break; }
  }
  if ((rx_obj_idx == 0xFFFFFFFFU) || (tx_obj_idx == 0xFFFFFFFFU)) { Error_Handler(); }
 
  // Set filter to receive messages with extended ID 0x12345678 to receive object
  status = ptrCAN->ObjectSetFilter(rx_obj_idx, ARM_CAN_FILTER_ID_EXACT_ADD, ARM_CAN_EXTENDED_ID(0x10U), 0U);
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->ObjectConfigure(tx_obj_idx, ARM_CAN_OBJ_TX);                 // Configure transmit object
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
 
  status = ptrCAN->ObjectConfigure(rx_obj_idx, ARM_CAN_OBJ_RX);                 // Configure receive object
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
  
  status = ptrCAN->SetMode (ARM_CAN_MODE_NORMAL);//ARM_CAN_MODE_LOOPBACK_INTERNAL);                               // Activate normal operation mode
  if (status != ARM_DRIVER_OK ) { Error_Handler(); }
	
}


void can1_thread (void const *argument) {
  uint8_t txbuff[12]={1,2,3,4,5,6,7,8,9,0,1,2};
	
	can1_init();
	
	osSignalSet(tid_can1_Thread, CAN1_SEND_OK_SIGNAL); 
	//can1_send(0x10, txbuff, 12);	// Initialize transmit data
  while (1) {
		osSignalWait(CAN1_RECIVE_OK_SIGNAL, osWaitForever);
		can1_send(0x10, rx_data, rx_msg_info.dlc);
    //osDelay(1000);                                // Wait a little while
  }
}

