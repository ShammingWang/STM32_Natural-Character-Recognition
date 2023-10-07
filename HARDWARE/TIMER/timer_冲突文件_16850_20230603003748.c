#include "timer.h"

#include "led.h"
#include "usart.h"
#include "usart3.h"

void tim3_NVIC_init(u16 arr, u16 psc) {
	
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // ��������tim3��ʱ��ԴAPB1
	
	TIM_TimeBaseInitStruct.TIM_Period=arr; // ��������ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  ����Ԥ��Ƶϵ��
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // ������ʱģʽ 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // �������һ��
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //ʹ�� tim3�� ���� �ж�
	NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn; //�ж�ͨ������ΪTIM3_IRQn
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0; //��ռ���ȼ�Ϊ0
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_SetCounter(TIM3, 0);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ��������жϵ�״̬
	TIM_Cmd(TIM3, DISABLE);
}

void TIM3_IRQHandler() { // �����жϷ�����
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update)) { // ��ȡ�����жϵ�״̬
		printf("TIM3�ж�\r\n");
		//LED_GREEN_SWITCH;
		over_rx(); //��ʱ��û���յ����� ֹͣ����
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ��������жϵ�״̬
	}
}

void TIM7_Int_Init(u16 arr, u16 psc) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // ��������tim7��ʱ��ԴAPB1
	
	TIM_TimeBaseInitStruct.TIM_Period=arr; // ��������ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  ����Ԥ��Ƶϵ��
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // ������ʱģʽ 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // �������һ��
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
	
	
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); //ʹ�� tim7�� ���� �ж�
	NVIC_InitStruct.NVIC_IRQChannel=TIM7_IRQn; //�ж�ͨ������ΪTIM7_IRQn
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0; //��ռ���ȼ�Ϊ0
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);
	
	
	TIM_Cmd(TIM7, DISABLE);
	
}

void TIM7_IRQHandler() { // �����жϷ�����
	
	if(TIM_GetITStatus(TIM7, TIM_IT_Update)) { // ��ȡ�����жϵ�״̬
		printf("TIM7�����ж�\r\n");
		//LED_GREEN_SWITCH;
		//USART3_RX_STA|=1<<15;//ǿ���ý��ս�����
		TIM_Cmd(TIM7,DISABLE); // ͣ��TIM7
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // ��������жϵ�״̬
	}
}

//extern vu16 USART3_RX_STA;
/*
void TIM7_IRQHandler() { // �����жϷ�����
	
	if(TIM_GetITStatus(TIM7, TIM_IT_Update)) { // ��ȡ�����жϵ�״̬
		printf("TIM7�����ж�\r\n");
		LED_GREEN_SWITCH;
		USART3_RX_STA|=1<<15;//ǿ���ý��ս�����
		TIM_Cmd(TIM7,DISABLE); // ͣ��TIM7
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // ��������жϵ�״̬
	}
}
*/



void tim3_PWM_init(u16 arr, u16 psc) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // ��������tim3��ʱ��ԴAPB1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // ��ʼ����ӳ���ʱ�� TIM3_CH2->PB5
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIOB_init(GPIO_Pin_5, GPIO_Speed_50MHz, GPIO_Mode_AF_PP); //alternate function push-pull
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	//��ʼ����ʱ����ʱ����Ԫ
	TIM_TimeBaseInitStruct.TIM_Period=arr; // ��������ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  ����Ԥ��Ƶϵ��
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // ������ʱģʽ 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // �������һ��
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	//TIM3��Channel2
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High; // ��������Ϊ��,��Ч��ƿΪ�ߵ�ƽ
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable; 
	TIM_OC2Init(TIM3, &TIM_OCInitStruct); //
	
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // ʹ��Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM3, ENABLE); //����TIM3��ʼ����
}

