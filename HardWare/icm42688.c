#include <stdio.h>
#include "icm42688.h"
#include "spi.h"
#include "ti_msp_dl_config.h"

static float accSensitivity   = 0.244f;   //加速度的最小分辨率 mg/LSB
static float gyroSensitivity  = 32.8f;    //陀螺仪的最小分辨率
//static uint8_t s_icm_spi_inited = 0;
//static uint8_t s_icm_spi_mode = 0; /* 0..3 -> mode0..mode3 */

// static inline void icm_spi_delay(void)
// {
//     __NOP();
//     __NOP();
//     __NOP();
//     __NOP();
// }

// static inline void icm_cs_low(void)
// {
//     DL_GPIO_clearPins(ICM_SPI_CS_PORT, ICM_SPI_CS_PIN);
// }

// static inline void icm_cs_high(void)
// {
//     DL_GPIO_setPins(ICM_SPI_CS_PORT, ICM_SPI_CS_PIN);
// }

// static void icm_spi_init(void)
// {
//     if (s_icm_spi_inited) {
//         return;
//     }

//     DL_GPIO_initDigitalOutput(ICM_SPI_MOSI_IOMUX);
//     DL_GPIO_initDigitalOutput(ICM_SPI_SCLK_IOMUX);
//     DL_GPIO_initDigitalOutput(ICM_SPI_CS_IOMUX);
//     DL_GPIO_initDigitalInput(ICM_SPI_MISO_IOMUX);

//     DL_GPIO_enableOutput(ICM_SPI_MOSI_PORT, ICM_SPI_MOSI_PIN);
//     DL_GPIO_enableOutput(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//     DL_GPIO_enableOutput(ICM_SPI_CS_PORT, ICM_SPI_CS_PIN);

//     DL_GPIO_clearPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//     DL_GPIO_clearPins(ICM_SPI_MOSI_PORT, ICM_SPI_MOSI_PIN);
//     icm_cs_high();

//     s_icm_spi_inited = 1;
// }

// static inline void icm_spi_apply_mode(uint8_t mode)
// {
//     s_icm_spi_mode = (mode & 0x03);
//     if ((s_icm_spi_mode == 0) || (s_icm_spi_mode == 1)) {
//         DL_GPIO_clearPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//     } else {
//         DL_GPIO_setPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//     }
//     icm_spi_delay();
// }

// static uint8_t icm_spi_txrx_byte(uint8_t tx)
// {
//     uint8_t rx = 0;
//     uint8_t cpol = (s_icm_spi_mode >> 1) & 0x01;
//     uint8_t cpha = s_icm_spi_mode & 0x01;

//     for (uint8_t i = 0; i < 8; i++) {
//         if (tx & 0x80) {
//             DL_GPIO_setPins(ICM_SPI_MOSI_PORT, ICM_SPI_MOSI_PIN);
//         } else {
//             DL_GPIO_clearPins(ICM_SPI_MOSI_PORT, ICM_SPI_MOSI_PIN);
//         }

//         icm_spi_delay();

//         if (cpol == 0) {
//             DL_GPIO_setPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//         } else {
//             DL_GPIO_clearPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//         }
//         icm_spi_delay();

//         if (cpha == 0) {
//             rx <<= 1;
//             if (DL_GPIO_readPins(ICM_SPI_MISO_PORT, ICM_SPI_MISO_PIN)) {
//                 rx |= 0x01;
//             }
//         }

//         if (cpol == 0) {
//             DL_GPIO_clearPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//         } else {
//             DL_GPIO_setPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//         }
//         icm_spi_delay();

//         if (cpha == 1) {
//             rx <<= 1;
//             if (DL_GPIO_readPins(ICM_SPI_MISO_PORT, ICM_SPI_MISO_PIN)) {
//                 rx |= 0x01;
//             }
//         }

//         tx <<= 1;
//     }

//     if ((s_icm_spi_mode == 0) || (s_icm_spi_mode == 1)) {
//         DL_GPIO_clearPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//     } else {
//         DL_GPIO_setPins(ICM_SPI_SCLK_PORT, ICM_SPI_SCLK_PIN);
//     }
//     return rx;
// }

