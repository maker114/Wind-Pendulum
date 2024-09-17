#ifndef __MOTOR_H
#define __MOTOR_H

#include "delay.h"
#include "usart.h"
#include "sys.h"

#define X_Channel 0 // x�������Ϊ0
#define Y_Channel 1 // y�������Ϊ1

// ���PID�ṹ��
typedef struct motor_pid
{
	int PWM_Value;		// PWMռ�ձ�
	int Channel_Number; // ͨ�����
	float KP;			// ����ֵ
	float KI;			// ����ֵ
	float KD;			// ΢��ֵ
	short Current;		// ��ǰֵ
	float Goal;			// Ŀ��ֵ
	float LastError;	// ��һ�����ֵ
	float Integral;		// ����ֵ
} Motor_PID_Stuct;

extern Motor_PID_Stuct X_PID_Stuct;
extern Motor_PID_Stuct Y_PID_Stuct;

#endif
void MOTOR_Init(void);
void MOTOR_SetPWM(int channel);
