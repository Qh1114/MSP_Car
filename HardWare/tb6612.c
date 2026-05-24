/**
 * @file tb6612.c
 * @brief TB6612FNG 双 H 桥电机驱动器（针对 MSPM0G3507 天猛星平台）
 *
 * - 保留原有 API（函数名与签名不变）
 * - PWMA -> PA16 (PWM_1 C1)
 * - PWMB -> PA17 (PWM_1 C0)
 * - 使用 MSP DriverLib 的 DL_GPIO / DL_TimerA 接口
 */

#include "tb6612.h"
#include "stdlib.h"
#include "Bat.h"
#include "math.h"  // 新增：用于fabsf（浮点数绝对值）

// =============== 配置区 ================
// 方向引脚（保持与工程中已有的 GPIO 分组一致）
#define LEFT_AIN1_PIN   DL_GPIO_PIN_13
#define LEFT_AIN1_PORT  GPIOB
#define LEFT_AIN2_PIN   DL_GPIO_PIN_12
#define LEFT_AIN2_PORT  GPIOB

#define RIGHT_BIN1_PIN  DL_GPIO_PIN_12
#define RIGHT_BIN1_PORT GPIOA
#define RIGHT_BIN2_PIN  DL_GPIO_PIN_23
#define RIGHT_BIN2_PORT GPIOB

#define STBY_PIN        DL_GPIO_PIN_13
#define STBY_PORT       GPIOA
// PWM 映射：PWMA = PB3 , PWMB = PB26 
#define PWM_INST        PWM_Motor_INST
#define PWMA_CC_IDX     GPIO_PWM_Motor_C1_IDX
#define PWMB_CC_IDX     GPIO_PWM_Motor_C0_IDX
#define PWM_PERIOD      (1000U)
// =======================================

// 内部：设置单个电机（in1_port/in2_port 使用工程中定义的 GPIO 端口宏）
static void __SetMotor(GPIO_Regs *in1_port, uint32_t in1_pin,
                       GPIO_Regs *in2_port, uint32_t in2_pin,
                       uint32_t cc_idx,
                       float speed)
{
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    if (speed < -MAX_SPEED) speed = -MAX_SPEED;
    
    uint32_t pwm_val = (uint32_t)(fabsf(speed) * (float)PWM_PERIOD / 100.0f);//计算输出PWM值
    if (pwm_val > PWM_PERIOD) pwm_val = PWM_PERIOD;

    if (speed > 0.0f) {
        // 正转
        DL_GPIO_setPins(in1_port, in1_pin);
        DL_GPIO_clearPins(in2_port, in2_pin);
    } else if (speed < 0.0f) {
        // 反转
        DL_GPIO_clearPins(in1_port, in1_pin);
        DL_GPIO_setPins(in2_port, in2_pin);
    } else {
        // 停止（滑行）
        DL_GPIO_clearPins(in1_port, in1_pin);
        DL_GPIO_clearPins(in2_port, in2_pin);
        DL_GPIO_clearPins(STBY_PORT, STBY_PIN);
    }

    DL_TimerA_setCaptureCompareValue(PWM_INST, pwm_val, cc_idx);
}

// ======== 公共 API ========

void TB6612_Init(void)
{
    // 确保 PWM 初始为 0 并设置方向引脚为滑行
    DL_TimerA_setCaptureCompareValue(PWM_INST, 0, PWMA_CC_IDX);
    DL_TimerA_setCaptureCompareValue(PWM_INST, 0, PWMB_CC_IDX);
    TB6612_Coast();
}

void TB6612_Forward(float speed)
{
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    if (speed < 0.0f) speed = 0.0f;
    TB6612_SetLeftMotor(speed);
    TB6612_SetRightMotor(speed);
}

void TB6612_Backward(float speed)
{
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    if (speed < 0.0f) speed = 0.0f;
    TB6612_SetLeftMotor(-speed);
    TB6612_SetRightMotor(-speed);
}

void TB6612_Brake(void)
{
    // 刹车：AIN1=1, AIN2=1
    DL_GPIO_setPins(LEFT_AIN1_PORT, LEFT_AIN1_PIN);
    DL_GPIO_setPins(LEFT_AIN2_PORT, LEFT_AIN2_PIN);
    DL_GPIO_setPins(RIGHT_BIN1_PORT, RIGHT_BIN1_PIN);
    DL_GPIO_setPins(RIGHT_BIN2_PORT, RIGHT_BIN2_PIN);
    DL_GPIO_clearPins(STBY_PORT, STBY_PIN);

    DL_TimerA_setCaptureCompareValue(PWM_INST, 0, PWMA_CC_IDX);
    DL_TimerA_setCaptureCompareValue(PWM_INST, 0, PWMB_CC_IDX);
}

void TB6612_Coast(void)
{
    // 滑行：AIN1=0, AIN2=0
    DL_GPIO_clearPins(LEFT_AIN1_PORT, LEFT_AIN1_PIN);
    DL_GPIO_clearPins(LEFT_AIN2_PORT, LEFT_AIN2_PIN);
    DL_GPIO_clearPins(RIGHT_BIN1_PORT, RIGHT_BIN1_PIN);
    DL_GPIO_clearPins(RIGHT_BIN2_PORT, RIGHT_BIN2_PIN);
    DL_GPIO_clearPins(STBY_PORT, STBY_PIN);

    DL_TimerA_setCaptureCompareValue(PWM_INST, 0, PWMA_CC_IDX);
    DL_TimerA_setCaptureCompareValue(PWM_INST, 0, PWMB_CC_IDX);
}

void TB6612_SetLeftMotor(float speed)
{
    __SetMotor(LEFT_AIN1_PORT, LEFT_AIN1_PIN,
               LEFT_AIN2_PORT, LEFT_AIN2_PIN,
               PWMA_CC_IDX,
               speed);
}

void TB6612_SetRightMotor(float speed)
{
    __SetMotor(RIGHT_BIN1_PORT, RIGHT_BIN1_PIN,
               RIGHT_BIN2_PORT, RIGHT_BIN2_PIN,
               PWMB_CC_IDX,
               speed);
}


