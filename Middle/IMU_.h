#ifndef __IMU__H__
#define __IMU__H__

void IMU_init_(void);
void IMU_GetOrientation(float *yaw, float *pitch, float *roll);
void IMU_Update(void);

#endif
