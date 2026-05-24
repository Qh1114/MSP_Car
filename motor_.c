#include "motor_.h"

#include <stdbool.h>
#include <stdint.h>
#include "tb6612.h"

/*
 * 实现思路（A 相双沿 + B 相判向）：
 * - GPIOA.8 (A) 触发中断：每来一次边沿，我们读取 A、B 当前电平
 * - 对于标准 AB 相 90° 相位差编码器：在 A 的边沿处，(A XOR B) 可用于判断方向
 *   - forward(正向)：A^B == 1
 *   - reverse(反向)：A^B == 0
 *   若你的接线导致方向相反，把 motor_.h 里的 MOTOR_ENCODER_DIR_INVERT 置 1。
 *
 * - TIMG0 每 10ms 进入一次中断：读取并清零 10ms 内的增量计数 deltaCount
 *   - 线速度(mm/s) = deltaCount * (mm/count) / 0.01
 *   - 轮子 RPM     = deltaCount / (count/rev) / 0.01 * 60
 */

/* 编码器累计计数（正负表示方向） */
static volatile int64_t s_encoderCount = 0;

/* 10ms 窗口内累计计数（用于测速，10ms 中断里会读取并清零） */
static volatile int32_t s_deltaCount10ms = 0;

/* 速度输出：每 10ms 更新一次 */
static volatile float s_speedMmps = 0.0f;
static volatile float s_wheelRpm = 0.0f;

/* ===== PID 闭环控制状态 ===== */
static volatile float s_targetSpeedMmps = 0.0f;
static volatile uint8_t s_pidEnabled = 0u;
static volatile float s_pid_kp = MOTOR_PID_DEFAULT_KP;
static volatile float s_pid_ki = MOTOR_PID_DEFAULT_KI;
static volatile float s_pid_kd = MOTOR_PID_DEFAULT_KD;
static float s_pid_integrator = 0.0f;
static float s_pid_lastError = 0.0f;

/* ========== 内部小工具：临界区（避免 10ms ISR 与 GPIO ISR 抢同一个计数变量） ========== */
static inline uint32_t motor_enter_critical(void)
{
    /* Cortex-M0+：PRIMASK=1 时屏蔽所有可屏蔽中断 */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static inline void motor_exit_critical(uint32_t primask)
{
    __set_PRIMASK(primask);
}

/* 根据 A/B 电平计算本次应 +1 还是 -1 */
static inline int8_t motor_encoder_step_from_ab(bool aLevel, bool bLevel)
{
    bool forward = (aLevel ^ bLevel);
    int8_t step = forward ? (int8_t)1 : (int8_t)-1;

#if (MOTOR_ENCODER_DIR_INVERT != 0u)
    step = (int8_t)-step;
#endif

    return step;
}

/* A 相边沿中断：更新累计计数与 10ms 增量 */
static void motor_encoder_onAEdge(void)
{
    uint32_t abPins = DL_GPIO_readPins(DC_MOTOR_PORT, (DC_MOTOR_A_PIN | DC_MOTOR_B_PIN));
    bool aLevel = ((abPins & DC_MOTOR_A_PIN) != 0u);
    bool bLevel = ((abPins & DC_MOTOR_B_PIN) != 0u);
    int8_t step = motor_encoder_step_from_ab(aLevel, bLevel);

    /* GPIO ISR 很短，直接更新即可 */
    s_encoderCount += (int64_t)step;
    s_deltaCount10ms += (int32_t)step;
}

/* 10ms 定时中断：计算并更新速度（mm/s 与 RPM） */
static void motor_encoder_on10msTick(void)
{
    int32_t delta;

    /* 原子交换：取走 10ms 内的增量并清零，避免与 GPIO ISR 同时读写导致丢计数 */
    uint32_t key = motor_enter_critical();
    delta = s_deltaCount10ms;
    s_deltaCount10ms = 0;
    motor_exit_critical(key);

    /* 线速度(mm/s) */
    s_speedMmps = ((float)delta * MOTOR_ENCODER_MM_PER_COUNT) / MOTOR_SPEED_SAMPLE_PERIOD_S;

    /* 轮子转速(RPM) */
    s_wheelRpm = ((float)delta / (float)MOTOR_ENCODER_COUNTS_PER_WHEEL_REV) *
                 (60.0f / MOTOR_SPEED_SAMPLE_PERIOD_S);

    /* PID 控制：每 10ms 用最新速度计算控制量并输出到 TB6612（如果使能） */
    if (s_pidEnabled)
    {
        float error = s_targetSpeedMmps - s_speedMmps;
        /* 积分 */
        s_pid_integrator += error * MOTOR_SPEED_SAMPLE_PERIOD_S;
        /* 简单积分抗饱和限制 */
        if (s_pid_integrator > MAX_SPEED)
            s_pid_integrator = MAX_SPEED;
        if (s_pid_integrator < -MAX_SPEED)
            s_pid_integrator = -MAX_SPEED;

        float derivative = (error - s_pid_lastError) / MOTOR_SPEED_SAMPLE_PERIOD_S;
        float out = s_pid_kp * error + s_pid_ki * s_pid_integrator + s_pid_kd * derivative;
        s_pid_lastError = error;

        /* 限幅到驱动接口允许的范围（tb6612.h 定义 MAX_SPEED） */
        if (out > MAX_SPEED)
            out = MAX_SPEED;
        if (out < -MAX_SPEED)
            out = -MAX_SPEED;

        /* 将控制量发送到驱动：正数前进，负数倒退 */
        TB6612_SetLeftMotor(out);
    }
}

/* ======================== 对外 API 实现 ======================== */

void Motor_Encoder_Init(void)
{
    /* 清零软件状态 */
    Motor_Encoder_Reset();

    /*
     * 重要：SysConfig 只配置了外设/端口寄存器，不会自动 NVIC_EnableIRQ。
     * 在这里把 GPIOA 与 TIMG0 的 NVIC 中断打开，保证功能“开箱即用”。
     */

    /* GPIOA：编码器 A 相 (PA8) 边沿中断 */
    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_SetPriority(GPIO_MULTIPLE_GPIOA_INT_IRQN, 0u);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);

    /* TIMG0：10ms 周期中断（LOAD_EVENT） */
    /* 有些 SysConfig 版本只做了 init/enableClock，但不一定真的 start counter。
     * 这里显式启动一次，保证 10ms tick 一定会跑起来。
     */
    DL_TimerG_clearInterruptStatus(MOTOR__INST, DL_TIMERG_INTERRUPT_LOAD_EVENT);
    DL_TimerG_enableInterrupt(MOTOR__INST, DL_TIMERG_INTERRUPT_LOAD_EVENT);
    DL_TimerG_enableClock(MOTOR__INST);
    DL_TimerG_startCounter(MOTOR__INST);

    NVIC_ClearPendingIRQ(MOTOR__INST_INT_IRQN);
    NVIC_SetPriority(MOTOR__INST_INT_IRQN, 1u);
    NVIC_EnableIRQ(MOTOR__INST_INT_IRQN);
}

