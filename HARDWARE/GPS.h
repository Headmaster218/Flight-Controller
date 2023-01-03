/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-23 09:34:57
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-03 18:44:46
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\HARDWARE\GPS.h
 * @Description: 
 * 
 * Copyright (c) 2023 by Headmaster1615, All Rights Reserved. 
 */
#ifndef __GPS_H
#define __GPS_H

#include "sys.h"
#include <delay.h>

extern struct GGA_DATA gga_data;
void GPS_Init(void);
void GPS_DMA_Init(void);

//RMC_DATA结构体

struct GGA_DATA
{
	u8 time[3]; //h,m,s
	char ng[13],lat[13];  //经纬度
	float lon_f, lat_f;
	u8 num;	//卫星数量
	short height;
	short speed;//km/h
};



#endif
