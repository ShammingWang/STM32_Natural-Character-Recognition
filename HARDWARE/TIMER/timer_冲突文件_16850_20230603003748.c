#include "timer.h"

#include "led.h"
#include "usart.h"
#include "usart3.h"

void tim3_NVIC_init(u16 arr, u16 psc) {
	
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 开启外设tim3的时钟源APB1
	
	TIM_TimeBaseInitStruct.TIM_Period=arr; // 计数器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  设置预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // 上升计时模式 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // 随便设置一下
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能 tim3的 更新 中断
	NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn; //中断通道设置为TIM3_IRQn
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0; //抢占优先级为0
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_SetCounter(TIM3, 0);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除更新中断的状态
	TIM_Cmd(TIM3, DISABLE);
}

void TIM3_IRQHandler() { // 配置中断服务函数
	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update)) { // 获取更新中断的状态
		printf("TIM3中断\r\n");
		//LED_GREEN_SWITCH;
		over_rx(); //长时间没接收到数据 停止接收
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除更新中断的状态
	}
}

void TIM7_Int_Init(u16 arr, u16 psc) {
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // 开启外设tim7的时钟源APB1
	
	TIM_TimeBaseInitStruct.TIM_Period=arr; // 计数器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  设置预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // 上升计时模式 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // 随便设置一下
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);
	
	
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); //使能 tim7的 更新 中断
	NVIC_InitStruct.NVIC_IRQChannel=TIM7_IRQn; //中断通道设置为TIM7_IRQn
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0; //抢占优先级为0
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=3;
	NVIC_Init(&NVIC_InitStruct);
	
	
	TIM_Cmd(TIM7, DISABLE);
	
}

void TIM7_IRQHandler() { // 配置中断服务函数
	
	if(TIM_GetITStatus(TIM7, TIM_IT_Update)) { // 获取更新中断的状态
		printf("TIM7结束中断\r\n");
		//LED_GREEN_SWITCH;
		//USART3_RX_STA|=1<<15;//强行让接收结束掉
		TIM_Cmd(TIM7,DISABLE); // 停用TIM7
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // 清除更新中断的状态
	}
}

//extern vu16 USART3_RX_STA;
/*
void TIM7_IRQHandler() { // 配置中断服务函数
	
	if(TIM_GetITStatus(TIM7, TIM_IT_Update)) { // 获取更新中断的状态
		printf("TIM7结束中断\r\n");
		LED_GREEN_SWITCH;
		USART3_RX_STA|=1<<15;//强行让接收结束掉
		TIM_Cmd(TIM7,DISABLE); // 停用TIM7
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // 清除更新中断的状态
	}
}
*/



void tim3_PWM_init(u16 arr, u16 psc) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 开启外设tim3的时钟源APB1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // 初始化重映射的时钟 TIM3_CH2->PB5
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//GPIOB_init(GPIO_Pin_5, GPIO_Speed_50MHz, GPIO_Mode_AF_PP); //alternate function push-pull
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	//初始化定时器的时基单元
	TIM_TimeBaseInitStruct.TIM_Period=arr; // 计数器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  设置预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // 上升计时模式 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // 随便设置一下
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	
	//TIM3的Channel2
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM2;
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High; // 极性设置为高,有效电瓶为高电平
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable; 
	TIM_OC2Init(TIM3, &TIM_OCInitStruct); //
	
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // 使能预装载寄存器
	
	TIM_Cmd(TIM3, ENABLE); //启动TIM3开始工作
}

void tim5_ch1_cap_init(u16 arr, u16 psc) { //tim5c1 默认是 PA0 也就是WK_UP按键 按下变高电平
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); // 开启外设tim3的时钟源APB1
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD; //PA0 IPD模式 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIOA_init(GPIO_Pin_0, GPIO_Speed_50MHz, GPIO_Mode_IPD); //PA0 IPD模式 
	GPIO_ResetBits(GPIOA, GPIO_Pin_0); // PA0 = 0 下拉PA0
	
	//初始化定时器5的时基单元
	TIM_TimeBaseInitStruct.TIM_Period=arr; // 计数器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  设置预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // 上升计时模式 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // 随便设置一下
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICFilter=0x00; // 不进行滤波
	TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising; // 设置上升沿捕获
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1; // 不进行分频
	TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI; //直接映射到TI1上
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM5_IRQn; //中断通道的设置：TIM5_IRQn
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2; //抢占优先级为2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE); //开启更新中断 也就是当tim5完成一次计数就触发这个中断
	TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE); //开启CC1捕获中断
	
	
	TIM_Cmd(TIM5, ENABLE);
}

u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态		    				
u32	TIM5CH1_CAPTURE_VAL=0;	//输入捕获值
//u16 TIM5CH1_CAPTURE_VAL_LAST = 0; // last val

void TIM5_IRQHandler() { // 配置TIM5中断服务函数
	
	if(TIM_GetITStatus(TIM5, TIM_IT_Update)) { // 获取更新中断的状态
		if(TIM5CH1_CAPTURE_STA&0x40) { //已经由一个上升沿启动了且当前周期计数完毕了还没停止
			TIM5CH1_CAPTURE_VAL += 65536;
			//TIM5CH1_CAPTURE_VAL_LAST = 0;
			//printf("%d\r\n", TIM5CH1_CAPTURE_VAL);
			//LED_GREEN_SWITCH;
			LED1=!LED1;
		}
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update); // 清除更新中断的状态
	}
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1)) { // 获取CC1中断的状态
		//第一次中断是上升沿
		if(TIM5CH1_CAPTURE_STA&0x40) { //触发这个函数的时候 捕获到了下降沿
			TIM5CH1_CAPTURE_STA=0; //结束计时
			TIM5CH1_CAPTURE_STA|=0x80; // 设置为结束态
			TIM5CH1_CAPTURE_VAL += TIM_GetCapture1(TIM5);
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising);		//设置为上升沿捕获
		} else if(TIM5CH1_CAPTURE_STA==0) { // 为0表示已经准备好了
			TIM5CH1_CAPTURE_VAL=0; // 清空答案
			TIM5CH1_CAPTURE_STA|=0x40; //开启计时
			TIM_SetCounter(TIM5,0); //从0开始计时
			//TIM5CH1_CAPTURE_VAL_LAST = TIM_GetCapture1(TIM5);
			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);		//设置为下降沿捕获
		}
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC1); // 清除更新中断的状态
	}
}


void tim5_ch2_cap_init(u16 arr, u16 psc) {
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); // 开启外设tim3的时钟源APB1
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; //PA0 IPD模式 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//GPIOA_init(GPIO_Pin_1, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING); //PA1 浮空输入
	
	//初始化定时器5的时基单元
	TIM_TimeBaseInitStruct.TIM_Period=arr; // 计数器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc; //  设置预分频系数
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up; // 上升计时模式 
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1; // 随便设置一下
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	
	TIM_ICInitStruct.TIM_Channel=TIM_Channel_2;
	TIM_ICInitStruct.TIM_ICFilter=0x03; // 八个周期的滤波
	TIM_ICInitStruct.TIM_ICPolarity=TIM_ICPolarity_Rising; // 设置上升沿捕获  因为充电充满了会触发一个上升沿
	TIM_ICInitStruct.TIM_ICPrescaler=TIM_ICPSC_DIV1; // 不进行分频
	TIM_ICInitStruct.TIM_ICSelection=TIM_ICSelection_DirectTI; //直接映射 直走到TI2上
	TIM_ICInit(TIM5, &TIM_ICInitStruct);
	
	TIM_Cmd(TIM5, ENABLE); // 开启TIM3 计时器
	
}

