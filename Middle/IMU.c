
#include "IMU.h"
#include "icm42688.h"
#include "IMU.h"
#include "icm42688.h"
#include <stdio.h>

/* XYZ 结构体 */
/* 加速度：地理北方向的加速度在加速度计 X 轴的投影 */
/* 加速度：地理西方向的加速度在加速度计 Y 轴的投影 */
xyz_f_t north, west;

volatile float exInt, eyInt, ezInt; // 误差积分
volatile float q0, q1, q2, q3;		// 全局四元数
volatile float integralFBhand, handdiff;
volatile uint32_t lastUpdate, now;		 // 采样时间，单位 us
volatile float yaw[5] = {0, 0, 0, 0, 0}; // 航向滤波值
int16_t Ax_offset = 0, Ay_offset = 0;
float TTangles_gyro[7]; // 临时角度/陀螺数据

float Angle_Final[3]; // X 轴倾斜角度
float Kp = 10.0f;
static const float gyro_deadband_dps = 0.35f;
static const float integ_limit = 0.2f;

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);

// Fast inverse square-root
/**************************实现说明********************************************
 * 函数原型:    float invSqrt1(float x)
 * 功能:        快速计算 1/sqrt(x)
 * 入口参数:    x - 输入值
 * 返回值:      1/sqrt(x) 的近似值
 ******************************************************************************/
float invSqrt1(float x)
{
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long *)&y;
	i = 0x5f3759df - (i >> 1);
	y = *(float *)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}

/**************************实现说明********************************************
 * 函数原型:    void IMU_init(void)
 * 功能:        初始化 IMU，初始化四元数和误差积分
 * 入口参数:    无
 * 返回值:      无
 ******************************************************************************/
void IMU_init(void)
{
	if (0x00 == bsp_Icm42688Init())
	{
		// initialize quaternion
		q0 = 1.0f;
		q1 = 0.0f;
		q2 = 0.0f;
		q3 = 0.0f;
		exInt = 0.0f;
		eyInt = 0.0f;
		ezInt = 0.0f;
		return;
	}
	printf("IMU ERROR!!\r\n");
}

static double Gyro_fill[3][300];
static double Gyro_total[3];
static double sqrGyro_total[3];
static int GyroinitFlag = 0;
static int GyroCount = 0;

// 计算陀螺仪方差
// 方差公式: S^2 = (X1^2 + X2^2 + ... + Xn^2)/n - (X平均)^2
// 参数说明:
//   data[]  - 输入数据数组
//   length  - 数据长度
// 输出:
//   sqrResult[] - 方差结果
//   avgResult[] - 平均值
void calGyroVariance(float data[], int length, float sqrResult[], float avgResult[])
{
	int i;
	double tmplen;
	if (GyroinitFlag == 0)
	{
		for (i = 0; i < 3; i++)
		{
			Gyro_fill[i][GyroCount] = data[i];
			Gyro_total[i] += data[i];
			sqrGyro_total[i] += data[i] * data[i];
			sqrResult[i] = 100;
			avgResult[i] = 0;
		}
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			Gyro_total[i] -= Gyro_fill[i][GyroCount];
			sqrGyro_total[i] -= Gyro_fill[i][GyroCount] * Gyro_fill[i][GyroCount];
			Gyro_fill[i][GyroCount] = data[i];
			Gyro_total[i] += Gyro_fill[i][GyroCount];
			sqrGyro_total[i] += Gyro_fill[i][GyroCount] * Gyro_fill[i][GyroCount];
		}
	}
	GyroCount++;
	if (GyroCount >= length)
	{
		GyroCount = 0;
		GyroinitFlag = 1;
		Kp = 0.5f;
	}
	if (GyroinitFlag == 0)
	{
		return;
	}
	tmplen = length;
	for (i = 0; i < 3; i++)
	{
		avgResult[i] = (float)(Gyro_total[i] / tmplen);
		sqrResult[i] = (float)((sqrGyro_total[i] - Gyro_total[i] * Gyro_total[i] / tmplen) / tmplen);
	}
}
float gyro_offset[3] = {0};
int CalCount = 0;

/**************************实现说明********************************************
 * 函数原型:    void IMU_getValues(float *values)
 * 功能:        获取加速度计和陀螺仪当前测量值，并做简单的去噪与自动校准
 * 入口参数:    values - 输出数组指针，格式: [ax, ay, az, gx, gy, gz]
 * 返回值:      无
 ******************************************************************************/
