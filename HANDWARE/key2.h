/*此程序用于适配摇杆按键，引脚暂不固定*/
#ifndef __KEY2_H
#define __KEY2_H
#include "stdlib.h"

#define KEY_UP GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_5)   
// 定义KEY_DWN变量，用于获取GPIOF引脚5的输入数据位
#define KEY_DWN GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3)  
// 定义KEY_LFT变量，用于获取GPIOF引脚1的输入数据位
#define KEY_LFT GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_1)  
// 定义KEY_RHT变量，用于获取GPIOC引脚13的输入数据位
#define KEY_RHT GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) 
// 定义KEY_MID变量，用于获取GPIOE引脚5的输入数据位
#define KEY_MID GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)  
// 定义KEY_SET变量，用于获取GPIOE引脚3的输入数据位
#define KEY_SET GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)  
// 定义KEY_RST变量，用于获取GPIOE引脚1的输入数据位
#define KEY_RST GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)

void key2_init(void);
int key2_scan(int mode);
int key2_scan_alone(void);

#endif
