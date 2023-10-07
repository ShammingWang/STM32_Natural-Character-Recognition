#include "my8266.h"
#include "sys.h"
#include "delay.h"
#include "usart3.h"
#include "stdio.h"
#include "string.h"
#include "led.h"
#include "malloc.h"
#include "atk_ncr.h"
//#include "cJSON.h"
#include "lcd.h"
#include "text.h" 

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

u8 esp8266_send_cmd(char* cmd, char* ack,u32 times) {
	usart3_clear_rx_buffer(); // 先清除接收缓冲区
	sprintf(usart3_tx_buffer, "%s\r\n", cmd); //用cmd输出到 发送缓冲区
	usart3_printf("%s\r\n", cmd); //发送命令
	if(ack==NULL || times == 0) return 0;
	while(times--) {
		//if(times%100==0) show_rx_buffer();
		delay_ms(1);
		if(strstr(usart3_rx_buffer, ack) != NULL) {
			return 0; //表示正常结束
		}
	}
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
	return 1; // err
}


#define MALLOC(x) mymalloc(0, x)
#define FREE(x) myfree(0, x)

#define SERVER_IP "8.130.43.220"
//#define SERVER_IP "172.20.10.5"
#define SERVER_PORT 80
#define WIFI_NAME "ShammingWang"
#define WIFI_PWD "1685087768"


u8 esp8266_tcp_init() {
	int i;
	char cmd[64];
	//u32 times = 10000;
	//设置为 STA 模式
	if(esp8266_send_cmd("AT+CWMODE=1", "OK", 500)) {
		printf("设置为STA模式失败\r\n"),Show_Str(30,10,200,16,(u8*)"设置STA模式失败",16,0);
		show_rx_buffer();
		return 1;
	} else printf("设置为STA模式成功\r\n"),show_rx_buffer(),Show_Str(30,10,200,16,(u8*)"设置STA模式成功",16,0);
	
	for(i=0;i<10000000;i++);
	
   //重启生效
	if(esp8266_send_cmd("AT+RST", "OK", 500)) {
		printf("软重启失败\r\n"),Show_Str(30,30,200,16,(u8*)"软重启失败",16,0);
		show_rx_buffer();
		return 1;
	} else printf("软重启成功\r\n"),show_rx_buffer(),Show_Str(30,30,200,16,(u8*)"软重启成功",16,0);
	
	for(i=0;i<10000000;i++);
	
   //链接WiFi
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", WIFI_NAME, WIFI_PWD);
	if(esp8266_send_cmd(cmd, "WIFI GOT IP", 10000)) {
		printf("WIFI连接失败\r\n"),Show_Str(30,50,200,16,(u8*)"WIFI连接失败",16,0);
		show_rx_buffer();
		return 1;
	} else printf("WIFI连接成功\r\n"),show_rx_buffer(),Show_Str(30,50,200,16,(u8*)"WIFI连接成功",16,0);
	
	for(i=0;i<10000000;i++);
	
   //开启单链接
	if(esp8266_send_cmd("AT+CIPMUX=0", "", 300)) {
		printf("单链接开启失败\r\n"),Show_Str(30,70,200,16,(u8*)"单链接开启失败",16,0);
		show_rx_buffer();
		return 1;
	} else printf("单链接开启成功\r\n"),show_rx_buffer(),Show_Str(30,70,200,16,(u8*)"单链接开启成功",16,0);
	
	for(i=0;i<10000000;i++);
	
    //建立TCP链接
	sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d", SERVER_IP, SERVER_PORT);
	//printf("原始命令：%s\r\n", cmd);
	if(esp8266_send_cmd_ack2(cmd, "OK", "ALREADY CONNECT", 15000)) {
		printf("建立TCP链接失败\r\n"),Show_Str(30,90,200,16,(u8*)"建立TCP链接失败",16,0);
		show_rx_buffer();
		return 1;
	} else printf("建立TCP链接成功\r\n"),show_rx_buffer(),show_rx_buffer(),Show_Str(30,90,200,16,(u8*)"建立TCP链接成功",16,0);
	
	for(i=0;i<10000000;i++);
	
    //开启透传模式
	if(esp8266_send_cmd("AT+CIPMODE=1", "OK", 500)) {
		printf("开启透传模式失败\r\n"),Show_Str(30,110,200,16,(u8*)"开启透传模式失败",16,0);
		show_rx_buffer();
		return 1;
	} else printf("开启透传模式成功\r\n"),show_rx_buffer(),show_rx_buffer(),Show_Str(30,110,200,16,(u8*)"开启透传模式成功",16,0);
	
	for(i=0;i<10000000;i++);
	
	if(esp8266_send_cmd("AT+CIPSEND", ">", 500)) {
		printf("AT+CIPSEND失败\r\n"),Show_Str(30,130,200,16,(u8*)"AT+CIPSEND失败",16,0);
		show_rx_buffer();
		return 1;
	} else printf("AT+CIPSEND成功\r\n"),show_rx_buffer(),Show_Str(30,130,200,16,(u8*)"AT+CIPSEND成功",16,0);
	
	Show_Str(30,150,200,16,(u8*)"配置 ESP8266 成功",16,0);
	
	printf("配置 ESP8266 成功\r\n");
	for(i=0;i<10000000;i++);
	return 0;
}

