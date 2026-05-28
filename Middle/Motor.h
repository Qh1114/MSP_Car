#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "PID.h"
typedef enum {
    MOTOR_LEFT,
    MOTOR_RIGHT
} MotorType;

void Motor_Init(void);
void Motor_SetSpeed(MotorType motor, float speed);
void Motor_SetK(float kp ,float ki, float kd);
void Motor_Start(void);
void Motor_Stop(void);
void Motor_Callback(void);

void Motor_Straight(float speed);
void Motor_Turn(float speed, float turn_rate);

#endif