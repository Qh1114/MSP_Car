#include "Motor.h"
#include "ti_msp_dl_config.h"
#include "PID.h"
#include "Encoder.h"
#include "tb6612.h"
#include "Delay.h"
#include "Bat.h"

PIDController pid_left;
PIDController pid_right;
volatile bool cmd_right_speed = false, cmd_left_speed = false;
volatile float Goal_Speed_Left = 0.0f, Goal_Speed_Right = 0.0f;

void Motor_Init(void)
{
    TB6612_Init();
    Encoder_Init();
    Bat_Init();
    PID_Init(&pid_left, 0.01f, 0.0f, 0.0f);
    PID_Init(&pid_right, 0.01f, 0.0f, 0.0f);
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
        // if(fabsf(speed) < 100.0f) {
        //     PID_SetK(&pid_left, 0.02f, 0.2f, 0.0); 
        // }else if(fabsf(speed) < 250.0f) {
        //     PID_SetK(&pid_left, 0.025f, 0.15f, 0.0);
        // }else if(fabsf(speed) < 350.0f) {
        //     PID_SetK(&pid_left, 0.025f, 0.18f, 0.0);
        // }else if(fabsf(speed) < 450.0f) {
        //     PID_SetK(&pid_left, 0.025f, 0.23f, 0.0);
        // }else if(fabsf(speed) < 600.0f) {
        //     PID_SetK(&pid_left, 0.025f, 0.3f, 0.0);
        // }else if(fabsf(speed) < 800.0f) {
        //     PID_SetK(&pid_left, 0.025f, 0.4f, 0.0);
        // }else {
        //     PID_SetK(&pid_left, 0.025f, 0.45f, 0.0);
        // }
        if(fabsf(speed) < 30.0f) {
            PID_SetK(&pid_left, 0.03f, 0.3f, 0.0); // 死区处理
        }else if(fabsf(speed) < 250.0f) {
            PID_SetK(&pid_left, 0.02f, 0.2f, 0.0); // 死区处理
        }else if(fabsf(speed) < 400.0f) {
            PID_SetK(&pid_left, 0.01f, 0.25f, 0.0); // 中速
        }else if(fabsf(speed) < 500.0f) {
            PID_SetK(&pid_left, 0.01f, 0.3f, 0.0); // 高速
        }else if(fabsf(speed) < 600.0f) {
            PID_SetK(&pid_left, 0.01f, 0.35f, 0.0); // 高速
        }else if(fabsf(speed) < 800.0f) {
            PID_SetK(&pid_left, 0.01f, 0.43f, 0.0); // 高速
        }else {
            PID_SetK(&pid_left, 0.01f, 0.5f, 0.0f); // 超高速
        }
        PID_SetGoal(&pid_left, speed);
        Goal_Speed_Left = speed;
        cmd_left_speed = true;
    } 
    if (motor == MOTOR_RIGHT) {
        // if(fabsf(speed) < 100.0f) {
        //     PID_SetK(&pid_right, 0.02f, 0.2f, 0.0);
        // }else if(fabsf(speed) < 250.0f) {
        //     PID_SetK(&pid_right, 0.025f, 0.15f, 0.0);
        // }else if(fabsf(speed) < 350.0f) {
        //     PID_SetK(&pid_right, 0.025f, 0.18f, 0.0);   
        // }else if(fabsf(speed) < 450.0f) {
        //     PID_SetK(&pid_right, 0.025f, 0.23f, 0.0);
        // }else if(fabsf(speed) < 600.0f) {
        //     PID_SetK(&pid_right, 0.025f, 0.3f, 0.0);    
        // }else if(fabsf(speed) < 800.0f) {
        //     PID_SetK(&pid_right, 0.025f, 0.4f, 0.0);
        // }else {
        //     PID_SetK(&pid_right, 0.025f, 0.45f, 0.0);
        // }
        if(fabsf(speed) < 30.0f) {
            PID_SetK(&pid_right, 0.03f, 0.3f, 0.0); // 死区处理
        }else if(fabsf(speed) < 250.0f) {
            PID_SetK(&pid_right, 0.02f, 0.2f, 0.0); // 死区处理
        }else if(fabsf(speed) < 400.0f) {
            PID_SetK(&pid_right, 0.01f, 0.25f, 0.0); // 中速
        }else if(fabsf(speed) < 500.0f) {
            PID_SetK(&pid_right, 0.01f, 0.3f, 0.0); // 高速
        }else if(fabsf(speed) < 600.0f) {
            PID_SetK(&pid_right, 0.01f, 0.35f, 0.0); // 高速
        }else if(fabsf(speed) < 800.0f) {
            PID_SetK(&pid_right, 0.01f, 0.43f, 0.0); // 高速
        }else {
            PID_SetK(&pid_right, 0.01f, 0.5f, 0.0f); // 超高速
        }
        // if(fabsf(speed) < 30.0f) {
        //     PID_SetK(&pid_right, 0.03f, 0.5f, 0.0); // 死区处理
        // }else if(fabsf(speed) < 200.0f) {
        //     PID_SetK(&pid_right, 0.03f, 0.5f, 0.000001); // 死区处理
        // }else if(fabsf(speed) < 400.0f) {
        //     PID_SetK(&pid_right, 0.04f, 0.6f, 0.000001); // 中速
        // }else if(fabsf(speed) < 500.0f) {
        //     PID_SetK(&pid_right, 0.045f, 0.68f, 0.000001); // 高速
        // }else if(fabsf(speed) < 650.0f) {
        //     PID_SetK(&pid_right, 0.045f, 0.8f, 0.000001); // 高速
        // }else if(fabsf(speed) < 800.0f) {
        //     PID_SetK(&pid_right, 0.05f, 1.0f, 0.000001); // 高速
        // }else {
        //     PID_SetK(&pid_right, 0.05f, 1.2f, 0.000005f); // 超高速
        // }
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
        Output_l = PID_Compute(&pid_left, (Goal_Speed_Left - speed_l) )/ Bat_Voltage *100.0f; // 将PID输出除以电池电压，得到占空比
        TB6612_SetLeftMotor(Output_l);
    }
    if(cmd_right_speed) {
        float speed_r, Bat_Voltage, Output_r;
        Encoder_Speed_Get_RIGHT(&speed_r);
        Bat_Voltage = Bat_Read();
        Output_r = PID_Compute(&pid_right, (Goal_Speed_Right - speed_r) )/ Bat_Voltage *100.0f; // 将PID输出除以电池电压，得到占空比
        TB6612_SetRightMotor(Output_r);
    }
}
