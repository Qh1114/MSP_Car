#ifndef MOTOR_H
#define MOTOR_H

/*
 * 310 直流减速电机编码器（测距 + 测速）
 *
 * 硬件连接（已在 SysConfig 中配置）：
 * - 编码器 A 相：PA8（GPIOA.8）  -> 双沿中断（上升沿 + 下降沿）
 * - 编码器 B 相：PA9（GPIOA.9）  -> 仅用于判定方向（无中断）
 * - TIMG0：10ms 周期中断（LOAD_EVENT）用于计算速度
 *
 * 机械参数（按你的描述写死在这里，若更换电机/轮子请改宏）：
 * - 减速比：1:20（电机轴转 20 圈，输出轴/轮子转 1 圈）
 * - 编码器线数：PPR = 13（每圈 13 线）
 * - 计数方式：仅对 A 相计数，且同时检测上升沿/下降沿 => x2 计数
 * - 橡胶轮胎直径：48mm
 */

#include "ti_msp_dl_config.h"

#include <stdint.h>
#include <stdbool.h>


/* ======================== 可配置参数区 ======================== */

/* 方向反相开关：
 * - 0：默认方向（A^B == 1 计数 +1）
 * - 1：反向（A^B == 1 计数 -1）
 *
 * 若你发现正反方向与实际行驶方向相反，把这个宏改为 1 即可。
 */
#define MOTOR_ENCODER_DIR_INVERT (0u)
/* 数学常量（避免依赖非标准的 M_PI） */
#define MOTOR_PI_F (3.1415926f)
/* 机械/编码器参数 */
#define MOTOR_ENCODER_GEAR_RATIO (20u)
#define MOTOR_ENCODER_PPR (13u)
#define MOTOR_ENCODER_A_EDGE_FACTOR (2u) /* A 相双沿 => x2 */

/* 轮子参数 */
#define MOTOR_WHEEL_DIAMETER_MM (48.0f)

/* TIMG0 周期（你的 SysConfig 配置为 10ms） */
#define MOTOR_SPEED_SAMPLE_PERIOD_S (0.01f)

/* 控制器输出范围：与驱动接口的最大输入相匹配（单位：任意，默认与 TB6612 的 MAX_SPEED 对应）
 * - 正值表示前进，负值表示后退
 */
#define MOTOR_CONTROL_OUTPUT_MAX (100.0f)
#define MOTOR_CONTROL_OUTPUT_MIN (-100.0f)

/* PID 默认参数（仅作起始点，实际需调参） */
#define MOTOR_PID_DEFAULT_KP (0.20f)
#define MOTOR_PID_DEFAULT_KI (0.25f)
#define MOTOR_PID_DEFAULT_KD (0.00f)

/* 积分器限幅，单位与输出相同 */
#define MOTOR_PID_INTEGRATOR_LIMIT (50.0f)

/* 控制器使能默认值（0=关闭，1=默认打开） */
#define MOTOR_CONTROL_DEFAULT_ENABLED (1u)

/* PWM / 驱动缩放因子：将 PID 输出映射到驱动输入范围，
 * 若驱动接口已经使用与 PID 相同的幅值，则设为 1.0
 */
#define MOTOR_DRIVE_SCALE (1.0f)

/* 调试开关：若置 1，编译时会保留更多调试输出（需在代码中使用） */
#define MOTOR_DEBUG_ENABLE (0u)

/* ======================================================================= */

/* ======================== 派生常量（一般不用改） ======================== */

/* 电机轴每圈的“计数”数量：PPR * A 双沿(x2) */
#define MOTOR_ENCODER_COUNTS_PER_MOTOR_REV ((MOTOR_ENCODER_PPR) * (MOTOR_ENCODER_A_EDGE_FACTOR))

/* 轮子/输出轴每圈计数：电机轴计数 * 减速比 */
#define MOTOR_ENCODER_COUNTS_PER_WHEEL_REV ((MOTOR_ENCODER_COUNTS_PER_MOTOR_REV) * (MOTOR_ENCODER_GEAR_RATIO))

/* 轮子周长（mm） */
#define MOTOR_WHEEL_CIRCUMFERENCE_MM ((MOTOR_PI_F) * (MOTOR_WHEEL_DIAMETER_MM))

/* 每个计数对应的位移（mm/count） */
#define MOTOR_ENCODER_MM_PER_COUNT ((MOTOR_WHEEL_CIRCUMFERENCE_MM) / (float)(MOTOR_ENCODER_COUNTS_PER_WHEEL_REV))

    /* ================================================================ */

    /* 初始化编码器测距/测速模块：
     * - 使能 GPIOA（PA8）中断
     * - 使能 TIMG0（10ms）中断
     *
     * 注意：请在调用过 SYSCFG_DL_init() 之后调用本函数。
     */
    void Motor_Encoder_Init(void);

    /* 清零编码器累计计数与速度输出 */
    void Motor_Encoder_Reset(void);

    /* 获取编码器累计计数（A 相双沿计数；正负表示方向） */
    int64_t Motor_Encoder_GetCount(void);

    /* 获取累计位移：单位 mm（正负表示方向） */
    float Motor_Encoder_GetDistanceMm(void);

    /* 获取线速度：单位 mm/s（每 10ms 更新一次） */
    float Motor_Encoder_GetSpeedMmps(void);

    /* 获取轮子转速：单位 RPM（每 10ms 更新一次） */
    float Motor_Encoder_GetWheelRPM(void);

    /* 获取电机轴转速：单位 RPM（假定编码器安装在电机轴上） */
    float Motor_Encoder_GetMotorRPM(void);
    /* ===== PID 闭环速度控制接口（实现于 motor_.c） ===== */
    /* 设置目标速度（mm/s）。正值前进，负值后退 */
    void Motor_Control_SetTargetSpeedMmps(float target);

    /* 设置 PID 参数 */
    void Motor_Control_SetPID(float kp, float ki, float kd);

    /* 使能/关闭速度闭环控制（关闭后不输出驱动命令） */
    void Motor_Control_SetEnabled(bool enabled);


#endif
