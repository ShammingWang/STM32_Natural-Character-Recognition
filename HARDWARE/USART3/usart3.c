#include "usart3.h"

#include "my8266.h"
#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


char usart3_tx_buffer[TX_BUF_SIZE];
char usart3_rx_buffer[RX_BUF_SIZE];
int usart3_rx_buffer_len; // ����������α�

u8 res;	

//����3��ʼ�� 
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

	// �ȴ�ESP8266�������
    delay_ms(1000);


	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //���������ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);
	
	//tim3_NVIC_init(1000-1,7200-1);
	//TIM7_Int_Init(1000-1,7200-1); //������1000����Ҫ100msһ��
	//TIM_Cmd(TIM7,DISABLE); // ͣ��TIM7
	//USART3_RX_STA=0;
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
        char data = USART_ReceiveData(USART3);
		//printf("%d\r\n", data);
		if(usart3_rx_buffer_len >= RX_BUF_SIZE) {
			printf("�������Ѿ�����\r\n");
			USART_ClearITPendingBit(USART3, USART_IT_RXNE);
			return;
		}
        usart3_rx_buffer[usart3_rx_buffer_len] = data;
        usart3_rx_buffer_len += 1;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

void usart3_clear_rx_buffer(void) {
	usart3_rx_buffer_len = 0;
	memset(usart3_rx_buffer, 0, sizeof(usart3_rx_buffer));
}

//����usart3_printf��������֤���벻�����趨����
void usart3_printf(char* fmt,...)  
{
	u16 i,len;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)usart3_tx_buffer,fmt,ap);
	va_end(ap);
	len=strlen((const char*)usart3_tx_buffer); // ������ڷ������ݵĳ���
	for(i=0;i<len;i++) 
	{
	  while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); //ֱ����ǰ״̬��ΪSET��ʾ�������
		USART_SendData(USART3, usart3_tx_buffer[i]);
	}
}
