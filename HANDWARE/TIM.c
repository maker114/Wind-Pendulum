#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "TIM.h"
#include "board.h"
#include "motor.h"
// 此文件共配置了五个定时器：
//  TIM1 -> PWM
//   |- CH1-> PA8/PE9
//   |- CH2-> PA9/PE11
//   |- CH3-> PA10/PE13
//   |- CH4-> PA11/PE14
//  TIM2 -> 计时循环
//  TIM3 -> 编码器1
//  TIM4 -> 编码器2
//  TIM5 -> 编码器3（备用）

/*= == == == == == == TIM1（pwm） == == == == == == = */
// arr：自动重装载值，定义了PWM信号的周期
// psc：预分频系数，定义了PWM信号的频率
// 返回值：无
// 通道配置：
//  |- CH1->PA8 ->UP  ->Y+
//  |- CH2->PE11->RIG ->X+
//  |- CH3->PE13->LFT ->X-
//  |- CH4->PA11->DOWN->Y-
void TIM1_Init(uint32_t arr, uint32_t psc)
{
    // 配置GPIOE的时钟与结构体
    GPIO_InitTypeDef GPIO_InitStructure;

    // 配置GPIOE的引脚11和引脚13的参数
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 默认上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // 配置GPIOA的引脚8和引脚11的参数
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 默认上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 使能APB2时钟，并配置时钟分频因子
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // 配置时基结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 时基结构体
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    // 配置输出比较结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // 选用PWM1模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCNPolarity_Low;     // 极性为高
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;  // 不加上这一句CH1输出不了PWM（md找了好久）
    TIM_OCInitStructure.TIM_Pulse = 0;                            // 占空比初始值为0

    // 配置输出比较1结构体
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    // 配置输出比较2结构体
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    // 配置输出比较3结构体
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
    // 配置输出比较4结构体
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE); // 使能TIM1的自动重加载寄存器（ARPE使能）
    TIM_Cmd(TIM1, ENABLE);              // 使能TIM1
    TIM_CtrlPWMOutputs(TIM1, ENABLE);   // 使能PWM输出（高级定时器特有）

    // 将比较值设为0，可以在此处设置初始值
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare2(TIM1, 0);
    TIM_SetCompare3(TIM1, 0);
    TIM_SetCompare3(TIM1, 0);
}

/*=============TIM2（定时循环）=============*/
// arr：自动重装载值
// psc：预分频系数
// 返回值：无
// 参考值：arr=5000-1(重装载值5000) psc=8400-1（分频系数8400）-->延时500ms
void TIM2_Init(uint32_t arr, uint32_t psc)
{
    // 开启定时器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    // 将定时器TIM2设置为一个周期触发的定时器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    // 初始化定时器
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    // 允许定时器产生中断
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    // 开启各项中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;           // tim2中断
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能TIM2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 响应优先级3
    NVIC_Init(&NVIC_InitStructure);
    // 使能定时器2（若需要定时器受控开启，可以将此指令移至所需部位）
    TIM_Cmd(TIM2, ENABLE);
}
// 定时器2中断服务子函数
void TIM2_IRQHandler(void)
{
    // 清除中断标志位
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        Board_LED_Toggle(LED_1);
        MOTOR_LoopFunction();
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
