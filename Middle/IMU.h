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
extern volatile float yaw[5];   //处理航向的增值
extern float motion6[7];
//Mini IMU AHRS 解算的API
void IMU_init(void); //初始化
void IMU_getYawPitchRoll(float * ypr); //更新姿态
void IMU_TT_getgyro(float * zsjganda);
float angle_diff(float current, float target);
void IMU_Callback(void);

#endif

