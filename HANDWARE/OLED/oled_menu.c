/**
 * @file oled_menu.c
 * @author maker114
 * @brief OLED菜单文件
 * @version 2.1
 * @date 2024-02-12
 * @note 传入字符串指针格式为u8 *menu[ ] = { ... };
 */

#include "oled.h"

typedef int (*Key_scan_fun)(void); 
Key_scan_fun key_scan;

/* ***************************************** 设置部分 ***************************************** */

// 以下由MENU_SetMode函数设置
int mode = 1;	// 下一次是否播放动画
int choose = 1; // 下一次进入动画时的选择
//以下由接入的按键类型决定
int continuous = 0; // 当continuous=1时支持连按
// 按照按键传入的情况自定义对应的键值
enum KeyPress
{
	KEY_NONE = 0,       // 没有按键按下
	KEY_DECREMENT = 1,  // 减少
	KEY_INCREMENT = 5,  // 增加
	KEY_SELECT = 4,	   // 确认
	KEY_EXIT = 2	   // 返回
};

/* ***************************************** 菜单函数部分 ***************************************** */

/**
 * ****************************************************************************************
 * @brief 光标移动动画函数
 * @param goal_cursor 目标数据
 * @param now_cursor 当前数据（指针）
 * @note 此函数由指针完成操作，无需返回值，通常搭配do...while()...使用
 * ****************************************************************************************
 */
void OLED_MoveCursor(float goal_cursor, float *now_cursor, u8 delay)
{
	// 锟叫断癸拷锟斤拷欠锟斤拷丫锟斤拷咏锟侥匡拷锟�
	int max_speed = 5;
	if (fabs(goal_cursor - *now_cursor) <= 1)
		*now_cursor = goal_cursor; // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷为float锟斤拷锟斤拷锟斤拷锟斤拷使锟斤拷fabs
	else
	{
		// 锟斤拷锟斤拷锟皆憋拷锟解当锟斤拷锟斤拷目锟斤拷锟斤拷远时锟斤拷锟斤拷俣裙锟斤拷炜达拷锟斤拷锟�
		float Increment = (goal_cursor - *now_cursor) / 6;
		if (Increment >= max_speed)
			*now_cursor += max_speed;
		else
			*now_cursor += Increment; // 锟斤拷锟接癸拷锟�
	}
	delay_ms(delay); // 锟斤拷锟狡诧拷锟斤拷时锟斤拷
}

/**
 * ****************************************************************************************
 * @brief 输出一个由某点旋转后的坐标
 * @param x 输出的x坐标
 * @param y 输出的y坐标
 * @param Core_x 旋转中心y坐标
 * @param Core_y 旋转中心y坐标
 * @param angle 旋转角度
 * @param distance 旋转半径
 * ****************************************************************************************
 */
void OLED_Rotate(u8 *x, u8 *y, u8 Core_x, u8 Core_y, float angle, float distance)
{
	*x = Core_x + (int)distance * cos(RADIAN(angle));
	*y = Core_y + (int)distance * sin(RADIAN(angle));
}


u8 show_data[100]; ///< 曲线图中数据的缓存数组
/**
 * ****************************************************************************************
 * @brief 曲线图生成函数
 * @param zoom 缩放(小于1时缩小，大于1时放大)
 * @param skew 偏移
 * @param data 传入的数据
 * @note
 * - 每调用一次函数时曲线图向左移动一个像素,故图像的移动速度取决于此函数的调用速度
 * - 当数据出现显示不全时应调整zoom与skew的值
 * @bug 当以垂直线连接相邻两点时会出现显示错误，目前使用像素点来显示数据
 * ****************************************************************************************
 */
