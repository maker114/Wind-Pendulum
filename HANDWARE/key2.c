#include "key2.h"
#include "delay.h"
#include "oled.h"
#include "usart.h"


void key2_init(void)
{
	/*��ʼ������*/
	GPIO_InitTypeDef GPIO_InitStructure;

	// ʹ��GPIOFʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);

	// ��ʼ��GPIOF1,3,5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	   // ��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // ����
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	// ��ʼ��GPIOE1,3,5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	   // ��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // ����
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	// ��ʼ��GPIOC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	   // ��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	   // ����
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

// ����ɨ��(Ŀǰ����������������)
// mode=1 -> ֧������
// mode=0 -> ��֧������
int key2_scan(int mode)
{
	static uint8_t key_up = 1; // �������ɿ���־
	if (mode)
		key_up = 1; // ֧������
	if (key_up && (KEY_DWN == 0 || KEY_LFT == 0 || KEY_RHT == 0 || KEY_MID == 0 || KEY_UP == 0))
	{
		delay_ms(10); // ȥ����
		key_up = 0;
		if (KEY_DWN == 0)
			return 1;
		else if (KEY_LFT == 0)
			return 2;
		else if (KEY_RHT == 0)
			return 3;
		else if (KEY_MID == 0)
			return 4;
		else if (KEY_UP == 0)
			return 5;
	}
	else if (KEY_DWN == 1 && KEY_LFT == 1 && KEY_RHT == 1 && KEY_MID == 1 && KEY_UP == 1)
		key_up = 1;
	return 0; // �ް�������
}

int key2_scan_alone(void)
{
	return key2_scan(1);
}
