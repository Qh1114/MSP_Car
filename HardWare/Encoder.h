#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "ti_msp_dl_config.h"

typedef enum {
    ENA_L,
    ENB_L,
    ENA_R,
    ENB_R
} EncoderType;

void Encoder_Init(void);
void Encoder_Count_Get(int64_t* count_l, int64_t* count_r);
void Encoder_Speed_Get(float* speed_l, float* speed_r);
void Encoder_Speed_Get_LEFT(float* speed_l);
void Encoder_Speed_Get_RIGHT(float* speed_r);
void Encoder_AngleSpeed_Get(float* angle_l, float* angle_r);
void Encoder_Callback(EncoderType type);
void Encoder_10ms_Callback(void);

#endif