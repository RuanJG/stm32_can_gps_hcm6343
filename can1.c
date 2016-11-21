#include <string.h>
#include <Driver_CAN.h>
#include "ringbuffer.h"
#include "protocol.h"
#include "can1.h"
#include "custom.h"

#define CAN_BUF_MAX                   10

extern ARM_DRIVER_CAN   Driver_CAN1;
uint32_t rx_obj_idx  = 0xFFFFFFFFU;
uint32_t tx_obj_idx  = 0xFFFFFFFFU;

uint8_t can_buffer[CAN_BUF_MAX][11] = { 0 };
uint32_t can_r_index                = 0;
uint32_t can_w_index                = 0;


static void Handle_Can1_Packet(uint8_t *data, uint32_t size) {
    int32_t packet_len = 0;
    if (size <= 2) { // Empty packet.
        return;
    }
    
    packet_len = Extract_Packet(data, size);
    if (packet_len < 1) { // Invalid packet.
        return;
    }
    
    Handle_Packet_Down(data, packet_len);
}


static uint8_t     can1_rev_buf[MAX_PACKET_SIZE] = { 0 };
static uint32_t    can1_buf_size = 0;
static void can1_protocol_parse(uint8_t revByte) {

    if (PACKET_START == revByte) { // Wait for the start byte.
        can1_rev_buf[0] = PACKET_START;        
        can1_buf_size = 1;
    }else{
			
			if( can1_buf_size > 0 )
			{
				can1_rev_buf[can1_buf_size++] = revByte;
				
				if (PACKET_END == revByte) 
				{ // Entire packect received.
					Handle_Can1_Packet(can1_rev_buf, can1_buf_size);
					can1_buf_size = 0;
					return;
				}
				
				if (can1_buf_size >= MAX_PACKET_SIZE) { // Packet too big!
					can1_buf_size = 0;
				}
			}
			
		}
}













static void SaveToBuffer(uint16_t id, uint8_t *data, uint32_t size) {
    uint8_t *arr = can_buffer[can_w_index];
    uint32_t next_w_index = ((can_w_index + 1) % CAN_BUF_MAX);
    
    if (can_r_index == next_w_index) { // full
        return;
    }
    
    if (size > 8) {
        size = 8;
    }
    
    arr[0] = ((id >> 8) & 0xFF);
    arr[1] = (id & 0xFF);
    arr[2] = (size & 0xFF);
    memcpy(arr + 3, data, size);
    can_w_index = next_w_index;
}

static void Begin_Send() {
    ARM_CAN_MSG_INFO tx_msg_info = { 0 };
    uint8_t *arr;
    uint16_t canId;

    if (can_r_index == can_w_index) { // empty
        return;
    }
    
    arr = can_buffer[can_r_index];
    canId = ((arr[0] << 8) & 0xFF00) + (arr[1] & 0xFF);
    tx_msg_info.id = ARM_CAN_STANDARD_ID(canId);
    tx_msg_info.dlc = arr[2];
    
    if (Driver_CAN1.MessageSend(tx_obj_idx, &tx_msg_info, arr + 3, arr[2]) < 0) {
        return;
    } 
    
    can_r_index = ((can_r_index + 1) % CAN_BUF_MAX);
}



static void CAN_SignalObjectEvent (uint32_t obj_idx, uint32_t event) {
    ARM_CAN_MSG_INFO rx_msg_info;
    uint8_t rx_data[8];
		uint8_t i;
    
    if ((obj_idx == rx_obj_idx) && (ARM_CAN_EVENT_RECEIVE == event)) {
        if (Driver_CAN1.MessageRead(rx_obj_idx, &rx_msg_info, rx_data , 8U) <= 0) {
            return;
        }
        for( i=0; i< rx_msg_info.dlc ; i++)
				{
					can1_protocol_parse(rx_data[i]);
				}

        return;
    }
    
    if ((obj_idx == tx_obj_idx) && (ARM_CAN_EVENT_SEND_COMPLETE == event)) {
        Begin_Send();
        return;
    }
}

void CAN1_Init() {
    ARM_CAN_CAPABILITIES     can_cap;
    ARM_CAN_OBJ_CAPABILITIES can_obj_cap;
    uint32_t                 index;
    
    can_cap = Driver_CAN1.GetCapabilities ();
    Driver_CAN1.Initialize(NULL, CAN_SignalObjectEvent);
    Driver_CAN1.PowerControl(ARM_POWER_FULL);
    Driver_CAN1.SetMode(ARM_CAN_MODE_INITIALIZATION);
    Driver_CAN1.SetBitrate(ARM_CAN_BITRATE_NOMINAL,  1000000U, // 500K
                           ARM_CAN_BIT_PROP_SEG(1U) |
                           ARM_CAN_BIT_PHASE_SEG1(2U) |
                           ARM_CAN_BIT_PHASE_SEG2(5U) |
                           ARM_CAN_BIT_SJW(1U));
    for (index = 0U; index < can_cap.num_objects; index++) {
        can_obj_cap = Driver_CAN1.ObjectGetCapabilities(index);
        if ((0xFFFFFFFFU == rx_obj_idx) && (1U == can_obj_cap.rx)) {
            rx_obj_idx = index;
        } else if ((0xFFFFFFFFU == tx_obj_idx) && (1U == can_obj_cap.tx)) {
            tx_obj_idx = index;
            break;
        }
    }

    Driver_CAN1.ObjectSetFilter(rx_obj_idx, ARM_CAN_FILTER_ID_MASKABLE_ADD, ARM_CAN_STANDARD_ID(CUSTOM_CAN1_ID), CUSTOM_CAN1_MASK);
    Driver_CAN1.ObjectConfigure(rx_obj_idx, ARM_CAN_OBJ_RX);
    Driver_CAN1.ObjectConfigure(tx_obj_idx, ARM_CAN_OBJ_TX);
    Driver_CAN1.SetMode(ARM_CAN_MODE_NORMAL);
}

void CAN1_Send(uint16_t id, uint8_t *data, uint32_t size) {
    if (size > 8) {
        size = 8;
    }
    
    SaveToBuffer(id, data, size);
    Begin_Send();
}
