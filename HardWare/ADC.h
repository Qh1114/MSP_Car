#ifndef __ADC_H__
#define __ADC_H__

#include "ti_msp_dl_config.h"

void ADC_Init(void);
void ADC_Read(uint16_t* raw_value);
void ADC_Callback(void);

#endif