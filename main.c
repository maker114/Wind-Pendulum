#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "sys.h"
#include "board.h"
#include "TIM.h"
#include "key2.h"
#include "resolving.h"

int main(void)
{
    delay_init(168);
    uart_init(115200);
	TIM1_Init(20000-1,840-1);
	

    float time = 0;
    float X_range = 0;
    float Y_range = 0;
		float side= 0;

    while (1)
    {
        resolving_XYRange(&X_range, &Y_range, 0.50, 45);
        float X_angle = resolving_Xangle(X_range, side, time);
        float Y_angle = resolving_Yangle(Y_range, 0, time);
        time += 5;
			side+=0.001f;
        delay_ms(5);
        printf("%f,%f\r\n", X_angle, Y_angle );
    }
}
