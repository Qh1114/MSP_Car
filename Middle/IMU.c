#include "icm42688.h"
#include "Fusion.h"
#include "Usart.h"
#include "IMU.h"
#include "Delay.h"

extern uint16_t data_length;  // icm42688.c 本地缓冲区状态

#define SAMPLE_RATE 200.0f
bool IMU_initialized = false;
FusionAhrs ahrs;
const FusionMatrix gyroscopeMisalignment = {.array = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
const FusionVector gyroscopeSensitivity = {.array = {1.0f, 1.0f, 1.0f}};
FusionVector gyroscopeOffset     = {.array = {0.0f, 0.0f, 0.0f}};

const FusionMatrix accelerometerMisalignment = {.array = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
const FusionVector accelerometerSensitivity = {.array = {1.0f, 1.0f, 1.0f}};
FusionVector accelerometerOffset  = {.array = {0.0f, 0.0f, 0.0f}};
FusionBias bias;
void IMU_init(void)
{

    // Instantiate AHRS algorithm
    FusionAhrsInitialise(&ahrs);
    bsp_Icm42688Init();

    ICM42688_Getoffset(&accelerometerOffset, &gyroscopeOffset); //获取初始偏移值

    const FusionAhrsSettings settings = {
        .convention = FusionConventionNwu,
        .gain = 0.5f,
        .gyroscopeRange = 1000.0f,          //陀螺仪量程 ±1000dps
        .accelerationRejection = 10.0f,
        .magneticRejection = 10.0f,
        .recoveryTriggerPeriod = 5 * SAMPLE_RATE, /* 5 seconds */
    };

    FusionAhrsSetSettings(&ahrs, &settings);

    // Instantiate bias algorithm
    FusionBiasInitialise(&bias);

    FusionBiasSettings biasSettings = fusionBiasDefaultSettings;
    biasSettings.sampleRate = SAMPLE_RATE;

    FusionBiasSetSettings(&bias, &biasSettings);

    IMU_initialized = true;
}


void IMU_GetOrientation(float *yaw, float *pitch, float *roll)
{
    if (!IMU_initialized) return;
    FusionQuaternion quaternion = FusionAhrsGetQuaternion(&ahrs);
    FusionEuler euler = FusionQuaternionToEuler(quaternion);
    *yaw = euler.angle.yaw;
    *pitch = euler.angle.pitch;
    *roll = euler.angle.roll;
}

float IMU_GetYaw(void)
{
    if (!IMU_initialized) return 0.0f;
    FusionQuaternion quaternion = FusionAhrsGetQuaternion(&ahrs);
    FusionEuler euler = FusionQuaternionToEuler(quaternion);
    return euler.angle.yaw;
}

void IMU_Callback(void)
{
    if (!IMU_initialized) return;
    ICM42688_UnBlocking_CallBack();

    // 用data_length判断本地缓冲区是否真的读到了数据
    // 避免硬件FIFO计数与本地缓冲不同步导致的死循环+垃圾数据
    while (data_length >= 16) {
        icm42688RealData_t accData;
        icm42688RealData_t gyroData;
        uint16_t delta_time;
        float delta_time_s;
        FusionVector accelerometer = {.array = {0.0f, 0.0f, 1.0f}};
        FusionVector gyroscope = {.array = {0.0f, 0.0f, 0.0f}};
        ICM42688_FIFO_Get_RealData(&accData, &gyroData);

        accelerometer.axis.x = accData.x / 1000.0f; // 将mg转换为g
        accelerometer.axis.y = accData.y / 1000.0f;
        accelerometer.axis.z = accData.z / 1000.0f;
        gyroscope.axis.x = gyroData.x;
        gyroscope.axis.y = gyroData.y;
        gyroscope.axis.z = gyroData.z;

        gyroscope = FusionModelInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
        accelerometer = FusionModelInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);

        gyroscope = FusionBiasUpdate(&bias, gyroscope);

        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, 0.005f); 

        ICM42688_UnBlocking_CallBack(); // 尝试读下一包
    }
}