/**
 * @file motor.c
 * @author maker114
 * @brief ��������������������ĸ����ı����,���PID����
 * @version 0.1
 * @date 2024-09-12
 * @note �����x��y��Ϊ�ο���Ϊ����
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
 * @brief �����ʼ������
 *
 */
void MOTOR_Init(void)
{
    // PID��ʼ���ṹ���
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
 * @brief ���õ��PWMֵ
 *
 * @param channel ����PWMֵͨ��
 * @note ͨ��0ΪX������ͨ��1ΪY����
 */
void MOTOR_SetPWM(int channel)
{
    if (channel == X_Channel)
    {
        // PID�޷�
        if (X_PID_Stuct.PWM_Value > PWM_MAX)
            X_PID_Stuct.PWM_Value = PWM_MAX;
        if (X_PID_Stuct.PWM_Value < PWM_MIN)
            X_PID_Stuct.PWM_Value = PWM_MIN;
        // PIDװ��
        if (X_PID_Stuct.PWM_Value > 0)
        {
            // x�����ƶ���LFT����ƶ���CH3->PE13
            TIM_SetCompare3(TIM1, X_PID_Stuct.PWM_Value);
        }
        if (X_PID_Stuct.PWM_Value <= 0)
        {
            // x�����ƶ���RIG����ƶ���CH2->PE11
            TIM_SetCompare2(TIM1, -X_PID_Stuct.PWM_Value);
        }
    }
    if (channel == Y_Channel)
    {
        // PID�޷�
        if (Y_PID_Stuct.PWM_Value > PWM_MAX)
            Y_PID_Stuct.PWM_Value = PWM_MAX;
        if (Y_PID_Stuct.PWM_Value < PWM_MIN)
            Y_PID_Stuct.PWM_Value = PWM_MIN;
        if (Y_PID_Stuct.PWM_Value > 0)
        {
            // y�����ƶ���DWN����ƶ���CH4->PA11
            TIM_SetCompare4(TIM1, Y_PID_Stuct.PWM_Value);
        }
        if (Y_PID_Stuct.PWM_Value <= 0)
        {
            // y�����ƶ���UP����ƶ���CH1->PA8
            TIM_SetCompare1(TIM1, -Y_PID_Stuct.PWM_Value);
        }
    }
}

#define MAX_Integral 10000
#define MIN_Integral -10000
/**
 * @brief �Ƕ�PID���㺯��
 *
 * @param channel x/y��ͨ��
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
    // PID���㲿��
    // ERROR
    float Error = PID_Stuct->Goal - PID_Stuct->Current;
    // P
    float Proportion = PID_Stuct->KP * Error;
    // I
    PID_Stuct->Integral += PID_Stuct->KI * Error;
    // D
    float Derivative = PID_Stuct->KD * (PID_Stuct->LastError - Error);
    PID_Stuct->LastError = Error;
    // PID�޷�
    PID_Stuct->Integral = (PID_Stuct->Integral > MAX_Integral) ? MAX_Integral : PID_Stuct->Integral;
    PID_Stuct->Integral = (PID_Stuct->Integral < MIN_Integral) ? MIN_Integral : PID_Stuct->Integral;
    PID_Stuct->PWM_Value = Proportion + PID_Stuct->Integral + Derivative;
}

uint8_t Mode_Flag = 0;
float All_range = 0;                // �ܰڷ�
float X_range = 0;                  // x��ڷ�
float Y_range = 0;                  // y��ڷ�
float Direction_Angle = 0;          // ƫת��
float Radius = 0;                   // ��ģʽ5�еİ뾶
float X_Goalangle = 0;              // x����Ŀ��Ƕ�
float Y_Goalangle = 0;              // y����Ŀ��Ƕ�
float Time = 0;                     // ʱ�����(ms)
float Pitch = 0, Roll = 0, Yaw = 0; // �����ǣ������

/**
 * @brief PIDѭ����������TIM.c�е���
 *
 */
void MOTOR_LoopFunction(void)
{
    // ��MPU6050���ݣ���ȡ��ǰֵ��
    mpu_dmp_get_data(&Pitch, &Roll, &Yaw);
    // ��ȡ��ǰӦ������İڽǣ���ȡĿ��ֵ��
    switch (Mode_Flag)
    {
    case 1: // ģʽ1������һ��������50cm����ֱ��
        X_Goalangle = resolving_Xangle(0.30, 0, Time);
        Y_Goalangle = 0;
        break;

    case 2: // ģʽ2��ֱ����30-60cm֮�������
        X_Goalangle = resolving_Xangle(X_range, 0, Time);
        Y_Goalangle = 0;
        break;

    case 3: // ģʽ3��ָ��������ֱ�߳��Ȳ�С��20cm
        resolving_XYRange(&X_range, &Y_range, 0.15, Direction_Angle);
        X_Goalangle = resolving_Xangle(X_range, 0, Time);
        Y_Goalangle = resolving_Yangle(Y_range, 0, Time);
        break;

    case 4: // ģʽ4����5s�ڿ���ֹͣ
        Y_Goalangle = 0;
        X_Goalangle = 0;
        // TODO:�й�PID����������
        break;

    case 5: // ģʽ5������ָ���뾶��Բ
        X_Goalangle = resolving_Xangle(Radius, 3.1415926 / 2, Time);
        Y_Goalangle = resolving_Yangle(Radius, 0, Time);
        break;

    default:
        break;
    }
    // PID���㣨���PWMֵ��
    X_PID_Stuct.Goal = X_Goalangle;
    Y_PID_Stuct.Goal = Y_Goalangle;
    X_PID_Stuct.Current = Pitch;
    Y_PID_Stuct.Current = Roll;
    MOTOR_PID(X_Channel);
    MOTOR_PID(Y_Channel);
    //  PIDװ��
    MOTOR_SetPWM(X_Channel);
    MOTOR_SetPWM(Y_Channel);
    //  ����ʱ��
    Time += 10;
    if (Time > 1592)
    {
        Time = 0;
    }
}

/**
 * @brief ģʽ1������һ��������50cm����ֱ��
 * @note ����Ĭ������60cm
 */
void MOTOR_SetMODE1(void)
{
    Mode_Flag = 1;
}

/**
 * @brief ģʽ2��ֱ����30-60cm֮�������
 *
 * @param Range ��ָ���ĳ��ȣ�m��
 */
void MOTOR_SetMODE2(float Range)
{
    Mode_Flag = 2;
    X_range = Range / 2;
}

/**
 * @brief ģʽ3��ָ��������ֱ�߳��Ȳ�С��20cm
 *
 * @param Angle ��ָ���ĽǶȣ��Ƕ��ƣ�
 * @note ����Ĭ������Ϊ30cm
 */
void MOTOR_SetMODE3(float Angle)
{
    Mode_Flag = 3;
    Direction_Angle = Angle;
}

/**
 * @brief ģʽ4����5s�ڿ���ֹͣ
 *
 */
void MOTOR_SetMODE4(void)
{
    Mode_Flag = 4;
}

/**
 * @brief ģʽ5������ָ���뾶��Բ
 *
 * @param Rad ��ָ���İ뾶��m��
 */
void MOTOR_SetMODE5(float Rad)
{
    Mode_Flag = 5;
    Radius = Rad / 2;
}
