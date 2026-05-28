#include "Motor.h"
#include "Encoder.h"
#include "Usart.h"
#include "Delay.h"
#include "Motor_Test.h"
#include "Uart_Data.h"
#include "Bat.h"

void Motor_Test1(void)
{
    Uart0_Printf("Motor Test1\n");
    Motor_Init();
    float Buff[10];
    float goal_speed_l = 0.0f, goal_speed_r = 0.0f;
    float speed_l, speed_r;
    float Kp = 0.01f, Ki = 0.0f, Kd = 0.0f;
    float Bat_Voltage;
    int64_t count_l, count_r;
    while(1) {
        Encoder_AngleSpeed_Get(&speed_l, &speed_r);
        Encoder_Count_Get(&count_l, &count_r);
        uint8_t length = Uart0_Receive_Command(Buff);
        Bat_Voltage = Bat_Read();
        if(length == 2) {
            goal_speed_l = Buff[0]*100;
            goal_speed_r = Buff[1]*100;
            Motor_SetSpeed(MOTOR_LEFT, goal_speed_l);
            Motor_SetSpeed(MOTOR_RIGHT, goal_speed_r);
        }else if(length == 1) {
            if(Buff[0] == -1.0f) {
                Motor_Stop();
            }
        }else if(length == 3) {
            Kp = Buff[0]/100;
            Ki = Buff[1];
            Kd = Buff[2]/100;
            Motor_SetK(Kp, Ki, Kd);
            //Motor_SetK(Kp, Ki, Kd);
        } 
        Uart0_Printf("%f,%f,%f,%f,%f,%f,%f,%f,%d,%d\n", goal_speed_l, goal_speed_r, speed_l, speed_r,Bat_Voltage, Kp, Ki, Kd, (int32_t)count_l, (int32_t)count_r);
        Delay_ms(5);
    }
}

float step = 30.0f;
uint8_t cmd = 0; //0：目标速度不变，1：目标速度增加，2：目标速度减少
float upper_limit = 1000.0f;
float lower_limit = 0.0f;
static void Goal_Step_Change(float* goal_speed)
{
    PERIODIC_R(1000)
    if(cmd == 0) {
        return;
    } else if(cmd == 1) {
        *goal_speed += step;
        Motor_SetSpeed(MOTOR_LEFT, *goal_speed);
        Motor_SetSpeed(MOTOR_RIGHT, *goal_speed);
        if(*goal_speed > upper_limit) {
            *goal_speed = upper_limit;
            cmd = 2;
        }
    } else if(cmd == 2) {
        *goal_speed -= step;
        Motor_SetSpeed(MOTOR_LEFT, *goal_speed);
        Motor_SetSpeed(MOTOR_RIGHT, *goal_speed);
        if(*goal_speed < lower_limit) {
            *goal_speed = lower_limit;
            cmd = 1;
        }
    }
}
void Motor_Test2(void)
{
    Uart0_Printf("Motor Test2\n");
    Motor_Init();
    float Buff[10];
    float goal_speed= 0.0f;
    float speed_l, speed_r;
    float Kp = 0.01f, Ki = 0.0f, Kd = 0.0f;
    float Bat_Voltage;
    int64_t count_l, count_r;
    while(1) {
        Goal_Step_Change(&goal_speed);
        Encoder_AngleSpeed_Get(&speed_l, &speed_r);
        Encoder_Count_Get(&count_l, &count_r);
        uint8_t length = Uart0_Receive_Command(Buff);
        Bat_Voltage = Bat_Read();
        if(length == 1) {
            if(Buff[0] == 0.0f) {
                Motor_Stop();
                cmd = 0;
            }else if(Buff[0] == 1.0f) {
                Motor_Start();
                cmd = 1;
            }
        }else if(length == 2)
        {
            upper_limit = Buff[0]*100;
            lower_limit = Buff[1]*100;
        }else if(length == 3) {
            Kp = Buff[0]/100;
            Ki = Buff[1];
            Kd = Buff[2]/10000;
            Motor_SetK(Kp, Ki, Kd);
        } 
        Uart0_Printf("%f,%f,%f,%f,%f,%f,%f,%d,%d,%f,%f\n", goal_speed, speed_l, speed_r,Bat_Voltage, Kp, Ki, Kd, (int32_t)count_l, (int32_t)count_r,upper_limit, lower_limit);
        Delay_ms(5);
    }
}

void Motor_Test3(void)
{
    Uart0_Printf("Motor Test3\n");
    Motor_Init();
    float Buff[10];
    float goal_speed= 0.0f;
    float speed_l, speed_r;
    float Kp = 0.01f, Ki = 0.0f, Kd = 0.0f;
    float Bat_Voltage;
    int64_t count_l, count_r;
    while(1) {
        Goal_Step_Change(&goal_speed);
        Encoder_AngleSpeed_Get(&speed_l, &speed_r);
        Encoder_Count_Get(&count_l, &count_r);
        uint8_t length = Uart0_Receive_Command(Buff);
        Bat_Voltage = Bat_Read();
        if(length == 1) {
            if(Buff[0] == 0.0f) {
                Motor_Stop();
                cmd = 0;
            }else if(Buff[0] == 1.0f) {
                Motor_Start();
                cmd = 1;
            }
        }else if(length == 3) {
            Kp = Buff[0]/100;
            Ki = Buff[1];
            Kd = Buff[2]/100;
            Motor_SetK(Kp, Ki, Kd);
        } 
        Uart0_Printf("%f,%f,%f,%f,%f,%f,%f,%d,%d\n", goal_speed, speed_l, speed_r,Bat_Voltage, Kp, Ki, Kd, (int32_t)count_l, (int32_t)count_r);
        Delay_ms(5);
    }
}