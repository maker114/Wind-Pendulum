#include "resolving.h"
#include <math.h>

#define PI 3.1415926
#define T 1592 // 周期1592ms
#define H 0.63 // 杆长0.63m

void resolving_XYangle(float *X_Now_Angle, float *Y_Now_Angle, float L, float Direction_Angle, float Time)
{
    // 将摆长分解为X轴与Y轴的分量
    float L_X = L * cos(Direction_Angle);
    float L_Y = L * sin(Direction_Angle);
    // 求出X轴与Y轴的分摆角
    float X_Angle = asin(L_X / H);
    float Y_Angle = asin(L_Y / H);
}

// TODO：X与Y轴分别写一个函数，这样方便控制摆幅与相位差，以上函数暂时作废，或者加上设置相位差的函数 XD