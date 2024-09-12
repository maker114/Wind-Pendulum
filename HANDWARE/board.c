#include "board.h"
/*==========文件说明==========*/
// 配置了开发板上的外设以方便使用
// 引脚可以在头文件中更改

/*==========初始化外设==========*/
void Board_Init(void)
{
    /***配置LED***/
    // LED1-> PF10
    // LED2-> PF9
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_SetBits(GPIOF, GPIO_Pin_9);
    GPIO_SetBits(GPIOF, GPIO_Pin_10);

    /***配置按键***/
    // KEY1->PE3
    // KEY0->PE4
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*==========按键扫描函数==========*/
int key_up = 1; // 松开：1  按下：0
// 对板载按键进行扫描
// mode=0 -> 不支持连按
// mode=1 -> 支持连按
// 无按键按下 -> 返回0
// KEY0 -> 返回1
// KEY1 -> 返回2
int Board_KeyScan(int mode)
{
    if (mode == 1)
        key_up = 1;
    if (key_up == 1 && (KEY0 == 0 || KEY1 == 0))
    {
        key_up = 0;
        if (KEY0 == 0)
            return 1; // 按键0按下
        if (KEY1 == 0)
            return 2; // 按键1按下
    }
    if (KEY0 == 1 && KEY1 == 1)
        key_up = 1;
    return 0;
}

// 反转LED
void Board_LED_Toggle(int LED_x)
{
    if (LED_x == LED_1)
        GPIO_ToggleBits(GPIOF, GPIO_Pin_10);
    else if (LED_x == LED_2)
        GPIO_ToggleBits(GPIOF, GPIO_Pin_9);
}
