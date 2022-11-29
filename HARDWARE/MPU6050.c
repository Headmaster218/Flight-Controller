#include <MPU6050.h>

struct _Mpu_Data Mpu_Data;

u8 MPU_Init(void)
{ 
	u16 data ,lpf;
	I2C1_Soft_Single_Write(MPU_ADDR,MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
	delay_ms(200);
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_GYRO_CFG_REG,2<<3);;					//gyro传感器,±1000dps *0.03051851 ->d/s
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_ACCEL_CFG_REG,1<<3);;					//acce传感器,±4g      *0.001196326 ->m/s2  //float have 7 useful number
	//I2C1_Soft_Single_Write(MPU_ADDR,MPU_CFG_REG, 0x03);					//fliter 42hz
	lpf = I2C1_Soft_Single_Write(MPU_ADDR, MPU_SAMPLE_RATE_REG, 9);//采样频率=1000 /（1+SMPLRT_DIV)
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_INT_EN_REG,0X00);	//关闭所有中断
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	I2C1_Soft_Single_Write(MPU_ADDR, MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	if(I2C1_Soft_Single_Read(MPU_ADDR,MPU_DEVICE_ID_REG) == MPU_ADDR)//器件ID正确
	{
		I2C1_Soft_Single_Write(MPU_ADDR, MPU_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
		I2C1_Soft_Single_Write(MPU_ADDR, MPU_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
		if(lpf>=188)data=1;
		else if(lpf>=98)data=2;
		else if(lpf>=42)data=3;
		else if(lpf>=20)data=4;
		else if(lpf>=10)data=5;
		else data=6; 
		return I2C1_Soft_Single_Write(MPU_ADDR, MPU_CFG_REG,data);
 	}
	return 1;
	
}

//二阶互补滤波系数，规律：基本时间常数tau得到基本系数a，Kp=2*a，Ki=a^2;
#define Kp 0.4f                           // proportional gain governs rate of convergence to accelerometer/magnetometer0.6
#define Ki 0.16f                           // integral gain governs rate of convergence of gyroscope biases0.1
#define half_T 0.005
#define IMU_INTEGRAL_LIM  ( 10.0f * ANGLE_TO_RAD )

void MPU_Fast_Calculate(void)
{
	float vx, vy, vz;//(r系到b系的第三列)
	float norm;
	float ex, ey, ez;
	static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;     // quaternion elements representing the estimated orientation
	static float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error

	if(Mpu_Data.acce[0]!=0||Mpu_Data.acce[1]!=0||Mpu_Data.acce[2]!=0)
	{ 
	Mpu_Data.temp = (3653+((double)Mpu_Data.temp)*0.294118);

	Mpu_Data.gyro_f[0] = (float)Mpu_Data.gyro[0] * 0.03051851 * ANGLE_TO_RAD;
	Mpu_Data.gyro_f[1] = (float)Mpu_Data.gyro[2] * 0.03051851 * ANGLE_TO_RAD;
	Mpu_Data.gyro_f[2] = (float)Mpu_Data.gyro[1] * 0.03051851 * ANGLE_TO_RAD;

	// estimated direction of gravity and flux (v and w)              估计重力方向和流量/变迁
	vx = 2*(q1*q3 - q0*q2);												//四元素中xyz的表示
	vy = 2*(q0*q1 + q2*q3);
	vz = 1 - 2*(q1*q1 + q2*q2);
		
	//acc数据归一化
	norm = my_sqrt((float)Mpu_Data.acce[0]*(float)Mpu_Data.acce[0] + (float)Mpu_Data.acce[1]*(float)Mpu_Data.acce[1] + (float)Mpu_Data.acce[2]*(float)Mpu_Data.acce[2]);       
	Mpu_Data.acce_f[0] = (float)Mpu_Data.acce[0] / norm;
	Mpu_Data.acce_f[1] = (float)Mpu_Data.acce[2] / norm;
	Mpu_Data.acce_f[2] = (float)Mpu_Data.acce[1] / norm;

	// error is sum of cross product between reference direction of fields and direction measured by sensors
	ex = (Mpu_Data.acce_f[1]*vz - Mpu_Data.acce_f[2]*vy) ;                         					 //向量外积在相减得到差分就是误差
	ey = (Mpu_Data.acce_f[2]*vx - Mpu_Data.acce_f[0]*vz) ;
	ez = (Mpu_Data.acce_f[0]*vy - Mpu_Data.acce_f[1]*vx) ;

	exInt = exInt + ex *Ki *2 *half_T;								  //对误差进行积分
	eyInt = eyInt + ey *Ki *2 *half_T;
	ezInt = ezInt + ez *Ki *2 *half_T;

	// 积分限幅
	//exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	//exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	//exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );

	// adjusted gyroscope measurements
	Mpu_Data.gyro_f[0] = Mpu_Data.gyro_f[0] + Kp *(ex + exInt);					   						
	Mpu_Data.gyro_f[1] = Mpu_Data.gyro_f[1] + Kp *(ey + eyInt);				   							
	Mpu_Data.gyro_f[2] = Mpu_Data.gyro_f[2] + Kp *(ez + ezInt);					   					  							

	// integrate quaternion rate and normalise						   //四元素的微分方程
	q0 = q0 + (-q1*Mpu_Data.gyro_f[0] - q2*Mpu_Data.gyro_f[1] - q3*Mpu_Data.gyro_f[2]) *half_T;
	q1 = q1 + ( q0*Mpu_Data.gyro_f[0] + q2*Mpu_Data.gyro_f[2] - q3*Mpu_Data.gyro_f[1]) *half_T;
	q2 = q2 + ( q0*Mpu_Data.gyro_f[1] - q1*Mpu_Data.gyro_f[2] + q3*Mpu_Data.gyro_f[0]) *half_T;
	q3 = q3 + ( q0*Mpu_Data.gyro_f[2] + q1*Mpu_Data.gyro_f[1] - q2*Mpu_Data.gyro_f[0]) *half_T;


	// normalise quaternion
	norm = my_sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	Mpu_Data.yaw = fast_atan2(2*q1*q2+2*q0*q3, -2*q2*q2-2*q3*q3+1) *57.3f;
	Mpu_Data.roll = fast_atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) *57.3f;
	Mpu_Data.pitch = asin(-2*q1*q3 + 2*q0*q2) *57.3f; 	
}
}

void MPU_Calculate(void)
{
	float vx, vy, vz;//(r系到b系的第三列)

	float norm;
	float ex, ey, ez;


	//float gx = Mpu_Data.gyro_f[0];
	//float gy = Mpu_Data.gyro_f[1];
	//float gz = Mpu_Data.gyro_f[2];
	//float ax = Mpu_Data.acce_f[0];
	//float ay = Mpu_Data.acce_f[1];
	//float az = Mpu_Data.acce_f[2];

	static float q0 = 1, q1 = 0, q2 = 0, q3 = 0;     // quaternion elements representing the estimated orientation
	static float exInt = 0, eyInt = 0, ezInt = 0;    // scaled integral error
	
	//GPIOC->ODR ^= GPIO_Pin_14;
	
	Mpu_Data.acce_f[0] = (float)Mpu_Data.acce[0] * 0.001196326;
	Mpu_Data.acce_f[1] = (float)Mpu_Data.acce[1] * 0.001196326;
	Mpu_Data.acce_f[2] = (float)Mpu_Data.acce[2] * 0.001196326;
	Mpu_Data.temp = (3653+((double)Mpu_Data.temp)*0.294118);
	Mpu_Data.gyro_f[0] = (float)Mpu_Data.gyro[0] * 0.03051851;
	Mpu_Data.gyro_f[1] = (float)Mpu_Data.gyro[1] * 0.03051851;
	Mpu_Data.gyro_f[2] = (float)Mpu_Data.gyro[2] * 0.03051851;
	Mpu_Data.dps_f[0]  = Mpu_Data.gyro_f[0] * ANGLE_TO_RAD;
	Mpu_Data.dps_f[1]  = Mpu_Data.gyro_f[1] * ANGLE_TO_RAD;
	Mpu_Data.dps_f[2]  = Mpu_Data.gyro_f[2] * ANGLE_TO_RAD;
	
	//20us
	

	if(Mpu_Data.acce_f[0]!=0||Mpu_Data.acce_f[1]!=0||Mpu_Data.acce_f[2]!=0)
	{
	//acc数据归一化
	norm = my_sqrt(Mpu_Data.acce_f[0]*Mpu_Data.acce_f[0] + Mpu_Data.acce_f[1]*Mpu_Data.acce_f[1] + Mpu_Data.acce_f[2]*Mpu_Data.acce_f[2]);       
	Mpu_Data.acce_f[0] = Mpu_Data.acce_f[0] / norm;
	Mpu_Data.acce_f[1] = Mpu_Data.acce_f[1] / norm;
	Mpu_Data.acce_f[2] = Mpu_Data.acce_f[2] / norm;

	// estimated direction of gravity and flux (v and w)              估计重力方向和流量/变迁
	vx = 2*(q1*q3 - q0*q2);												//四元素中xyz的表示
	vy = 2*(q0*q1 + q2*q3);
	vz = 1 - 2*(q1*q1 + q2*q2);

	// error is sum of cross product between reference direction of fields and direction measured by sensors
	ex = (Mpu_Data.acce_f[1]*vz - Mpu_Data.acce_f[2]*vy) ;                         					 //向量外积在相减得到差分就是误差
	ey = (Mpu_Data.acce_f[2]*vx - Mpu_Data.acce_f[0]*vz) ;
	ez = (Mpu_Data.acce_f[0]*vy - Mpu_Data.acce_f[1]*vx) ;

	exInt = exInt + ex *Ki *2 *half_T;								  //对误差进行积分
	eyInt = eyInt + ey *Ki *2 *half_T;
	ezInt = ezInt + ez *Ki *2 *half_T;

	// 积分限幅
	exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );
	exInt = LIMIT(exInt, - IMU_INTEGRAL_LIM ,IMU_INTEGRAL_LIM );

	// adjusted gyroscope measurements
	Mpu_Data.gyro_f[0] = Mpu_Data.gyro_f[0] + Kp *(ex + exInt);					   						
	Mpu_Data.gyro_f[1] = Mpu_Data.gyro_f[1] + Kp *(ey + eyInt);				   							
	Mpu_Data.gyro_f[2] = Mpu_Data.gyro_f[2] + Kp *(ez + ezInt);					   					  							

	// integrate quaternion rate and normalise						   //四元素的微分方程
	q0 = q0 + (-q1*Mpu_Data.gyro_f[0] - q2*Mpu_Data.gyro_f[1] - q3*Mpu_Data.gyro_f[2]) *half_T;
	q1 = q1 + ( q0*Mpu_Data.gyro_f[0] + q2*Mpu_Data.gyro_f[2] - q3*Mpu_Data.gyro_f[1]) *half_T;
	q2 = q2 + ( q0*Mpu_Data.gyro_f[1] - q1*Mpu_Data.gyro_f[2] + q3*Mpu_Data.gyro_f[0]) *half_T;
	q3 = q3 + ( q0*Mpu_Data.gyro_f[2] + q1*Mpu_Data.gyro_f[1] - q2*Mpu_Data.gyro_f[0]) *half_T;


	// normalise quaternion
	norm = my_sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
	q0 = q0 / norm;
	q1 = q1 / norm;
	q2 = q2 / norm;
	q3 = q3 / norm;

	Mpu_Data.yaw   = fast_atan2(2*q1*q2+2*q0*q3, -2*q2*q2-2*q3*q3+1) *57.3f;
	Mpu_Data.roll  = fast_atan2(2*q2*q3 + 2*q0*q1, -2*q1*q1 - 2*q2*q2 + 1) *57.3f;
	Mpu_Data.pitch = asin(-2*q1*q3 + 2*q0*q2) *57.3f; 	
}
	
	
	//GPIOC->ODR ^= GPIO_Pin_14;
	
	//250us
	
}


