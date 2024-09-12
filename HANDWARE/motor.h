#ifndef __MOTOR_H
#define __MOTOR_H

#include "delay.h"
#include "usart.h"
#include "sys.h"

// 引脚控制初始化
#define Direction_Group GPIOD
#define Direction_CLK RCC_AHB1Periph_GPIOD
#define Direction_A1 GPIO_Pin_0
#define Direction_A2 GPIO_Pin_1
#define Direction_B1 GPIO_Pin_2
#define Direction_B2 GPIO_Pin_3

// 电机PID结构体
typedef struct motor_pid
{
	int PWM_Value;	  // PWM占空比
	int Motor_Number; // 电机编号
	int Start_Value;  // 启动值
	float KP;
	float KI;
	float KD;
	short Current;	 // 当前值（速度值）
	float Goal;		 // 目标值
	float LastError; // 上一次误差值
	float Integral;	 // 积分值
} Motor_PID_Stuct;

extern Motor_PID_Stuct Left_Motor_PID;
extern Motor_PID_Stuct Right_Motor_PID;

#endif
