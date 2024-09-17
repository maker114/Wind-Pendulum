#include "resolving.h"
#include <math.h>

float PI = 3.1415926; // 圆周率
float H2 = 0.83;      // 从万向节到地面的高度
float T = 1592;       // 摆动周期
float H = 0.63;       // 摆长

/**
 * @brief 将摆长分解为X轴与Y轴的分量
 *
 * @param X_Range X轴摆幅（m）
 * @param Y_Range Y轴摆幅（m）
 * @param All_Range 总摆幅（m）
 * @param Direction_Angle 方向角（角度制）
 */
void resolving_XYRange(float *X_Range, float *Y_Range, float All_Range, float Direction_Angle)
{
    // 将摆长分解为X轴与Y轴的分量
    float Angle = Direction_Angle * PI / 180.0f; // 角度值转弧度制
    *X_Range = All_Range * cos(Angle);
    *Y_Range = All_Range * sin(Angle);
}

/**
 * @brief 在X方向上的摆角
 *
 * @param Range 摆幅（长度，单位m）
 * @param PhaseDifference 相位偏移（弧度制,即a*PI/b）
 * @param Time 时间系数，单位MS
 * @return float X轴角度值
 */
float resolving_Xangle(float Range, float PhaseDifference, float Time)
{
    float Angle_Range = (atan((Range / H2))) * 180.0f / PI;                      // 摆幅角度
    float Now_Angle = Angle_Range * sin(2.0f * PI * Time / T + PhaseDifference); // 当前角度
    return Now_Angle;
}

/**
 * @brief 在Y方向上的摆角
 *
 * @param Range 摆幅（长度，单位cm）
 * @param PhaseDifference 相位偏移（弧度制,即a*PI/b）
 * @param Time 时间系数，单位MS
 * @return float Y轴角度值
 */
float resolving_Yangle(float Range, float PhaseDifference, float Time)
{
    float Angle_Range = (atan((Range / H2))) * 180.0f / PI;                      // 摆幅角度
    float Now_Angle = Angle_Range * sin(2.0f * PI * Time / T + PhaseDifference); // 当前角度
    return Now_Angle;
}
