#include "Encoder.h"
#include "Delay.h"

#define PULSE_PER_REVOLUTION 1060.0f  // 每转一圈的边沿总数
#define MIN_DELTA_US         50       // 小于50us的脉冲间隔视为噪声毛刺
#define SPEED_FILTER_A       0.93f    // 低通滤波系数 (越大越平滑)

volatile int64_t encoder_count_l = 0, encoder_count_r = 0;
volatile int64_t encoder_count_l_10ms = 0, encoder_count_r_10ms = 0;
volatile int64_t encoder_count_l_last = 0, encoder_count_r_last = 0;
static uint64_t last_10ms_update = 0, duration_time = 0;

// 4边沿移位寄存器
static volatile uint32_t t0_l = 0, t0_r = 0, t1_l = 0, t1_r = 0;
static volatile uint32_t t2_l = 0, t2_r = 0, t3_l = 0, t3_r = 0;
static volatile int direction_l = 0, direction_r = 0;
static float filtered_speed_l = 0.0f, filtered_speed_r = 0.0f;

void Encoder_Init(void)
{
    encoder_count_l = 0;
    encoder_count_r = 0;
    encoder_count_l_10ms = 0;
    encoder_count_r_10ms = 0;
    encoder_count_l_last = 0;
    encoder_count_r_last = 0;
    filtered_speed_l = 0.0f;
    filtered_speed_r = 0.0f;
}

void Encoder_Count_Get(int64_t* count_l, int64_t* count_r)
{
    *count_l = encoder_count_l;
    *count_r = encoder_count_r;
}

void Encoder_Speed_Get(float* speed_l, float* speed_r)
{
    *speed_l = filtered_speed_l;
    *speed_r = filtered_speed_r;
}

void Encoder_AngleSpeed_Get(float* angle_l, float* angle_r)
{
    *angle_l = filtered_speed_l;
    *angle_r = filtered_speed_r;
}

void Encoder_Speed_Get_LEFT(float* speed_l)
{
    *speed_l = filtered_speed_l;
}

void Encoder_Speed_Get_RIGHT(float* speed_r)
{
    *speed_r = filtered_speed_r;
}

void Encoder_Callback(EncoderType type)
{
    switch (type) {
        case ENA_L:
        {
            t0_l = t1_l; t1_l = t2_l; t2_l = t3_l;
            t3_l = (uint32_t)Get_us();
            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
            if (ena && !enb) {
                encoder_count_l++; direction_l = 1;
            } else if (!ena && enb) {
                encoder_count_l++; direction_l = 1;
            } else if (!ena && !enb) {
                encoder_count_l--; direction_l = -1;
            } else {
                encoder_count_l--; direction_l = -1;
            }
            break;
        }
        case ENB_L:
        {
            t0_l = t1_l; t1_l = t2_l; t2_l = t3_l;
            t3_l = (uint32_t)Get_us();
            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
            if (ena && !enb) {
                encoder_count_l--; direction_l = -1;
            } else if (!ena && enb) {
                encoder_count_l--; direction_l = -1;
            } else if (!ena && !enb) {
                encoder_count_l++; direction_l = 1;
            } else {
                encoder_count_l++; direction_l = 1;
            }
            break;
        }
        case ENA_R:
        {
            t0_r = t1_r; t1_r = t2_r; t2_r = t3_r;
            t3_r = (uint32_t)Get_us();
            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena && !enb) {
                encoder_count_r--; direction_r = -1;
            } else if (!ena && enb) {
                encoder_count_r--; direction_r = -1;
            } else if (!ena && !enb) {
                encoder_count_r++; direction_r = 1;
            } else {
                encoder_count_r++; direction_r = 1;
            }
            break;
        }
        case ENB_R:
        {
            t0_r = t1_r; t1_r = t2_r; t2_r = t3_r;
            t3_r = (uint32_t)Get_us();
            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena && !enb) {
                encoder_count_r++; direction_r = 1;
            } else if (!ena && enb) {
                encoder_count_r++; direction_r = 1;
            } else if (!ena && !enb) {
                encoder_count_r--; direction_r = -1;
            } else {
                encoder_count_r--; direction_r = -1;
            }
            break;
        }
    }
}

void Encoder_10ms_Callback(void)
{
    uint64_t current_time = Get_us();
    uint32_t now = (uint32_t)current_time;

    uint32_t t0l = t0_l, t3l = t3_l, t0r = t0_r, t3r = t3_r;
    int dl = direction_l, dr = direction_r;

    // 左轮：3个边沿间隔
    uint32_t dt_l = t3l - t0l;
    if (t0l && dt_l < (now - t0l)) dt_l = now - t0l;
    if (t0l && dt_l >= MIN_DELTA_US) {
        float raw_l = (float)dl * 3.0f * 360.0f * 1000000.0f / (float)dt_l / PULSE_PER_REVOLUTION;
        filtered_speed_l = filtered_speed_l * SPEED_FILTER_A + raw_l * (1.0f - SPEED_FILTER_A);
    }

    // 右轮：3个边沿间隔
    uint32_t dt_r = t3r - t0r;
    if (t0r && dt_r < (now - t0r)) dt_r = now - t0r;
    if (t0r && dt_r >= MIN_DELTA_US) {
        float raw_r = (float)dr * 3.0f * 360.0f * 1000000.0f / (float)dt_r / PULSE_PER_REVOLUTION;
        filtered_speed_r = filtered_speed_r * SPEED_FILTER_A + raw_r * (1.0f - SPEED_FILTER_A);
    }

    encoder_count_l_10ms = encoder_count_l - encoder_count_l_last;
    encoder_count_r_10ms = encoder_count_r - encoder_count_r_last;
    encoder_count_l_last = encoder_count_l;
    encoder_count_r_last = encoder_count_r;
    duration_time = current_time - last_10ms_update;
    last_10ms_update = current_time;
}