//@brief  ICM42688 SPI单字节传输（内部使用，不控制CS）
//@param  tx: 要发送的字节
//@retval 接收到的字节
static uint8_t icm_spi_transfer_byte(uint8_t tx)
{
    DL_SPI_transmitData8(SPI_1_INST, tx);
    while(DL_SPI_isBusy(SPI_1_INST));
    return DL_SPI_receiveData8(SPI_1_INST);
}

static inline void icm_cs_low(void)
{
    SPI_CS(1, SPI_ICM42688);
}

static inline void icm_cs_high(void)
{
    SPI_CS(0, SPI_ICM42688);
}

void delay_us(uint32_t __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
void delay_ms(uint32_t __ms) { delay_cycles( (CPUCLK_FREQ / 1000)*__ms); }

// void delay_1us(uint32_t __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
// void delay_1ms(uint32_t __ms) { delay_cycles( (CPUCLK_FREQ / 1000)*__ms); }

/*ICM42688使用的ms级延时函数，须由用户提供。*/
#define ICM42688DelayMs(_nms)  delay_ms(_nms)

/*******************************************************************************
* 名    称： icm42688_read_reg
* 功    能： 读取单个寄存器的值
* 入口参数： reg: 寄存器地址
* 出口参数： 当前寄存器地址的值
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： 使用SPI读取寄存器时要注意:最高位为读写位，详见datasheet page51.
*******************************************************************************/
static uint8_t icm42688_read_reg(uint8_t reg)
{
    uint8_t regval = 0;

    icm_cs_low();
    icm_spi_transfer_byte(reg | 0x80);
    regval = icm_spi_transfer_byte(0xFF);
    icm_cs_high();

    return regval;
}

/*******************************************************************************
* 名    称： icm42688_read_regs
* 功    能： 连续读取多个寄存器的值
* 入口参数： reg: 起始寄存器地址 *buf数据指针,uint16_t len长度
* 出口参数： 无
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： 使用SPI读取寄存器时要注意:最高位为读写位，详见datasheet page50.
*******************************************************************************/
static void icm42688_read_regs(uint8_t reg, uint8_t* buf, uint16_t len)
{
    icm_cs_low();
    icm_spi_transfer_byte(reg | 0x80);
    while(len)
    {
        *buf = icm_spi_transfer_byte(0xFF);
        len--;
        buf++;
    }
    icm_cs_high();
}


/*******************************************************************************
* 名    称： icm42688_write_reg
* 功    能： 向单个寄存器写数据
* 入口参数： reg: 寄存器地址 value:数据
* 出口参数： 0
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： 使用SPI读取寄存器时要注意:最高位为读写位，详见datasheet page50.
*******************************************************************************/
static uint8_t icm42688_write_reg(uint8_t reg, uint8_t value)
{
    icm_cs_low();
    icm_spi_transfer_byte(reg & 0x7F);
    icm_spi_transfer_byte(value);
    icm_cs_high();

    return 0;
}



float bsp_Icm42688GetAres(uint8_t Ascale)
{
    switch(Ascale)
    {
    // Possible accelerometer scales (and their register bit settings) are:
    // 2 Gs (11), 4 Gs (10), 8 Gs (01), and 16 Gs  (00).
    case AFS_2G:
        accSensitivity = 2000 / 32768.0f;
        break;
    case AFS_4G:
        accSensitivity = 4000 / 32768.0f;
        break;
    case AFS_8G:
        accSensitivity = 8000 / 32768.0f;
        break;
    case AFS_16G:
        accSensitivity = 16000 / 32768.0f;
        break;
    }

    return accSensitivity;
}

float bsp_Icm42688GetGres(uint8_t Gscale)
{
    switch(Gscale)
    {
    case GFS_15_125DPS:
        gyroSensitivity = 15.125f / 32768.0f;
        break;
    case GFS_31_25DPS:
        gyroSensitivity = 31.25f / 32768.0f;
        break;
    case GFS_62_5DPS:
        gyroSensitivity = 62.5f / 32768.0f;
        break;
    case GFS_125DPS:
        gyroSensitivity = 125.0f / 32768.0f;
        break;
    case GFS_250DPS:
        gyroSensitivity = 250.0f / 32768.0f;
        break;
    case GFS_500DPS:
        gyroSensitivity = 500.0f / 32768.0f;
        break;
    case GFS_1000DPS:
        gyroSensitivity = 1000.0f / 32768.0f;
        break;
    case GFS_2000DPS:
        gyroSensitivity = 2000.0f / 32768.0f;
        break;
    }
    return gyroSensitivity;
}

/*******************************************************************************
* 名    称： bsp_Icm42688RegCfg
* 功    能： Icm42688 寄存器配置
* 入口参数： 无
* 出口参数： 无
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注：
*******************************************************************************/
int8_t bsp_Icm42688RegCfg(void)
{
    uint8_t reg_val = 0;
    /* 读取 who am i 寄存器 */
    reg_val = icm42688_read_reg(ICM42688_WHO_AM_I);
    icm42688_write_reg(ICM42688_REG_BANK_SEL, 0); //设置bank 0区域寄存器
    icm42688_write_reg(ICM42688_DEVICE_CONFIG, 0x01); //软复位传感器
    ICM42688DelayMs(100);


   // if(reg_val == ICM42688_ID)
    //{

        bsp_Icm42688GetAres(AFS_4G);
        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        //reg_val = icm42688_read_reg(ICM42688_ACCEL_CONFIG0);//page74
        reg_val = (AFS_4G << 5);   //量程 ±2g
        reg_val |= (AODR_100Hz);     //输出速率 100HZ
        icm42688_write_reg(ICM42688_ACCEL_CONFIG0, reg_val);

        bsp_Icm42688GetGres(GFS_1000DPS);
        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        //reg_val = icm42688_read_reg(ICM42688_GYRO_CONFIG0);//page73
        reg_val = (GFS_1000DPS << 5);   //量程 ±1000dps
        reg_val |= (GODR_100Hz);     //输出速率 100HZ
        icm42688_write_reg(ICM42688_GYRO_CONFIG0, reg_val);

        icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00);
        reg_val = icm42688_read_reg(ICM42688_PWR_MGMT0); //读取PWR—MGMT0当前寄存器的值(page72)
        reg_val &= ~(1 << 5);//使能温度测量
        reg_val |= ((3) << 2);//设置GYRO_MODE  0:关闭 1:待机 2:预留 3:低噪声
        reg_val |= (3);//设置ACCEL_MODE 0:关闭 1:关闭 2:低功耗 3:低噪声
        icm42688_write_reg(ICM42688_PWR_MGMT0, reg_val);
        ICM42688DelayMs(100); //操作完PWR—MGMT0寄存器后 200us内不能有任何读写寄存器的操作

        return 0;
   // }
   // return -1;
}
/*******************************************************************************
* 名    称： bsp_Icm42688Init
* 功    能： Icm42688 传感器初始化
* 入口参数： 无
* 出口参数： 0: 初始化成功  其他值: 初始化失败
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注：
*******************************************************************************/
int8_t bsp_Icm42688Init(void)
{
    //icm_spi_init();
    //icm_spi_apply_mode(0);
    return(bsp_Icm42688RegCfg());

}

