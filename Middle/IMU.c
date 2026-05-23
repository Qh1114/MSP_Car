
#include "IMU.h"
#include "icm42688.h"
#include <stdio.h>
/* XYZ 向量 */

/* 机体系下的导航系单位向量 */
xyz_f_t north,west;
volatile float exInt, eyInt, ezInt;  // 积分误差
volatile float q0, q1, q2, q3; // 全局四元数
volatile float integralFBhand,handdiff;
volatile uint32_t lastUpdate, now; // 时间戳（us）
volatile float yaw[5]= {0,0,0,0,0};  // 航向历史
int16_t Ax_offset=0,Ay_offset=0;
float TTangles_gyro[7]; // 原始加速度/陀螺缓存
	
float Angle_Final[3];	// 倾角（度）
float Kp = 10.0f;
static const float gyro_deadband_dps = 0.35f;
static const float integ_limit = 0.2f;

void MadgwickAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);


// 快速倒平方根（近似）
float invSqrt1(float x) {
	float halfx = 0.5f * x;
	float y = x;
	long i = *(long*)&y;
	i = 0x5f3759df - (i>>1);
	y = *(float*)&i;
	y = y * (1.5f - (halfx * y * y));
	return y;
}


// 初始化 IMU 并清零四元数/积分误差
void IMU_init(void)
{	 
	//while(!ICM_Init());   // 旧初始化路径
	if (0x00 == bsp_Icm42688Init())
	{
		//Initial_Timer3();
		// 初始化四元数
		q0 = 1.0f;  // 初始四元数
		q1 = 0.0f;
		q2 = 0.0f;
		q3 = 0.0f;
		exInt = 0.0;
		eyInt = 0.0;
		ezInt = 0.0;
		NVIC_EnableIRQ(TIMER_10ms_INST_INT_IRQN);
		DL_Timer_startCounter(TIMER_10ms_INST);
		return;
	}
	
	//printf("IMU ERROR!!\r\n");
}

static double Gyro_fill[3][300];
static double Gyro_total[3];
static double sqrGyro_total[3];
static int GyroinitFlag = 0;
static int GyroCount = 0;

// 用滑动方差判断陀螺稳定性
// data[]: 最新陀螺样本（x/y/z）
// length: 窗口长度
// sqrResult[]: 方差
// avgResult[]: 均值

