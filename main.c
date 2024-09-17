#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "sys.h"
#include "board.h"
#include "TIM.h"
#include "key2.h"
#include "resolving.h"
#include "motor.h"

int main(void)
{
    delay_init(168);
    TIM1_Init(21000 - 1, 2 - 1); // 4KHZ
    uart_init(115200);
    MOTOR_Init();
    while (1)
    {
        X_PID_Stuct.PWM_Value = -10000;
        Y_PID_Stuct.PWM_Value = -10000;
        MOTOR_SetPWM(X_Channel);
        MOTOR_SetPWM(Y_Channel);
    }
}
