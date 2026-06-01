#include "TB6612_Test.h"
#include "tb6612.h"
#include "Uart_Data.h"
void TB6612_Test1(void)
{
    TB6612_Init();
    TB6612_Forward(20.0f);
    while (1)
    {
        
        /* code */
    }
       
}

void TB6612_Test2(void)
{
    TB6612_Init();
    float duty = 0.0f;
    float Buff[10];
    while (1)
    {
        uint8_t length = Uart0_Receive_Command(Buff);
        if(length == 1) {
            duty = Buff[0];
            TB6612_Forward(duty);
        }
    }
}