#ifndef __IMU_H
#define __IMU_H

#include "ti_msp_dl_config.h"
#include <math.h>
#undef M_PI
#define M_PI  (float)3.1415926535
typedef struct
{
    float x;
    float y;
    float z;
} xyz_f_t;
extern xyz_f_t north,west;
extern volatile float yaw[5];   // 航向滤波历史值（度）
extern float motion6[7];

// Mini IMU AHRS 接口
void IMU_init(void);
void IMU_getYawPitchRoll(float * ypr);
void IMU_TT_getgyro(float * zsjganda);
void IMU_Callback(void);

#endif

