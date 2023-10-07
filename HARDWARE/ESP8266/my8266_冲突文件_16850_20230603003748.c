#include "my8266.h"
#include "sys.h"
#include "delay.h"
#include "usart3.h"
#include "stdio.h"
#include "string.h"
#include "led.h"
#include "malloc.h"

extern char usart3_tx_buffer[TX_BUF_SIZE];
extern char usart3_rx_buffer[RX_BUF_SIZE];


void esp8266_hd_rst_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
    //USART_InitTypeDef USART_InitStructure;
    //NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //GPIOA
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
	// 这里初始化PA4 就是ESP8266的RST引脚
	GPIO_InitStructure.GPIO_Pin = ESP8266_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStructure);
		
	//进行硬件重启
	GPIO_WriteBit(ESP8266_RST_PORT, ESP8266_RST_PIN, Bit_RESET); // 将PA4设置为RESET
    delay_ms(100);
    GPIO_WriteBit(ESP8266_RST_PORT, ESP8266_RST_PIN, Bit_SET); // 将PA4设置为RESET
    delay_ms(500);
}

void esp8266_hd_off(void) {
	GPIO_WriteBit(ESP8266_RST_PORT, ESP8266_RST_PIN, Bit_RESET); // 将PA4设置为RESET
    delay_ms(100);
}

void esp8266_hd_on(void) {
	GPIO_WriteBit(ESP8266_RST_PORT, ESP8266_RST_PIN, Bit_SET); // 将PA4设置为RESET
    delay_ms(500);
}

void esp8266_hd_rst(void) {
	esp8266_hd_off();
	esp8266_hd_on();
}

void esp8266_soft_rst(void) {
	if(esp8266_send_cmd("AT+RST", "OK", 500)) {
		printf("软重启失败\r\n");
	}
}

u8 esp8266_init(void) {
	esp8266_hd_rst_init(); // 进行硬件重启初始化
	usart3_init(115200); //初始化串口 便于收发数据
	if(esp8266_at_test()) {
		//printf("esp8266_init fail!\r\n");
		//while(1) LED_RED_SWITCH;
		return 1;
	}
	return 0;
}

u8 esp8266_at_test(void) {
	return esp8266_send_cmd("AT", "OK", 500);
}

u8 esp8266_set_mode(u8 mode) {
	char cmd[16];
	sprintf(cmd, "AT+CWMODE=%d", mode);
	if(esp8266_send_cmd(cmd, "OK", 500)) {
		printf("模式设置失败\r\n");
		return 1;
	}
	return 0;
	//if(esp8266_send_cmd("AT+"))
}

extern u8 usart3_rx_buffer_len;

void show_rx_buffer() {
	u8 i;
	printf("usart3_rx_buffer(%d):{", usart3_rx_buffer_len);
	for(i=0;i<usart3_rx_buffer_len;i++) {
		while(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET); //有输入中断就停止
		printf("%c", usart3_rx_buffer[i]);
	}
	printf("}\r\n");
}

extern u8 over_flag;

void send_data(char* data) {
	usart3_clear_rx_buffer(); // 先清除接收缓冲区
	sprintf(usart3_tx_buffer, "%s\r\n", data); //用data输出到 发送缓冲区
	usart3_printf("%s\r\n", data); //发送数据
}

u8 esp8266_send_cmd(char* cmd, char* ack,u32 times) { //这个函数是必须要有回应的
	usart3_clear_rx_buffer(); // 先清除接收缓冲区
	sprintf(usart3_tx_buffer, "%s\r\n", cmd); //用cmd输出到 发送缓冲区
	usart3_printf("%s\r\n", cmd); //发送命令
	
	while(!over_flag); //只要传输数据没结束就死等
	printf("传输已经结束了\r\n");
	show_rx_buffer(); //
	
	
	if(ack==NULL || times == 0) {
		over_flag = 0; //清除结束标志
		return 0;
	}
	while(times--) {
		//delay_ms(1);
		if(strstr(usart3_rx_buffer, ack) != NULL) {
			over_flag = 0; //
			return 0; //表示正常结束
		}
	}
	delay_ms(50); //等到esp8266处理完成
	over_flag = 0; //
	return 1; // err
}