void Motor_Encoder_Reset(void)
{
    uint32_t key = motor_enter_critical();
    s_encoderCount = 0;
    s_deltaCount10ms = 0;
    s_speedMmps = 0.0f;
    s_wheelRpm = 0.0f;
    motor_exit_critical(key);
}

int64_t Motor_Encoder_GetCount(void)
{
    uint32_t key = motor_enter_critical();
    int64_t count = s_encoderCount;
    motor_exit_critical(key);
    return count;
}

float Motor_Encoder_GetDistanceMm(void)
{
    /* 测距：距离(mm) = count * (mm/count) */
    return (float)Motor_Encoder_GetCount() * MOTOR_ENCODER_MM_PER_COUNT;
}

float Motor_Encoder_GetSpeedMmps(void)
{
    return s_speedMmps;
}

float Motor_Encoder_GetWheelRPM(void)
{
    return s_wheelRpm;
}

float Motor_Encoder_GetMotorRPM(void)
{
    /* 电机轴转速 = 输出轴转速 * 减速比（假定编码器在电机轴上） */
    return s_wheelRpm * (float)MOTOR_ENCODER_GEAR_RATIO;
}

/* ===== PID 控制接口实现 ===== */
void Motor_Control_SetTargetSpeedMmps(float target)
{
    s_targetSpeedMmps = target;
}

void Motor_Control_SetPID(float kp, float ki, float kd)
{
    s_pid_kp = kp;
    s_pid_ki = ki;
    s_pid_kd = kd;
}

void Motor_Control_SetEnabled(bool enabled)
{
    s_pidEnabled = enabled ? 1u : 0u;
    if (!enabled)
    {
        /* 关闭控制时复位积分与输出 */
        s_pid_integrator = 0.0f;
        s_pid_lastError = 0.0f;
        TB6612_SetLeftMotor(0.0f);
    }
}

/* ======================== 中断服务函数（向量表会直接调用） ======================== */

void GROUP1_IRQHandler(void)
{
    /* MSPM0G350x：外部 IRQ1 对应 GROUP1。
     * SysConfig 里 GPIOA 的中断属于 Group1，需要在这里分发到 GPIOA。
     */
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
    case GPIO_MULTIPLE_GPIOA_INT_IIDX: /* == DL_INTERRUPT_GROUP1_IIDX_GPIOA */
        switch (DL_GPIO_getPendingInterrupt(DC_MOTOR_PORT))
        {
        case DC_MOTOR_A_IIDX:
            DL_GPIO_clearInterruptStatus(DC_MOTOR_PORT, DC_MOTOR_A_PIN);
            motor_encoder_onAEdge();
            break;

        default:
            break;
        }

        /* 可选：清 GROUP1 中 GPIOA 的组中断标志（保险起见） */
        DL_Interrupt_clearGroup(DL_INTERRUPT_GROUP_1, DL_INTERRUPT_GROUP1_GPIOA);
        break;

    default:
        break;
    }
}

/* 兼容占位：如果有人误以为向量表会调用 GPIOA_IRQHandler，这里也能继续工作。
 * 实际向量表条目是 GROUP1_IRQHandler。
 */
void GPIOA_IRQHandler(void)
{
    GROUP1_IRQHandler();
}

void TIMG0_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(MOTOR__INST))
    {
    case DL_TIMERG_IIDX_LOAD:
        /* 清 LOAD_EVENT 中断标志（必须） */
        DL_TimerG_clearInterruptStatus(MOTOR__INST, DL_TIMERG_INTERRUPT_LOAD_EVENT);
        motor_encoder_on10msTick();
        break;

    default:
        break;
    }
}