uint8_t bsp_IcmGetWhoAmI(void)
{
    uint8_t who = 0;
    //icm_spi_init();

   // for (uint8_t mode = 0; mode < 4; mode++) {
        //icm_spi_apply_mode(mode);
    who = icm42688_read_reg(ICM42688_WHO_AM_I);
    //if (who == ICM42688_ID) {
    return who;
    //}
   // }

    //icm_spi_apply_mode(0);
   // return who;
}

/*******************************************************************************
* 名    称： bsp_IcmGetTemperature
* 功    能： 读取Icm42688 内部传感器温度
* 入口参数： 无
* 出口参数： 无
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： datasheet page62
*******************************************************************************/
int8_t bsp_IcmGetTemperature(int16_t* pTemp)
{
    uint8_t buffer[2] = {0};

    icm42688_read_regs(ICM42688_TEMP_DATA1, buffer, 2);

    *pTemp = (int16_t)(((int16_t)((buffer[0] << 8) | buffer[1])) / 132.48 + 25);
    return 0;
}

/*******************************************************************************
* 名    称： bsp_IcmGetAccelerometer
* 功    能： 读取Icm42688 加速度的值
* 入口参数： 三轴加速度的值
* 出口参数： 无
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： datasheet page62
*******************************************************************************/
int8_t bsp_IcmGetAccelerometer(icm42688RawData_t* accData)
{
    uint8_t buffer[6] = {0};

    icm42688_read_regs(ICM42688_ACCEL_DATA_X1, buffer, 6);

    accData->x = ((uint16_t)buffer[0] << 8) | buffer[1];
    accData->y = ((uint16_t)buffer[2] << 8) | buffer[3];
    accData->z = ((uint16_t)buffer[4] << 8) | buffer[5];

    accData->x = (int16_t)(accData->x * accSensitivity);
    accData->y = (int16_t)(accData->y * accSensitivity);
    accData->z = (int16_t)(accData->z * accSensitivity);

    return 0;
}