u8 time = 0;
#define MAXERR 5
float norm, total[3] = {0};
void MPU_My_Calculate(void)
{
	total[0] += Mpu_Data.acce[0];
	total[1] += Mpu_Data.acce[1];
	total[2] += Mpu_Data.acce[2];
	
	if(time %10 == 0)
	{
		norm = sqrt(total[0]*total[0] + total[1]*total[1] + total[2]*total[2]);
		
		if(norm > 75000 && norm < 84000)
		{
	Mpu_Data.pitch= fast_atan2(Mpu_Data.acce[0],Mpu_Data.acce[2])/ANGLE_TO_RAD;  
	Mpu_Data.yaw  = fast_atan2(Mpu_Data.acce[0],Mpu_Data.acce[1])/ANGLE_TO_RAD;
	Mpu_Data.roll = fast_atan2(Mpu_Data.acce[2],Mpu_Data.acce[1])/ANGLE_TO_RAD;	
		}
		time = 0;
		total[0] = total[1] = total[2] = 0;
	}
time ++;
	if(1){
	Mpu_Data.pitch-=(float)Mpu_Data.gyro[1] * 0.03051851 * 0.01;
	Mpu_Data.yaw  +=(float)Mpu_Data.gyro[2] * 0.03051851 * 0.01;
  Mpu_Data.roll -=(float)Mpu_Data.gyro[0] * 0.03051851 * 0.01;//100hz;
	}
}

