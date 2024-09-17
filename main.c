#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "sys.h"
#include "board.h"
#include "TIM.h"
#include "resolving.h"
#include "motor.h"

#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

int main(void)
{
    delay_init(168);
    Board_Init();
    TIM1_Init(21000 - 1, 2 - 1);  // f=4KHZ
    TIM2_Init(10000 - 1, 84 - 1); // T=10ms
    uart_init(115200);
    MOTOR_Init();

    //while (MPU_Init()) // 初始化MPU6050
    //    delay_ms(200);
    //while (mpu_dmp_init()) // 初始化DMP
    //    delay_ms(200);

    while (1)
    {
        MOTOR_SetMODE3(120);
        printf("%.2f,%.2f,%.2f,%.2f\r\n", X_PID_Stuct.Goal, Y_PID_Stuct.Goal, X_PID_Stuct.Current, Y_PID_Stuct.Current);
    }
}