void IMU_getValues(float *values)
{
	icm42688RealData_t accval;
	icm42688RealData_t gyroval;

	float sqrResult_gyro[3];
	float avgResult_gyro[3];
	// 获取加速度和陀螺仪的当前值
	bsp_IcmGetRawData(&accval, &gyroval);
	TTangles_gyro[0] = accval.x;
	TTangles_gyro[1] = accval.y;
	TTangles_gyro[2] = accval.z;
	TTangles_gyro[3] = gyroval.x;
	TTangles_gyro[4] = gyroval.y;
	TTangles_gyro[5] = gyroval.z;
	TTangles_gyro[6] = 0;

	calGyroVariance(&TTangles_gyro[3], 100, sqrResult_gyro, avgResult_gyro);
	if (sqrResult_gyro[0] < 0.02f && sqrResult_gyro[1] < 0.02f && sqrResult_gyro[2] < 0.02f && CalCount >= 99)
	{
		gyro_offset[0] = avgResult_gyro[0];
		gyro_offset[1] = avgResult_gyro[1];
		gyro_offset[2] = avgResult_gyro[2];
		exInt = 0;
		eyInt = 0;
		ezInt = 0;
		CalCount = 0;
	}
	else if (CalCount < 100)
	{
		CalCount++;
	}
	values[0] = accval.x;
	values[1] = accval.y;
	values[2] = accval.z;
	values[3] = gyroval.x - gyro_offset[0];
	values[4] = gyroval.y - gyro_offset[1];
	values[5] = gyroval.z - gyro_offset[2];

	if (fabsf(values[3]) < gyro_deadband_dps)
		values[3] = 0.0f;
	if (fabsf(values[4]) < gyro_deadband_dps)
		values[4] = 0.0f;
	if (fabsf(values[5]) < gyro_deadband_dps)
		values[5] = 0.0f;
}

/**************************实现说明********************************************
 * 函数原型:    void IMU_AHRSupdate(...)
 * 功能:        Madgwick AHRS 算法，使用陀螺和加速度更新四元数
 * 入口参数:    gx,gy,gz - 陀螺角速度 (rad/s)
 *               ax,ay,az - 加速度计测量值
 *               mx,my,mz - 磁力计测量值（未使用）
 * 返回值:      无
 ******************************************************************************/
// proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.001f // integral gain governs rate of convergence of gyroscope biases