//acce x,y,z Temp ,Gyro x,y,z
u8 MPU_Get_Raw_Data(void)
{
	u8 MPU_reg_buf[14],i = 0;
	short *MPU_data = (void*)Mpu_Data.acce;
	I2C1_Soft_Mult_Read(MPU_ADDR,MPU_ACCEL_XOUTH_REG,MPU_reg_buf,14);
	for(;i < 7;i++)
		MPU_data[i] = ((u16)MPU_reg_buf[i*2]<<8)|MPU_reg_buf[i*2+1]; 
	Mpu_Data.gyro[0] -=77;
	Mpu_Data.gyro[1] +=20;
	Mpu_Data.gyro[2] -=8 ;
	Mpu_Data.temp = (3653+((double)Mpu_Data.temp)*0.294118);
	return 1;
}

//wait 5s to set offset
void MPU_Set_Offset_Data(void)
{
	u16 i = 0;
	float total[3] = {0};
	   for(;i<5000;i++)
	{
		delay_ms(1);
		total[0] += Mpu_Data.gyro[0];       
		total[1] += Mpu_Data.gyro[1];   
		total[2] += Mpu_Data.gyro[2];   
	}
	Mpu_Data.offset.gyro[0] = total[0]/5000;
	Mpu_Data.offset.gyro[1] = total[1]/5000;
	Mpu_Data.offset.gyro[2] = total[2]/5000;
}


