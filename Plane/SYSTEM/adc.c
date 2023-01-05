/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-12-31 12:43:50
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-03 13:30:54
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\SYSTEM\adc.c
 * @Description:
 *
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved.
 */
#include "adc.h"
#include "delay.h"

// 初始化ADC
// 这里我们仅以规则通道为例
// 我们默认将开启通道0~3
void Adc_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1通道时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	// PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1); // 复位ADC1

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					// ADC工作模式:ADC1和ADC1工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						// 模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					// 模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				// ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 4;								// 顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);									// 根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

	// 规则采样通道次序与采样时间

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5);

	ADC_DMACmd(ADC1, ENABLE); // ADC命令，和DMA关联

	ADC_Cmd(ADC1, ENABLE); // 使能ADC

	ADC_ResetCalibration(ADC1); // 复位ADC校准寄存器
	while (ADC_GetResetCalibrationStatus(ADC1))
		; // 等待校准寄存器复位完成

	ADC_StartCalibration(ADC1); // 开始ADC校准
	while (ADC_GetCalibrationStatus(ADC1))
		; // 等待校准完成

	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 注释掉软件启动AD转换
	//ADC_ExternalTrigConvCmd(ADC1, ENABLE);	// 使能外部定时器触发
}

// DMA配置
u16 ADC_DMA_Value[4]; // DMA缓存区
void ADC_DMA_Init(void)
{

	DMA_InitTypeDef DMA_InitStruct;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	// DMA init;  Using DMA1 channel 1
	DMA_DeInit(DMA1_Channel1);												 // 复位DMA1的第1通道
	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;					 // DMA对应的外设基地址
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 转换结果16bits
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)ADC_DMA_Value;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;					 // DMA的转换模式是SRC模式，外设to内存
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;						 // M2M模式禁止，memory to memory
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // DMA搬运的数据16bits

	// 接收一次数据后，目标内存地址后移，用来采集多个数据的
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	// 接收一次数据后，设备地址是否后移
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// 转换模式：常用循环缓存模式。如果M2M开启了，则这个模式失效
	// 另一种是Normal模式：不循环，仅一次DMA
	 DMA_InitStruct.DMA_Mode  = DMA_Mode_Circular;
	//DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;

	DMA_InitStruct.DMA_Priority = DMA_Priority_High; // DMA优先级，高
	DMA_InitStruct.DMA_BufferSize = 4;				 // DMA缓存大小
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);

	// DMA_ClearITPendingBit(DMA1_IT_TC1);
	// DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                //开启DMA1CH1中断

	DMA_Cmd(DMA1_Channel1, ENABLE);
}

u16 adc_vol[4];
int tmpvol[4];
int nummm[4];
//需要10ms
u8 adc_times=0;
u16 Get_Adc(void)
{
	if(adc_times==10)
	{
		adc_times=0;
		adc_vol[0]=tmpvol[0]/10*3195/10000;
		adc_vol[1]=tmpvol[1]/10*nummm[1];
		tmpvol[0]=0;
		tmpvol[1]=0;
		tmpvol[2]=0;
		tmpvol[3]=0;
	}
	else
	{
		adc_times++;
		tmpvol[0]+=ADC_DMA_Value[0];
		tmpvol[1]+=ADC_DMA_Value[1];
		tmpvol[2]+=ADC_DMA_Value[2];
		tmpvol[3]+=ADC_DMA_Value[3];
	}
		return tmpvol[0]/10*3195/10000;
}


/*
// 获得ADC值
// ch:通道值 0~3
u16 Get_Adc(u8 ch)
{
	// 设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5); // ADC1,ADC通道,采样时间为239.5周期

	ADC_SoftwareStartConvCmd(ADC1, ENABLE); // 使能指定的ADC1的软件转换启动功能

	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		; // 等待转换结束

	return ADC_GetConversionValue(ADC1); // 返回最近一次ADC1规则组的转换结果
}*/
/*
float Get_Adc_Average(u8 ch, u8 times)
{
	float temp_val = 0;
	u8 t;
	for (t = 0; t < times; t++)
	{
		temp_val += Get_Adc(ch);
	}
	temp_val /= times;
	temp_val = temp_val / 4096 * 3.3;

	return temp_val;
}

float Get_Temperature(void)
{
	float temp;
	temp = Get_Adc_Average(9, 3);
	return 1.42 * temp * temp * temp - 9.299 * temp * temp + 39.23 * temp - 25.93;
}
*/