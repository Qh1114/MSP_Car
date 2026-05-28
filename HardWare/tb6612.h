#ifndef TB6612_H
#define TB6612_H

#include "ti_msp_dl_config.h"

#define MAX_DUTY 60.0f

// 函数声明（全部同步改为浮点数参数）
void TB6612_Init(void);
void TB6612_Forward(float duty);
void TB6612_Backward(float duty);
void TB6612_Brake(void);
void TB6612_Coast(void);
void TB6612_SetLeftMotor(float duty);
void TB6612_SetRightMotor(float duty);

#endif