u8 esp8266_send_cmd_ack2(char* cmd, char* ack, char* ack2,u32 times) {
	usart3_clear_rx_buffer(); // 先清除接收缓冲区
	sprintf(usart3_tx_buffer, "%s\r\n", cmd); //用cmd输出到 发送缓冲区
	usart3_printf("%s\r\n", cmd); //发送命令
	if(ack==NULL || times == 0) return 0;
	while(times--) {
		if(strstr(usart3_rx_buffer, ack) != NULL || strstr(usart3_rx_buffer, ack2) != NULL) {
			return 0; //表示正常结束
		}
	}
	delay_ms(50); //等到esp8266处理完成
	return 1; // err
}


#define MALLOC(x) mymalloc(0, x)
#define FREE(x) myfree(0, x)

#define SERVER_IP "47.242.235.82"
//#define SERVER_IP "172.20.10.5"
#define SERVER_PORT 8080
#define WIFI_NAME "ShammingWang"
#define WIFI_PWD "1685087768"


u8 esp8266_tcp_init() {
	
	char cmd[64];
	
	//设置为 STA 模式
	if(esp8266_send_cmd("AT+CWMODE=1", "OK", 500)) {
		printf("设置为STA模式失败\r\n");
		show_rx_buffer();
		return 1;
	} else printf("设置为STA模式成功\r\n");
   //重启生效
	if(esp8266_send_cmd("AT+RST", "OK", 500)) {
		printf("软重启失败\r\n");
		show_rx_buffer();
		return 1;
	} else printf("软重启成功\r\n");
   //链接WiFi
	delay_ms(1000); //等待软重启
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", WIFI_NAME, WIFI_PWD);
	while(esp8266_send_cmd(cmd, "WIFI GOT IP", 20000)) {
		printf("WIFI连接失败\r\n");
		show_rx_buffer();
		//return 1;
	}
	printf("WIFI连接成功\r\n");
   //开启单链接
	if(esp8266_send_cmd("AT+CIPMUX=0", "", 300)) {
		printf("单链接开启失败\r\n");
		show_rx_buffer();
		return 1;
	} else printf("单链接开启成功\r\n");
    //建立TCP链接
	sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d", SERVER_IP, SERVER_PORT);
	//printf("原始命令：%s\r\n", cmd);
	if(esp8266_send_cmd_ack2(cmd, "OK", "ALREADY CONNECT", 10000)) {
		printf("建立TCP链接失败\r\n");
		show_rx_buffer();
		return 1;
	} else printf("建立TCP链接成功\r\n");
    //开启透传模式
	if(esp8266_send_cmd("AT+CIPMODE=1", "OK", 300)) {
		printf("开启透传模式失败\r\n");
		show_rx_buffer();
		return 1;
	} else printf("开启透传模式成功\r\n");
	
	if(esp8266_send_cmd("AT+CIPSEND", ">", 300)) {
		printf("AT+CIPSEND失败\r\n");
		show_rx_buffer();
		return 1;
	} else printf("AT+CIPSEND成功\r\n");
	
	printf("配置 ESP8266 成功\r\n");
	return 0;
}

// 定义JSON格式的POST请求数据格式
const char *post_data_fmt = "{\"image\": \"%s\"}";
const char *request_fmt = "POST /predict HTTP/1.1\r\n"
						  "Host: %s:%d\r\n"
						  "Content-Type: application/json\r\n"
						  "Content-Length: %d\r\n\r\n"
						  "%s";

char image_data[28*28];

void send_post_request(const char *ip, int port, const char *post_data, char *response, int response_size) {
	char send_buffer[2048];
	char request_body[1024];
	sprintf(request_body, post_data_fmt, post_data);
	sprintf(send_buffer, request_fmt, ip, port, strlen(request_body), request_body);
	printf("%s\r\n", send_buffer);
	send_data(send_buffer); //串口直接发送数据
}

void esp8266_tcp_post(void)
{
	int i;
	//char cmd[64];
	
	//esp8266_tcp_init(); // 发送指令建立tcp连接
	
	for(i=0;i<28*28;i++) {
		image_data[i]='1';
	}
	
	if(1) {
		//esp8266_send_cmd("TCP TEST", "", 0);
		//show_rx_buffer();
		send_post_request(SERVER_IP, SERVER_PORT, image_data, image_data, 0);
		delay_ms(1000);
		printf("\r\n响应如下:\r\n");
		show_rx_buffer();
		LED1=!LED1;
	}
}
