#include "Drive_Test.h"
#include "Drive.h"
#include "OLED.h"
#include "IMU.h"
#include "Uart_Data.h"
#include "Delay.h"
#include "Usart.h"
#include "Key.h"
#include <stdio.h>
void Drive_Test1(void)
{
    Uart0_Printf("Drive Test1\n");
    Drive_Init();
    Drive_Straight(200.0f, 0.0f);
    while(1) {
        
    }
}

void Drive_Test2(void)
{
    Uart0_Printf("Drive Test2\n");
    Drive_Init();
    Drive_Turn(90.0f);
    while(1) {
        float yaw;
        yaw = IMU_GetYaw();
        Uart0_Printf("Current Yaw: %.2f\n", yaw);
        Delay_ms(100);
    }
}

float Kp = 1.0f, Ki = 0.0f, Kd = 0.0f;
int Goal_Angle = 0;
bool pid_mode = false;
uint8_t menu_index = 0;
float Kp_Step = 0.1f, Ki_Step = 0.01f, Kd_Step = 0.01f;
uint8_t Goal_Angle_Step = 10;
uint8_t set_state = 0;      //0:Striaght;1:Turn

static void Drive_Test_Set(void)
{
    uint8_t key_num = 0;
    char str[20];
    while(1) {
        key_num = Key_Num_Get();
        if(key_num == 1) {
            switch(menu_index)
            {
                case 0:
                    Goal_Angle += Goal_Angle_Step;
                    if(Goal_Angle > 180) {
                        Goal_Angle -= 360;
                    }
                    break;
                case 1:
                    Kp += Kp_Step;
                    break;
                case 2:
                    Ki += Ki_Step;
                    break;
                case 3:
                    Kd += Kd_Step;
                    break;
            }
        }else if(key_num == 2) {
            switch(menu_index)
            {
                case 0:
                    Goal_Angle -= Goal_Angle_Step;
                    if(Goal_Angle < -180) {
                        Goal_Angle += 360;
                    }
                    break;
                case 1:
                    Kp -= Kp_Step;
                    if(Kp < 0.0f) {
                        Kp = 0.0f;
                    }
                    break;
                case 2:
                    Ki -= Ki_Step;
                    if(Ki < 0.0f) {
                        Ki = 0.0f;
                    }
                    break;
                case 3:
                    Kd -= Kd_Step;
                    if(Kd < 0.0f) {
                        Kd = 0.0f;
                    }
                    break;
            }
        }else if(key_num == 3) {
            if(set_state)
            {
                Drive_Turn_SetK(Kp, Ki, Kd);
            }else {
                Drive_Straight_SetK(Kp, Ki, Kd);
            }
            return;
        }

        PERIODIC_C(50)
        float yaw = IMU_GetYaw();
        OLED_Clear();
        sprintf(str, "Goal Angle: %d", Goal_Angle);
        OLED_ShowString(0, 0, str, 16, 1);
        sprintf(str, "Kp: %.2f", Kp);
        OLED_ShowString(0, 16, str, 16, 1);
        sprintf(str, "Ki: %.2f", Ki);
        OLED_ShowString(0, 32, str, 16, 1);
        sprintf(str, "Kd: %.2f", Kd);
        OLED_ShowString(0, 48, str, 16, 1);
        sprintf(str, "%.2f", yaw);
        OLED_ShowString(80, 48, str, 16, 1);
        if(pid_mode) {
            OLED_ShowString(80, 32, "ON", 16, 1);
        }else {
            OLED_ShowString(80, 32, "OFF", 16, 1);
        }
        switch(menu_index)
        {
            case 0:
                sprintf(str, "%d", Goal_Angle);
                OLED_ShowString(96, 0, str, 16, 0);
                break;
            case 1:
                sprintf(str, "%.2f", Kp);
                OLED_ShowString(32, 16, str, 16, 0);
                break;
            case 2:
                sprintf(str, "%.2f", Ki);
                OLED_ShowString(32, 32, str, 16, 0);
                break;
            case 3:
                sprintf(str, "%.2f", Kd);
                OLED_ShowString(32, 48, str, 16, 0);
                break;
        }
        OLED_Refresh();

    }
}
void Drive_Test3(void)
{
    OLED_Init();
    Drive_Init();
    uint8_t Key_Num = 0;
    char str[20];
    while(1) {
        Key_Num = Key_Num_Get();
        if(Key_Num == 1) {
            menu_index = (menu_index + 1) % 4;
        }else if(Key_Num == 2) {
            menu_index = (menu_index + 3) % 4;
        }else if(Key_Num == 3) {
            switch(menu_index)
            {
                case 0:
                    Drive_Test_Set();
                    break;
                case 1:
                    Drive_Test_Set();
                    break;
                case 2:
                    Drive_Test_Set();
                    break;
                case 3:
                    Drive_Test_Set();
                    break;
            }
        }else if(Key_Num == 4) {
            if(!pid_mode) {
                pid_mode = true;
                if(!set_state)
                {
                    Drive_Straight(700.0f, Goal_Angle);
                }else {
                    Drive_Turn(Goal_Angle);
                }
            }else {
                pid_mode = false;
                Drive_Stop();
            }
        }

        PERIODIC_C(50)
        float yaw = IMU_GetYaw();
        OLED_Clear();
        sprintf(str, "%.2f", yaw);
        OLED_ShowString(80, 48, str, 16, 1);
        sprintf(str, "Goal Angle: %d",Goal_Angle);
        OLED_ShowString(0, 0, str, 16, 1);
        sprintf(str, "Kp: %.2f", Kp);
        OLED_ShowString(0, 16, str, 16, 1);
        sprintf(str, "Ki: %.2f", Ki);
        OLED_ShowString(0, 32, str, 16, 1);
        sprintf(str, "Kd: %.2f", Kd);
        OLED_ShowString(0, 48, str, 16, 1);
        if(pid_mode) {
            OLED_ShowString(80, 32, "ON", 16, 1);
        }else {
            OLED_ShowString(80, 32, "OFF", 16, 1);
        }
        switch(menu_index)
        {
            case 0:
                OLED_ShowString(0, 0, "Goal Angle", 16, 0);
                break;
            case 1:
                OLED_ShowString(0, 16, "Kp", 16, 0);
                break;
            case 2:
                OLED_ShowString(0, 32, "Ki", 16, 0);
                break;
            case 3:
                OLED_ShowString(0, 48, "Kd", 16, 0);
                break;
        }
        OLED_Refresh();
        
    }
}