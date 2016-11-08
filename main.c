#include "protocol.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "can1.h"

int main (void) {
    USART1_Init(115200);
    USART2_Init(9600);
    USART3_Init(9600);
    
    CAN1_Init();
    
    while (1) {
        // Do nothing!
    }
}
