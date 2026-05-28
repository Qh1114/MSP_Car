#include "PID.h"
#include "Delay.h"

void PID_Init(PIDController* pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->Goal = 0.0f;
    pid->Max_out = 1.0e30f;
    pid->Min_out = -1.0e30f;
    pid->Max_integral = 1.0e30f;
    pid->Min_integral = -1.0e30f;
    pid->is_first_compute = true;
    pid->last_compute_time = 0;
}

void PID_SetLimit(PIDController* pid, float Max_out, float Min_out, float Max_integral, float Min_integral)
{
    pid->Max_out = Max_out;
    pid->Min_out = Min_out;
    pid->Max_integral = Max_integral;
    pid->Min_integral = Min_integral;
}

void PID_SetGoal(PIDController* pid, float goal)
{
    pid->Goal = goal;
}

float PID_Compute(PIDController* pid, float error)
{
    uint64_t current_time = Get_us();
    if (pid->is_first_compute) {
        pid->last_compute_time = current_time;
        pid->last_error = error;
        pid->is_first_compute = false;
        return 0.0f;
    }

    float delta_time = (float)(current_time - pid->last_compute_time) * 1e-6f;
    float delta_error = error - pid->last_error;
    pid->integral += error * delta_time;
    // 积分限幅
    if (pid->integral > pid->Max_integral) {
        pid->integral = pid->Max_integral;
    } else if (pid->integral < pid->Min_integral) {
        pid->integral = pid->Min_integral;
    }

    float derivative = delta_error / delta_time;
    float output = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

    // 输出限幅
    if (output > pid->Max_out) {
        output = pid->Max_out;
    } else if (output < pid->Min_out) {
        output = pid->Min_out;
    }

    pid->last_error = error;
    pid->last_compute_time = current_time;

    return output;
}

void PID_Reset(PIDController* pid)
{
    pid->integral = 0.0f;
    pid->last_error = 0.0f;
    pid->is_first_compute = true;
}

void PID_SetK(PIDController* pid, float kp, float ki, float kd)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}