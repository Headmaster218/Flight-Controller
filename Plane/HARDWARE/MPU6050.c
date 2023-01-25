#include <MPU6050.h>

struct _MPU_Data MPU_Data;
/**
 * @description: 
 * @param {u8} state 	1:第一次初始化	0:非第一次初始化
 * @return {*}
 */
u8 MPU_Init(u8 state)
{
	if (state)
	{
		I2C1_Soft_Single_Write(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X80); // 复位MPU6050
		delay_ms(200);
		MPU_Data.offline_cnt=100;
	}
	else
	{
		I2C_GenerateSTOP(I2C1, ENABLE);
		I2C_GenerateSTART(I2C1, ENABLE);
	}
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X00); // 唤醒MPU6050
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_GYRO_CFG_REG, 2 << 3);// gyro传感器,±1000dps *0.03051851 ->d/s
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_ACCEL_CFG_REG, 1 << 3);// acce传感器,±4g      *0.001196326 ->m/s2  //float have 7 useful number
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_CFG_REG, 0x05); // fliter 10hz
	// lpf = I2C1_Soft_Single_Write(MPU_ADDR, MPU_SAMPLE_RATE_REG, 9);//采样频率=1000 /（1+SMPLRT_DIV)
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_INT_EN_REG, 0X00);				// 关闭所有中断
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_USER_CTRL_REG, 0X00);			// I2C主模式关闭
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_FIFO_EN_REG, 0X00);			// 关闭FIFO
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_INTBP_CFG_REG, 0X80);			// INT引脚低电平有效
	if (I2C1_Soft_Single_Read(MPU_ADDR, MPU_DEVICE_ID_REG) == MPU_ADDR) // 器件ID正确
	{
		I2C1_Soft_Single_Write(MPU_ADDR, MPU_PWR_MGMT1_REG, 0X01); // 设置CLKSEL,PLL X轴为参考
		I2C1_Soft_Single_Write(MPU_ADDR, MPU_PWR_MGMT2_REG, 0X00); // 加速度与陀螺仪都工作
	}
	MPU_Data.offset.gyro[0] = 38;
	MPU_Data.offset.gyro[1] = -9;
	MPU_Data.offset.gyro[2] = 13;
	MPU_Data.offset.acce[0] = 0;
	MPU_Data.offset.acce[1] = 0;
	MPU_Data.offset.acce[2] = 525;
	return 1;
}

u8 MPU_times = 0;
double norm, total[3] = {0};
void MPU_My_Calculate(void)
{
	total[0] += MPU_Data.acce_f[0];
	total[1] += MPU_Data.acce_f[1];
	total[2] += MPU_Data.acce_f[2];

	if (MPU_times % 10 == 0)
	{
		norm = sqrt(total[0] * total[0] + total[1] * total[1] + total[2] * total[2]);

		if (norm > 95 && norm < 105)
		{
			MPU_Data.pitch_raw = fast_atan2(MPU_Data.acce[0], MPU_Data.acce[2]) / ANGLE_TO_RAD;
			MPU_Data.yaw_raw = fast_atan2(MPU_Data.acce[0], MPU_Data.acce[1]) / ANGLE_TO_RAD;
			MPU_Data.roll_raw = fast_atan2(MPU_Data.acce[2], MPU_Data.acce[1]) / ANGLE_TO_RAD;
		}
		MPU_times = 0;
		total[0] = total[1] = total[2] = 0;
	}
	MPU_times++;

	MPU_Data.pitch_raw -= (double)MPU_Data.gyro[1] * 0.03051851 * 0.01;
	MPU_Data.yaw_raw += (double)MPU_Data.gyro[2] * 0.03051851 * 0.01;
	MPU_Data.roll_raw -= (double)MPU_Data.gyro[0] * 0.03051851 * 0.01; // 100hz;

	///////////down////////up
	//pitch 0  -180  180  +180
	////////L/////////R
	// roll	0	-90	-180
	if(MPU_Data.pitch_raw>0)
		MPU_Data.pitch=180-MPU_Data.pitch_raw;
	else
		MPU_Data.pitch=-180-MPU_Data.pitch_raw;
	MPU_Data.roll=-90-MPU_Data.roll_raw;
}

