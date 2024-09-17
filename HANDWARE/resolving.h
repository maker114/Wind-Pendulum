#ifndef __MOTOR_H
#define __MOTOR_H

#include "delay.h"
#include "usart.h"
#include "sys.h"
#define P 3.1415926

void resolving_XYRange(float *X_Range, float *Y_Range, float All_Range, float Direction_Angle);

float resolving_Xangle(float Range, float PhaseDifference, float Time);

float resolving_Yangle(float Range, float PhaseDifference, float Time);

#endif
