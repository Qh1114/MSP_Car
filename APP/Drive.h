#ifndef __DRIVE_H
#define __DRIVE_H

void Drive_Init(void);
void Drive_Straight(float speed_in, float angle);
void Drive_Turn(float angle);
void Drive_Stop(void);
void Drive_Straight_SetK(float kp, float ki, float kd);
void Drive_Turn_SetK(float kp, float ki, float kd);

void Drive_Callback(void);

#endif // __DRIVE_H