void calGyroVariance(float data[], int length, float sqrResult[], float avgResult[])
{
	int i;
	double tmplen;
	if (GyroinitFlag == 0)
	{
		for (i = 0; i< 3; i++)
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
		for (i = 0; i< 3; i++)
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
	for (i = 0; i< 3; i++)
	{
		avgResult[i] = (float)(Gyro_total[i] / tmplen);
		sqrResult[i] = (float)((sqrGyro_total[i] - Gyro_total[i] * Gyro_total[i] / tmplen) / tmplen);
	}
}
float gyro_offset[3] = {0};
int CalCount = 0;
// 读取加速度/陀螺，并应用零偏和死区
void IMU_getValues(float * values) 
{  
	//int16_t accgyroval[7];
	icm42688RealData_t accval;
	icm42688RealData_t gyroval;
	
	float sqrResult_gyro[3];
	float avgResult_gyro[3];
	// 读取加速度/陀螺
	bsp_IcmGetRawData(&accval, &gyroval);
    TTangles_gyro[0] =  accval.x;
    TTangles_gyro[1] =  accval.y;
    TTangles_gyro[2] =  accval.z;
	TTangles_gyro[3] =  gyroval.x;
	TTangles_gyro[4] =  gyroval.y;
	TTangles_gyro[5] =  gyroval.z;
	TTangles_gyro[6] =  0;
	
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
    values[0] =  accval.x;
    values[1] =  accval.y;
    values[2] =  accval.z;
	values[3] =  gyroval.x - gyro_offset[0];
	values[4] =  gyroval.y - gyro_offset[1];
	values[5] =  gyroval.z - gyro_offset[2];

	if (fabsf(values[3]) < gyro_deadband_dps) values[3] = 0.0f;
	if (fabsf(values[4]) < gyro_deadband_dps) values[4] = 0.0f;
	if (fabsf(values[5]) < gyro_deadband_dps) values[5] = 0.0f;
	

	// 量程：1000 dps（32.8 LSB/每 dps）
}


// AHRS 更新：加速度+陀螺融合（未使用磁力计）
	// 比例增益：控制收敛速度
#define Ki 0.001f   // 积分增益：抑制陀螺零偏

void IMU_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) 
{
  	float norm;
  	//float hx, hy, hz, bx, bz;
  	float vx, vy, vz;//, wx, wy, wz;
  	float ex, ey, ez,halfT;
 	float tempq0,tempq1,tempq2,tempq3;

	// 预计算重复项
  	float q0q0 = q0*q0;
  	float q0q1 = q0*q1;
  	float q0q2 = q0*q2;
  	float q0q3 = q0*q3;
  	float q1q1 = q1*q1;
  	float q1q2 = q1*q2;
  	float q1q3 = q1*q3;
  	float q2q2 = q2*q2;   
  	float q2q3 = q2*q3;
  	float q3q3 = q3*q3;   
	// 旧调试块已移除
  
  	halfT = 0.01f;

  
	norm = ax*ax + ay*ay + az*az;
	if (norm < 1e-8f) {
			return;
	}
	norm = invSqrt1(norm);
  	ax = ax * norm;
  	ay = ay * norm;
  	az = az * norm;
	// 归一化加速度

	// 估计重力方向（v）
  // 参考磁通方向（未使用）
//  hx = 2*mx*(0.5f - q2q2 - q3q3) + 2*my*(q1q2 - q0q3) + 2*mz*(q1q3 + q0q2);
//  hy = 2*mx*(q1q2 + q0q3) + 2*my*(0.5f - q1q1 - q3q3) + 2*mz*(q2q3 - q0q1);
//  hz = 2*mx*(q1q3 - q0q2) + 2*my*(q2q3 + q0q1) + 2*mz*(0.5f - q1q1 - q2q2);         
//  bx = sqrt((hx*hx) + (hy*hy));
//  bz = hz;     
  
	// 估计重力/磁通方向（v 和 w）
  	vx = 2*(q1q3 - q0q2);
  	vy = 2*(q0q1 + q2q3);
  	vz = q0q0 - q1q1 - q2q2 + q3q3;
  
	// 机体系下的导航系基向量
	north.x = 1 - 2*(q3*q3 + q2*q2);
	north.y = 2* (-q0*q3 + q1*q2);
	north.z = 2* (+q0*q2  - q1*q3);
	west.x = 2* (+q0*q3 + q1*q2);
	west.y = 1 - 2*(q3*q3 + q1*q1);
	west.z = 2* (-q0*q1 + q2*q3);
//  wx = 2*bx*(0.5 - q2q2 - q3q3) + 2*bz*(q1q3 - q0q2);
//  wy = 2*bx*(q1q2 - q0q3) + 2*bz*(q0q1 + q2q3);
//  wz = 2*bx*(q0q2 + q1q3) + 2*bz*(0.5 - q1q1 - q2q2);  
  
	// 误差为测量方向与估计方向的叉积
  	ex = (ay*vz - az*vy);// + (my*wz - mz*wy);
  	ey = (az*vx - ax*vz);// + (mz*wx - mx*wz);
  	ez = (ax*vy - ay*vx);// + (mx*wy - my*wx);
	// 误差为测量重力与估计重力的叉积
  	exInt = exInt + ex * Ki * halfT;
  	eyInt = eyInt + ey * Ki * halfT;	
 	ezInt = ezInt + ez * Ki * halfT;

	if (exInt > integ_limit) exInt = integ_limit;
	if (exInt < -integ_limit) exInt = -integ_limit;
	if (eyInt > integ_limit) eyInt = integ_limit;
	if (eyInt < -integ_limit) eyInt = -integ_limit;
	if (ezInt > integ_limit) ezInt = integ_limit;
	if (ezInt < -integ_limit) ezInt = -integ_limit;

	// 应用 PI 反馈
  	gx = gx + Kp*ex + exInt;
  	gy = gy + Kp*ey + eyInt;
  	gz = gz + Kp*ez + ezInt;

	// 四元数积分
  	tempq0 = q0 + (-q1*gx - q2*gy - q3*gz)*halfT;
  	tempq1 = q1 + (q0*gx + q2*gz - q3*gy)*halfT;
  	tempq2 = q2 + (q0*gy - q1*gz + q3*gx)*halfT;
  	tempq3 = q3 + (q0*gz + q1*gy - q2*gx)*halfT;  
  
	// 四元数归一化
  	norm = invSqrt1(tempq0*tempq0 + tempq1*tempq1 + tempq2*tempq2 + tempq3*tempq3);
  	q0 = tempq0 * norm;
  	q1 = tempq1 * norm;
  	q2 = tempq2 * norm;
  	q3 = tempq3 * norm;
}


