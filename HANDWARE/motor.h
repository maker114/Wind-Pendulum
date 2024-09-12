#ifndef __MOTOR_H
#define __MOTOR_H

#include "delay.h"
#include "usart.h"
#include "sys.h"

// ���ſ��Ƴ�ʼ��
#define Direction_Group GPIOD
#define Direction_CLK RCC_AHB1Periph_GPIOD
#define Direction_A1 GPIO_Pin_0
#define Direction_A2 GPIO_Pin_1
#define Direction_B1 GPIO_Pin_2
#define Direction_B2 GPIO_Pin_3

// ���PID�ṹ��
typedef struct motor_pid
{
	int PWM_Value;	  // PWMռ�ձ�
	int Motor_Number; // ������
	int Start_Value;  // ����ֵ
	float KP;
	float KI;
	float KD;
	short Current;	 // ��ǰֵ���ٶ�ֵ��
	float Goal;		 // Ŀ��ֵ
	float LastError; // ��һ�����ֵ
	float Integral;	 // ����ֵ
} Motor_PID_Stuct;

extern Motor_PID_Stuct Left_Motor_PID;
extern Motor_PID_Stuct Right_Motor_PID;

#endif