void OLED_DrawCurve(float zoom, float skew, float data)
{
    float data2;
    // 应用缩放和偏移
    data2 = data * zoom + skew;
    // y轴反转
    data2 = 64 - data2;
    // 将数据放到数组最后一位
    show_data[99] = data2;
    for (int i = 0; i < 99; i++)
    {
        show_data[i] = show_data[i + 1];
    }
    // 清屏
    OLED_Clear();

    // 绘制坐标轴
    OLED_DrawFastHLine(10, 54, 108, 1); // 横轴
    OLED_DrawFastVLine(10, 10, 44, 1);  // 纵轴

    // 显示坐标轴标签
    OLED_ShowString(118, 50, "x", 8, 1); // x轴标签
    OLED_ShowString(7, 0, "y=", 8, 1);   // y轴标签
    OLED_ShowFNum(7 + 13, 0, data2, 3, 8, 1); // 显示最新的数据点的y值

    for (int i = 0; i < 99; i++)
    {
        // 绘制数据点
        if (show_data[i] - 10 > 10)
        {
            OLED_DrawPoint(i + 10, show_data[i] - 10, 1); // 绘制点
        }
    }

    OLED_Refresh(); // 刷新显示
}

/**
 * ****************************************************************************************
 * @brief 按键扫描注册函数
 * @param put_in_fun 输入的函数指针
 * @note 此函数接受的扫描函数类型为 int fun(void)返回值为255以内的整数且支持连按
 * ****************************************************************************************
 */
void MENU_SignKeyFun(int put_in_fun(void))
{
	key_scan = put_in_fun;
}

/**
 * ****************************************************************************************
 * @brief 菜单显示函数
 * @param menu 指针数组，用于传入菜单显示的文本内容
 * @param arr_len 选项个数（注意不是数组长度）
 * @return int 选项编号
 * @note 当选项个数超过3个时会自动以8号字体显示
 * ****************************************************************************************
 */
int MENU_NormalDisplay(u8 *menu[], u8 arr_len)
{
    u8 size, key_num = 1, flag = choose, last_flag = choose, key_flag = 0; // key_flag: 键状态标志0未按下, 1已按下
    float this_x = 15, last_x = 0, cursor_y = 0;

    if (mode == 1)
    {
        OLED_DrawVacuumization(0, 0, 128, 64);
        OLED_Refresh();
        delay_ms(50);
    }

    delay_ms(50); // 延迟

    size = (arr_len <= 3) ? 12 : 8; // 根据选项数量设定字体大小
    cursor_y = choose * size + 5;

    if (mode)
        Normal_First_IN(menu, arr_len, size); // 进入时的动画

    do
    {
        OLED_Clear();
        key_num = key_scan();
        // 支持连续按键
        if (continuous)
            key_flag = 0;

        // 显示菜单标题
        OLED_ShowString(0, 0 * size, menu[0], size, 1);
        OLED_DrawLine(0, 1 * size + 2, 128, 1 * size + 2, 1);
        // 循环显示菜单选项
        for (int i = 1; i <= arr_len; i++)
        {
            if (i == flag)
            {
                OLED_MoveCursor(15, &this_x, 2);
                OLED_MoveCursor(i * size + 5, &cursor_y, 2);
                OLED_ShowString(this_x, i * size + 5, menu[i], size, 1);
                OLED_ShowString(0, cursor_y, ">", size, 1); // 绘制指示符
            }
            else if (i == last_flag)
            {
                OLED_MoveCursor(0, &last_x, 2);
                OLED_ShowString(last_x, i * size + 5, menu[i], size, 1);
            }
            else
            {
                OLED_ShowString(0, i * size + 5, menu[i], size, 1);
            }
        } // for
        // 按键状态检测
        if (key_num != KEY_NONE && key_flag == 0)
        {
            last_flag = flag;          // 记录上一次的选项
            if (key_num == KEY_DECREMENT) // 向上翻页
                flag = (flag % arr_len) + 1;
            else if (key_num == KEY_INCREMENT) // 向下翻页
                flag = (flag == 1) ? arr_len : flag - 1;
            else if (key_num == KEY_SELECT)    // 确认选择
            {
                choose = flag, mode = 1;
                return flag;
            }
            // 更新光标位置
            this_x = 0, last_x = 15, cursor_y = last_flag * size + 5;
            // 更新按键按下状态
            key_flag = 1; // 按键已按下
        }
        if (key_num == KEY_NONE && key_flag == 1)
            key_flag = 0; // 当KEY_NONE时重置按键状态
        OLED_Refresh();
    } while (key_num != KEY_EXIT);
    choose = 1, mode = 1; // 退出时重置状态
    return 0; // 返回0, 表示无选择退出
}

