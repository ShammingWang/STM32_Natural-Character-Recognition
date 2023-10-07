#include "usart3.h"

#include "my8266.h"
#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


char usart3_tx_buffer[TX_BUF_SIZE];
char usart3_rx_buffer[RX_BUF_SIZE];
int usart3_rx_buffer_len; // 接收数组的游标

u8 over_flag = 0;

u8 res;	
//串口接收发送缓存区		
//u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			
//u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		

//vu16 USART3_RX_STA=0;   	

//串口3中断
/*
void USART3_IRQHandler_old(void)
{
  //printf("USART3中断！！\r\n");
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 如果为串口3的数据输入中断
	{	 
		printf("USART3接收中断\r\n");
		res=USART_ReceiveData(USART3);
		printf("%c\r\n", res);
		//printf("接收到了%c\r\n", res);
		if((USART3_RX_STA&(1<<15))==0) //最高位为0 还没结束
		{ 
			if(USART3_RX_STA<USART3_MAX_RECV_LEN)	
			{
				TIM_SetCounter(TIM7,0); // 重置时间间隔
				if(USART3_RX_STA==0) 				
				{
					TIM_Cmd(TIM7,ENABLE);
					printf("开启定时器\r\n");
				}
				USART3_RX_BUF[USART3_RX_STA++]=res;	
			}else
			{
				USART3_RX_STA|=1<<15;//强行让接收结束掉
			} 
		}
	}  				 											 
}
*/
//串口3初始化 
void usart3_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); 

	USART_DeInit(USART3); 
	//USART3_TX   PB10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	//USART3_RX	  PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART3, ENABLE);       

	// 等待ESP8266启动完成
    delay_ms(1000);


	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //开启接收中断
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);
	
	//tim3_NVIC_init(1000-1,7200-1);
	//TIM7_Int_Init(1000-1,7200-1); //计数到1000，需要100ms一次
	//TIM_Cmd(TIM7,DISABLE); // 停用TIM7
	//USART3_RX_STA=0;
}

void over_rx() {
	over_flag = 1;
	TIM_Cmd(TIM3, DISABLE); // 停用TIM3
	TIM_SetCounter(TIM3, 0); // 准备下一次重新开始计数
}

void USART3_IRQHandler(void)
{
	char data;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
		if(over_flag) return; //如果当前已经停止接收数据了，且数据还没处理完成
		if(!usart3_rx_buffer_len) { //准备开始接收数据 且是第一个数据
			TIM_SetCounter(TIM3, 0);
			TIM_Cmd(TIM3, ENABLE);
		}
        data = USART_ReceiveData(USART3);
		//printf("%d\r\n", data);
		if(usart3_rx_buffer_len >= RX_BUF_SIZE) {
			over_rx();
			printf("缓冲区已经满了\r\n");
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
			return;
		} 
		// 接收到了数据
		TIM_SetCounter(TIM3, 0); //重新开始计数
        usart3_rx_buffer[usart3_rx_buffer_len] = data;
        usart3_rx_buffer_len += 1;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

void usart3_clear_rx_buffer(void) {
	usart3_rx_buffer_len = 0;
	memset(usart3_rx_buffer, 0, sizeof(usart3_rx_buffer));
}

//串口usart3_printf函数，保证输入不大于设定长度
void usart3_printf(char* fmt,...)  
{
	u16 i,len;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)usart3_tx_buffer,fmt,ap);
	va_end(ap);
	len=strlen((const char*)usart3_tx_buffer); // 求出串口发送数据的长度
	for(i=0;i<len;i++) 
	{
	  while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); //直到当前状态变为SET表示传输完成
		USART_SendData(USART3, usart3_tx_buffer[i]);
	}
}
