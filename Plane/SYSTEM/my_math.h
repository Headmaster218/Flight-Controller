/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2023-01-14 18:10:56
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-14 19:19:08
 * @FilePath: \USERd:\Program_Data\STM32\Flight-Controler\Plane\SYSTEM\my_math.h
 * @Description: 
 * 
 * Copyright (c) 2023 by Headmaster1615, All Rights Reserved. 
 */
/*****************************************************************************
File name: TDT_Alg\src\my_math.h
Description: 快速数学计�?
Author: 祖传
Version: 1.1.1.191112_alpha
Date: 19.11.12
History:
	—————————————————————————————————————————————————————————————————————————�?
	19.11.16 肖银�?-改写my_deathzoom函数-解决遥控器最大值会小于660问题
	—————————————————————————————————————————————————————————————————————————�?
	19.11.12 首次完成
	—————————————————————————————————————————————————————————————————————————�?
*****************************************************************************/
#ifndef __MY_MATH_H
#define __MY_MATH_H
#include "sys.h"
#include "math.h"

#define TAN_MAP_RES 0.003921569f /* (smallest non-zero value in table) */
#define DEG2RAD 0.017453293f
#define TAN_MAP_SIZE 256
#define MY_PPPIII 3.14159f
#define MY_PPPIII_HALF 1.570796f

float fast_atan2(float yy, float xx);
float my_pow(float a);
float my_sqrt(float number);
double mx_sin(double rad);
double my_sin(double rad);
float my_cos(double rad);
float my_deathzoom(float xx, float zoom);
float To_180_degrees(float xx);
float Math_Max(float a, float b, float c, float d, float e, float f);

#endif