// acce x,y,z Temp ,Gyro x,y,z
u8 MPU_Get_Raw_Data(void)
{
	u8 MPU_reg_buf[14], i = 0;
	short *MPU_data = (void *)MPU_Data.acce;
	if (ERROR == I2C1_Soft_Mult_Read(MPU_ADDR, MPU_ACCEL_XOUTH_REG, MPU_reg_buf, 14))
		return ERROR;
	for (; i < 7; i++)
		MPU_data[i] = ((u16)MPU_reg_buf[i * 2] << 8) | MPU_reg_buf[i * 2 + 1];
	MPU_Data.gyro[0] += MPU_Data.offset.gyro[0];
	MPU_Data.gyro[1] += MPU_Data.offset.gyro[1];
	MPU_Data.gyro[2] += MPU_Data.offset.gyro[2];
	MPU_Data.acce[0] += MPU_Data.offset.acce[0];
	MPU_Data.acce[1] += MPU_Data.offset.acce[1];
	MPU_Data.acce[2] += MPU_Data.offset.acce[2];

	MPU_Data.gyro_f[0] = MPU_Data.gyro[0] * 0.03051851;
	MPU_Data.gyro_f[1] = MPU_Data.gyro[1] * 0.03051851;
	MPU_Data.gyro_f[2] = MPU_Data.gyro[2] * 0.03051851;
	MPU_Data.acce_f[0] = MPU_Data.acce[0] * 0.001196326;
	MPU_Data.acce_f[1] = MPU_Data.acce[1] * 0.001196326;
	MPU_Data.acce_f[2] = MPU_Data.acce[2] * 0.001196326;

	MPU_Data.temp = (3653 + ((double)MPU_Data.temp) * 0.294118);
	return SUCCESS;
}

// wait 5s to set offset
void MPU_Set_Offset_Data(void)
{
	u16 i = 0;
	float total[3] = {0};
	for (; i < 5000; i++)
	{
		delay_ms(1);
		total[0] += MPU_Data.gyro[0];
		total[1] += MPU_Data.gyro[1];
		total[2] += MPU_Data.gyro[2];
	}
	MPU_Data.offset.gyro[0] = total[0] / 5000;
	MPU_Data.offset.gyro[1] = total[1] / 5000;
	MPU_Data.offset.gyro[2] = total[2] / 5000;
}

