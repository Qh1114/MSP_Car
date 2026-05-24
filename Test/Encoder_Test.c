#include "Encoder_Test.h"
#include "ti_msp_dl_config.h"
#include "Encoder.h"
#include "Usart.h"

void Encoder_Test1(void)
{
    int64_t count_l = 0, count_r = 0;
    Encoder_Init();
    while(1)
    {
        Encoder_Count_Get(&count_l, &count_r);
        Uart0_Printf("%d,%d\n", (int32_t)count_l, (int32_t)count_r);
    }
}