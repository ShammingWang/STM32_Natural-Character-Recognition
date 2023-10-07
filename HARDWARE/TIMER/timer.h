#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void tim3_NVIC_init(u16 arr, u16 psc);
void TIM7_Int_Init(u16 arr, u16 psc);


void tim3_PWM_init(u16 arr, u16 psc);


void tim5_ch1_cap_init(u16 arr, u16 psc);

void tim5_ch2_cap_init(u16 arr, u16 psc);

#endif

