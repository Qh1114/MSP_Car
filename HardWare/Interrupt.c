#include "ti_msp_dl_config.h"
#include "my_vl53l1x.h"
#include "Usart.h"

void GROUP1_IRQHandler(void)
{
    uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIOA,LASER_GPIO1_PIN | LASER_GPIO3_PIN);
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB, LASER_GPIO2_PIN);

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
}