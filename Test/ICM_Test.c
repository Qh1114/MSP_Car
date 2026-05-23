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
    {
        float ypr[3];
        //IMU_Callback();
        IMU_getYawPitchRoll(ypr);
        Uart0_Printf("IMU Yaw: %.2f, Pitch: %.2f, Roll: %.2f\r\n", ypr[0], ypr[1], ypr[2]);
        Delay_ms(50);
    }
}
