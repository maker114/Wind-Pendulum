#include "key2.h"
#include "delay.h"
#include "oled.h"
#include "usart.h"


void key2_init(void)
{
	/*初始化按键*/
	GPIO_InitTypeDef GPIO_InitStructure;

	// 使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);

	// 初始化GPIOF1,3,5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	   // 普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // 上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	// 初始化GPIOE1,3,5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	   // 普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // 上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// 初始化GPIOC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	   // 普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // 上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

// 按键扫描(目前仅启用了五个方向键)
// mode=1 -> 支持连按
// mode=0 -> 不支持连按
int key2_scan(int mode)
{
	static uint8_t key_up = 1; // 按键按松开标志
	if (mode)
		key_up = 1; // 支持连按
	if (key_up && (KEY_DWN == 0 || KEY_LFT == 0 || KEY_RHT == 0 || KEY_MID == 0 || KEY_UP == 0))
	{
		delay_ms(10); // 去抖动
		key_up = 0;
		if (KEY_DWN == 0)
			return 1;
		else if (KEY_LFT == 0)
			return 2;
		else if (KEY_RHT == 0)
			return 3;
		else if (KEY_MID == 0)
			return 4;
		else if (KEY_UP == 0)
			return 5;
	}
	else if (KEY_DWN == 1 && KEY_LFT == 1 && KEY_RHT == 1 && KEY_MID == 1 && KEY_UP == 1)
		key_up = 1;
	return 0; // 无按键按下
}

int key2_scan_alone(void)
{
	return key2_scan(1);
}
