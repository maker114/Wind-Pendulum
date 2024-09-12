#ifndef __BOARD_H
#define __BOARD_H

#include "delay.h"
#include "usart.h"
#include "sys.h"
#include "stdlib.h"

#define LED0 PFout(9)
#define LED1 PFout(10)

#define LED_1 1
#define LED_2 2

#define KEY0 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)
#define KEY1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)

void Board_Init(void);
int Board_KeyScan(int mode);
void Board_LED_Toggle(int LED_x);

#endif
