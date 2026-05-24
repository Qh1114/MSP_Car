#ifndef TB6612_H
#define TB6612_H

#include "ti_msp_dl_config.h"

#define MAX_SPEED 100.0f

// 函数声明（全部同步改为浮点数参数）
void TB6612_Init(void);
void TB6612_Forward(float speed);
void TB6612_Backward(float speed);
void TB6612_Brake(void);
void TB6612_Coast(void);
void TB6612_SetLeftMotor(float speed);
void TB6612_SetRightMotor(float speed);

/* 简单实验函数：主函数里调用用于快速验证电机驱动 */
void TB6612_Experiment(void);

#endif