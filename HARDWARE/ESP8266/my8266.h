#ifndef __MY8266_H
#define __MY8266_H

#define ESP8266_RST_PORT GPIOA
#define ESP8266_RST_PIN GPIO_Pin_4

#define ESP8266_TX_PORT GPIOB
#define ESP8266_TX_PIN GPIO_Pin_10

#define ESP8266_RX_PORT GPIOB
#define ESP8266_RX_PIN GPIO_Pin_11

#include "sys.h"



u8 esp8266_init(void);
u8 esp8266_tcp_init(void);

void esp8266_hd_rst_init(void);
void esp8266_hd_off(void);
void esp8266_hd_on(void);
u8 esp8266_send_cmd(char* cmd,char* ack,u32 times);

u8 esp8266_at_test(void);
u8 esp8266_set_mode(u8 mode);

char* esp8266_tcp_post(void);

#endif
