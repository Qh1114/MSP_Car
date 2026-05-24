#include "ADC.h"

uint16_t ADC_RawValue;
void ADC_Init(void)
{
    ADC_RawValue = 0;
    NVIC_EnableIRQ(BAT_ADC_INST_INT_IRQN);
    DL_ADC12_startConversion(BAT_ADC_INST);
}

void ADC_Read(uint16_t* raw_value)
{
    *raw_value = ADC_RawValue ;
}

void ADC_Callback(void)
{
    ADC_RawValue = DL_ADC12_getMemResult(BAT_ADC_INST, BAT_ADC_ADCMEM_0);
}