/**
 * ****************************************************************************************
 * @brief  侧边菜单显示函数
 * @param menu 指针数组，用于传入菜单显示的文本内容
 * @param arr_len 选项个数
 * @param w 侧面弹窗的宽度，视显示字符长短而定
 * @return int 选项编号
 * @note 此函数默认且推荐使用8号字体，不过依然可以强制使用其他字号
 * ****************************************************************************************
 */
int MENU_SideDisplay(u8 *menu[], u8 arr_len, u8 w)
{
    w = 128 - w;													 
    u8 size = 8, key_num = 1, flag = 1, last_flag = 1, key_flag = 0; // key_flag: 标记键盘状态0表示按下 1表示释放
    float this_x = w + 6, last_x = w + 6;
    Side_First_IN(menu, arr_len, size, 128 - w);
    // delay_ms(100); // 延时(如果之前需要做一些初始化操作则要进行延时)
    do
    {
        OLED_DrawFill(w, 0, w + 1, 64, 0);
        key_num = key_scan();
        // 支持连按
        if (continuous)
            key_flag = 0;
        OLED_DrawFastVLine(w, 0, 64, 1);
        // 显示菜单选项
        for (int i = 1; i <= arr_len; i++)
        {
            if (i == flag)
            {
                OLED_MoveCursor(w + 2, &this_x, 2);
                // (int)((64-arr_len*8)/2)表示选项在屏幕中垂直居中的位置
                OLED_ShowString(this_x, (i - 1) * size + (int)((64 - arr_len * 8) / 2), menu[i - 1], size, 1);
            }
            else if (i == last_flag)
            {
                OLED_MoveCursor(w + 6, &last_x, 2);
                OLED_ShowString(last_x, (i - 1) * size + (int)((64 - arr_len * 8) / 2), menu[i - 1], size, 1);
            }
            else
                OLED_ShowString(w + 6, (i - 1) * size + (int)((64 - arr_len * 8) / 2), menu[i - 1], size, 1);
        } // for
        // 处理按键事件
        if (key_num != KEY_NONE && key_flag == 0)
        {
            last_flag = flag;			  // 记录上一次选中的选项
            if (key_num == KEY_DECREMENT) // 上翻页
                flag = (flag % arr_len) + 1;
            else if (key_num == KEY_INCREMENT) // 下翻页
                flag = (flag == 1) ? arr_len : flag - 1;
            else if (key_num == KEY_SELECT) // 确定
            {
                Side_First_OUT(menu, arr_len, size, 128 - w); // 退出菜单
                return flag;
            }
            // 刷新选中和上一次选中的状态
            this_x = w + 6, last_x = w + 2;
            // 刷新键盘状态
            key_flag = 1; // 按键释放状态
        }
        if (key_num == KEY_NONE && key_flag == 1)
            key_flag = 0; // 当KEY_NONE时需要重置按键状态
        OLED_Refresh();
    } while (key_num != KEY_EXIT); // 直到按下退出键才退出循环
    Side_First_OUT(menu, arr_len, size, 128 - w); // 退出菜单
    return 0;									  // 返回0表示取消选择
}

/**
 * ****************************************************************************************
 * @brief  侧边菜单显示函数
 * @param menu 指针数组，用于传入菜单显示的文本内容
 * @param arr_len 选项个数
 * @param w 侧面弹窗的宽度，视显示字符长短而定
 * @return int 选项编号
 * @note 此函数默认且推荐使用8号字体，不过依然可以强制使用其他字号
 * ****************************************************************************************
 */
