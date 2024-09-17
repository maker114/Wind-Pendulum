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
void MOTOR_PIDPID(int channel)
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