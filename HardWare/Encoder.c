#include "Encoder.h"

#define  PULSE_PER_REVOLUTION 1060.0f  // 每转一圈的脉冲数，根据实际编码器规格调整

volatile int64_t encoder_count_l = 0 , encoder_count_r = 0;
volatile int64_t encoder_count_l_10ms = 0 , encoder_count_r_10ms = 0;
volatile int64_t encoder_count_l_last = 0 , encoder_count_r_last = 0;
static uint64_t last_10ms_update = 0,  duration_time= 0;
void Encoder_Init(void)
{
    encoder_count_l = 0;
    encoder_count_r = 0;
    encoder_count_l_10ms = 0;
    encoder_count_r_10ms = 0;
    encoder_count_l_last = 0;
    encoder_count_r_last = 0;
    // 编码器初始化代码
}

void Encoder_Count_Get(int64_t* count_l, int64_t* count_r)
{
    *count_l = encoder_count_l;
    *count_r = encoder_count_r;
}

// 获取速度，单位为转每秒
void Encoder_Speed_Get(float* speed_l, float* speed_r)
{
    // 计算速度，单位为转每秒
    *speed_l = (float)encoder_count_l_10ms / PULSE_PER_REVOLUTION / duration_time * 1000000.0f; // 10ms -> 100 times per second
    *speed_r = (float)encoder_count_r_10ms / PULSE_PER_REVOLUTION / duration_time * 1000000.0f; // 10ms -> 100 times per second
}

// 获取左右轮速度，单位为度每秒
void Encoder_AngleSpeed_Get(float* angle_l, float* angle_r)
{
    // 计算角速度，单位为度每秒
    *angle_l = (float)encoder_count_l_10ms / PULSE_PER_REVOLUTION * 360.0f / duration_time * 1000000.0f; // 10ms -> 100 times per second
    *angle_r = (float)encoder_count_r_10ms / PULSE_PER_REVOLUTION * 360.0f / duration_time * 1000000.0f; // 10ms -> 100 times per second
}

void Encoder_Speed_Get_LEFT(float* speed_l)
{
    *speed_l = (float)encoder_count_l_10ms / PULSE_PER_REVOLUTION * 360.0f / duration_time * 1000000.0f; // 10ms -> 100 times per second
}

void Encoder_Speed_Get_RIGHT(float* speed_r)
{
    *speed_r = (float)encoder_count_r_10ms / PULSE_PER_REVOLUTION * 360.0f / duration_time * 1000000.0f; // 10ms -> 100 times per second
}

void Encoder_Callback(EncoderType type)
{
    switch (type) {
        case ENA_L:
        {
            int ena_l_1 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb_l_1 = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
             if (ena_l_1 && !enb_l_1) {
                encoder_count_l++;
            } else if(!ena_l_1 && enb_l_1) {
                encoder_count_l ++;
            } else if(!ena_l_1 && !enb_l_1) {
                encoder_count_l --;
            } else if(ena_l_1 && enb_l_1) {
                encoder_count_l --;
            }
            break;
        }
        case ENB_L:
        {
            int ena_l_2 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb_l_2 = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
            if (ena_l_2 && !enb_l_2) {
                encoder_count_l --;
            } else if(!ena_l_2 && enb_l_2) {
                encoder_count_l --;
            } else if(!ena_l_2 && !enb_l_2) {
                encoder_count_l ++;
            } else if(ena_l_2 && enb_l_2) {
                encoder_count_l ++;
            }
            break;
        }
        case ENA_R:
        {
            int ena_r_1 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb_r_1 = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena_r_1 && !enb_r_1) {
                encoder_count_r--;
            } else if(!ena_r_1 && enb_r_1) {
                encoder_count_r --;
            } else if(!ena_r_1 && !enb_r_1) {
                encoder_count_r ++;
            } else if(ena_r_1 && enb_r_1) {
                encoder_count_r ++;
            }
            break;
        }
        case ENB_R:
        {
            int ena_r_2 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb_r_2 = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena_r_2 && !enb_r_2) {
                encoder_count_r ++;
            } else if(!ena_r_2 && enb_r_2) {
                encoder_count_r ++;
            } else if(!ena_r_2 && !enb_r_2) {
                encoder_count_r --;
            } else if(ena_r_2 && enb_r_2) {
                encoder_count_r --;
            }
            break;
        }
    }
}


void Encoder_10ms_Callback(void)
{
    uint64_t current_time = Get_us();
    encoder_count_l_10ms = encoder_count_l - encoder_count_l_last;
    encoder_count_r_10ms = encoder_count_r - encoder_count_r_last;
    encoder_count_l_last = encoder_count_l;
    encoder_count_r_last = encoder_count_r;
    duration_time = current_time - last_10ms_update;
    last_10ms_update = current_time;
}