void IMU_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz)
{
	float norm;
	float vx, vy, vz;
	float ex, ey, ez, halfT;
	float tempq0, tempq1, tempq2, tempq3;

	// 预计算四元数乘积项
	float q0q0 = q0 * q0;
	float q0q1 = q0 * q1;
	float q0q2 = q0 * q2;
	// float q0q3 = q0 * q3;
	float q1q1 = q1 * q1;
	// float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q3q3 = q3 * q3;

	halfT = 0.005f;

	norm = ax * ax + ay * ay + az * az;
	if (norm < 1e-8f)
	{
		return;
	}
	norm = invSqrt1(norm);
	ax = ax * norm;
	ay = ay * norm;
	az = az * norm;

	// 估计的重力方向向量 v
	vx = 2 * (q1q3 - q0q2);
	vy = 2 * (q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3;

	/* 计算地理坐标系方向在机体坐标系上的投影:
	   north: 地理北方向在机体坐标系上的投影
	   west:  地理西方向在机体坐标系上的投影 */
	north.x = 1 - 2 * (q3 * q3 + q2 * q2);
	north.y = 2 * (-q0 * q3 + q1 * q2);
	north.z = 2 * (+q0 * q2 - q1 * q3);
	west.x = 2 * (+q0 * q3 + q1 * q2);
	west.y = 1 - 2 * (q3 * q3 + q1 * q1);
	west.z = 2 * (-q0 * q1 + q2 * q3);

	// 误差为加速度测量向量与估计重力向量的叉乘
	ex = (ay * vz - az * vy);
	ey = (az * vx - ax * vz);
	ez = (ax * vy - ay * vx);

	exInt = exInt + ex * Ki * halfT;
	eyInt = eyInt + ey * Ki * halfT;
	ezInt = ezInt + ez * Ki * halfT;

	// 积分限幅
	if (exInt > integ_limit)
		exInt = integ_limit;
	if (exInt < -integ_limit)
		exInt = -integ_limit;
	if (eyInt > integ_limit)
		eyInt = integ_limit;
	if (eyInt < -integ_limit)
		eyInt = -integ_limit;
	if (ezInt > integ_limit)
		ezInt = integ_limit;
	if (ezInt < -integ_limit)
		ezInt = -integ_limit;

	// 用 PI 修正陀螺零偏
	gx = gx + Kp * ex + exInt;
	gy = gy + Kp * ey + eyInt;
	gz = gz + Kp * ez + ezInt;

	// 四元数积分（简单一阶近似）
	tempq0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
	tempq1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
	tempq2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
	tempq3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

	// 四元数归一化
	norm = invSqrt1(tempq0 * tempq0 + tempq1 * tempq1 + tempq2 * tempq2 + tempq3 * tempq3);
	q0 = tempq0 * norm;
	q1 = tempq1 * norm;
	q2 = tempq2 * norm;
	q3 = tempq3 * norm;
}

/**************************实现说明********************************************
 * 函数原型:    void IMU_getQ(float *q)
 * 功能:        更新四元数并返回当前四元数值
 * 入口参数:    q - 输出数组地址 [q0,q1,q2,q3]
 * 返回值:      无
 ******************************************************************************/
float mygetqval[9]; // 缓冲传感器数据
void IMU_getQ(float *q)
{
	IMU_getValues(mygetqval);
	IMU_AHRSupdate(mygetqval[3] * M_PI / 180, mygetqval[4] * M_PI / 180, mygetqval[5] * M_PI / 180,
				   mygetqval[0], mygetqval[1], mygetqval[2], mygetqval[6], mygetqval[7], mygetqval[8]);

	q[0] = q0;
	q[1] = q1;
	q[2] = q2;
	q[3] = q3;
}

/**************************实现说明********************************************
 * 函数原型:    void IMU_getYawPitchRoll(float *angles)
 * 功能:        计算并返回当前姿态角 (Yaw/Pitch/Roll)，单位为度
 * 入口参数:    angles - 输出数组 [yaw, pitch, roll]
 * 返回值:      无
 ******************************************************************************/
void IMU_getYawPitchRoll(float *angles)
{
	float q[4];
	IMU_getQ(q);

	angles[0] = -atan2(2 * q[1] * q[2] + 2 * q[0] * q[3], -2 * q[2] * q[2] - 2 * q[3] * q[3] + 1) * 180 / M_PI; // yaw
	angles[1] = -asin(-2 * q[1] * q[3] + 2 * q[0] * q[2]) * 180 / M_PI;											// pitch
	angles[2] = atan2(2 * q[2] * q[3] + 2 * q[0] * q[1], -2 * q[1] * q[1] - 2 * q[2] * q[2] + 1) * 180 / M_PI;	// roll

	if (fabsf(angles[0]) < 0.05f)
	{
		angles[0] = 0.0f;
	}
}

void IMU_TT_getgyro(float *zsjganda)
{
	zsjganda[0] = TTangles_gyro[0];
	zsjganda[1] = TTangles_gyro[1];
	zsjganda[2] = TTangles_gyro[2];
	zsjganda[3] = TTangles_gyro[3];
	zsjganda[4] = TTangles_gyro[4];
	zsjganda[5] = TTangles_gyro[5];
	zsjganda[6] = TTangles_gyro[6];
}
// 角度差计算
float angle_diff(float current, float target)
{
	while (target > 180)
		target -= 360;
	while (target < -180)
		target += 360;
	float diff = current - target;
	while (diff > 180)
		diff -= 360;
	while (diff < -180)
		diff += 360;
	return diff;
}
	//------------------End of File----------------------------


// void IMU_Callback(void)
// {
// 	if(!cmd_imu) {
// 		return; // 如果未启用 IMU 输出，则直接返回
// 	}
	
// 	// 10ms 更新四元数（结果写入全局 q0~q3）
//   	IMU_getValues(mygetqval_1);	 
// 	// 陀螺 dps 转 rad/s，并更新 AHRS
// 	IMU_AHRSupdate(mygetqval_1[3] * M_PI/180, mygetqval_1[4] * M_PI/180, mygetqval_1[5] * M_PI/180,
//    			mygetqval_1[0], mygetqval_1[1], mygetqval_1[2], mygetqval_1[6], mygetqval_1[7], mygetqval_1[8]);
// }
 