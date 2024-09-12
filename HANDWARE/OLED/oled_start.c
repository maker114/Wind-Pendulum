/**
 * @file oled_start.c
 * @author maker114
 * @brief OLED�����ļ���ײ�ʵ��
 * @version 2.1
 * @date 2024-02-12
 */
#include "oled.h"
u8 OLED_GRAM[128][8]; ///< OLED������

/**
 * ****************************************************************************************
 * @brief ���Ժ���
 * @param i 0:������ʾ 1:������ʾ
 * ****************************************************************************************
 */
// ���Ժ���
void OLED_ColorTurn(u8 i)
{
	if (i == 0)
	{
		OLED_WR_Byte(0xA6, OLED_CMD); // ������ʾ
	}
	if (i == 1)
	{
		OLED_WR_Byte(0xA7, OLED_CMD); // ��ɫ��ʾ
	}
}

/**
 * ****************************************************************************************
 * @brief  ��Ļ��ת180��
 * @param i 0:������ʾ 1:��ת��ʾ
 * ****************************************************************************************
 */
// ��Ļ��ת180��
void OLED_DisplayTurn(u8 i)
{
	if (i == 0)
	{
		OLED_WR_Byte(0xC8, OLED_CMD); // ������ʾ
		OLED_WR_Byte(0xA1, OLED_CMD);
	}
	if (i == 1)
	{
		OLED_WR_Byte(0xC0, OLED_CMD); // ��ת��ʾ
		OLED_WR_Byte(0xA0, OLED_CMD);
	}
}

/**
 * ****************************************************************************************
 * @brief ��дһ���ֽ�
 * @param dat ����
 * @param cmd ָ��
 * ****************************************************************************************
 */
void OLED_WR_Byte(u8 dat, u8 cmd)
{
	u8 i;
	if (cmd)
		OLED_DC_Set();
	else
		OLED_DC_Clr();
	OLED_CS_Clr();
	for (i = 0; i < 8; i++)
	{
		OLED_SCL_Clr();
		if (dat & 0x80)
			OLED_SDA_Set();
		else
			OLED_SDA_Clr();
		OLED_SCL_Set();
		dat <<= 1;
	}
	OLED_CS_Set();
	OLED_DC_Set();
}

/**
 * ****************************************************************************************
 * @brief ����OLED��ʾ
 * ****************************************************************************************
 */
// ����OLED��ʾ
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D, OLED_CMD); // ��ɱ�ʹ��
	OLED_WR_Byte(0x14, OLED_CMD); // ������ɱ�
	OLED_WR_Byte(0xAF, OLED_CMD); // ������Ļ
}

/**
 * ****************************************************************************************
 * @brief �ر�OLED��ʾ
 * ****************************************************************************************
 */
// �ر�OLED��ʾ
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D, OLED_CMD); // ��ɱ�ʹ��
	OLED_WR_Byte(0x10, OLED_CMD); // �رյ�ɱ�
	OLED_WR_Byte(0xAE, OLED_CMD); // �ر���Ļ
}

/**
 * ****************************************************************************************
 * @brief ���ͻ�����
 * @note
 * - ʹ��1.3��OLED��ʾ��ʱ������ʼ��ַΪ0x02
 * - ʹ��0.96��OLED��ʾ��ʱ����ʼ��ַΪ0x00
 * ****************************************************************************************
 */
// �����Դ浽OLED
void OLED_Refresh(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // ��������ʼ��ַ
		OLED_WR_Byte(0x02, OLED_CMD);	  // ���õ�����ʼ��ַ
		OLED_WR_Byte(0x10, OLED_CMD);	  // ���ø�����ʼ��ַ
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
	}
}
/**
 * ****************************************************************************************
 * @brief ��������
 * @attention ȡ����Ĭ�ϵ��������ͻ�����������Ҫ����Ļ����ȫ�������Ҫ�ڴ˺��������OLED_Refresh();
 * ****************************************************************************************
 */
// ��������
void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		for (n = 0; n < 128; n++)
		{
			OLED_GRAM[n][i] = 0; // �����������
		}
	}
	// OLED_Refresh(); // ������ʾ  <-�氭��
}

/**
 * ****************************************************************************************
 * @brief ����
 * @param x 0~127
 * @param y 0~63
 * @param t 1 ��� 0,���
 * ****************************************************************************************
 */
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{

	if (x > 128 || y > 63)
	{
		return; // ������Χ��
	}

	u8 i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;

	if (t)
	{
		OLED_GRAM[x][i] |= n;
	}
	else
	{
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
		OLED_GRAM[x][i] |= n;
		OLED_GRAM[x][i] = ~OLED_GRAM[x][i];
	}
}

/**
 * ****************************************************************************************
 * @brief OLED��ʼ��
 * @note ע���ʼ�������������Ҫ����һ�λ�����
 * ****************************************************************************************
 */
