#include <stdio.h>
#include "icm42688.h"
#include "spi.h"
#include "ti_msp_dl_config.h"
#include <string.h>
#include "Delay.h"
#include "Usart.h"
static float accSensitivity   = 0.244f;   //加速度的最小分辨率 mg/LSB
static float gyroSensitivity  = 32.8f;    //陀螺仪的最小分辨率

#define FIFO_BUF_SIZE  400
uint8_t  FIFO_Buffer[FIFO_BUF_SIZE];
bool     ICM42688_is_initialized = false;
uint16_t data_length = 0;
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

    icm42688_write_reg(ICM42688_SIGNAL_PATH_RESET, 0x02); //清空fifo

    reg_val = icm42688_read_reg(ICM42688_PWR_MGMT0); //读取PWR—MGMT0当前寄存器的值(page72)
    reg_val &= ~(1 << 5);//使能温度测量
    reg_val |= (3 << 2);//设置GYRO_MODE  0:关闭 1:待机 2:预留 3:低噪声
    reg_val |= (3);//设置ACCEL_MODE 0:关闭 1:关闭 2:低功耗 3:低噪声
    icm42688_write_reg(ICM42688_PWR_MGMT0, reg_val);    
    ICM42688DelayMs(100); //操作完PWR—MGMT0寄存器后 200us内不能有任何读写寄存器的操作

    bsp_Icm42688GetAres(AFS_4G);
    //reg_val = icm42688_read_reg(ICM42688_ACCEL_CONFIG0);//page74
    reg_val = (AFS_4G << 5);        //量程 ±4g
    reg_val |= (AODR_1000Hz);        //输出速率 100HZ
    icm42688_write_reg(ICM42688_ACCEL_CONFIG0, reg_val);

    bsp_Icm42688GetGres(GFS_1000DPS);
    //reg_val = icm42688_read_reg(ICM42688_GYRO_CONFIG0);//page73
    reg_val = (GFS_1000DPS << 5);   //量程 ±1000dps
    reg_val |= (GODR_1000Hz);     //输出速率 100HZ
    icm42688_write_reg(ICM42688_GYRO_CONFIG0, reg_val);

    reg_val = 0x00;
    reg_val |= (1 << 2);    //0:不使用FIFO中断 1:使用FIFO中断
    icm42688_write_reg(ICM42688_INT_SOURCE0, reg_val); 

    reg_val = 0x00;
    reg_val |= (01 << 6);    //00:不使用FIFO 01:连续使用FIFO 1x:FIFO满即停止采集
    icm42688_write_reg(ICM42688_FIFO_CONFIG, reg_val);

    reg_val = 0x00;
    reg_val |= (1 << 6);    //0:每次读取必须读取FIFO所有数据 1:每次读取可以读取部分FIFO数据
    reg_val |= (1 << 5);    //0:不使用FIFO中断 1:使用FIFO中断
    reg_val |= (0 << 4);    //0:不使用20位数据 1:使用20位数据 //选择20位数据后，此寄存器后三位的设置无效
    reg_val |= (1 << 3);    //0:不使用时间戳 1:使用时间戳
    reg_val |= (1 << 2);    //0:不使用温度数据 1:使用温度数据
    reg_val |= (1 << 1);    //0:不使用陀螺仪数据 1:使用陀螺仪数据
    reg_val |= 1;           //0:不使用加速度数据 1:使用加速度数据
    icm42688_write_reg(ICM42688_FIFO_CONFIG1, reg_val);

    reg_val = 17;    //设置FIFO的水位为1，即FIFO中有17字节数据时就触发中断
    icm42688_write_reg(ICM42688_FIFO_CONFIG2, reg_val);

    
    reg_val = 0x00;
    reg_val |= (1 << 2);        //0:脉冲模式 1:持续模式
    reg_val |= (1 << 1);        //0:开漏输出 1:推挽输出
    //reg_val &= 0 ;            //0:低电平有效 1:高电平有效
    icm42688_write_reg(ICM42688_INT_CONFIG, reg_val);

    icm42688_write_reg(ICM42688_SIGNAL_PATH_RESET, 0x02); //清空fifo

    ICM42688_is_initialized = true; //标记ICM42688已初始化

    return 0; 
    
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

