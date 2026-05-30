#include "Grayscale.h"
#include <stdint.h>
#include "Delay.h"

static void delay_us(uint32_t us);
static void select_channel(uint8_t channel);
static uint16_t Read_OUT_value(void);

//
//@简介: 获取所有巡线传感器的值，存储在传入的数组中
//@参数 - 传感器值的数组，长度应至少为GRAYSCALE_SENSOR_CHANNELS - 8
//@返回: 无
void Grayscale_Sensor_Read_All(uint16_t* sensor_values)
{
    uint8_t i;
    for (i = 0; i < GRAYSCALE_SENSOR_CHANNELS; i++)
    {
        select_channel(i);
        delay_us(20);
        sensor_values[i] = Read_OUT_value();
    }
}

//
//@简介: 获取单个巡线传感器的值，基于通道号选择对应的传感器并读取其值
//@参数 - channel: 传感器通道号，范围为0到GRAYSCALE_SENSOR_CHANNELS-1
//@返回: 传感器的数值，1表示检测到黑线，0表示检测到白线
uint16_t Grayscale_Sensor_Read_Single(uint8_t channel)
{
    if (channel >= GRAYSCALE_SENSOR_CHANNELS)
    {
        return 0; // 无效通道
    }
    select_channel(channel);
    delay_us(50);
    return Read_OUT_value();
}

// 
//@简介: 延迟函数，单位为微秒
static void delay_us(uint32_t us)
{
    Delay_us(us);
}
static void select_channel(uint8_t channel)
{
    SENSOR_AD0_WRITE((channel >> 0) & 0x01);  // bit0 -> AD0
    SENSOR_AD1_WRITE((channel >> 1) & 0x01);  // bit1 -> AD1
    SENSOR_AD2_WRITE((channel >> 2) & 0x01);  // bit2 -> AD2
}

static uint16_t Read_OUT_value(void)
{
    return SENSOR_OUT_READ();
}

