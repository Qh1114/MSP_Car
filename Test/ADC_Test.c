#include "ADC.h"
#include "ADC_Test.h"
#include "Usart.h"
#include "Delay.h"
#include "Bat.h"

void ADC_Test1(void)
{
    ADC_Init();
    uint16_t raw_value = 0;
    while(1)
    {
        ADC_Read(&raw_value);
        Uart0_Printf("ADC Raw Value: %d\r\n", raw_value);
        Delay_ms(10);
    }
}

void ADC_Test2(void)
{
    Bat_Init();
    while(1)
    {
        float voltage = Bat_Read();
        Uart0_Printf("Battery Voltage: %.2f V\r\n", voltage);
        Delay_ms(10);
    }
}
