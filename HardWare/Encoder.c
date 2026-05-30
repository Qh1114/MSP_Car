#include "Encoder.h"
#include "Delay.h"
#define  PULSE_PER_REVOLUTION 1060.0f  // 每转一圈的边沿总数
#define  MIN_DELTA_US    50          // 小于50us的脉冲间隔视为噪声毛刺
// 分段滤波：低速轻滤（响应快），中高速逐步加强
#define  FILTER_LOW      0.70f   // <200°/s 响应优先
#define  FILTER_MID      0.93f   // 200~500°/s
#define  FILTER_HIGH     0.96f   // >500°/s 平滑优先

volatile int64_t encoder_count_l = 0 , encoder_count_r = 0;
volatile int64_t encoder_count_l_10ms = 0 , encoder_count_r_10ms = 0;
volatile int64_t encoder_count_l_last = 0 , encoder_count_r_last = 0;
static uint64_t last_10ms_update = 0,  duration_time= 0;
static volatile int direction_l = 0, direction_r = 0;
static volatile uint32_t t0_l = 0, t0_r = 0, t1_l = 0, t1_r = 0, t2_l = 0, t2_r = 0, t3_l = 0, t3_r = 0;
static float filtered_speed_l = 0.0f, filtered_speed_r = 0.0f;
static int  intv_lvl_l = 1, intv_lvl_r = 1;

static float Encoder_Filter_Select(float speed)
{
    float abs_spd = fabsf(speed);
    if (abs_spd < 100.0f)      return FILTER_LOW;
    else if (abs_spd < 500.0f) return FILTER_MID;
    else                       return FILTER_HIGH;
}

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
            int ena_l_1 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb_l_1 = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
             if (ena_l_1 && !enb_l_1) {
                encoder_count_l++;
                direction_l = 1;
            } else if(!ena_l_1 && enb_l_1) {
                encoder_count_l ++;
                direction_l = 1;
            } else if(!ena_l_1 && !enb_l_1) {
                encoder_count_l --;
                direction_l = -1;
            } else if(ena_l_1 && enb_l_1) {
                encoder_count_l --;
                direction_l = -1;
            }
            break;
        }
        case ENB_L:
        {
            t0_l = t1_l; t1_l = t2_l; t2_l = t3_l;
            t3_l = (uint32_t)Get_us();
            int ena_l_2 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_L_PIN);
            int enb_l_2 = DL_GPIO_readPins(GPIOB, ENCODER_ENB_L_PIN);
            if (ena_l_2 && !enb_l_2) {
                encoder_count_l --;
                direction_l = -1;
            } else if(!ena_l_2 && enb_l_2) {
                encoder_count_l --;
                direction_l = -1;
            } else if(!ena_l_2 && !enb_l_2) {
                encoder_count_l ++;
                direction_l = 1;
            } else if(ena_l_2 && enb_l_2) {
                encoder_count_l ++;
                direction_l = 1;
            }
            break;
        }
        case ENA_R:
        {
            t0_r = t1_r; t1_r = t2_r; t2_r = t3_r;
            t3_r = (uint32_t)Get_us();
            int ena_r_1 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb_r_1 = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena_r_1 && !enb_r_1) {
                encoder_count_r--;
                direction_r = -1;
            } else if(!ena_r_1 && enb_r_1) {
                encoder_count_r --;
                direction_r = -1;
            } else if(!ena_r_1 && !enb_r_1) {
                encoder_count_r ++;
                direction_r = 1;
            } else if(ena_r_1 && enb_r_1) {
                encoder_count_r ++;
                direction_r = 1;
            }
            break;
        }
        case ENB_R:
        {
            t0_r = t1_r; t1_r = t2_r; t2_r = t3_r;
            t3_r = (uint32_t)Get_us();
            int ena_r_2 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb_r_2 = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena_r_2 && !enb_r_2) {
                encoder_count_r ++;
                direction_r = 1;
            } else if(!ena_r_2 && enb_r_2) {
                encoder_count_r ++;
                direction_r = 1;
            } else if(!ena_r_2 && !enb_r_2) {
                encoder_count_r --;
                direction_r = -1;
            } else if(ena_r_2 && enb_r_2) {
                encoder_count_r --;
                direction_r = -1;
            }
            break;
        }
    }
}


