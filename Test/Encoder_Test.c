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
        // int ena_r = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
        // int enb_r = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
        // Uart0_Printf("%d,%d\n", ena_r, enb_r);
        Encoder_Count_Get(&count_l, &count_r);
        Uart0_Printf("%d,%d\n", (int32_t)count_l, (int32_t)count_r);
    }
}

void Encoder_Test2(void)
{
    int64_t count_l = 0, count_r = 0;
    float speed_l = 0.0f, speed_r = 0.0f;
    float angle_l = 0.0f, angle_r = 0.0f;
    Encoder_Init();
    while(1)
    {
        Encoder_Count_Get(&count_l, &count_r);
        Encoder_Speed_Get(&speed_l, &speed_r);
        Encoder_AngleSpeed_Get(&angle_l, &angle_r);
        Uart0_Printf("%d,%d,%.2f, %.2f, %.2f, %.2f\n", (int32_t)count_l, (int32_t)count_r, speed_l, speed_r, angle_l, angle_r);
    }
}