#include "vl53l1x_Test.h"
#include "vl53l1.h"
#include "Usart.h"
#include "Delay.h"
#include "ti_msp_dl_config.h"
#include "my_vl53l1x.h"

void VL53L1X_Test1(void)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    int32_t distance1 = 0;
    Status = VL53L1Init(&VL53, 0x54);
    Status = VL53InitParam(&VL53,2);
    while(1)
    {
        //VL53L1_Error Status = VL53L1_ERROR_NONE;
        uint64_t start_time = Get_us();
        //Status = getDistance(&VL53);
        Status = getDistance_Simply(&VL53, &distance1);
        uint64_t end_time = Get_us();
        if(Status == -7)
        {
            Delay_ms(1);
            continue;
        }
        if(Status!=VL53L1_ERROR_NONE)
        {
            Uart0_Printf("get distance failed!\r\n");
            return;
        }
        Uart0_Printf("distance:%d mm\r\n",distance1);
        Uart0_Printf("time: %u us\r\n",(uint32_t)(end_time-start_time));
        Delay_ms(1000);
    }
}

int32_t distance1 = 0;
int32_t distance2 = 0;
int32_t distance3 = 0;
static void Uart0_Send(void)
{
    PERIODIC_R(50)
    Uart0_Printf("distance1:%d mm,distance2:%d mm,distance3:%d mm\r\n",distance1,distance2,distance3);

}
void VL53L1X_Test2(void)
{
    my_vl53l1x_init();
    while(1)
    {
        my_vl53l1x_get_distance(&distance1, 1);
        my_vl53l1x_get_distance(&distance2, 2);
        // my_vl53l1x_get_distance(&distance1, 2);
        // Uart0_Printf("distance2:%d mm\r\n",distance1);
        my_vl53l1x_get_distance(&distance3, 3);
        // Uart0_Printf("distance3:%d mm\r\n",distance1);
        Uart0_Send();
    }
}

