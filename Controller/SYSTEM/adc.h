/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-12-31 12:43:50
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-03 18:47:19
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\SYSTEM\adc.h
 * @Description: 
 * 
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved. 
 */
#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

struct ADC_Value_
{
	short offset;
    u16 percent;//0-200
    u16 ADC_raw_Value;
};

extern struct ADC_Value_ ADC_Value[8];

void Adc_Init(void);
void ADC_DMA_Init(void);
u16 Get_Adc(void);
float Get_Adc_Average(u8 ch,u8 times); 
float Get_Temperature(void);

#endif 
