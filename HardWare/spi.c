#include "spi.h"
#include "ti_msp_dl_config.h"
bool spi_cs_locked = false;
uint8_t spi_cs_locked_channel = 0;
static spi_device_t spi_devices[] = {
    {ICM_ICM_CS_PORT, ICM_ICM_CS_PIN, false},
    {FLASH_PORT, FLASH_FLASH_CS_PIN, false},
    {OLED_PORT, OLED_OLED_CS_PIN, false}
};

//@brief  SPI总线初始化函数，配置所有SPI设备的CS引脚为高电平
//@param  无
//@retval 无
void SPI_Init(void)
{
    SPI_CS_HIGH(spi_devices[SPI_ICM42688].cs_port, spi_devices[SPI_ICM42688].cs_pin);
    SPI_CS_HIGH(spi_devices[SPI_W25Q128].cs_port, spi_devices[SPI_W25Q128].cs_pin);
    SPI_CS_HIGH(spi_devices[SPI_OLED].cs_port, spi_devices[SPI_OLED].cs_pin);

    spi_devices[SPI_ICM42688].is_active_low = false;
    spi_devices[SPI_W25Q128].is_active_low = false;
    spi_devices[SPI_OLED].is_active_low = false;
}

//@brief  SPI设备CS控制函数，根据传入的状态和通道控制对应设备的CS引脚
//@param  state: 1表示拉低CS引脚，0表示拉高CS引脚
//@param  channel: SPI设备通道，0表示ICM42688，1表示W25Q128，2表示OLED
//@retval true表示成功，false表示失败
bool SPI_CS(uint8_t state, spi_channel_t channel)
{
    if (channel >= sizeof(spi_devices) / sizeof(spi_device_t)) {
        return false; // 无效的通道
    }

    if(spi_cs_locked && spi_cs_locked_channel != channel) {
        return false; // 其他通道已锁定
    }

    spi_device_t* device = &spi_devices[channel];

    if(state) {
        SPI_CS_LOW(device->cs_port, device->cs_pin);
    } else {
        SPI_CS_HIGH(device->cs_port, device->cs_pin);
    }

    return true;
}
