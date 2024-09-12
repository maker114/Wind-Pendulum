#ifndef __TIM_H
#define __TIM_H
#include "delay.h"


void TIM1_Init(uint32_t arr, uint32_t psc);
void TIM2_Init(uint32_t arr, uint32_t psc);
void TIM4_Init(u16 arr, u16 psc);
void TIM3_Init(u16 arr, u16 psc);

void TIM5_Init(u16 arr, u16 psc);

#endif

