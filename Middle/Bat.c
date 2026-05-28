#include "Bat.h"
#include "ADC.h"

#define RATIO 4.085
#define ADC_TO_VOLTAGE(raw) ((float)(raw) / 4096.0f / 8.0f * 3.3f * RATIO) // 采样八次

bool cmd_bat = false; 
volatile float Battery_Voltage = 0.0f;
void Bat_Init(void)
{
    Battery_Voltage = 0.0f;
    cmd_bat = true; 
    ADC_Init();
}

float Bat_Read(void)
{
    return Battery_Voltage;
}

void Bat_Callback(void)
{
    if(!cmd_bat) {
        return; // 如果未启用电池电压输出，则直接返回
    }
    uint16_t raw_value = 0;
    ADC_Read(&raw_value);
    Battery_Voltage = ADC_TO_VOLTAGE(raw_value);
    DL_ADC12_enableConversions(BAT_ADC_INST);
    DL_ADC12_startConversion(BAT_ADC_INST);
}
