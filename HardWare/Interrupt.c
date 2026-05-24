#include "ti_msp_dl_config.h"
#include "my_vl53l1x.h"
#include "Usart.h"
#include "IMU.h"
#include "ADC.h"
#include "Encoder.h"
#include "Bat.h"
void GROUP1_IRQHandler(void)
{
    uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIOA, LASER_GPIO1_PIN | LASER_GPIO3_PIN | ENCODER_ENB_R_PIN);
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB, LASER_GPIO2_PIN | ENCODER_ENA_L_PIN  | ENCODER_ENB_L_PIN | ENCODER_ENA_R_PIN);

    //----------------------------VL53L1X---------------------------·//
    if ((gpioA & LASER_GPIO1_PIN) ==LASER_GPIO1_PIN) {
        my_vl53l1x_callback(1);
        DL_GPIO_clearInterruptStatus(GPIOA, LASER_GPIO1_PIN);
    }

    if ((gpioA & LASER_GPIO3_PIN) ==LASER_GPIO3_PIN) {
        my_vl53l1x_callback(3);
        DL_GPIO_clearInterruptStatus(GPIOA, LASER_GPIO3_PIN);
    }

    if ((gpioB & LASER_GPIO2_PIN) == LASER_GPIO2_PIN) {
        my_vl53l1x_callback(2);
        DL_GPIO_clearInterruptStatus(GPIOB, LASER_GPIO2_PIN);
    }

    //----------------------------编码器---------------------------·//

    if((gpioA & ENCODER_ENB_R_PIN) == ENCODER_ENB_R_PIN) {
        Encoder_Callback(ENB_R);
        DL_GPIO_clearInterruptStatus(GPIOA, ENCODER_ENB_R_PIN);
    }

    if((gpioB & ENCODER_ENA_L_PIN) == ENCODER_ENA_L_PIN) {
        Encoder_Callback(ENA_L);
        DL_GPIO_clearInterruptStatus(GPIOB, ENCODER_ENA_L_PIN);
    }

    if((gpioB & ENCODER_ENB_L_PIN) == ENCODER_ENB_L_PIN) {
        Encoder_Callback(ENB_L);
        DL_GPIO_clearInterruptStatus(GPIOB, ENCODER_ENB_L_PIN);
    }

    if((gpioB & ENCODER_ENA_R_PIN) == ENCODER_ENA_R_PIN) {
        Encoder_Callback(ENA_R);
        DL_GPIO_clearInterruptStatus(GPIOB, ENCODER_ENA_R_PIN);
    }


}

void TIMER_10ms_INST_IRQHandler(void)
{
    IMU_Callback();
    Bat_Callback();
}

void ADC0_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(BAT_ADC_INST)) {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            ADC_Callback();
            break;
        default:
            break;
    }
}