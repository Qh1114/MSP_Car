#ifndef __PID_H__
#define __PID_H__
#include "ti_msp_dl_config.h"

typedef struct {
    float Goal;
    float kp;
    float ki;
    float kd;
    float integral;
    float last_error;
    float Max_out;
    float Min_out;
    float Max_integral;
    float Min_integral;
    uint64_t last_compute_time;
    bool  is_first_compute;
} PIDController;

void PID_Init(PIDController* pid, float kp, float ki, float kd);
void PID_SetLimit(PIDController* pid, float Max_out, float Min_out, float Max_integral, float Min_integral);
void PID_SetGoal(PIDController* pid, float goal);
float PID_Compute(PIDController* pid, float error);
void PID_Reset(PIDController* pid);
void PID_SetK(PIDController* pid, float kp, float ki, float kd);

#endif
