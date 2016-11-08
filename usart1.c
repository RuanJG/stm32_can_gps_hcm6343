#include <Driver_USART.h>
#include "ringbuffer.h"
#include "protocol.h"
#include "usart1.h"

extern ARM_DRIVER_USART Driver_USART1;

static uint8_t     uart_send_buf[32] = { 0 };
static uint8_t     uart1_rev_buf[MAX_PACKET_SIZE] = { 0 };
static uint32_t    uart1_buf_size = 0;
DECLEAR_RINGBUFFER(MAX_PACKET_SIZE * 2, 1, uart1_send_buf);

static void Handle_USART1_Packet(uint8_t *data, uint32_t size) {
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

static void Begin_Send() {
    uint32_t buf_len = 0;
    RingBuffer *rb = &uart1_send_buf;
    
    ARM_USART_STATUS status = Driver_USART1.GetStatus();
    if (status.tx_busy) {
        return;
    }
    
    while (!ringbuffer_is_empty(rb)) {
        ringbuffer_read(rb, uart_send_buf + buf_len, 1);
        buf_len += 1;
        if (buf_len >= sizeof(uart_send_buf)) {
            break;
        }
    }
    
    if (buf_len > 0) {
        Driver_USART1.Send(uart_send_buf, buf_len);
    }
}

static void On_USART1_Received() {
    uint8_t revByte = uart1_rev_buf[uart1_buf_size];    
    if (PACKET_START == revByte) { // Wait for the start byte.
        uart1_rev_buf[0] = PACKET_START;        
        uart1_buf_size = 1;
        Driver_USART1.Receive(uart1_rev_buf + uart1_buf_size, 1);
        return;
    }
    
    if (0 == uart1_buf_size) { // No start byte yet, abandon current byte.
        Driver_USART1.Receive(uart1_rev_buf + uart1_buf_size, 1);
        return;
    }
    
    if (PACKET_END == revByte) { // Entire packect received.
        Handle_USART1_Packet(uart1_rev_buf, uart1_buf_size + 1);
        uart1_buf_size = 0;
        Driver_USART1.Receive(uart1_rev_buf + uart1_buf_size, 1);
        return;
    }
    
    ++uart1_buf_size; // Save packet data.
    if (uart1_buf_size >= MAX_PACKET_SIZE) { // Packet too big!
        uart1_buf_size = 0;
    }
    Driver_USART1.Receive(uart1_rev_buf + uart1_buf_size, 1);
}

static void On_USART1_Event(uint32_t event) {
    int index = 0;
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) {
        On_USART1_Received();
    }
    
    if (event & ARM_USART_EVENT_SEND_COMPLETE) {
        for(index = 0; index < 0xFFF; ++index); // wait for the hardware complete!
        Begin_Send();
    }
}

void USART1_Init(int32_t bitrate) {
    Driver_USART1.Initialize(On_USART1_Event);
    Driver_USART1.PowerControl(ARM_POWER_FULL);
    Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS | 
                               ARM_USART_DATA_BITS_8 | 
                               ARM_USART_PARITY_NONE | 
                               ARM_USART_STOP_BITS_1 | 
                               ARM_USART_FLOW_CONTROL_NONE, bitrate);
    Driver_USART1.Control(ARM_USART_CONTROL_TX, 1);
    Driver_USART1.Control(ARM_USART_CONTROL_RX, 1);
    
    Driver_USART1.Receive(uart1_rev_buf + uart1_buf_size, 1);
}

void USART1_Send(uint8_t *data, uint32_t size) {
    uint32_t index = 0;
    RingBuffer *rb = &uart1_send_buf;
    
    for (index = 0; index < size; ++index) {
        if (ringbuffer_is_full(rb)) {
            ringbuffer_clear(rb);
        }
        ringbuffer_write(rb, data + index, 1);
    }
    
    Begin_Send();
}
