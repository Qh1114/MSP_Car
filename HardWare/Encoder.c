#include "Encoder.h"
#include "Delay.h"

#define PULSE_PER_REVOLUTION 1060.0f
#define MIN_DELTA_US         50
#define SPEED_FILTER_A       0.95f

volatile int64_t encoder_count_l = 0, encoder_count_r = 0;
volatile int64_t encoder_count_l_10ms = 0, encoder_count_r_10ms = 0;
volatile int64_t encoder_count_l_last = 0, encoder_count_r_last = 0;
static uint64_t last_10ms_update = 0, duration_time = 0;

// 单周期捕获
static volatile uint32_t last_edge_l = 0, last_edge_r = 0;
static volatile float    raw_speed_cap_l = 0.0f, raw_speed_cap_r = 0.0f;
static volatile int      direction_l = 0, direction_r = 0;
static volatile bool     new_edge_l = false, new_edge_r = false;

static float filtered_speed_l = 0.0f, filtered_speed_r = 0.0f;

void Encoder_Init(void)
{
    encoder_count_l = 0; encoder_count_r = 0;
    encoder_count_l_10ms = 0; encoder_count_r_10ms = 0;
    encoder_count_l_last = 0; encoder_count_r_last = 0;
    last_edge_l = 0; last_edge_r = 0;
    raw_speed_cap_l = 0.0f; raw_speed_cap_r = 0.0f;
    filtered_speed_l = 0.0f; filtered_speed_r = 0.0f;
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
        case ENA_L: {
            uint32_t now = (uint32_t)Get_us();
            uint32_t dt  = last_edge_l ? (now - last_edge_l) : 0;
            last_edge_l  = now;

            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
            if (ena && !enb)      { encoder_count_l++; direction_l = 1; }
            else if (!ena && enb) { encoder_count_l++; direction_l = 1; }
            else if (!ena && !enb){ encoder_count_l--; direction_l = -1; }
            else                  { encoder_count_l--; direction_l = -1; }

            if (dt >= MIN_DELTA_US) {
                raw_speed_cap_l = (float)direction_l * 360.0f * 1000000.0f / (float)dt / PULSE_PER_REVOLUTION;
                new_edge_l = true;
            }
            break;
        }
        case ENB_L: {
            uint32_t now = (uint32_t)Get_us();
            uint32_t dt  = last_edge_l ? (now - last_edge_l) : 0;
            last_edge_l  = now;

            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
            if (ena && !enb)      { encoder_count_l--; direction_l = -1; }
            else if (!ena && enb) { encoder_count_l--; direction_l = -1; }
            else if (!ena && !enb){ encoder_count_l++; direction_l = 1; }
            else                  { encoder_count_l++; direction_l = 1; }

            if (dt >= MIN_DELTA_US) {
                raw_speed_cap_l = (float)direction_l * 360.0f * 1000000.0f / (float)dt / PULSE_PER_REVOLUTION;
                new_edge_l = true;
            }
            break;
        }
        case ENA_R: {
            uint32_t now = (uint32_t)Get_us();
            uint32_t dt  = last_edge_r ? (now - last_edge_r) : 0;
            last_edge_r  = now;

            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena && !enb)      { encoder_count_r--; direction_r = -1; }
            else if (!ena && enb) { encoder_count_r--; direction_r = -1; }
            else if (!ena && !enb){ encoder_count_r++; direction_r = 1; }
            else                  { encoder_count_r++; direction_r = 1; }

            if (dt >= MIN_DELTA_US) {
                raw_speed_cap_r = (float)direction_r * 360.0f * 1000000.0f / (float)dt / PULSE_PER_REVOLUTION;
                new_edge_r = true;
            }
            break;
        }
        case ENB_R: {
            uint32_t now = (uint32_t)Get_us();
            uint32_t dt  = last_edge_r ? (now - last_edge_r) : 0;
            last_edge_r  = now;

            int ena = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena && !enb)      { encoder_count_r++; direction_r = 1; }
            else if (!ena && enb) { encoder_count_r++; direction_r = 1; }
            else if (!ena && !enb){ encoder_count_r--; direction_r = -1; }
            else                  { encoder_count_r--; direction_r = -1; }

            if (dt >= MIN_DELTA_US) {
                raw_speed_cap_r = (float)direction_r * 360.0f * 1000000.0f / (float)dt / PULSE_PER_REVOLUTION;
                new_edge_r = true;
            }
            break;
        }
    }
}

void Encoder_10ms_Callback(void)
{
    uint64_t current_time = Get_us();
    uint32_t now = (uint32_t)current_time;

    //--- 左轮 ---//
    float raw_l;
    if (new_edge_l) {
        raw_l = raw_speed_cap_l;         // ISR刚算好的精确值
        new_edge_l = false;
    } else if (last_edge_l) {
        uint32_t dt = now - last_edge_l; // 无边沿：衰减raw自然趋零
        raw_l = (dt >= MIN_DELTA_US)
            ? (float)direction_l * 360.0f * 1000000.0f / (float)dt / PULSE_PER_REVOLUTION
            : raw_speed_cap_l;
    } else {
        raw_l = 0.0f;
    }

    if (raw_l != 0.0f)
        filtered_speed_l = filtered_speed_l * SPEED_FILTER_A + raw_l * (1.0f - SPEED_FILTER_A);

    //--- 右轮 ---//
    float raw_r;
    if (new_edge_r) {
        raw_r = raw_speed_cap_r;
        new_edge_r = false;
    } else if (last_edge_r) {
        uint32_t dt = now - last_edge_r;
        raw_r = (dt >= MIN_DELTA_US)
            ? (float)direction_r * 360.0f * 1000000.0f / (float)dt / PULSE_PER_REVOLUTION
            : raw_speed_cap_r;
    } else {
        raw_r = 0.0f;
    }

    if (raw_r != 0.0f)
        filtered_speed_r = filtered_speed_r * SPEED_FILTER_A + raw_r * (1.0f - SPEED_FILTER_A);

    encoder_count_l_10ms = encoder_count_l - encoder_count_l_last;
    encoder_count_r_10ms = encoder_count_r - encoder_count_r_last;
    encoder_count_l_last = encoder_count_l;
    encoder_count_r_last = encoder_count_r;
    duration_time = current_time - last_10ms_update;
    last_10ms_update = current_time;
}
