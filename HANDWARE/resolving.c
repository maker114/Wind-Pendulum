#include "resolving.h"
#include <math.h>

#define PI 3.1415926
#define T 1592 // ����1592ms
#define H 0.63 // �˳�0.63m

void resolving_XYangle(float *X_Now_Angle, float *Y_Now_Angle, float L, float Direction_Angle, float Time)
{
    // ���ڳ��ֽ�ΪX����Y��ķ���
    float L_X = L * cos(Direction_Angle);
    float L_Y = L * sin(Direction_Angle);
    // ���X����Y��ķְڽ�
    float X_Angle = asin(L_X / H);
    float Y_Angle = asin(L_Y / H);
}

// TODO��X��Y��ֱ�дһ������������������ưڷ�����λ����Ϻ�����ʱ���ϣ����߼���������λ��ĺ��� XD