/*******************************************************************************
* 名    称： bsp_IcmGetGyroscope
* 功    能： 读取Icm42688 陀螺仪的值
* 入口参数： 三轴陀螺仪的值
* 出口参数： 无
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： datasheet page63
*******************************************************************************/
int8_t bsp_IcmGetGyroscope(icm42688RawData_t* GyroData)
{
    uint8_t buffer[6] = {0};

    icm42688_read_regs(ICM42688_GYRO_DATA_X1, buffer, 6);

    GyroData->x = ((uint16_t)buffer[0] << 8) | buffer[1];
    GyroData->y = ((uint16_t)buffer[2] << 8) | buffer[3];
    GyroData->z = ((uint16_t)buffer[4] << 8) | buffer[5];

    GyroData->x = (int16_t)(GyroData->x * gyroSensitivity);
    GyroData->y = (int16_t)(GyroData->y * gyroSensitivity);
    GyroData->z = (int16_t)(GyroData->z * gyroSensitivity);
    return 0;
}

/*******************************************************************************
* 名    称： bsp_IcmGetRawData
* 功    能： 读取Icm42688加速度陀螺仪数据
* 入口参数： 六轴
* 出口参数： 无
* 作　　者： Baxiange
* 创建日期： 2024-07-25
* 修    改：
* 修改日期：
* 备    注： datasheet page62,63
*******************************************************************************/
int8_t bsp_IcmGetRawData(icm42688RealData_t* accData, icm42688RealData_t* GyroData)
{
    uint8_t buffer[12] = {0};
	icm42688RawData_t accRaw;
	icm42688RawData_t gyroRaw;

    icm42688_read_regs(ICM42688_ACCEL_DATA_X1, buffer, 12);

    accRaw.x  = ((uint16_t)buffer[0] << 8)  | buffer[1];
    accRaw.y  = ((uint16_t)buffer[2] << 8)  | buffer[3];
    accRaw.z  = ((uint16_t)buffer[4] << 8)  | buffer[5];
    gyroRaw.x = ((uint16_t)buffer[6] << 8)  | buffer[7];
    gyroRaw.y = ((uint16_t)buffer[8] << 8)  | buffer[9];
    gyroRaw.z = ((uint16_t)buffer[10] << 8) | buffer[11];


    accData->x = (float)(accRaw.x * accSensitivity);
    accData->y = (float)(accRaw.y * accSensitivity);
    accData->z = (float)(accRaw.z * accSensitivity);

    GyroData->x = (float)(gyroRaw.x * gyroSensitivity);
    GyroData->y = (float)(gyroRaw.y * gyroSensitivity);
    GyroData->z = (float)(gyroRaw.z * gyroSensitivity);

    return 0;
}


