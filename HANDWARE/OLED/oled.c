/**
 * @file oled.c
 * @author maker114
 * @brief OLED������ʾ����
 * @version STM32F407(���SPI) 2.1
 * @date 2024-02-12
 */
#include "oled.h"
#include "oledfont.h"
#include "bmp.h"

/**
 * ****************************************************************************************
 * @brief ���ߺ���
 * @param x1 ���x����
 * @param y1 ���y����
 * @param x2 �յ�x����
 * @param y2 �յ�y����
 * @param mode 0:���� 1:����
 * ****************************************************************************************
 */
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // ������������
	delta_y = y2 - y1;
	uRow = x1; // �����������
	uCol = y1;
	incx = (delta_x > 0) - (delta_x < 0); // �������õ�������
	incy = (delta_y > 0) - (delta_y < 0); // �������õ�������
	delta_x = abs(delta_x);				  // ȡ����ֵ���������ж�
	delta_y = abs(delta_y);
	distance = MAX(delta_x, delta_y); // ѡȡ��������������

	for (t = 0; t <= distance; t++)
	{
		OLED_DrawPoint(uRow, uCol, mode); // ����

		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/**
 * ****************************************************************************************
 * @brief �����ߺ���
 * @param x1 ���x����
 * @param y1 ���y����
 * @param x2 �յ�x����
 * @param y2 �յ�y����
 * @param mode 0:���� 1:����
 * ****************************************************************************************
 */
void OLED_DrawDashedLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int Interval = 0, Polarity = 0;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // ������������
	delta_y = y2 - y1;
	uRow = x1; // �����������
	uCol = y1;
	incx = (delta_x > 0) - (delta_x < 0); // �������õ�������
	incy = (delta_y > 0) - (delta_y < 0); // �������õ�������
	delta_x = abs(delta_x);				  // ȡ����ֵ���������ж�
	delta_y = abs(delta_y);
	distance = MAX(delta_x, delta_y); // ѡȡ��������������

	for (t = 0; t <= distance; t++)
	{
		Interval++;
		if (Interval <= 1)
		{
			// ͨ�����������ֱ�Ӿ��� OLED_DrawPoint �ĵ���������
			OLED_DrawPoint(uRow, uCol, Polarity == 0 ? 1 : 0);
		}
		else
		{
			Interval = 0;
			Polarity = !Polarity; // �ߵ� Polarity ��ֵ
		}

		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
/**
 * ****************************************************************************************
 * @brief ������Բ
 * @param x Բ��x����
 * @param y Բ��y����
 * @param r Բ�İ뾶
 * ****************************************************************************************
 */
void OLED_DrawCircle(int usX_Center, int usY_Center, int usRadius)
{
	short sCurrentX, sCurrentY;
	short sError;
	sCurrentX = 0;
	sCurrentY = usRadius;
	sError = 3 - (usRadius << 1); // �ж��¸���λ�õı�־
	while (sCurrentX <= sCurrentY)
	{
		OLED_DrawPoint(usX_Center + sCurrentX, usY_Center + sCurrentY, 1); // 1���о�����
		OLED_DrawPoint(usX_Center - sCurrentX, usY_Center + sCurrentY, 1); // 2
		OLED_DrawPoint(usX_Center - sCurrentY, usY_Center + sCurrentX, 1); // 3
		OLED_DrawPoint(usX_Center - sCurrentY, usY_Center - sCurrentX, 1); // 4
		OLED_DrawPoint(usX_Center - sCurrentX, usY_Center - sCurrentY, 1); // 5
		OLED_DrawPoint(usX_Center + sCurrentX, usY_Center - sCurrentY, 1); // 6
		OLED_DrawPoint(usX_Center + sCurrentY, usY_Center - sCurrentX, 1); // 7
		OLED_DrawPoint(usX_Center + sCurrentY, usY_Center + sCurrentX, 1); // 0
		sCurrentX++;
		if (sError < 0)
			sError += 4 * sCurrentX + 6;
		else
		{
			sError += 10 + 4 * (sCurrentX - sCurrentY);
			sCurrentY--;
		}
	}
}

#define PI 3.1415926535
/**
 * ****************************************************************************************
 * @brief ������Բ�����Ǻ����棩
 * @param x Բ��x����
 * @param y Բ��y����
 * @param r Բ�İ뾶
 * ****************************************************************************************
 */
void OLED_DrawCircle_math(u8 x, u8 y, u8 r)
{
	float Deviation_x = 0, Deviation_y = 0;
	for (u8 i = 1; i < 90; i++)
	{
		if (i == 45)
			continue;

		Deviation_x = r * cos(i * 2 * PI / 360);
		Deviation_y = r * sin(i * 2 * PI / 360);

		// ��һ����
		OLED_DrawPoint(x + Deviation_x, y + Deviation_y, 1);
		// �ڶ�����
		OLED_DrawPoint(x - Deviation_x, y + Deviation_y, 1);
		// ��������
		OLED_DrawPoint(x + Deviation_x, y - Deviation_y, 1);
		// ��������
		OLED_DrawPoint(x - Deviation_x, y - Deviation_y, 1);
	}
}

/**
 * ****************************************************************************************
 * @brief ��ָ��λ����ʾһ���ַ�,���������ַ�
 * @param x 0~127
 * @param y 0~63
 * @param chr �ı�����
 * @param size1 ѡ������ 6x8/6x12/8x16/12x24
 * @param mode 0,��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode)
{
	u8 i, m, temp, size2, chr1;
	u8 x0 = x, y0 = y;
	if (size1 == 8)
		size2 = 6;
	else
		size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * (size1 / 2); // �õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr1 = chr - ' ';											   // ����ƫ�ƺ��ֵ
	for (i = 0; i < size2; i++)
	{
		if (size1 == 8)
		{
			temp = asc2_0806[chr1][i];
		} // ����0806����
		else if (size1 == 12)
		{
			temp = asc2_1206[chr1][i];
		} // ����1206����
		else if (size1 == 16)
		{
			temp = asc2_1608[chr1][i];
		} // ����1608����
		else if (size1 == 24)
		{
			temp = asc2_2412[chr1][i];
		} // ����2412����
		else
			return;
		for (m = 0; m < 8; m++)
		{
			if (temp & 0x01)
				OLED_DrawPoint(x, y, mode);
			else
				OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((size1 != 8) && ((x - x0) == size1 / 2))
		{
			x = x0;
			y0 = y0 + 8;
		}
		y = y0;
	}
}

/**
 * ****************************************************************************************
 * @brief ��ʾһ���ַ���
 * @param x x�������
 * @param y y�������
 * @param chr �ַ�����ʼ��ַ
 * @param size1 �����С
 * @param mode 0,��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode)
{
	while ((*chr >= ' ') && (*chr <= '~')) // �ж��ǲ��ǷǷ��ַ�!
	{
		OLED_ShowChar(x, y, *chr, size1, mode);
		if (size1 == 8)
			x += 6;
		else
			x += size1 / 2;
		chr++;
	}
}

// m^n
u32 OLED_Pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
	{
		result *= m;
	}
	return result;
}

/**
 * ****************************************************************************************
 * @brief ��ʾ����
 * @param x x�������
 * @param y y�������
 * @param num Ҫ��ʾ������
 * @param len ���ֵ�λ��
 * @param size1 �����С
 * @param mode 0,��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowNum(u8 x, u8 y, int num, u8 len, u8 size1, u8 mode)
{
	u8 t, temp, next_temp, first_flag = 0, m = 0;
	if (size1 == 8)
		m = 2;
	for (t = 0; t < len; t++)
	{
		temp = (num / OLED_Pow(10, len - t - 1)) % 10; // ��t=0ʱ��ʾǧλ�����Դ�����
		next_temp = (num / OLED_Pow(10, len - t)) % 10;
		if (temp == 0)
		{
			if (next_temp == 0 && first_flag == 0) // ��һ�����ֲ���0�һ�û��ʼ��ʾ����
			{
				OLED_ShowChar(x + (size1 / 2 + m) * t, y, ' ', size1, mode);
			}
			if (first_flag == 1 || t == len - 1)
			{
				OLED_ShowChar(x + (size1 / 2 + m) * t, y, '0', size1, mode);
				first_flag = 1;
			}
		}
		else
		{
			OLED_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
			first_flag = 1;
		}
	}
}
/**
 * ****************************************************************************************
 * @brief ��ʾ����������
 * @param x x�������
 * @param y y�������
 * @param num Ҫ��ʾ������
 * @param len ���ֵ�λ��
 * @param size1 �����С
 * @param mode 0,��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowSNum(u8 x, u8 y, int num, u8 len, u8 size1, u8 mode)
{
	u8 t, temp, next_temp, first_flag = 0, m = 0;
	if (size1 == 8)
		m = 2;
	if (num < 0)
	{
		OLED_ShowString(x, y, "-", size1, mode);
		num = -num;
	}
	x += size1 / 2 + m;
	for (t = 0; t < len; t++)
	{
		temp = (num / OLED_Pow(10, len - t - 1)) % 10; // ��t=0ʱ��ʾǧλ�����Դ�����
		next_temp = (num / OLED_Pow(10, len - t)) % 10;
		if (temp == 0)
		{
			if (next_temp == 0 && first_flag == 0) // ��һ�����ֲ���0�һ�û��ʼ��ʾ����
			{
				OLED_ShowChar(x + (size1 / 2 + m) * t, y, ' ', size1, mode);
			}
			if (first_flag == 1 || t == len - 1)
			{
				OLED_ShowChar(x + (size1 / 2 + m) * t, y, '0', size1, mode);
				first_flag = 1;
			}
		}
		else
		{
			OLED_ShowChar(x + (size1 / 2 + m) * t, y, temp + '0', size1, mode);
			first_flag = 1;
		}
	}
}
/**
 * ****************************************************************************************
 * @brief ��ʾ����
 * @param x x�������
 * @param y y�������
 * @param num ���ֶ�Ӧ�����
 * @param size1 �����С
 * @param mode 0,��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode)
{
	u8 m, temp;
	u8 x0 = x, y0 = y;
	u16 i, size3 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) * size1; // �õ�����һ���ַ���Ӧ������ռ���ֽ���
	for (i = 0; i < size3; i++)
	{
		if (size1 == 16)
		{
			temp = Hzk1[num][i];
		} // ����16*16����
		else if (size1 == 24)
		{
			temp = Hzk2[num][i];
		} // ����24*24����
		else if (size1 == 32)
		{
			temp = Hzk3[num][i];
		} // ����32*32����
		else if (size1 == 64)
		{
			temp = Hzk4[num][i];
		} // ����64*64����
		else
			return;
		for (m = 0; m < 8; m++)
		{
			if (temp & 0x01)
				OLED_DrawPoint(x, y, mode);
			else
				OLED_DrawPoint(x, y, !mode);
			temp >>= 1;
			y++;
		}
		x++;
		if ((x - x0) == size1)
		{
			x = x0;
			y0 = y0 + 8;
		}
		y = y0;
	}
}

/**
 * ****************************************************************************************
 * @brief ��ʾͼƬ
 * @param x x�������
 * @param y y�������
 * @param sizex ͼƬ����
 * @param sizey ͼƬ���
 * @param BMP Ҫд���ͼƬ����
 * @param mode ��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode)
{
	u16 j = 0;
	u8 i, n, temp, m;
	u8 x0 = x, y0 = y;
	sizey = sizey / 8 + ((sizey % 8) ? 1 : 0);
	for (n = 0; n < sizey; n++)
	{
		for (i = 0; i < sizex; i++)
		{
			temp = BMP[j];
			j++;
			for (m = 0; m < 8; m++)
			{
				if (temp & 0x01)
					OLED_DrawPoint(x, y, mode);
				else
					OLED_DrawPoint(x, y, !mode);
				temp >>= 1;
				y++;
			}
			x++;
			if ((x - x0) == sizex)
			{
				x = x0;
				y0 = y0 + 8;
			}
			y = y0;
		}
	}
}

/**
 * @}
 */

/*======================================================����ͼ�ο�=====================================================*/
/** @addtogroup ����ͼ�ο�
 * @{
 */

/**
 * ****************************************************************************************
 * @brief ����ˮƽ��
 * @param x x�������
 * @param y y�������
 * @param w ��ȣ����ȣ�
 * @param mode 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawFastHLine(int x, int y, int w, int mode)
{
	int end = x + w;
	int a;
	for (a = MAX(0, x); a < MIN(end, 128); a++)
	{
		OLED_DrawPoint(a, y, mode);
	}
}

/**
 * ****************************************************************************************
 * @brief ���ٴ�ֱ��
 * @param x x�������
 * @param y y�������
 * @param h �߶ȣ����ȣ�
 * @param mode 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawFastVLine(int x, int y, int h, int mode)
{
	int end = y + h;
	int a;
	for (a = MAX(0, y); a < MIN(end, 64); a++)
	{
		OLED_DrawPoint(x, a, mode);
	}
}

/**
 * ****************************************************************************************
 * @brief ����������
 * @param x x�������
 * @param y y�������
 * @param w ���
 * @param h �߶�
 * @param mode 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawFill(int x, int y, int w, int h, int mode)
{
	int end_x = x + w;
	int end_y = y + h;
	int a, b;
	for (a = MAX(0, x); a < MIN(end_x, 128); a++)
	{
		for (b = MAX(0, y); b < MIN(end_y, 64); b++)
		{
			OLED_DrawPoint(a, b, mode);
		}
	}
}

/**
 * ****************************************************************************************
 * @brief ����ָ���߿����
 * @param x x�������
 * @param y y�������
 * @param w ���
 * @param h �߶�
 * @param Thickness ��ȣ���ȣ�
 * @param mode 1 ��� 0,���
 * @note �ߵĴ�ϸ������չ
 * ****************************************************************************************
 */
void OLED_DrawFill_T(int x, int y, int w, int h, u8 Thickness, int mode)
{
	for (u8 i = 0; i < Thickness; i++)
	{
		// ����������
		OLED_DrawFastVLine(x + i, y + i, h - 2 * i, mode);
		// �����Ϻ���
		OLED_DrawFastHLine(x + i, y + i, w - 2 * i, mode);
		// �����º���
		OLED_DrawFastHLine(x + i, y - i + h, w - 2 * i, mode);
		// ����������
		OLED_DrawFastVLine(x - i + w, y + i, h + 1 - 2 * i, mode);
	}
}

/**
 * ****************************************************************************************
 * @brief ���ƿ��ľ���
 * @param x x�������
 * @param y y�������
 * @param w ���
 * @param h �߶�
 * @param mode 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawBox(int x, int y, int w, int h, int mode)
{
	// ���������߶�
	// ����������
	OLED_DrawFastVLine(x, y, h, mode);
	// �����Ϻ���
	OLED_DrawFastHLine(x, y, w, mode);
	// �����º���
	OLED_DrawFastHLine(x, y + h, w, mode);
	// ����������
	OLED_DrawFastVLine(x + w, y, h + 1, mode);
}

/**
 * ****************************************************************************************
 * @brief ���ƿ�Բ�Ǿ���
 * @param x x�������
 * @param y y�������
 * @param w ���
 * @param h �߶�
 * @param r Բ�ǰ뾶
 * @param mode 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawRoundedBox(int x, int y, int w, int h, int r, int mode)
{
	short sCurrentX, sCurrentY;
	short sError;

	// ����Բ��
	sCurrentX = 0;
	sCurrentY = r;
	sError = 3 - (r << 1); // �ж��¸���λ�õı�־

	while (sCurrentX <= sCurrentY)
	{
		// ���Ͻ�
		OLED_DrawPoint(x + w - r + sCurrentX, y + r - sCurrentY, mode); // 6
		OLED_DrawPoint(x + w - r + sCurrentY, y + r - sCurrentX, mode); // 7

		// ���Ͻ�
		OLED_DrawPoint(x + r - sCurrentX, y + r - sCurrentY, mode); // 5
		OLED_DrawPoint(x + r - sCurrentY, y + r - sCurrentX, mode); // 4

		// ���½�
		OLED_DrawPoint(x + r - sCurrentX, y + h - r + sCurrentY, mode); // 1
		OLED_DrawPoint(x + r - sCurrentY, y + h - r + sCurrentX, mode); // 2

		// ���½�
		OLED_DrawPoint(x + w - r + sCurrentX, y + h - r + sCurrentY, mode); // 0
		OLED_DrawPoint(x + w - r + sCurrentY, y + h - r + sCurrentX, mode); // 3

		sCurrentX++;
		if (sError < 0)
		{
			sError += 4 * sCurrentX + 6;
		}
		else
		{
			sError += 10 + 4 * (sCurrentX - sCurrentY);
			sCurrentY--;
		}
	}
	// �����ϱ�ˮƽֱ��
	OLED_DrawFastHLine(x + r, y, w - 2 * r, 1);
	// �����±�ˮƽֱ��
	OLED_DrawFastHLine(x + r, y + h, w - 2 * r, 1);
	// ������ߴ�ֱֱ��
	OLED_DrawFastVLine(x, y + r, h - 2 * r, 1);
	// �����ұߴ�ֱֱ��
	OLED_DrawFastVLine(x + w, y + r, h - 2 * r, 1);
}

/**
 * ****************************************************************************************
 * @brief ʮ��������ʾ����
 * @param x x������
 * @param y y������
 * @param num ��Ҫ��ʾ�����ݣ�ʮ���ƣ�
 * @param size �ֺ�
 * @param mode 1:���ϡ�0x��  0:ֱ����ʾ
 * @note
 * - ֻ����ʾһ���ֽڵ�ʮ�������޷�������
 * - Ĭ��������ʾ���Ƿ�ɫ
 * ****************************************************************************************
 */
void OLED_ShowNum16(u8 x, u8 y, u8 num, u8 size, u8 mode)
{
	// �ж��Ƿ���ʾ��0x����ע����ܴ��ڵ���ʾ�ռ䲻������⣩
	if (mode == 1)
	{
		OLED_ShowString(x, y, "0x", size, 1);
		x += size;
	}
	// ������ת��Ϊʮ�����ƺ���ʾ
	// ʮλ
	if (num / 16 < 10)
	{
		OLED_ShowNum(x, y, num / 16, 1, size, 1);
	}
	else
	{
		OLED_ShowChar(x, y, 65 + (int)(num / 16) - 10, size, 1);
	}
	// ��λ
	x += size / 2;

	if (num % 16 < 10)
	{
		OLED_ShowNum(x, y, num % 16, 1, size, 1);
	}
	else
	{
		OLED_ShowChar(x, y, 65 + (int)(num % 16) - 10, size, 1);
	}
}

/**
 * ****************************************************************************************
 * @brief ���ƵȾ�ˮƽƽ����
 * @param x x����ʼ����
 * @param y y����ʼ����
 * @param w ���
 * @param h �߶�
 * @param section �ֶΣ�ϸ�֣���
 * @param mode 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawParallelHLine(u8 x, u8 y, u8 w, u8 h, u8 section, u8 mode)
{
	// �ж�����ֵ�Ƿ�Ϸ�
	if (section < 2)
		return;
	// ȷ�����ֶ�������
	float space = (float)h / (float)(section - 1);
	// ������ʾÿһ��ֱ��
	for (u8 i = 0; i < section; i++)
	{
		OLED_DrawFastHLine(x, y + i * space, w, mode);
	}
}

/**
 * ****************************************************************************************
 * @brief ���ƵȾഹֱƽ����
 * @param x x����ʼ����
 * @param y y����ʼ����
 * @param w ���
 * @param h �߶�
 * @param section �ֶΣ�ϸ�֣���
 * @param mode 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawParallelVLine(u8 x, u8 y, u8 w, u8 h, u8 section, u8 mode)
{
	// �ж�����ֵ�Ƿ�Ϸ�
	if (section < 2)
		return;
	// ȷ�����ֶ�������
	float space = (float)w / (float)(section - 1);
	// ������ʾÿһ��ֱ��
	for (u8 i = 0; i < section; i++)
	{
		OLED_DrawFastVLine(x + i * space, y, h, mode);
	}
}

/**
 * ****************************************************************************************
 * @brief �黯Ŀ������
 * @param x x������ʼ��
 * @param y y������ʼ��
 * @param w ���
 * @param h �߶�
 * ****************************************************************************************
 */
void OLED_DrawVacuumization(u8 x, u8 y, u8 w, u8 h)
{
#if 1
	// ����״�黯
	for (u8 m = x; m < w; m++)
		for (u8 n = y; n < h; n++)
			if ((m % 2) == (n % 2))
				OLED_DrawPoint(m, n, 0);
#else
	// ����״�黯
	for (u8 m = x; m < w; m++)
		if ((m % 2) == 1)
			OLED_DrawFastVLine(m, y, h, 0);
	for (u8 n = y; n < h; n++)
		if ((n % 2) == 1)
			OLED_DrawFastHLine(x, n, w, 0);
#endif
}

/**
 * ****************************************************************************************
 * @brief ��ʾ��λС��
 * @param x x�������
 * @param y y�������
 * @param data Ҫ��ʾ������
 * @param len ��������
 * @param size �����С
 * @param mode 0,��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowFNum(u8 x, u8 y, float data, u8 len, u8 size, u8 mode)
{
	int m = (size == 8) ? 2 : 0;
	// �ᴿ�����С��������
	int num = data * 100;
	int fnum = num % 100;
	int inum = num / 100;
	// ��ʾ����
	OLED_ShowNum(x, y, inum, len, size, 1);
	OLED_ShowString(x + (size / 2 + m) * len, y, ".", size, 1);
	OLED_ShowNum(x + (size / 2 + m) * (len + 1), y, fnum, 2, size, 1);

	if (fnum < 10)
		OLED_ShowChar(x + (size / 2 + m) * (len + 1), y, '0', size, 1);
}

/**
 * ****************************************************************************************
 * @brief ��ʾ�и�������λС��
 * @param x x�������
 * @param y y�������
 * @param data Ҫ��ʾ������
 * @param len ��������
 * @param size �����С
 * @param mode 0,��ɫ��ʾ;1,������ʾ
 * ****************************************************************************************
 */
void OLED_ShowSFNum(u8 x, u8 y, float data, u8 len, u8 size, u8 mode)
{
	int m = (size == 8) ? 2 : 0;
	if (data < 0)
	{
		OLED_ShowString(x, y, "-", size, mode);
		data = -data;
	}
	x += size / 2 + m;
	// �ᴿ�����С��������
	int num = data * 100;
	int fnum = num % 100;
	int inum = num / 100;
	// ��ʾ����
	OLED_ShowNum(x, y, inum, len, size, 1);
	OLED_ShowString(x + (size / 2 + m) * len, y, ".", size, 1);
	OLED_ShowNum(x + (size / 2 + m) * (len + 1), y, fnum, 2, size, 1);

	if (fnum < 10)
		OLED_ShowChar(x + (size / 2 + m) * (len + 1), y, '0', size, 1);
}

/**
 * ****************************************************************************************
 * @brief ����һ������������
 * @param x0 �˵�A��x����
 * @param y0 �˵�A��y����
 * @param x1 ���Ƶ��x����
 * @param y1 ���Ƶ��y����
 * @param x2 �˵�B��x����
 * @param y2 �˵�B��y����
 * ****************************************************************************************
 */
void OLED_DrawBezierCurve(int x0, int y0, int x1, int y1, int x2, int y2)
{
	// ��̬���ڲ���
	double a = abs(x2 - x0);
	double b = abs(y2 - y0);
	double step = 0.6 / sqrt(a * a + b * b);

	for (float t = 0; t <= 1; t += step)

	{
		// ���ݶ��α��������ߵĹ�ʽ���� x �� y ����
		int xt = (int)((1 - t) * (1 - t) * x0 + 2 * (1 - t) * t * x1 + t * t * x2);
		int yt = (int)((1 - t) * (1 - t) * y0 + 2 * (1 - t) * t * y1 + t * t * y2);

		// ʹ�� OLED_drawPoint �������Ƽ�����ĵ�
		OLED_DrawPoint(xt, yt, 1);
	}
}

/**
 * ****************************************************************************************
 * @brief ����һ���ɱ��������߹��ɵ�G2Բ�Ǿ���
 * @param x ���x����
 * @param y ���y����
 * @param w ��
 * @param h ��
 * @param r Բ��ϵ��������Ϊ�̱߳��ȵ�40%����
 * ****************************************************************************************
 */
void OLED_DrawBezierBox(int x, int y, int w, int h, int r)
{
	int x_start = x + r,
		x_end = x + w - r,
		y_start = y + r,
		y_end = y + h - r;
	/****ֱ�߲���****/
	// ��ֱ��
	OLED_DrawFastHLine(x_start, y_start - r, w - 2 * r + 1, 1);
	// ��ֱ��
	OLED_DrawFastHLine(x_start, y_end + r, w - 2 * r + 1, 1);
	// ��ֱ��
	OLED_DrawFastVLine(x_start - r, y_start, h - 2 * r + 1, 1);
	// ��ֱ��
	OLED_DrawFastVLine(x_end + r, y_start, h - 2 * r + 1, 1);
	/****���߲���****/

	// ��̬���ڲ���
	double step = 0.6 / sqrt(2 * r * r);

	for (float t = 0; t <= 1; t += step)
	{
		// ���ݶ��α��������ߵĹ�ʽ���� x �� y ����
		int xt = (int)((1 - t) * (1 - t) * 0 + 2 * (1 - t) * t * 0 + t * t * r);
		int yt = (int)((1 - t) * (1 - t) * 0 + 2 * (1 - t) * t * r + t * t * r);

		// ʹ�� OLED_drawPoint �������Ƽ�����ĵ�
		OLED_DrawPoint(x_start - r + xt, y_start - yt - 1, 1);
		OLED_DrawPoint(x_end + r - xt, y_start - yt - 1, 1);
		OLED_DrawPoint(x_start - r + xt, y_end + yt + 1, 1);
		OLED_DrawPoint(x_end + r - xt, y_end + yt + 1, 1);
	}
}
