#ifndef __MPU6050_H
#define __MPU6050_H

extern struct _MPU_Data MPU_Data;
#include "sys.h"
#include "iic.h"
#include "delay.h"
#include "my_math.h"

#define ANGLE_TO_RAD 0.01745329f

#define MPU_SELF_TESTX_REG 0X0D	  // 自检寄存器X
#define MPU_SELF_TESTY_REG 0X0E	  // 自检寄存器Y
#define MPU_SELF_TESTZ_REG 0X0F	  // 自检寄存器Z
#define MPU_SELF_TESTA_REG 0X10	  // 自检寄存器A
#define MPU_SAMPLE_RATE_REG 0X19  // 采样频率分频�?
#define MPU_CFG_REG 0X1A		  // 配置寄存�?
#define MPU_GYRO_CFG_REG 0X1B	  // 陀螺仪配置寄存�?
#define MPU_ACCEL_CFG_REG 0X1C	  // 加速度计配置寄存器
#define MPU_MOTION_DET_REG 0X1F	  // 运动检测阀值设置寄存器
#define MPU_FIFO_EN_REG 0X23	  // FIFO使能寄存�?
#define MPU_I2CMST_CTRL_REG 0X24  // IIC主机控制寄存�?
#define MPU_I2CSLV0_ADDR_REG 0X25 // IIC从机0器件地址寄存�?
#define MPU_I2CSLV0_REG 0X26	  // IIC从机0数据地址寄存�?
#define MPU_I2CSLV0_CTRL_REG 0X27 // IIC从机0控制寄存�?
#define MPU_I2CSLV1_ADDR_REG 0X28 // IIC从机1器件地址寄存�?
#define MPU_I2CSLV1_REG 0X29	  // IIC从机1数据地址寄存�?
#define MPU_I2CSLV1_CTRL_REG 0X2A // IIC从机1控制寄存�?
#define MPU_I2CSLV2_ADDR_REG 0X2B // IIC从机2器件地址寄存�?
#define MPU_I2CSLV2_REG 0X2C	  // IIC从机2数据地址寄存�?
#define MPU_I2CSLV2_CTRL_REG 0X2D // IIC从机2控制寄存�?
#define MPU_I2CSLV3_ADDR_REG 0X2E // IIC从机3器件地址寄存�?
#define MPU_I2CSLV3_REG 0X2F	  // IIC从机3数据地址寄存�?
#define MPU_I2CSLV3_CTRL_REG 0X30 // IIC从机3控制寄存�?
#define MPU_I2CSLV4_ADDR_REG 0X31 // IIC从机4器件地址寄存�?
#define MPU_I2CSLV4_REG 0X32	  // IIC从机4数据地址寄存�?
#define MPU_I2CSLV4_DO_REG 0X33	  // IIC从机4写数据寄存器
#define MPU_I2CSLV4_CTRL_REG 0X34 // IIC从机4控制寄存�?
#define MPU_I2CSLV4_DI_REG 0X35	  // IIC从机4读数据寄存器

#define MPU_I2CMST_STA_REG 0X36 // IIC主机状态寄存器
#define MPU_INTBP_CFG_REG 0X37	// 中断/旁路设置寄存�?
#define MPU_INT_EN_REG 0X38		// 中断使能寄存�?
#define MPU_INT_STA_REG 0X3A	// 中断状态寄存器

#define MPU_ACCEL_XOUTH_REG 0X3B // 加速度�?,X轴高8位寄存器
#define MPU_ACCEL_XOUTL_REG 0X3C // 加速度�?,X轴低8位寄存器
#define MPU_ACCEL_YOUTH_REG 0X3D // 加速度�?,Y轴高8位寄存器
#define MPU_ACCEL_YOUTL_REG 0X3E // 加速度�?,Y轴低8位寄存器
#define MPU_ACCEL_ZOUTH_REG 0X3F // 加速度�?,Z轴高8位寄存器
#define MPU_ACCEL_ZOUTL_REG 0X40 // 加速度�?,Z轴低8位寄存器

#define MPU_TEMP_OUTH_REG 0X41 // 温度值高八位寄存�?
#define MPU_TEMP_OUTL_REG 0X42 // 温度值低8位寄存器

#define MPU_GYRO_XOUTH_REG 0X43 // 陀螺仪�?,X轴高8位寄存器
#define MPU_GYRO_XOUTL_REG 0X44 // 陀螺仪�?,X轴低8位寄存器
#define MPU_GYRO_YOUTH_REG 0X45 // 陀螺仪�?,Y轴高8位寄存器
#define MPU_GYRO_YOUTL_REG 0X46 // 陀螺仪�?,Y轴低8位寄存器
#define MPU_GYRO_ZOUTH_REG 0X47 // 陀螺仪�?,Z轴高8位寄存器
#define MPU_GYRO_ZOUTL_REG 0X48 // 陀螺仪�?,Z轴低8位寄存器

#define MPU_I2CSLV0_DO_REG 0X63 // IIC从机0数据寄存�?
#define MPU_I2CSLV1_DO_REG 0X64 // IIC从机1数据寄存�?
#define MPU_I2CSLV2_DO_REG 0X65 // IIC从机2数据寄存�?
#define MPU_I2CSLV3_DO_REG 0X66 // IIC从机3数据寄存�?

#define MPU_I2CMST_DELAY_REG 0X67 // IIC主机延时管理寄存�?
#define MPU_SIGPATH_RST_REG 0X68  // 信号通道复位寄存�?
#define MPU_MDETECT_CTRL_REG 0X69 // 运动检测控制寄存器
#define MPU_USER_CTRL_REG 0X6A	  // 用户控制寄存�?
#define MPU_PWR_MGMT1_REG 0X6B	  // 电源管理寄存�?1
#define MPU_PWR_MGMT2_REG 0X6C	  // 电源管理寄存�?2
#define MPU_FIFO_CNTH_REG 0X72	  // FIFO计数寄存器高八位
#define MPU_FIFO_CNTL_REG 0X73	  // FIFO计数寄存器低八位
#define MPU_FIFO_RW_REG 0X74	  // FIFO读写寄存�?
#define MPU_DEVICE_ID_REG 0X75	  // 器件ID寄存�?

// 如果AD0�?(9�?)接地,IIC地址�?0X68(不包含最低位).
// 如果接V3.3,则IIC地址�?0X69(不包含最低位).
#define MPU_ADDR 0X68

struct _MPU_Data
{
	short acce[3];
	short temp;
	short gyro[3];
	double acce_f[3];
	double gyro_f[3];
	double yaw_raw, roll_raw, pitch_raw;
	double yaw, roll, pitch;
	struct _offset
	{
		short acce[3];
		short gyro[3];
	} offset;
	u8 offline_cnt;	// 掉线计数
	u8 offline_flag;// 掉线标志
};

u8 MPU_Init(u8);
void MPU_Calculate(void);
void MPU_Fast_Calculate(void);
void MPU_My_Calculate(void);
u8 MPU_Get_Raw_Data(void);
void MPU_Set_Offset_Data(void);

#endif
