/**
 * @file motor.c
 * @author maker114
 * @brief 电机驱动程序，用以驱动四个空心杯电机,兼具PID功能
 * @version 0.1
 * @date 2024-09-12
 * @note 电机以x与y轴为参考分为两组
 *
 */
#include "motor.h"
#include "resolving.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#define X_KP 0.1
#define X_KI 0.1
#define X_KD 0.1

#define Y_KP 0.1
#define Y_KI 0.1
#define Y_KD 0.1

#define PWM_MAX 20000
#define PWM_MIN -20000

Motor_PID_Stuct X_PID_Stuct;
Motor_PID_Stuct Y_PID_Stuct;

/**
 * @brief 电机初始化函数
 *
 */
void MOTOR_Init(void)
{
    // PID初始化结构体参
    X_PID_Stuct.KP = X_KP;
    X_PID_Stuct.KI = X_KI;
    X_PID_Stuct.KD = X_KD;
    X_PID_Stuct.Channel_Number = X_Channel;

    Y_PID_Stuct.KP = Y_KP;
    Y_PID_Stuct.KI = Y_KI;
    Y_PID_Stuct.KD = Y_KD;
    Y_PID_Stuct.Channel_Number = Y_Channel;
}

/**
 * @brief 设置电机PWM值
 *
 * @param channel 设置PWM值通道
 * @note 通道0为X轴电机，通道1为Y轴电机
 */
void MOTOR_SetPWM(int channel)
{
    if (channel == X_Channel)
    {
        // PID限幅
        if (X_PID_Stuct.PWM_Value > PWM_MAX)
            X_PID_Stuct.PWM_Value = PWM_MAX;
        if (X_PID_Stuct.PWM_Value < PWM_MIN)
            X_PID_Stuct.PWM_Value = PWM_MIN;
        // PID装载
        if (X_PID_Stuct.PWM_Value > 0)
        {
            // x正向移动，LFT电机推动，CH3->PE13
            TIM_SetCompare3(TIM1, X_PID_Stuct.PWM_Value);
        }
        if (X_PID_Stuct.PWM_Value <= 0)
        {
            // x反向移动，RIG电机推动，CH2->PE11
            TIM_SetCompare2(TIM1, -X_PID_Stuct.PWM_Value);
        }
    }
    if (channel == Y_Channel)
    {
        // PID限幅
        if (Y_PID_Stuct.PWM_Value > PWM_MAX)
            Y_PID_Stuct.PWM_Value = PWM_MAX;
        if (Y_PID_Stuct.PWM_Value < PWM_MIN)
            Y_PID_Stuct.PWM_Value = PWM_MIN;
        if (Y_PID_Stuct.PWM_Value > 0)
        {
            // y正向移动，DWN电机推动，CH4->PA11
            TIM_SetCompare4(TIM1, Y_PID_Stuct.PWM_Value);
        }
        if (Y_PID_Stuct.PWM_Value <= 0)
        {
            // y反向移动，UP电机推动，CH1->PA8
            TIM_SetCompare1(TIM1, -Y_PID_Stuct.PWM_Value);
        }
    }
}

#define MAX_Integral 10000
#define MIN_Integral -10000
/**
 * @brief 角度PID计算函数
 *
 * @param channel x/y轴通道
 */
void MOTOR_PID(int channel)
{
    Motor_PID_Stuct *PID_Stuct;
    if (channel == X_Channel)
    {
        PID_Stuct = &X_PID_Stuct;
    }
    if (channel == Y_Channel)
    {
        PID_Stuct = &Y_PID_Stuct;
    }
    // PID计算部分
    // ERROR
    float Error = PID_Stuct->Goal - PID_Stuct->Current;
    // P
    float Proportion = PID_Stuct->KP * Error;
    // I
    PID_Stuct->Integral += PID_Stuct->KI * Error;
    // D
    float Derivative = PID_Stuct->KD * (PID_Stuct->LastError - Error);
    PID_Stuct->LastError = Error;
    // PID限幅
    PID_Stuct->Integral = (PID_Stuct->Integral > MAX_Integral) ? MAX_Integral : PID_Stuct->Integral;
    PID_Stuct->Integral = (PID_Stuct->Integral < MIN_Integral) ? MIN_Integral : PID_Stuct->Integral;
    PID_Stuct->PWM_Value = Proportion + PID_Stuct->Integral + Derivative;
}