// 更新并返回四元数
float mygetqval[9];	// 加速度/陀螺临时缓存
void IMU_getQ(float * q) 
{
  	IMU_getValues(mygetqval);	 
	// 陀螺 dps 转 rad/s，并更新 AHRS
	IMU_AHRSupdate(mygetqval[3] * M_PI/180, mygetqval[4] * M_PI/180, mygetqval[5] * M_PI/180,
   			mygetqval[0], mygetqval[1], mygetqval[2], mygetqval[6], mygetqval[7], mygetqval[8]);

	q[0] = q0; // 最新四元数
 	q[1] = q1;
  	q[2] = q2;
  	q[3] = q3;
}


// 由四元数计算偏航/俯仰/横滚（度）
void IMU_getYawPitchRoll(float * angles) {
	float q[4]; // 四元数
	// volatile float gx=0.0, gy=0.0, gz=0.0; // 未使用（预留）
	//IMU_getQ(q); // 更新四元数
	
	q[0] = q0;
	q[1] = q1;
	q[2] = q2;
	q[3] = q3;
  	angles[0] = -atan2(2 * q[1] * q[2] + 2 * q[0] * q[3], -2 * q[2]*q[2] - 2 * q[3] * q[3] + 1)* 180/M_PI; // yaw
  	angles[1] = -asin(-2 * q[1] * q[3] + 2 * q[0] * q[2])* 180/M_PI; // pitch
  	angles[2] = atan2(2 * q[2] * q[3] + 2 * q[0] * q[1], -2 * q[1] * q[1] - 2 * q[2] * q[2] + 1)* 180/M_PI; // roll
	if (fabsf(angles[0]) < 0.05f) {
			angles[0] = 0.0f;
	}
}

 void IMU_TT_getgyro(float * zsjganda)
{
	zsjganda[0] = TTangles_gyro[0];
    zsjganda[1] = TTangles_gyro[1];
    zsjganda[2] = TTangles_gyro[2];
	zsjganda[3] = TTangles_gyro[3];
	zsjganda[4] = TTangles_gyro[4];
	zsjganda[5] = TTangles_gyro[5];
	zsjganda[6] = TTangles_gyro[6];
}

float mygetqval_1[9] = {0,0,0,0,0,0,0,0,10.0f};

void IMU_Callback(void)
{
	// 10ms 更新四元数（结果写入全局 q0~q3）
  	IMU_getValues(mygetqval_1);	 
	// 陀螺 dps 转 rad/s，并更新 AHRS
	IMU_AHRSupdate(mygetqval_1[3] * M_PI/180, mygetqval_1[4] * M_PI/180, mygetqval_1[5] * M_PI/180,
   			mygetqval_1[0], mygetqval_1[1], mygetqval_1[2], mygetqval_1[6], mygetqval_1[7], mygetqval_1[8]);
}
 