void OLED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE); // ʹ��PORTE,PORTDʱ��

	// GPIO��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	   // ��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	   // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // ����
	GPIO_Init(GPIOD, &GPIO_InitStructure);			   // ��ʼ��
	GPIO_SetBits(GPIOD, GPIO_Pin_10);

	// GPIO��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	   // ��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	   // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // ����
	GPIO_Init(GPIOE, &GPIO_InitStructure);			   // ��ʼ��

	OLED_RES_Clr();
	delay_ms(200);
	OLED_RES_Set();

	OLED_WR_Byte(0xAE, OLED_CMD); //--�ر�OLED��ʾ��
	OLED_WR_Byte(0x00, OLED_CMD); //---���õ��е�ַ
	OLED_WR_Byte(0x10, OLED_CMD); //---���ø��е�ַ
	OLED_WR_Byte(0x40, OLED_CMD); //--������ʼ�е�ַ  ����ӳ��RAM��ʾ��ʼ�У�0x00~0x3F��
	OLED_WR_Byte(0x81, OLED_CMD); //--���öԱȶȿ��ƼĴ���
	OLED_WR_Byte(0xCF, OLED_CMD); // ����SEG�����������
	OLED_WR_Byte(0xA1, OLED_CMD); //--����SEG/��ӳ��     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8, OLED_CMD); // ����COM/��ɨ�跽��   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6, OLED_CMD); //--����������ʾ
	OLED_WR_Byte(0xA8, OLED_CMD); //--���ö�·����(1��64)
	OLED_WR_Byte(0x3f, OLED_CMD); //--1/64��������
	OLED_WR_Byte(0xD3, OLED_CMD); //-������ʾƫ��	�ƶ�ӳ��RAM��������0x00~0x3F��
	OLED_WR_Byte(0x00, OLED_CMD); //-��ƫ��
	OLED_WR_Byte(0xd5, OLED_CMD); //--������ʾʱ�ӷ�Ƶ��/��Ƶ��
	OLED_WR_Byte(0x80, OLED_CMD); //--���÷�Ƶ�ȣ�����ʱ��Ϊ100֡/��
	OLED_WR_Byte(0xD9, OLED_CMD); //--����Ԥ�������
	OLED_WR_Byte(0xF1, OLED_CMD); // ����Ԥ���Ϊ15��ʱ������&�ŵ�Ϊ1��ʱ������
	OLED_WR_Byte(0xDA, OLED_CMD); //--����com����Ӳ������
	OLED_WR_Byte(0x12, OLED_CMD);
	OLED_WR_Byte(0xDB, OLED_CMD); //--����vcomh
	OLED_WR_Byte(0x40, OLED_CMD); // ����VCOMȥѡ���ƽ
	OLED_WR_Byte(0x20, OLED_CMD); //-����ҳ��Ѱַģʽ��0x00/0x01/0x02��
	OLED_WR_Byte(0x02, OLED_CMD); //
	OLED_WR_Byte(0x8D, OLED_CMD); //--���ó�������/����
	OLED_WR_Byte(0x14, OLED_CMD); //--����(0x10)����
	OLED_WR_Byte(0xA4, OLED_CMD); // ����������ʾ��������0xa4/0xa5��
	OLED_WR_Byte(0xA6, OLED_CMD); // ���÷�����ʾ������0xa6/a7��
	OLED_Clear();
	OLED_Refresh();
	OLED_WR_Byte(0xAF, OLED_CMD);
}

/**
 * ****************************************************************************************
 * @brief �ı���Ļ����
 * @param level ���ȵȼ���0~255��
 * ****************************************************************************************
 */
void OELD_ChangeLuminance(u8 level)
{
	// �ж�����ֵ�Ϸ���
	if (level > 255)
		level = 255;
	else if (level < 1)
		level = 1;
	// ����OLED�Ĵ���
	OLED_WR_Byte(0x81, OLED_CMD);
	OLED_WR_Byte(level, OLED_CMD);
}

u8 OLED_Backup[128][8]; ///< ���ݻ�����
/**
 * ****************************************************************************************
 * @brief ���滺��������ͼ��
 * ****************************************************************************************
 */
void OLED_BufferSave(void)
{
	for (u8 i = 0; i < 128; i++)
		for (u8 j = 0; j < 8; j++)
			OLED_Backup[i][j] = OLED_GRAM[i][j];
}

/**
 * ****************************************************************************************
 * @brief ���ػ�����
 * ****************************************************************************************
 */
void OLED_BufferRegain(void)
{
	for (u8 i = 0; i < 128; i++)
		for (u8 j = 0; j < 8; j++)
			OLED_GRAM[i][j] = OLED_Backup[i][j];
}
