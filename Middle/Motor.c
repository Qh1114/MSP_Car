#include "Motor.h"
#include "ti_msp_dl_config.h"
#include "PID.h"
#include "Encoder.h"
#include "tb6612.h"
#include "Delay.h"
#include "Bat.h"

// #define MOTOR_MIN_DUTY 5.0f   

PIDController pid_left;
PIDController pid_right;
volatile bool cmd_right_speed = false, cmd_left_speed = false;
volatile float Goal_Speed_Left = 0.0f, Goal_Speed_Right = 0.0f;

void Motor_Init(void)
{
    TB6612_Init();
    Encoder_Init();
    Bat_Init();
    PID_Init(&pid_left, 0.05f, 0.5f, 0.0015f);
    PID_Init(&pid_right, 0.05f, 0.5f, 0.0015f);
    PID_SetLimit(&pid_left, 7.0f, -7.0f, 5.0f, -5.0f);
    PID_SetLimit(&pid_right, 7.0f, -7.0f, 5.0f, -5.0f);
    Goal_Speed_Left = 0.0f;
    Goal_Speed_Right = 0.0f;
    cmd_right_speed = false;
    cmd_left_speed = false;
}

void Motor_SetSpeed(MotorType motor, float speed)
{
    if (motor == MOTOR_LEFT) {
        PID_SetGoal(&pid_left, speed);
        Goal_Speed_Left = speed;
        cmd_left_speed = true;
    } 
    if (motor == MOTOR_RIGHT) {
        PID_SetGoal(&pid_right, speed);
        Goal_Speed_Right = speed;
        cmd_right_speed = true;
    }
}

void Motor_Stop(void)
{
    cmd_left_speed = false;
    cmd_right_speed = false;
    TB6612_SetLeftMotor(0.0f);
    TB6612_SetRightMotor(0.0f);
}

void Motor_Start(void)
{
    cmd_left_speed = true;
    cmd_right_speed = true;
}

void Motor_SetK( float kp ,float ki, float kd)
{
    PID_SetK(&pid_left, kp, ki, kd);
    PID_SetK(&pid_right, kp, ki, kd);
}

void Motor_Callback(void)
{
    if(cmd_left_speed) {
        float speed_l, Bat_Voltage, Output_l;
        Encoder_Speed_Get_LEFT(&speed_l);
        Bat_Voltage = Bat_Read();
        float pid_out = PID_Compute(&pid_left, (Goal_Speed_Left - speed_l));
        Output_l = pid_out / Bat_Voltage * 100.0f;

        // // 死区：低于最小占空比时关断，防止积分胀死
        // if (fabsf(Output_l) < MOTOR_MIN_DUTY) {
        //     Output_l = 0.0f;
        //     //pid_left.integral = 0.0f;
        // }

        TB6612_SetLeftMotor(Output_l);
    }
    if(cmd_right_speed) {
        float speed_r, Bat_Voltage, Output_r;
        Encoder_Speed_Get_RIGHT(&speed_r);
        Bat_Voltage = Bat_Read();
        float pid_out = PID_Compute(&pid_right, (Goal_Speed_Right - speed_r));
        Output_r = pid_out / Bat_Voltage * 100.0f;

        // // 死区：低于最小占空比时关断，防止积分胀死
        // if (fabsf(Output_r) < MOTOR_MIN_DUTY) {
        //     Output_r = 0.0f;
        //     //pid_right.integral = 0.0f;
        // }

        TB6612_SetRightMotor(Output_r);
    }
}
