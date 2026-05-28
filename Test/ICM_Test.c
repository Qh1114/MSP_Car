#include "ICM_Test.h"
#include "icm42688.h"
#include "Usart.h"
#include "Delay.h"
#include "IMU.h"
void ICM42688_Test1(void)
{
    int16_t temp = 0;
    icm42688RawData_t accData;
    icm42688RawData_t GyroData;
    icm42688RealData_t accRealData;
    icm42688RealData_t GyroRealData;
    Delay_ms(1000);
    uint8_t who = 0;
    // while(1)
    // {
    who = bsp_IcmGetWhoAmI();
    //     Uart0_Printf("ICM42688 whoami code: %d\r\n", who);
    // }
    uint8_t init_result = bsp_Icm42688Init();
    if(init_result != 0)
    {
        while(1)
        {
            Uart0_Printf("ICM42688 initialization failed with error code: %d\r\n", init_result);
        }
    }
    
    while(1)
    {
        bsp_IcmGetTemperature(&temp);
        bsp_IcmGetAccelerometer(&accData);
        bsp_IcmGetGyroscope(&GyroData);
        bsp_IcmGetRawData(&accRealData, &GyroRealData);
        Uart0_Printf("ICM42688 WhoAmI: 0x%02X, Temperature: %.2f C\r\n", who, temp / 100.0);
        Uart0_Printf("ICM42688 Accelerometer: %d, %d, %d\r\n", accData.x, accData.y, accData.z);
        Uart0_Printf("ICM42688 Gyroscope: %d, %d, %d\r\n", GyroData.x, GyroData.y, GyroData.z);
        Uart0_Printf("ICM42688 Accelerometer Real Data: %.2f mg, %.2f mg, %.2f mg\r\n", accRealData.x, accRealData.y, accRealData.z);
        Uart0_Printf("ICM42688 Gyroscope Real Data: %.2f dps, %.2f dps, %.2f dps\r\n", GyroRealData.x, GyroRealData.y, GyroRealData.z);
        Delay_ms(100);
    }
}

void ICM42688_Test2(void)
{
    IMU_init();
    while (1) 
    {   extern uint32_t count;
        extern float ypr[3];
        Uart0_Printf("%.2f, %.2f, %.2f\n", ypr[0], ypr[1], ypr[2]);
        Delay_ms(50);
    }
}

void ICM42688_Test3(void)
{
    bsp_Icm42688Init();
    // uint8_t values[400] = {0};
    while (1) 
    {   
    //     uint16_t data_length = ICM42688_Get_FIFO_Data_Length();
    //     Uart0_Printf("ICM42688 FIFO Data Length: %d bytes\r\n", data_length);
    //   ICM42688_UnBlocking_CallBack();
        // uint16_t data_length = ICM42688_Get_FIFO_Data(values);
        // if(data_length > 0) {
        //     Uart0_Printf("ICM42688 FIFO Data Length: %d bytes\r\n", data_length);
        //     for (uint16_t i = 0; i < data_length; i++) {
        //         Uart0_Printf("%02X ", values[i]);
        //         if ((i + 1) % 16 == 0) {
        //             Uart0_Printf("\r\n");
        //         }
        //     }
        //     Uart0_Printf("\n");
        // }
        icm42688RealData_t accRealData;
        icm42688RealData_t GyroRealData;
        ICM42688_FIFO_Get_RealData(&accRealData, &GyroRealData);
        // Uart0_Printf("ICM42688 Accelerometer Real Data: %.2f mg, %.2f mg, %.2f mg\r\n", accRealData.x, accRealData.y, accRealData.z);
        // Uart0_Printf("ICM42688 Gyroscope Real Data: %.2f dps, %.2f dps, %.2f dps\r\n", GyroRealData.x, GyroRealData.y, GyroRealData.z);
        
    }
}
