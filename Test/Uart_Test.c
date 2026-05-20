#include "ti_msp_dl_config.h"
#include "Uart_Test.h"
#include "Usart.h"
#include "Delay.h"

void Uart_Test1(void)
{
    Uart0_Printf("Uart Test1\n");
    while(1)
    {
        Uart0_Printf("Time: %u\n", Get_ms());
    }
}

void Uart_Test2(void)
{
    Uart0_Printf("Uart Test2\n");
    while(1)
    {
        Uart0_Printf("Time: %u\n", Get_us());
        Delay_ms(100);
    }
}