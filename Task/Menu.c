#include "Menu.h"
#include "Key.h"
#include "Usart.h"
#include "spi.h"
#include "OLED.h"
#include "Laser_Distance.h"
#include "Drive.h"
#include "Delay.h"
#include "IMU.h"
#include "Bat.h"
#include "Grayscale.h"
#include <stdio.h>

uint8_t Key = 0;
char str[30];
//自检查菜单
void Menu_Pre(void)
{
    Uart_Init();
    SPI_Init();
    Key_Init();
    OLED_Init();
    Laser_Distance_Init();
    Drive_Init();
    
    NVIC_EnableIRQ(TIMER_10ms_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_10ms_INST);
    
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);

    NVIC_EnableIRQ(TIMER_2ms_INST_INT_IRQN);
	DL_Timer_startCounter(TIMER_2ms_INST);
    while(1) {
        Key = Key_Num_Get();
        if(Key) {
            Menu1();
        }

        PERIODIC_C(50);
        OLED_Clear();
        int32_t distance1 = Laser_Distance_Get(LASER_LEFT);
        int32_t distance2 = Laser_Distance_Get(LASER_FRONT);
        int32_t distance3 = Laser_Distance_Get(LASER_RIGHT);
        sprintf(str, "L:%d", distance1);
        OLED_ShowString(0, 0, str, 16, 1);
        sprintf(str, "F:%d", distance2);
        OLED_ShowString(0, 16, str, 16, 1);
        sprintf(str, "R:%d", distance3);
        OLED_ShowString(0, 32, str, 16, 1);
        float yaw = IMU_GetYaw();
        sprintf(str, "%.2f", yaw);
        OLED_ShowString(0, 48, str, 16, 1);
        float Battery_Voltage = Bat_Read();
        sprintf(str, "Bat:%.2f", Battery_Voltage);
        OLED_ShowString(50, 0, str, 16, 1);
        uint16_t Grayscale_Values[GRAYSCALE_SENSOR_CHANNELS];
        Grayscale_Sensor_Read_All(Grayscale_Values);
        sprintf(str, "Grayscale:");
        OLED_ShowString(50, 16, str, 16, 1);
        sprintf(str, "%d %d %d %d", Grayscale_Values[0], Grayscale_Values[1], Grayscale_Values[2], Grayscale_Values[3]);
        OLED_ShowString(50, 32, str, 16, 1);
        sprintf(str, "%d %d %d %d", Grayscale_Values[4], Grayscale_Values[5], Grayscale_Values[6], Grayscale_Values[7]);
        OLED_ShowString(50, 48, str, 16, 1);
        OLED_Refresh();
    }
}
//主菜单
void Menu1(void)
{
    uint8_t menu_index = 1;
    char Oled_str[5][20] = {"<-", "Task 1", "Task 2", "Task 3", "Task 4"};
    while(1) {
        Key = Key_Num_Get();
        if(Key == 1) {
            menu_index = (menu_index + 1) % 5;
        }else if(Key == 2) {
            menu_index = (menu_index + 4) % 5;
        }else if(Key == 3) {
            switch(menu_index)
            {
                case 0:
                    return;
                case 1:
                    Menu2_Task1();
                    break;
                case 2:
                    Menu2_Task2();
                    break;
                case 3:
                    Menu2_Task3();
                    break;
                case 4:
                    Menu2_Task4();
                    break;
            }
        }

        PERIODIC_C(50);
        OLED_Clear();
        if(menu_index <= 3)
        {
            for(int i = 0; i < 4; i++) {
                if(i == menu_index) {
                    OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 0);
                }else {
                    OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 1);
                }
            }
        }else {
            OLED_ShowString(0, 0, Oled_str[4], 16, 0);
        }
        OLED_Refresh();
    }
}

void Menu2_Task1(void)
{
    bool Task1_cmd = false;
    uint8_t menu_index = 1;
    char Oled_str[2][20] = {"<-", "Off"};
    bool first_entry = true;
    while(1) {
        Key = Key_Num_Get();
        if(Key == 1) {
            menu_index = (menu_index + 1) % 2;
        }else if(Key == 2) {
            menu_index = (menu_index + 1) % 2;
        }else if(Key == 3) {
            if(menu_index == 1) {
                if(Task1_cmd) {
                    Task1_cmd = false;
                    Drive_Stop();
                    sprintf(Oled_str[1], "Off");
                }else {
                    Task1_cmd = true;
                    sprintf(Oled_str[1], "On");
                }
                OLED_Clear();
                for(int i = 0; i < 2; i++) {
                    if(i == menu_index) {
                        OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 0);
                    }else {
                        OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 1);
                    }
                }
                OLED_Refresh();
            }else {
                return;
            }
        }

        //写入任务代码
        if(Task1_cmd) {
            if(first_entry) {
                first_entry = false;
                float initial_yaw = IMU_GetYaw();
                Drive_Straight(300, initial_yaw);
                Delay_ms(2000);
                Drive_Stop();
                Delay_ms(500);
                float Turn_angle = initial_yaw + 90;
                if(Turn_angle >= 180) {
                    Turn_angle -= 360;
                }else if(Turn_angle < -180) {
                    Turn_angle += 360;
                }
                Drive_Turn(Turn_angle);
            }
        }else {
            PERIODIC_C(50);
            OLED_Clear();
            for(int i = 0; i < 2; i++) {
                if(i == menu_index) {
                    OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 0);
                }else {
                    OLED_ShowString(0, (i * 16) % 64, Oled_str[i], 16, 1);
                }
            }
            OLED_Refresh();
        }
    }
}

void Menu2_Task2(void)
{

}

void Menu2_Task3(void)
{

}

void Menu2_Task4(void)
{

}