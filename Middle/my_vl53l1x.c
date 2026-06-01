#include "my_vl53l1x.h"
#include "ti_msp_dl_config.h"
#include "vl53l1.h"
#include "Delay.h"

#define VL53L1X_I2C_ADDR 0x54
#define VL53L1X_NUM 3
#define VL53L1X_SHUT1_PORT LASER_XSHUT1_PORT
#define VL53L1X_SHUT1_PIN LASER_XSHUT1_PIN
#define VL53L1X_SHUT2_PORT LASER_XSHUT2_PORT
#define VL53L1X_SHUT2_PIN LASER_XSHUT2_PIN
#define VL53L1X_SHUT3_PORT LASER_XSHUT3_PORT
#define VL53L1X_SHUT3_PIN LASER_XSHUT3_PIN
#define DISTANCE_FILTER_A 0.85f  // 距离低通滤波系数

VL53L1_Dev_t VL53_[VL53L1X_NUM];
int32_t distance_[VL53L1X_NUM];
static float filtered_distance_[VL53L1X_NUM];

void my_vl53l1x_init(void)
{
    DL_GPIO_setPins(VL53L1X_SHUT1_PORT, VL53L1X_SHUT1_PIN);
    DL_GPIO_clearPins(VL53L1X_SHUT2_PORT, VL53L1X_SHUT2_PIN);
    DL_GPIO_clearPins(VL53L1X_SHUT3_PORT, VL53L1X_SHUT3_PIN);
    Delay_ms(5);
    VL53L1Init(&VL53_[0], VL53L1X_I2C_ADDR+2);
    VL53InitParam(&VL53_[0],2);

    //DL_GPIO_clearPins(VL53L1X_SHUT1_PORT, VL53L1X_SHUT1_PIN);
    DL_GPIO_setPins(VL53L1X_SHUT2_PORT, VL53L1X_SHUT2_PIN);
    DL_GPIO_clearPins(VL53L1X_SHUT3_PORT, VL53L1X_SHUT3_PIN);
    Delay_ms(5);
    VL53L1Init(&VL53_[1], VL53L1X_I2C_ADDR+4);
    VL53InitParam(&VL53_[1],2);

    //DL_GPIO_clearPins(VL53L1X_SHUT1_PORT, VL53L1X_SHUT1_PIN);
   // DL_GPIO_clearPins(VL53L1X_SHUT2_PORT, VL53L1X_SHUT2_PIN);
    DL_GPIO_setPins(VL53L1X_SHUT3_PORT, VL53L1X_SHUT3_PIN);
    Delay_ms(5);
    VL53L1Init(&VL53_[2], VL53L1X_I2C_ADDR+6);
    VL53InitParam(&VL53_[2],2);
}

void my_vl53l1x_get_distance(int32_t *distance_out, int VL53L1X_ID)
{
    if (VL53L1X_ID < 1 || VL53L1X_ID > VL53L1X_NUM)
    {
        *distance_out = -1; // Invalid ID
        return;
    }
    *distance_out = (int32_t)filtered_distance_[--VL53L1X_ID];
}

void my_vl53l1x_callback(int VL53L1X_ID)
{
    int idx = --VL53L1X_ID;
    getDistance_Simply(&VL53_[idx], &distance_[idx]);
    filtered_distance_[idx] = filtered_distance_[idx] * DISTANCE_FILTER_A
                            + (float)distance_[idx] * (1.0f - DISTANCE_FILTER_A);
}
