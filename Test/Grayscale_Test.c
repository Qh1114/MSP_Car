#include "Grayscale_Test.h"
#include "Grayscale.h"
#include "Usart.h"
#include "Delay.h"

void Grayscale_Test1(void)
{
    Uart0_Printf("Grayscale Test1\n");
    uint16_t sensor_values[GRAYSCALE_SENSOR_CHANNELS];
    Grayscale_Sensor_Read_Single(1);
    while(1) {
        Grayscale_Sensor_Read_All(sensor_values);
        Uart0_Printf("Sensor Values: ");
        for(int i = 0; i < GRAYSCALE_SENSOR_CHANNELS; i++) {
            Uart0_Printf("%d ", sensor_values[i]);
        }
        Uart0_Printf("\n");
        // Uart0_Printf("Value:%d",SENSOR_OUT_READ());
        Delay_ms(500);
    }
}

void Grayscale_Test2(void)
{
    Uart0_Printf("Grayscale Test2\n");
    while(1) {
        for (uint8_t i = 0; i < GRAYSCALE_SENSOR_CHANNELS; i++)
        {
            uint16_t value = Grayscale_Sensor_Read_Single(i);
            Uart0_Printf("Channel %d: %d\n", i, value);
        }
        Uart0_Printf("\n");
        Delay_ms(500);
    }
}