uint8_t Mode_Flag = 0;
float All_range = 0;                // 总摆幅
float X_range = 0;                  // x轴摆幅
float Y_range = 0;                  // y轴摆幅
float Direction_Angle = 0;          // 偏转角
float Radius = 0;                   // 在模式5中的半径
float X_Goalangle = 0;              // x方向目标角度
float Y_Goalangle = 0;              // y方向目标角度
float Time = 0;                     // 时间变量(ms)
float Pitch = 0, Roll = 0, Yaw = 0; // 俯仰角，横滚角

/**
 * @brief PID循环函数，在TIM.c中调用
 *
 */
void MOTOR_LoopFunction(void)
{
    // 读MPU6050数据（获取当前值）
    mpu_dmp_get_data(&Pitch, &Roll, &Yaw);
    // 获取当前应当到达的摆角（获取目标值）
    switch (Mode_Flag)
    {
    case 1: // 模式1，画出一条不少于50cm长的直线
        X_Goalangle = resolving_Xangle(0.30, 0, Time);
        Y_Goalangle = 0;
        break;

    case 2: // 模式2，直线在30-60cm之间可设置
        X_Goalangle = resolving_Xangle(X_range, 0, Time);
        Y_Goalangle = 0;
        break;

    case 3: // 模式3，指定方向且直线长度不小于20cm
        resolving_XYRange(&X_range, &Y_range, 0.15, Direction_Angle);
        X_Goalangle = resolving_Xangle(X_range, 0, Time);
        Y_Goalangle = resolving_Yangle(Y_range, 0, Time);
        break;

    case 4: // 模式4，在5s内快速停止
        Y_Goalangle = 0;
        X_Goalangle = 0;
        // TODO:有关PID参数的设置
        break;

    case 5: // 模式5，画出指定半径的圆
        X_Goalangle = resolving_Xangle(Radius, 3.1415926 / 2, Time);
        Y_Goalangle = resolving_Yangle(Radius, 0, Time);
        break;

    default:
        break;
    }
    // PID计算（获得PWM值）
    X_PID_Stuct.Goal = X_Goalangle;
    Y_PID_Stuct.Goal = Y_Goalangle;
    X_PID_Stuct.Current = Pitch;
    Y_PID_Stuct.Current = Roll;
    MOTOR_PID(X_Channel);
    MOTOR_PID(Y_Channel);
    //  PID装载
    MOTOR_SetPWM(X_Channel);
    MOTOR_SetPWM(Y_Channel);
    //  更新时间
    Time += 10;
    if (Time > 1592)
    {
        Time = 0;
    }
}

/**
 * @brief 模式1，画出一条不少于50cm长的直线
 * @note 长度默认设置60cm
 */
void MOTOR_SetMODE1(void)
{
    Mode_Flag = 1;
}

/**
 * @brief 模式2，直线在30-60cm之间可设置
 *
 * @param Range 所指定的长度（m）
 */
void MOTOR_SetMODE2(float Range)
{
    Mode_Flag = 2;
    X_range = Range / 2;
}

/**
 * @brief 模式3，指定方向且直线长度不小于20cm
 *
 * @param Angle 所指定的角度（角度制）
 * @note 长度默认设置为30cm
 */
void MOTOR_SetMODE3(float Angle)
{
    Mode_Flag = 3;
    Direction_Angle = Angle;
}

/**
 * @brief 模式4，在5s内快速停止
 *
 */
void MOTOR_SetMODE4(void)
{
    Mode_Flag = 4;
}

/**
 * @brief 模式5，画出指定半径的圆
 *
 * @param Rad 所指定的半径（m）
 */
void MOTOR_SetMODE5(float Rad)
{
    Mode_Flag = 5;
    Radius = Rad / 2;
}