void Encoder_10ms_Callback(void)
{
    uint64_t current_time = Get_us();
    uint32_t now = (uint32_t)current_time;

    uint32_t t0l = t0_l, t1l = t1_l, t2l = t2_l, t3l = t3_l;
    uint32_t t0r = t0_r, t1r = t1_r, t2r = t2_r, t3r = t3_r;
    int dl = direction_l, dr = direction_r;

    float abs_l = fabsf(filtered_speed_l);
    float abs_r = fabsf(filtered_speed_r);

    //--- 左轮 ---//
    if      (intv_lvl_l == 1 && abs_l > 120.0f) intv_lvl_l = 2;
    else if (intv_lvl_l == 2 && abs_l < 80.0f)  intv_lvl_l = 1;
    else if (intv_lvl_l == 2 && abs_l > 330.0f) intv_lvl_l = 3;
    else if (intv_lvl_l == 3 && abs_l < 270.0f) intv_lvl_l = 2;

    uint32_t dt_l;
    float intervals_l;
    if (intv_lvl_l == 1 && t2l) {
        dt_l = t3l - t2l;  intervals_l = 1.0f;
    } else if (intv_lvl_l == 2 && t1l) {
        dt_l = t3l - t1l;  intervals_l = 2.0f;
    } else if (t0l) {
        dt_l = t3l - t0l;  intervals_l = 3.0f;
    } else {
        dt_l = 0;
    }

    if (t0l && dt_l < (now - t0l)) dt_l = now - t0l;

    if (dt_l && dt_l >= MIN_DELTA_US) {
        float raw_l = (float)dl * intervals_l * 360.0f * 1000000.0f / (float)dt_l / PULSE_PER_REVOLUTION;
        float a_l = Encoder_Filter_Select(filtered_speed_l);
        filtered_speed_l = filtered_speed_l * a_l + raw_l * (1.0f - a_l);
    }

    //--- 右轮 ---//
    if      (intv_lvl_r == 1 && abs_r > 120.0f) intv_lvl_r = 2;
    else if (intv_lvl_r == 2 && abs_r < 80.0f)  intv_lvl_r = 1;
    else if (intv_lvl_r == 2 && abs_r > 330.0f) intv_lvl_r = 3;
    else if (intv_lvl_r == 3 && abs_r < 270.0f) intv_lvl_r = 2;

    uint32_t dt_r;
    float intervals_r;
    if (intv_lvl_r == 1 && t2r) {
        dt_r = t3r - t2r;  intervals_r = 1.0f;
    } else if (intv_lvl_r == 2 && t1r) {
        dt_r = t3r - t1r;  intervals_r = 2.0f;
    } else if (t0r) {
        dt_r = t3r - t0r;  intervals_r = 3.0f;
    } else {
        dt_r = 0;
    }

    if (t0r && dt_r < (now - t0r)) dt_r = now - t0r;

    if (dt_r && dt_r >= MIN_DELTA_US) {
        float raw_r = (float)dr * intervals_r * 360.0f * 1000000.0f / (float)dt_r / PULSE_PER_REVOLUTION;
        float a_r = Encoder_Filter_Select(filtered_speed_r);
        filtered_speed_r = filtered_speed_r * a_r + raw_r * (1.0f - a_r);
    }

    encoder_count_l_10ms = encoder_count_l - encoder_count_l_last;
    encoder_count_r_10ms = encoder_count_r - encoder_count_r_last;
    encoder_count_l_last = encoder_count_l;
    encoder_count_r_last = encoder_count_r;
    duration_time = current_time - last_10ms_update;
    last_10ms_update = current_time;
}
