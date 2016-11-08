#include <Driver_USART.h>
#include "protocol.h"
#include "usart2.h"

extern ARM_DRIVER_USART Driver_USART2;

static uint8_t rev_buffer[11] = { 0 };

static void OnUsart2Event(uint32_t event) {
    uint8_t packet[(sizeof(rev_buffer) + 3) * 2] = { 0 };
    int32_t pack_len = 0;
    
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
        rev_buffer[0] = (TRANSFER_NEXT << 6) + (PORT_USART << 3) + 1;
        pack_len = Pack_Data(rev_buffer, sizeof(rev_buffer), packet, sizeof(packet));
        
        if (pack_len > 0) {
            Send_To_Port(packet, pack_len, PORT_USART, 0);
        }
        
        Driver_USART2.Receive(rev_buffer + 1, sizeof(rev_buffer) - 1);
    }
}

void USART2_Init(int bitrate) {
    Driver_USART2.Initialize(OnUsart2Event);
    Driver_USART2.PowerControl(ARM_POWER_FULL);
    Driver_USART2.Control(ARM_USART_MODE_ASYNCHRONOUS | 
                               ARM_USART_DATA_BITS_8 | 
                               ARM_USART_PARITY_NONE | 
                               ARM_USART_STOP_BITS_1 | 
                               ARM_USART_FLOW_CONTROL_NONE, bitrate);
    Driver_USART2.Control(ARM_USART_CONTROL_TX, 1);
    Driver_USART2.Control(ARM_USART_CONTROL_RX, 1);
    
    Driver_USART2.Receive(rev_buffer + 1, sizeof(rev_buffer) - 1);
}
