#ifndef __GRAYSCALE_H
#define __GRAYSCALE_H

#include "ti_msp_dl_config.h"

// 通道选择引脚定义 (AD0, AD1, AD2)  
#define SENSOR_AD0_PORT         GRAYSCALE_AD0_PORT
#define SENSOR_AD0_PIN          GRAYSCALE_AD0_PIN

#define SENSOR_AD1_PORT         GRAYSCALE_AD1_PORT
#define SENSOR_AD1_PIN          GRAYSCALE_AD1_PIN

#define SENSOR_AD2_PORT         GRAYSCALE_AD2_PORT
#define SENSOR_AD2_PIN          GRAYSCALE_AD2_PIN

#define GrayS_OUT_PORT          GRAYSCALE_OUT_PORT
#define GrayS_OUT_PIN           GRAYSCALE_OUT_PIN

// 传感器通道选择宏定义
#define SENSOR_AD0_WRITE(state)  (state) ? DL_GPIO_setPins(SENSOR_AD0_PORT, SENSOR_AD0_PIN) : DL_GPIO_clearPins(SENSOR_AD0_PORT, SENSOR_AD0_PIN)
#define SENSOR_AD1_WRITE(state)  (state) ? DL_GPIO_setPins(SENSOR_AD1_PORT, SENSOR_AD1_PIN) : DL_GPIO_clearPins(SENSOR_AD1_PORT, SENSOR_AD1_PIN)
#define SENSOR_AD2_WRITE(state)  (state) ? DL_GPIO_setPins(SENSOR_AD2_PORT, SENSOR_AD2_PIN) : DL_GPIO_clearPins(SENSOR_AD2_PORT, SENSOR_AD2_PIN)

// 传感器输出读取宏定义
#define SENSOR_OUT_READ()        (DL_GPIO_readPins(GrayS_OUT_PORT, GrayS_OUT_PIN) !=0 )

// 灰度传感器通道数量
#define GRAYSCALE_SENSOR_CHANNELS   8

void Grayscale_Sensor_Read_All(uint16_t* sensor_values);
uint16_t Grayscale_Sensor_Read_Single(uint8_t channel);



#endif 