/*
//二阶互补滤波系数，规律：基本时间常数tau得到基本系数a，Kp=2*a，Ki=a^2;
#define Kp 0.4f                           // proportional gain governs rate of convergence to accelerometer/magnetometer0.6
#define Ki 0.16f                           // integral gain governs rate of convergence of gyroscope biases0.1
#define half_T 0.005
#define IMU_INTEGRAL_LIM  ( 10.0f * ANGLE_TO_RAD )

void MPU_Calculate(void)
{
	float vx, vy, vz;//(r系到b系的第三列)

	float norm;
	float ex, ey, ez;


	//float gx = MPU_Data.gyro_f[0];
	//float gy = MPU_Data.gyro_f[1];
	//float gz = MPU_Data.gyro_f[2];
	//float ax = MPU_Data.acce_f[0];
	//float ay = MPU_Data.acce_f[1];
	//float az = MPU_Data.acce_f[2];

	static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;     // quaternion elements representing the estimated orientation
	static float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error

	//GPIOC->ODR ^= GPIO_Pin_14;

	MPU_Data.acce_f[0] = (float)MPU_Data.acce[0] * 0.001196326;
	MPU_Data.acce_f[1] = (float)MPU_Data.acce[1] * 0.001196326;
	MPU_Data.acce_f[2] = (float)MPU_Data.acce[2] * 0.001196326;
	MPU_Data.temp = (3653+((double)MPU_Data.temp)*0.294118);
	MPU_Data.gyro_f[0] = (float)MPU_Data.gyro[0] * 0.03051851;
	MPU_Data.gyro_f[1] = (float)MPU_Data.gyro[1] * 0.03051851;
	MPU_Data.gyro_f[2] = (float)MPU_Data.gyro[2] * 0.03051851;
	MPU_Data.dps_f[0]  = MPU_Data.gyro_f[0] * ANGLE_TO_RAD;
	MPU_Data.dps_f[1]  = MPU_Data.gyro_f[1] * ANGLE_TO_RAD;
	MPU_Data.dps_f[2]  = MPU_Data.gyro_f[2] * ANGLE_TO_RAD;

	//20us


	if(MPU_Data.acce_f[0]!=0||MPU_Data.acce_f[1]!=0||MPU_Data.acce_f[2]!=0)
	{
	//acc数据归一化
	norm = my_sqrt(MPU_Data.acce_f[0]*MPU_Data.acce_f[0] + MPU_Data.acce_f[1]*MPU_Data.acce_f[1] + MPU_Data.acce_f[2]*MPU_Data.acce_f[2]);
	MPU_Data.acce_f[0] = MPU_Data.acce_f[0] / norm;
	MPU_Data.acce_f[1] = MPU_Data.acce_f[1] / norm;
	MPU_Data.acce_f[2] = MPU_Data.acce_f[2] / norm;

	// estimated direction of gravity and flux (v and w)              估计重力方向和流量/变迁
	vx = 2*(q1*q3 - q0*q2);												//四元素中xyz的表示
	vy = 2*(q0*q1 + q2*q3);
	vz = 1 - 2*(q1*q1 + q2*q2);

	// error is sum of cross product between reference direction of fields and direction measured by sensors
	ex = (MPU_Data.acce_f[1]*vz - MPU_Data.acce_f[2]*vy) ;                         					 //向量外积在相减得到差分就是误差
	ey = (MPU_Data.acce_f[2]*vx - MPU_Data.acce_f[0]*vz) ;
	ez = (MPU_Data.acce_f[0]*vy - MPU_Data.acce_f[1]*vx) ;

	exInt = exInt + ex *Ki *2 *half_T;								  //对误差进行积分
	eyInt = eyInt + ey *Ki *2 *half_T;
	ezInt = ezInt + ez *Ki *2 *half_T;

	// 积分限幅
	exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );

	// adjusted gyroscope measurements
	MPU_Data.gyro_f[0] = MPU_Data.gyro_f[0] + Kp *(ex + exInt);
	MPU_Data.gyro_f[1] = MPU_Data.gyro_f[1] + Kp *(ey + eyInt);
	MPU_Data.gyro_f[2] = MPU_Data.gyro_f[2] + Kp *(ez + ezInt);

	// integrate quaternion rate and normalise						   //四元素的微分方程
	q0 = q0 + (-q1*MPU_Data.gyro_f[0] - q2*MPU_Data.gyro_f[1] - q3*MPU_Data.gyro_f[2]) *half_T;
	q1 = q1 + ( q0*MPU_Data.gyro_f[0] + q2*MPU_Data.gyro_f[2] - q3*MPU_Data.gyro_f[1]) *half_T;
	q2 = q2 + ( q0*MPU_Data.gyro_f[1] - q1*MPU_Data.gyro_f[2] + q3*MPU_Data.gyro_f[0]) *half_T;
	q3 = q3 + ( q0*MPU_Data.gyro_f[2] + q1*MPU_Data.gyro_f[1] - q2*MPU_Data.gyro_f[0]) *half_T;


	// normalise quaternion
	norm = my_sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	MPU_Data.yaw   = fast_atan2(2*q1*q2+2*q0*q3, -2*q2*q2-2*q3*q3+1) *57.3f;
	MPU_Data.roll  = fast_atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) *57.3f;
	MPU_Data.pitch = asin(-2*q1*q3 + 2*q0*q2) *57.3f;
}


	//GPIOC->ODR ^= GPIO_Pin_14;

	//250us

}
*/
