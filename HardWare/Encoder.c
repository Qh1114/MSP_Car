#include "Encoder.h"

volatile int64_t encoder_count_l = 0 , encoder_count_r = 0;
void Encoder_Init(void)
{
    encoder_count_l = 0;
    encoder_count_r = 0;

    // 编码器初始化代码
}

void Encoder_Count_Get(int64_t* count_l, int64_t* count_r)
{
    *count_l = encoder_count_l;
    *count_r = encoder_count_r;
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
                encoder_count_r++;
            } else if(!ena_r_1 && enb_r_1) {
                encoder_count_r ++;
            } else if(!ena_r_1 && !enb_r_1) {
                encoder_count_r --;
            } else if(ena_r_1 && enb_r_1) {
                encoder_count_r --;
            }
            break;
        }
        case ENB_R:
        {
            int ena_r_2 = DL_GPIO_readPins(GPIOB, ENCODER_ENA_R_PIN);
            int enb_r_2 = DL_GPIO_readPins(GPIOA, ENCODER_ENB_R_PIN);
            if (ena_r_2 && !enb_r_2) {
                encoder_count_r --;
            } else if(!ena_r_2 && enb_r_2) {
                encoder_count_r --;
            } else if(!ena_r_2 && !enb_r_2) {
                encoder_count_r ++;
            } else if(ena_r_2 && enb_r_2) {
                encoder_count_r ++;
            }
            break;
        }
    }
}