void tim5_ch1_cap_init(u16 arr, u16 psc) { //tim5c1 Ĭ���� PA0 Ҳ����WK_UP���� ���±�ߵ�ƽ
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); // ��������tim3��ʱ��ԴAPB1
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD; //PA0 IPDģʽ 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIOA_init(GPIO_Pin_0, GPIO_Speed_50MHz, GPIO_Mode_IPD); //PA0 IPDģʽ 
	GPIO_ResetBits(GPIOA, GPIO_Pin_0); // PA0 = 0 ����PA0
	
	//��ʼ����ʱ��5��ʱ����Ԫ
	TIM_TimeBaseInitStruct.TIM_Period=arr; // ��������ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  ����Ԥ��Ƶϵ��
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // ������ʱģʽ 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // �������һ��
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter=0x00; // �������˲�
	TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising; // ���������ز���
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1; // �����з�Ƶ
	TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI; //ֱ��ӳ�䵽TI1��
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM5_IRQn; //�ж�ͨ�������ã�TIM5_IRQn
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2; //��ռ���ȼ�Ϊ2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE); //���������ж� Ҳ���ǵ�tim5���һ�μ����ʹ�������ж�
	TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE); //����CC1�����ж�
	
	
	TIM_Cmd(TIM5, ENABLE);
}

u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬		    				
u32	TIM5CH1_CAPTURE_VAL=0;	//���벶��ֵ
//u16 TIM5CH1_CAPTURE_VAL_LAST = 0; // last val

void TIM5_IRQHandler() { // ����TIM5�жϷ�����
	
	if(TIM_GetITStatus(TIM5, TIM_IT_Update)) { // ��ȡ�����жϵ�״̬
		if(TIM5CH1_CAPTURE_STA&0x40) { //�Ѿ���һ���������������ҵ�ǰ���ڼ�������˻�ûֹͣ
			TIM5CH1_CAPTURE_VAL += 65536;
			//TIM5CH1_CAPTURE_VAL_LAST = 0;
			//printf("%d\r\n", TIM5CH1_CAPTURE_VAL);
			//LED_GREEN_SWITCH;
			LED1=!LED1;
		}
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update); // ��������жϵ�״̬
	}
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1)) { // ��ȡCC1�жϵ�״̬
		//��һ���ж���������
		if(TIM5CH1_CAPTURE_STA&0x40) { //�������������ʱ�� �������½���
			TIM5CH1_CAPTURE_STA=0; //������ʱ
			TIM5CH1_CAPTURE_STA|=0x80; // ����Ϊ����̬
			TIM5CH1_CAPTURE_VAL += TIM_GetCapture1(TIM5);
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);		//����Ϊ�����ز���
		} else if(TIM5CH1_CAPTURE_STA==0) { // Ϊ0��ʾ�Ѿ�׼������
			TIM5CH1_CAPTURE_VAL=0; // ��մ�
			TIM5CH1_CAPTURE_STA|=0x40; //������ʱ
			TIM_SetCounter(TIM5,0); //��0��ʼ��ʱ
			//TIM5CH1_CAPTURE_VAL_LAST = TIM_GetCapture1(TIM5);
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);		//����Ϊ�½��ز���
		}
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1); // ��������жϵ�״̬
	}
}


void tim5_ch2_cap_init(u16 arr, u16 psc) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); // ��������tim3��ʱ��ԴAPB1
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; //PA0 IPDģʽ 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIOA_init(GPIO_Pin_1, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING); //PA1 ��������
	
	//��ʼ����ʱ��5��ʱ����Ԫ
	TIM_TimeBaseInitStruct.TIM_Period=arr; // ��������ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  ����Ԥ��Ƶϵ��
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // ������ʱģʽ 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // �������һ��
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter=0x03; // �˸����ڵ��˲�
	TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising; // ���������ز���  ��Ϊ�������˻ᴥ��һ��������
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1; // �����з�Ƶ
	TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI; //ֱ��ӳ�� ֱ�ߵ�TI2��
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	
	TIM_Cmd(TIM5, ENABLE); // ����TIM3 ��ʱ��
	
}