//uint64_t start_time = 0, end_time = 0;
void ICM42688_UnBlocking_CallBack(void)
{
    //start_time = Get_us();
    if (!ICM42688_is_initialized) {
        return;
    }

    if(data_length != 0)
    {
        return;
    }
    if (DL_GPIO_readPins(ICM_ICM_INT_PORT, ICM_ICM_INT_PIN)) {
        return;
    }

    if (!SPI_TryLock(SPI_ICM42688)) {
        return;
    }

    // 读 INT_STATUS 清除中断（独立CS周期）
    icm_cs_low();
    icm_spi_transfer_byte(ICM42688_INT_STATUS | 0x80);
    icm_spi_transfer_byte(0xFF);
    icm_cs_high();

    // 读 FIFO 数据（新CS周期）
    icm_cs_low();
    icm_spi_transfer_byte(ICM42688_FIFO_DATA | 0x80);
    for(uint16_t i = 0; i < 16; i++)
    {
        FIFO_Buffer[i] = icm_spi_transfer_byte(0xFF);
    }
    data_length = 16;
    icm_cs_high();
    SPI_Unlock(SPI_ICM42688);
    //end_time = Get_us();
    //Uart0_Printf("ICM42688 UnBlocking Callback Time: %d us\n", (uint32_t)(end_time - start_time));

}

uint16_t ICM42688_Get_FIFO_Data(uint8_t* buf)
{
    if (!ICM42688_is_initialized) {
        return 0;
    }
    if(data_length == 0)
    {
        return 0;
    }
    NVIC_DisableIRQ(TIMER_2ms_INST_INT_IRQN);

    memcpy(buf, FIFO_Buffer, data_length);
    data_length = 0;
    NVIC_EnableIRQ(TIMER_2ms_INST_INT_IRQN);

    return 16; //每组数据16字节 (1H+6Accel+6Gyro+2Temp+2Timestamp)
}

uint16_t ICM42688_Get_FIFO_Data_Length(void)
{
    uint16_t data_length;

    uint8_t  fifo_cnt_buf[2];
    icm42688_read_regs(ICM42688_FIFO_COUNTH, fifo_cnt_buf, 2);
    data_length = ((uint16_t)fifo_cnt_buf[0] << 8) | fifo_cnt_buf[1];

    return data_length;
}
   
void ICM42688_FIFO_Get_RealData(icm42688RealData_t* accData, icm42688RealData_t* GyroData)
{
    if (!ICM42688_is_initialized) {
        return;
    }
    if(data_length == 0)
    {
        return;
    }
    icm42688RawData_t accRaw;
    icm42688RawData_t gyroRaw;
    NVIC_DisableIRQ(TIMER_2ms_INST_INT_IRQN);
    accRaw.x  = ((uint16_t)FIFO_Buffer[1] << 8)  | FIFO_Buffer[2];
    accRaw.y  = ((uint16_t)FIFO_Buffer[3] << 8)  | FIFO_Buffer[4];
    accRaw.z  = ((uint16_t)FIFO_Buffer[5] << 8)  | FIFO_Buffer[6];
    gyroRaw.x = ((uint16_t)FIFO_Buffer[7] << 8)  | FIFO_Buffer[8];
    gyroRaw.y = ((uint16_t)FIFO_Buffer[9] << 8)  | FIFO_Buffer[10];
    gyroRaw.z = ((uint16_t)FIFO_Buffer[11] << 8) | FIFO_Buffer[12];   
    data_length = 0;
    NVIC_ClearPendingIRQ(TIMER_2ms_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_2ms_INST_INT_IRQN);


    accData->x = (float)(accRaw.x * accSensitivity);
    accData->y = (float)(accRaw.y * accSensitivity);
    accData->z = (float)(accRaw.z * accSensitivity);
    GyroData->x = (float)(gyroRaw.x * gyroSensitivity);
    GyroData->y = (float)(gyroRaw.y * gyroSensitivity);
    GyroData->z = (float)(gyroRaw.z * gyroSensitivity);
}