#include "sys.h"

#define RX_BUF_SIZE 1024
#define TX_BUF_SIZE 1024



// void USART3_IRQHandler(void);

void usart3_init(u32 bound);

void usart3_printf(char* fmt,...);

void usart3_clear_rx_buffer(void);

void over_rx(void);