void MENU_MiddleDisplay(u8 *chr, float MIN, float MAX, float *value)
{
    u8 key_num = 0, y = 15, key_flag = 0;
    float percent, step = 10, change1;

    // 计算百分比
    percent = ((*value - MIN) / (MAX - MIN) * 100);
    change1 = percent * 0.85f;
    Middle_First_IN(*value, percent, chr);
    do
    {
        key_num = key_scan();
        // 支持连按
        if (continuous)
            key_flag = 0;
        OLED_Clear();
        OLED_BufferRegain(); // 恢复写入缓冲区数据
        OLED_DrawVacuumization(0, 0, 128, 64);
        OLED_DrawFill(15 - 3, y - 3, 98 + 6, 37 + 6, 0);

        if (key_num != KEY_NONE && key_flag == 0)
        {
            if (key_num == KEY_INCREMENT && percent < 100)
                percent += step;
            else if (key_num == KEY_DECREMENT && percent > 0)
                percent -= step;
            else if (key_num == KEY_SELECT) // 确定
            {
                switch ((int)(step * 100))
                {
                case 1000:
                    step = 1;
                    break; // 将step从10设置为1
                case 100:
                    step = 0.1;
                    break; // 将step从1设置为0.1
                case 10:
                    step = 0.01;
                    break; // 将step从0.1设置为0.01
                case 1:
                    step = 10;
                    break; // 将step从0.01设置为10
                default:
                    step = 10;
                    break; // 默认step设置为10
                }
            }
            percent = fmax(0, fmin(100, percent)); // 使用math.h中的函数进行限制

            key_flag = 1; // 键盘释放状态
            *value = percent * (MAX - MIN) / 100 + MIN;
        }
        if (key_num == KEY_NONE && key_flag == 1)
            key_flag = 0; // 当KEY_NONE时需要重置按键状态

        OLED_MoveCursor(percent * 0.85f, &change1, 2);
        // 绘制
        OLED_DrawBox(15, y, 98, 37, 1);			  // 边框
        OLED_DrawBox(20, y + 14, 88, 10, 1);	  // 内框
        OLED_DrawFill(22, y + 16, change1, 7, 1); // 进度条
        OLED_DrawFastHLine(15, y + 11, 98, 1);	  // 水平线
        // 文本
        OLED_ShowString(16, y + 2, chr, 8, 1);		   // 标题
        OLED_ShowString(19, y + 27, "PERCENT:", 8, 1); // 百分比
        OLED_ShowFNum(70, y + 2, *value, 4, 8, 1);	   // 当前值
        OLED_ShowFNum(66, y + 27, percent, 3, 8, 1);   // 百分比
        OLED_ShowString(102, y + 27, "%", 8, 1);	   // 百分号

        // 进度条志示
        switch ((int)(step * 100))
        {
        case 1000:
            OLED_DrawFastHLine(72, y + 35, 6, 1);
            break;
        case 100:
            OLED_DrawFastHLine(72 + 6, y + 35, 6, 1);
            break;
        case 10:
            OLED_DrawFastHLine(72 + 18, y + 35, 6, 1);
            break;
        case 1:
            OLED_DrawFastHLine(72 + 24, y + 35, 6, 1);
            break;
        default:
            OLED_DrawFastHLine(72 + 30, y + 35, 6, 1);
            ;
            break;
        }

        OLED_Refresh();
    } while (key_num != KEY_EXIT);
    Middle_First_OUT(*value, percent, chr);
}

/**
 * ****************************************************************************************
 * @brief 单数值中心弹窗
 * @param chr 弹窗标题
 * @param step 调节步长
 * @param num 被调节对象
 * @return float 按下确定键后返回改变值，按下返回键值将不做改变
 * @note 暂不支持负数
 * ****************************************************************************************
 */
