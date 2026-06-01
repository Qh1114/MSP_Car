#include "Drive.h"
#include "Motor.h"
#include "IMU.h"
#include "PID.h"
#include "tb6612.h"

bool cmd_straight_drive = false,cmd_angle_drive = false;;
PIDController pid_straight_drive,pid_angle_drive;
float Goal_Straight_Angle = 0.0f, Goal_Turn_Angle = 0.0f;
float speed = 0.0f;
uint8_t angle_tick = 0, straight_tick = 0;
void Drive_Init(void)
{
    Motor_Init();
    IMU_init();
    PID_Init(&pid_straight_drive, 4.0f, 0.0f, 0.0f);
    PID_SetLimit(&pid_straight_drive, 1000.0f, -1000.0f, 200.0f, -200.0f);
    PID_Init(&pid_angle_drive, 15.0f, 0.0f, 0.13f);
    PID_SetLimit(&pid_angle_drive, 1000.0f, -1000.0f, 200.0f, -200.0f);
    Goal_Straight_Angle = 0.0f;
    Goal_Turn_Angle = 0.0f;
    cmd_straight_drive = false;
    cmd_angle_drive = false;
    angle_tick = 0;
    straight_tick = 0;
}

void Drive_Straight(float speed_in, float angle)
{
    PID_SetGoal(&pid_straight_drive, angle);
    Goal_Straight_Angle = angle;
    speed = speed_in;
    cmd_straight_drive = true;
    cmd_angle_drive = false;
}

void Drive_Straight_SetK(float kp, float ki, float kd)
{
    PID_SetK(&pid_straight_drive, kp, ki, kd);
}

void Drive_Turn(float angle)
{
    PID_SetGoal(&pid_angle_drive, angle);
    Goal_Turn_Angle = angle;
    cmd_angle_drive = true;
    cmd_straight_drive = false;
}

void Drive_Turn_SetK(float kp, float ki, float kd)
{
    PID_SetK(&pid_angle_drive, kp, ki, kd);
}

void Drive_Stop(void)
{
    cmd_straight_drive = false;
    cmd_angle_drive = false;
    TB6612_Brake();
    Motor_SetSpeed(MOTOR_LEFT, 0.0f);
    Motor_SetSpeed(MOTOR_RIGHT, 0.0f);
}


static float CalculateAngleError(float target_angle, float current_angle)
{
    float error = target_angle - current_angle;
    if (error > 180.0f) {
        error -= 360.0f;
    } else if (error < -180.0f) {
        error += 360.0f;
    }
    return error;
}

void Drive_Callback(void)
{
    if(cmd_angle_drive)
    {
        angle_tick++;
        if(angle_tick >= 3) {
            float current_angle = IMU_GetYaw();
            float angle_error = CalculateAngleError(Goal_Turn_Angle, current_angle);
            // if(fabsf(angle_error) < 7.0f) {
            //     PID_SetK(&pid_angle_drive, 8.0f, 0.0f, 0.4f);
            // } else if(fabsf(angle_error) < 15.0f) {
            //     PID_SetK(&pid_angle_drive, 12.0f, 0.0f, 0.7f);
            // } else {
            //     PID_SetK(&pid_angle_drive, 16.0f, 0.0f, 1.12f);
            // }
            float correction = PID_Compute(&pid_angle_drive, angle_error);
            Motor_SetSpeed(MOTOR_LEFT, -correction);
            Motor_SetSpeed(MOTOR_RIGHT, correction);
            angle_tick = 0;
        }
    }else if(cmd_straight_drive)
    {
        straight_tick++;
        if(straight_tick >= 3) {
            float current_angle = IMU_GetYaw();
            float angle_error = CalculateAngleError(Goal_Straight_Angle, current_angle);
            float correction = PID_Compute(&pid_straight_drive, angle_error);
            Motor_SetSpeed(MOTOR_LEFT, speed - correction);
            Motor_SetSpeed(MOTOR_RIGHT, speed + correction);
            straight_tick = 0;
        }
    }
}