// 定义JSON格式的POST请求数据格式
const char *post_data_fmt = "{\"points\": %s}";
const char *request_fmt = "POST / HTTP/1.1\r\n"
						  "Host: %s:%d\r\n"
						  "Content-Type: application/json\r\n"
						  "Content-Length: %d\r\n\r\n"
						  "%s";
//char image_data[28*28+1];
char points_value_str[2048];
char response[512];
u8 response_len = 0;
extern atk_ncr_point READ_BUF[200];
extern u16 pcnt;
void generate_points_value_str(char *str) {
	u16 i=0;
	u16 j=0;
	
	for(i=0;i<1024;i++) str[i]=0;
	i=0;
	
	//printf("%d:\r\n", pcnt);
	if(pcnt>200) pcnt=200;
	str[i++]='[';
	for(j=0;j<pcnt;j++) {
		if(j) str[i++]=',';
		sprintf(str+i, "[%d,%d]", READ_BUF[j].x, READ_BUF[j].y);
		while(str[i])i++;
	}
	str[i++]=']';
	str[i++]='\0';
	//printf("points_value_str(%d):%s\r\n",strlen(str), str);
}

u8 send_post_request(const char *ip, int port, const char *post_data_value, char *res) {
	u8 i=0;
	char send_buffer[2048];
	char request_body[1024];
	sprintf(request_body, post_data_fmt, post_data_value);
	sprintf(send_buffer, request_fmt, ip, port, strlen(request_body), request_body);
	printf("%s\r\n", send_buffer);
	 //5s的响应时间
	if(esp8266_send_cmd(send_buffer, "}", 10000)) {
		printf("响应超时\r\n");
		return 1;
	}
	strcpy(response, usart3_rx_buffer);
	res[usart3_rx_buffer_len]=0;
	return 0;
	//strcpy(res, usart3_rx_buffer);
}

void handle(char* s) {
	u16 i = strlen(s) - 1;
	while(i>0 && s[i]!='{') i--;
	strcpy(s, s+i);
}

char ret[10];

char* esp8266_tcp_post(void)
{
	int i;
	//cJSON *json;
	//cJSON *message;
	generate_points_value_str(points_value_str);
	
	if(1) {
		//show_rx_buffer();
		for(i=0;i<512;i++) response[i]=0;
		generate_points_value_str(points_value_str);
		if(send_post_request(SERVER_IP, SERVER_PORT, points_value_str, response)) {
			printf("time out\r\n");
			return "?";
		}
		printf("\r\nthe response is :\r\n");
		printf("%s\r\n", response);
		handle(response);
		printf("response body:%s\r\n", response);
		if(!strstr(response, "message")) {
			printf("fail\r\n");
			return "?";
		}
		i=strlen(response)-1;
		while(response[i]!='\"')i--;
		response[i]=0;
		printf("result: <%s>\r\n", response+i-1);
		strcpy(ret, response+i-1);
		LED1=!LED1;
		
		return ret;
	}
}

		/*
		json = cJSON_Parse(response);
		if(json == NULL) {
			printf("JSON解析失败\r\n");
			return "JSON解析失败";
		}
		message = cJSON_GetObjectItem(json, "message");
		if(message == NULL) {
			printf("识别失败\r\n");
			return "识别失败";
		} */