float MENU_ChangeDisplay(u8 *chr, float step, float num)
{
    u8 key_num = 0, y = 26, key_flag = 0;
    float save_num = num;
    Change_First_IN(num, chr);
    do
    {
        key_num = key_scan();
        OLED_Clear();
        OLED_BufferRegain(); // 恢复写入缓冲区数据
        OLED_DrawVacuumization(0, 0, 128, 64);
        OLED_DrawFill(12, y - 3, 104, 18, 0);

        if (key_num != KEY_NONE && key_flag == 0)
        {
            if (key_num == KEY_INCREMENT)
                num += step;
            else if (key_num == KEY_DECREMENT)
                num -= step;
            else if (key_num == KEY_SELECT) // 确定
            {
                Change_First_OUT(num, chr);
                return num;
            }
            // num = fmax(0, fmin(100, percent)); // 使用math.h中的函数进行限制

            //	key_flag = 1; // 键盘释放状态
            delay_ms(120);
        }
        // if (key_num == KEY_NONE && key_flag == 1)
        //	key_flag = 0; // 键值为KEY_NONE时需要重置按键状态

        // 绘制
        // OLED_DrawBox(15, y, 98, 12, 1); // 边框
        OLED_DrawBezierBox(15, y, 98, 12, 6);
        // 文本
        OLED_ShowString(18, y + 2, chr, 8, 1);	// 标题
        OLED_ShowFNum(68, y + 2, num, 4, 8, 1); // 当前值

        OLED_Refresh();
    } while (key_num != KEY_EXIT);
    Change_First_OUT(num, chr);
    return save_num;
}

/**
 * ****************************************************************************************
 * @brief 串口显示
* @note 接受串口内容并依次显示在屏幕上
 * ****************************************************************************************
 */
u8 USART_DATA[21][8]; ///< 文字存储数组，8号字体可以显示8行21列
void MENU_USARTDisplay(void)
{
	// 定义变量
	u8 RX_Data = 0;
	// 由于实现方法不同且与delay使用同一串口故暂时禁用串口中断
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); // 关闭中断
	// 第一次进入函数时清除全数组内容
	for (u8 i = 0; i < 21; i++)
	{
		for (u8 j = 0; j < 8; j++)
		{
			USART_DATA[i][j] = 32; // ASCII码中32为SPACE
		}
	}
	// 显示欢迎页面
	OLED_Clear();
	OLED_ShowString(0, 8 * 8, "Waiting...", 8, 1);
	OLED_Refresh();
	// 显示循环
	do
	{
		// 接收串口的数据并存入数组

		while (1)
		{
			// 在接受下一行数据前先对之前的数据进行处理
			// 舍弃第一行，将剩下的行上移，清空第八行
			for (u8 n = 0; n < 21; n++)
			{
				// 剩下的行上移
				for (u8 t = 1; t <= 7; t++)
				{
					USART_DATA[n][t - 1] = USART_DATA[n][t];
				}
				// 清空第八行
				USART_DATA[n][7] = 32;
			}
			for (u8 m = 0; m < 21; m++) // 遍历以存满第八行
			{

				// 接收数据
				while (1)
				{
					if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET) // 0 循环等待 1 可以接收数据
					{
						RX_Data = USART_ReceiveData(USART1);
						// printf("RX_DATA=%d\r\n", RX_Data);
						break;
					}
				}
				// 分析数据
				if (RX_Data >= 0x20 && RX_Data <= 0x7E)
				{
					USART_DATA[m][7] = RX_Data; // 检查数据合法性
				}
				if (RX_Data == '\n')
				{
					break; // 当检查到换行或回车时跳出循环，刷新显示
				}
			}
			// 运行到这里代表已经接受满一行或接受到换行
			// 遍历以显示8行
			OLED_Clear();
			for (u8 y = 0; y < 8; y++)
			{
				for (u8 x = 0; x < 21; x++)
				{
					OLED_ShowChar(6 * x, y * 8, USART_DATA[x][y], 8, 1);
				}
			}
			OLED_Refresh();
		}
	} while (key_scan() != KEY_EXIT);
	// 程序结束后为避免影响其他程序的正常使用，重新开启串口中断
	// USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启中断
	// return;
}

u8 *text[8] = {" ", " ", " ", " ", " ", " ", " ", " "};
/**
 * **************************************************************************************
 * @brief 滚动显示函数
 * @param chr 需要显示的字符
 * **************************************************************************************
 */
void MENU_RollingDisplaced(u8 *chr)
{
	OLED_Clear();
	text[0] = chr;
	for (u8 i = 7; i > 0; i--)
	{
		text[i] = text[i - 1];
	}
	for (u8 j = 0; j < 8; j++)
	{
		OLED_ShowString(0, 64 - j * 8, text[j], 8, 1);
	}
	OLED_Refresh();
}

