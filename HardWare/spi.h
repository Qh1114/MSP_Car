#ifndef __SPI_H__
#define __SPI_H__

#include "ti_msp_dl_config.h"

typedef enum {
    SPI_ICM42688 = 0,
    SPI_W25Q128,
    SPI_OLED
} spi_channel_t;

typedef struct {
    GPIO_Regs* cs_port;
    uint32_t cs_pin;
    bool is_active_low;
} spi_device_t;

#define SPI_CS_LOW(port, pin)  DL_GPIO_clearPins(port, pin)
#define SPI_CS_HIGH(port, pin) DL_GPIO_setPins(port, pin)

void SPI_Init(void);
bool SPI_CS(uint8_t state, spi_channel_t channel);

#endif
