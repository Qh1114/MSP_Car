#ifndef __SERVO_H_
#define __SERVO_H_


/*
舵机控制
- 使用 TIMA0 的 3 个通道（CCP0/1/3）输出 PWM 信号，频率 50Hz（20ms 周期）
- CCR 寄存器值范围：0~20000
引脚——通道0：GPIOA.0（IOMUX_PINCM1），通道1：GPIOA.1（IOMUX_PINCM2），通道3：GPIOA.28（IOMUX_PINCM3）
 */
#define SERVO_INST PWM_SERVO_INST
#define SERVO1_CCR_IDX GPIO_PWM_SERVO_C0_IDX
#define SERVO2_CCR_IDX GPIO_PWM_SERVO_C1_IDX
#define SERVO3_CCR_IDX GPIO_PWM_SERVO_C3_IDX

#define SERVO_MAX_CCR_VALUE 655U
#define MAX_DUTY 12.5f
#define MIN_DUTY 2.5f

typedef enum
{
    SERVO_ID_1 = 1,
    SERVO_ID_2 = 2,
    SERVO_ID_3 = 3
} ServoId;

void Servo_SetPosition(ServoId servo_id, float duty);


#endif