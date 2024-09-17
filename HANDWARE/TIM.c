#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "TIM.h"
#include "board.h"
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
// 参考值：arr=20000-1(重装载值500) psc=84-1（分频系数8400）–>周期20ms（舵机需要）
// 通道一A8 通道二E11 通道三E13 通道四A11
void TIM1_Init(uint32_t arr, uint32_t psc)
{
    // 配置GPIOE的时钟与结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    // 配置GPIOE的引脚11和引脚13的复用功能
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
    // 配置GPIOE的引脚11和引脚13的参数
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 默认上拉
    // 结构体装载，完成初始化
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // 配置GPIOA的引脚11和引脚13的复用功能
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
    // 配置GPIOE的引脚11和引脚13的参数
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 默认上拉
    // 结构体装载，完成初始化
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /*将TIM1配置为PWM输出模式，通过通道1与通道2输出两个PWM波*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; // 时基结构体
    // 使能APB2时钟，并配置时钟分频因子
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    // 配置时基结构体
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
    TIM_OCInitStructure.TIM_Pulse = 0;                            // 占空比初始值为0

    // 配置输出比较1结构体
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    // 使能输出比较1预加载寄存器（通道1）
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // 配置输出比较2结构体
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    // 使能输出比较2预加载寄存器（通道2）
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // 配置输出比较3结构体
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    // 使能输出比较3预加载寄存器（通道3）（舵机PE13）
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    // 使能TIM1的自动重加载寄存器（ARPE使能）
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    // 使能TIM1
    TIM_Cmd(TIM1, ENABLE);
    // 使能PWM输出（高级定时器特有）
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    // 将比较值设为0，可以在此处设置初始值
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare2(TIM1, 0);
    TIM_SetCompare3(TIM1, 2500 + 800); // 在此改变舵机的初始值
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
    }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

/*=============TIM4（编码器2）=============*/
// arr：自动重装载值
// psc：预分频系数
// 返回值：无
// 参考值：arr=65535（0xFFFF）psc=0
// 读取编码器转动圈数（计数值）：TIM_GetCounter(TIMx);
// 清除编码器转动圈数（计数值）：TIM_SetCounter(TIMx,0);
// 引脚：PB6，PB7
void TIM4_Init(u16 arr, u16 psc)
{
    // 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  // 使能定时器4的时钟
    RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // 使能B端口的时钟

    // 配置引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 浮空
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // 配置引脚复用
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);

    // 配置定时器时基结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

    // 配置定时器输入模式(由可能存在的问题,暂时去除此段代码)
    // TIM_ICInitTypeDef TIM_ICInitStructure;
    // TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2;
    // TIM_ICInitStructure.TIM_ICFilter = 0xf;
    // TIM_ICInit(TIM4, &TIM_ICInitStructure);

    // 配置定时器4为编码器接口
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    // 使能tim4
    TIM_Cmd(TIM4, ENABLE);
}

/*=============TIM3（编码器1）=============*/
// arr：自动重装载值
// psc：预分频系数
// 返回值：无
// 参考值：arr=65535（0xFFFF）psc=0
// 读取编码器转动圈数（计数值）：TIM_GetCounter(TIMx);
// 清除编码器转动圈数（计数值）：TIM_SetCounter(TIMx,0);
// 引脚：PB4，PB5
void TIM3_Init(u16 arr, u16 psc)
{
    // 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  // 使能定时器3的时钟
    RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); // 使能B端口的时钟

    // 配置引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 浮空
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置引脚复用
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

    // 配置定时器时基结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    // 配置定时器输入模式(由可能存在的问题,暂时去除此段代码)
    // TIM_ICInitTypeDef TIM_ICInitStructure;
    // TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2;
    // TIM_ICInitStructure.TIM_ICFilter = 0xf;
    // TIM_ICInit(TIM3, &TIM_ICInitStructure);

    // 配置定时器3为编码器接口
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    // 使能tim3
    TIM_Cmd(TIM3, ENABLE);
}
/*=============TIM5（编码器5）（备用）=============*/
// arr：自动重装载值
// psc：预分频系数
// 返回值：无
// 参考值：arr=65535（0xFFFF）psc=0
// 读取编码器转动圈数（计数值）：TIM_GetCounter(TIMx);
// 清除编码器转动圈数（计数值）：TIM_SetCounter(TIMx,0);
// 引脚：PB4，PB5
void TIM5_Init(u16 arr, u16 psc)
{
    // 开启时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  // 使能定时器3的时钟
    RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能B端口的时钟

    // 配置引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // 复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   // 浮空
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置引脚复用
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);

    // 配置定时器时基结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);

    // 配置定时器输入模式(由可能存在的问题,暂时去除此段代码)
    // TIM_ICInitTypeDef TIM_ICInitStructure;
    // TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2;
    // TIM_ICInitStructure.TIM_ICFilter = 0xf;
    // TIM_ICInit(TIM5, &TIM_ICInitStructure);

    // 配置定时器5为编码器接口
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    // 使能tim5
    TIM_Cmd(TIM5, ENABLE);
}
