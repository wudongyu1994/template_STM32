#include "sys.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "timer.h"
#include "exti.h"

/**********************************************
代码逻辑：


**********************************************/

int main(void)
{
    delay_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    usart1_init(115200);
    printf("uart1_init success!\n");
    usart2_init(115200);
    printf("uart2_init success!\n");
    LED_Init();

    printf("start while(1)\n");
    while(1)
    {
        delay_ms(1000);
        LED0=!LED0;
    }
}