/**
 * **************************************************************************************
 * @brief 清除滚动显示函数缓冲区
 * **************************************************************************************
 */
void MENU_RollingClear(void)
{
	for (u8 i = 0; i < 8; i++)
	{
		MENU_RollingDisplaced(" ");
	}
}

/**
 * ****************************************************************************************
 * @brief 显示设置，用于从上一级返回时保存选项同时避免重复播放进入动画
 * @param set 在进入函数时光标所指的选项编号
 * @param Mode 1：播放动画  0：不播放动画
 * @note 在不使用此函数时默认为选项1并播放动画
 * ****************************************************************************************
 */
void MENU_SetMode(int set, int Mode)
{
	mode = Mode;
	choose = set;
}

/*======================================================菜单动画部分======================================================*/

float now_y_arr[6] = {0, 0, 0, 0, 0, 0};
/**
 * ****************************************************************************************
 * @brief 初次进入普通菜单时的动画
 * ****************************************************************************************
 */
void Normal_First_IN(u8 *menu[], u8 arr_len, u8 size)
{
	float change2 = size + 5, change3 = 0; // 锟斤拷锟街ｏ拷锟斤拷锟解，锟街革拷锟斤拷
	int delay = 1;
	do
	{
		OLED_Clear();

		OLED_MoveCursor(0, &change2, delay + 3); // 锟斤拷锟斤拷
		OLED_MoveCursor(128, &change3, delay);	 // 锟街革拷锟斤拷

		for (int i = 1; i <= arr_len; i++)
		{
			if (i == choose)
			{
				OLED_MoveCursor(i * size + 5, &now_y_arr[i], delay + 2); // 锟斤拷锟斤拷
				OLED_ShowString(0, now_y_arr[i], ">", size, 1);
				OLED_ShowString(15, now_y_arr[i], menu[i], size, 1);
			}
			else
			{
				OLED_MoveCursor(i * size + 5, &now_y_arr[i], delay + 2); // 锟斤拷锟斤拷
				OLED_ShowString(0, now_y_arr[i], menu[i], size, 1);
			}
		} // for

		// 锟斤拷示锟斤拷锟斤拷锟剿碉拷锟斤拷
		OLED_ShowString(0, change2, menu[0], size, 1);
		OLED_DrawLine(0, 1 * size + 2, change3, 1 * size + 2, 1);
		// OLED_DrawFastHLine(0, 1 * size + 2, change3, 1);
		delay_ms(2);
		OLED_Refresh();
	} while (change2 > 0 || change3 < 128);

	for (u8 j = 0; j < 6; j++)
	{
		now_y_arr[j] = 0;
	}
}

/**
 * ****************************************************************************************
 * @brief 初次进入侧边弹窗菜单时的动画
 * ****************************************************************************************
 */
void Side_First_IN(u8 *menu[], u8 arr_len, u8 size, u8 w)
{
	float change1 = 128, change2 = 128;	   // 锟街革拷锟竭ｏ拷锟斤拷锟斤拷
	OLED_DrawVacuumization(0, 0, 128, 64); // 锟介化
	OLED_BufferSave();					   // 锟斤拷锟芥缓锟斤拷
	do
	{
		OLED_DrawFill(change1, 0, 128 - change1 + 1, 64, 0);
		OLED_MoveCursor(128 - w, &change1, 2);	   // 锟街革拷锟斤拷
		OLED_MoveCursor(128 - w + 6, &change2, 2); // 锟斤拷锟斤拷
		for (int i = 1; i <= arr_len; i++)
		{
			OLED_ShowString(change2, (i - 1) * size + (int)((64 - arr_len * 8) / 2), menu[i - 1], size, 1);
		} // for
		OLED_DrawFastVLine(change1, 0, 64, 1);
		OLED_Refresh();
	} while (change1 > 128 - w || change2 > 128 - w + 6);
}

/**
 * ****************************************************************************************
 * @brief 离开侧边弹窗菜单时的动画
 * ****************************************************************************************
 */
