/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-23 09:34:57
 * @LastEditors: Headmaster1615(Mac)  e-mail:hm-218@qq.com
 * @LastEditTime: 2023-01-15 19:50:01
 * @FilePath: \USERd:\Program_Data\STM32\Flight-Controler\Plane\HARDWARE\GPS.h
 * @Description:
 *
 * Copyright (c) 2023 by Headmaster1615, All Rights Reserved.
 */
#ifndef __GPS_H
#define __GPS_H

#include "sys.h"
#include <delay.h>

extern struct GPS_Data_ GPS_Data;
void GPS_Init(void);
void GPS_DMA_Init(void);

// RMC_DATA结构体

struct GPS_Data_
{
	u8 time[3];				   // h,m,s
	float lon_f, lat_f;		   // 原始经纬度
	double lon_real, lat_real; // 实际经纬度
	double home_lon, home_lat; // 起点经纬度
	u8 num;					   // 卫星数量
	float height;
	float home_height; // 起点高度
	float speed;		// km/h
	u16 distance2home; // 米
	u8 home_point_flag; // 起点设定完成标志
	u8 no_locate_flag;	// 定位状态
	u8 offline_flag;	// 在线状态
	u8 offline_cnt;		// 在线计数
};

#endif
