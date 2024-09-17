#include "resolving.h"
#include <math.h>

float PI = 3.1415926; // Բ����
float H2 = 0.83;      // ������ڵ�����ĸ߶�
float T = 1592;       // �ڶ�����
float H = 0.63;       // �ڳ�

/**
 * @brief ���ڳ��ֽ�ΪX����Y��ķ���
 *
 * @param X_Range X��ڷ���m��
 * @param Y_Range Y��ڷ���m��
 * @param All_Range �ܰڷ���m��
 * @param Direction_Angle ����ǣ��Ƕ��ƣ�
 */
void resolving_XYRange(float *X_Range, float *Y_Range, float All_Range, float Direction_Angle)
{
    // ���ڳ��ֽ�ΪX����Y��ķ���
    float Angle = Direction_Angle * PI / 180.0f; // �Ƕ�ֵת������
    *X_Range = All_Range * cos(Angle);
    *Y_Range = All_Range * sin(Angle);
}

/**
 * @brief ��X�����ϵİڽ�
 *
 * @param Range �ڷ������ȣ���λm��
 * @param PhaseDifference ��λƫ�ƣ�������,��a*PI/b��
 * @param Time ʱ��ϵ������λMS
 * @return float X��Ƕ�ֵ
 */
float resolving_Xangle(float Range, float PhaseDifference, float Time)
{
    float Angle_Range = (atan((Range / H2))) * 180.0f / PI;                      // �ڷ��Ƕ�
    float Now_Angle = Angle_Range * sin(2.0f * PI * Time / T + PhaseDifference); // ��ǰ�Ƕ�
    return Now_Angle;
}

/**
 * @brief ��Y�����ϵİڽ�
 *
 * @param Range �ڷ������ȣ���λcm��
 * @param PhaseDifference ��λƫ�ƣ�������,��a*PI/b��
 * @param Time ʱ��ϵ������λMS
 * @return float Y��Ƕ�ֵ
 */
float resolving_Yangle(float Range, float PhaseDifference, float Time)
{
    float Angle_Range = (atan((Range / H2))) * 180.0f / PI;                      // �ڷ��Ƕ�
    float Now_Angle = Angle_Range * sin(2.0f * PI * Time / T + PhaseDifference); // ��ǰ�Ƕ�
    return Now_Angle;
}