void Side_First_OUT(u8 *menu[], u8 arr_len, u8 size, u8 w)
{
	float change1 = 128 - w, change2 = 128 - w + 6; // 锟街革拷锟竭ｏ拷锟斤拷锟斤拷
	do
	{
		OLED_BufferRegain(); // 锟斤拷锟斤拷写锟诫缓锟斤拷锟斤拷
		OLED_DrawFill(change1, 0, 128 - change1 + 1, 64, 0);
		OLED_MoveCursor(128, &change1, 2); // 锟街革拷锟斤拷
		OLED_MoveCursor(128, &change2, 2); // 锟斤拷锟斤拷
		for (int i = 1; i <= arr_len; i++)
		{
			OLED_ShowString(change2, (i - 1) * size + (int)((64 - arr_len * 8) / 2), menu[i - 1], size, 1);
		} // for
		OLED_DrawFastVLine(change1, 0, 64, 1);
		OLED_Refresh();
	} while (change1 < 128 || change2 < 128);
}

/**
 * ****************************************************************************************
 * @brief 进入百分比中心弹窗时的动画
 * ****************************************************************************************
 */
void Middle_First_IN(float data, float percent, u8 *chr)
{
	float change1 = 64;
	float change2 = 0;
	float change3 = 0;
	OLED_BufferSave(); // 锟斤拷锟芥当前锟斤拷锟斤拷锟斤拷
	do
	{
		OLED_Clear();
		OLED_BufferRegain();				   // 锟斤拷锟斤拷写锟诫缓锟斤拷锟斤拷
		OLED_DrawVacuumization(0, 0, 128, 64); // 锟介化
		OLED_DrawFill(15 - 3, change1 - 3, 98 + 6, 37 + 6, 0);
		OLED_MoveCursor(15, &change1, 2);
		OLED_MoveCursor(percent, &change2, 2);
		OLED_MoveCursor(88, &change3, 2);
		// 锟斤拷锟斤拷
		OLED_DrawBox(15, change1, 98, 37, 1);					// 锟斤拷锟�
		OLED_DrawBox(20, change1 + 14, change3, 10, 1);			// 锟斤拷锟斤拷锟斤拷锟斤拷
		OLED_DrawFill(22, change1 + 16, change2 * 0.85f, 7, 1); // 锟斤拷锟斤拷锟斤拷锟斤拷
		OLED_DrawFastHLine(15, change1 + 11, 98, 1);			// 锟街革拷锟斤拷
		// 锟斤拷锟斤拷
		OLED_ShowString(16, change1 + 2, chr, 8, 1);		 // 锟斤拷锟斤拷
		OLED_ShowString(19, change1 + 27, "PERCENT:", 8, 1); // 锟劫分憋拷
		OLED_ShowFNum(70, change1 + 2, data, 4, 8, 1);		 // 锟斤拷前值
		OLED_ShowFNum(66, change1 + 27, change2, 3, 8, 1);	 // 锟劫分憋拷
		OLED_ShowString(102, change1 + 27, "%", 8, 1);		 // 锟劫分猴拷
		OLED_DrawFastHLine(72 + 6, change1 + 35, 6, 1);		 // 锟斤拷锟侥憋拷志
		OLED_Refresh();
	} while (change1 > 15);
}

/**
 * ****************************************************************************************
 * @brief 离开百分比中心弹窗时的动画
 * ****************************************************************************************
 */
