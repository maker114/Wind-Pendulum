#ifndef __MOTOR_H
#define __MOTOR_H

#include "delay.h"
#include "usart.h"
#include "sys.h"

#define X_Channel 0 // x轴电机编号为0
#define Y_Channel 1 // y轴电机编号为1

// 电机PID结构体
typedef struct motor_pid
{
	int PWM_Value;		// PWM占空比
	int Channel_Number; // 通道编号
	float KP;			// 比例值
	float KI;			// 积分值
	float KD;			// 微分值
	float Current;		// 当前值
	float Goal;			// 目标值
	float LastError;	// 上一次误差值
	float Integral;		// 积分值
} Motor_PID_Stuct;

extern Motor_PID_Stuct X_PID_Stuct;
extern Motor_PID_Stuct Y_PID_Stuct;

void MOTOR_Init(void);
void MOTOR_SetPWM(int channel);
void MOTOR_PID(int channel);
void MOTOR_LoopFunction(void);

void MOTOR_SetMODE1(void);

void MOTOR_SetMODE2(float Range);

void MOTOR_SetMODE3(float Angle);

void MOTOR_SetMODE4(void);

void MOTOR_SetMODE5(float Rad);

#endif
