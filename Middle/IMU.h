#ifndef __IMU_H__
#define __IMU_H__

void IMU_init(void);
void IMU_GetOrientation(float *yaw, float *pitch, float *roll);
float IMU_GetYaw(void);
void IMU_Callback(void);

#endif