void Middle_First_OUT(float data, float percent, u8 *chr)
{
	float change1 = 15;

	do
	{
		OLED_Clear();
		OLED_BufferRegain(); // 锟斤拷锟斤拷写锟诫缓锟斤拷锟斤拷
		OLED_DrawFill(15, change1, 98, 37, 0);
		OLED_MoveCursor(64, &change1, 2);
		// 锟斤拷锟斤拷
		OLED_DrawBox(15, change1, 98, 37, 1);					// 锟斤拷锟�
		OLED_DrawBox(20, change1 + 14, 88, 10, 1);				// 锟斤拷锟斤拷锟斤拷锟斤拷
		OLED_DrawFill(22, change1 + 16, percent * 0.85f, 7, 1); // 锟斤拷锟斤拷锟斤拷锟斤拷
		OLED_DrawFastHLine(15, change1 + 11, 98, 1);			// 锟街革拷锟斤拷
		// 锟斤拷锟斤拷
		OLED_ShowString(16, change1 + 2, chr, 8, 1);		 // 锟斤拷锟斤拷
		OLED_ShowString(19, change1 + 27, "PERCENT:", 8, 1); // 锟劫分憋拷
		OLED_ShowFNum(70, change1 + 2, data, 4, 8, 1);		 // 锟斤拷前值
		OLED_ShowFNum(66, change1 + 27, percent, 3, 8, 1);	 // 锟劫分憋拷
		OLED_ShowString(102, change1 + 27, "%", 8, 1);		 // 锟劫分猴拷
		OLED_DrawFastHLine(72 + 6, change1 + 35, 6, 1);		 // 锟斤拷锟侥憋拷志
		OLED_Refresh();
	} while (change1 < 64);
}

/**
 * ****************************************************************************************
 * @brief 进入数值改变中心弹窗时的动画
 * ****************************************************************************************
 */
void Change_First_IN(float num, u8 *chr)
{
	float change1 = 64;
	OLED_BufferSave(); // 锟斤拷锟芥当前锟斤拷锟斤拷锟斤拷
	do
	{
		OLED_Clear();
		OLED_BufferRegain();				   // 锟斤拷锟斤拷写锟诫缓锟斤拷锟斤拷
		OLED_DrawVacuumization(0, 0, 128, 64); // 锟介化
		OLED_DrawFill(12, change1 - 3, 104, 18, 0);
		OLED_MoveCursor(26, &change1, 2);
		// 锟斤拷锟斤拷
		OLED_DrawBezierBox(15, change1, 98, 12, 6); // 锟斤拷锟�
		// 锟斤拷锟斤拷
		OLED_ShowString(18, change1 + 2, chr, 8, 1);  // 锟斤拷锟斤拷
		OLED_ShowFNum(68, change1 + 2, num, 4, 8, 1); // 锟斤拷前值
		OLED_Refresh();
	} while (change1 > 26);
}

/**
 * ****************************************************************************************
 * @brief 离开数值改变中心弹窗时的动画
 * ****************************************************************************************
 */
void Change_First_OUT(float num, u8 *chr)
{
	float change1 = 26;

	do
	{
		OLED_Clear();
		OLED_BufferRegain(); // 锟斤拷锟斤拷写锟诫缓锟斤拷锟斤拷
		OLED_DrawFill(15, change1, 98, 12, 0);
		OLED_MoveCursor(64, &change1, 2);
		// 锟斤拷锟斤拷
		OLED_DrawBezierBox(15, change1, 98, 12, 6); // 锟斤拷锟�
		// 锟斤拷锟斤拷
		OLED_ShowString(18, change1 + 2, chr, 8, 1);  // 锟斤拷锟斤拷
		OLED_ShowFNum(68, change1 + 2, num, 4, 8, 1); // 锟斤拷前值
		OLED_Refresh();
	} while (change1 < 64);
}

/**
 * ****************************************************************************************
 * @brief 进入曲线图时的动画
 * ****************************************************************************************
 */
void Curve_First_IN(void)
{
	float change1 = 0, change2 = 0;
	do
	{
		OLED_Clear();
		OLED_MoveCursor(108, &change1, 2);
		OLED_MoveCursor(44, &change2, 2);

		OLED_DrawFastHLine(10, 55, change1, 1);
		OLED_DrawFastVLine(10, 55 - change2, change2, 1);

		OLED_ShowString(change1 + 10, 50, "x", 8, 1);
		OLED_ShowString(7, 45 - change2, "y=", 8, 1);
		OLED_ShowFNum(7 + 13, 45 - change2, 0, 3, 8, 1);

		OLED_Refresh();
	} while (change1 < 118 && change2 < 44);

	for (u8 i = 0; i < 100; i++)
		show_data[i] = 0;
}
