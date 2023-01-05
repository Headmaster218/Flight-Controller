/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-12-31 12:43:50
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-05 17:35:54
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\Controller\SYSTEM\adc.c
 * @Description:
 *
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved.
 */
#include "adc.h"
#include "delay.h"
u16 ADC_DMA_Value[8]; // DMA缓存区
struct ADC_Value_  ADC_Value[8];
int tmpvol[8];
//需要10ms
u8 adc_times=0;
u16 Get_Adc(void)
{
	u8 i=0;
	for(i=0;i<8;i++)
		{
			ADC_Value[i].ADC_raw_Value=ADC_DMA_Value[i]+ADC_Value[i].offset;
			ADC_Value[i].percent=(ADC_Value[i].ADC_raw_Value*100/2048);
		}
	
	/*
	u8 i=0;
	if(adc_times==10)
	{
		adc_times=0;
		adc_vol[0]=tmpvol[0]/10;//左摇杆上下通道
		adc_vol[1]=tmpvol[1]/10;//左摇杆左右通道
		adc_vol[2]=tmpvol[2]/10;//右摇杆上下通道
		adc_vol[3]=tmpvol[3]/10;//右摇杆左右通道
		adc_vol[4]=tmpvol[4]/10;//左电位器通道
		adc_vol[5]=tmpvol[5]/10;//右电位器通道
		adc_vol[6]=tmpvol[6]/10;//电池电压通道
		for(i=0;i<8;i++)
		{
			tmpvol[i]=0;
		}
	}
	else
	{
		adc_times++;
		for(i=0;i<8;i++)
		{
			tmpvol[i]=ADC_DMA_Value[i];
		}
	}*/
		return 0;
}
int times=0;
void DMA1_Channel1_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA1_IT_TC1) == SET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);
		Get_Adc();
	}
	times++;
}


// 初始化ADC
void Adc_Init(void)//240*8Hz
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE); // 使能ADC1通道时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	// PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // 模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1); // 复位ADC1

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					// ADC工作模式:ADC1和ADC1工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;						// 模数转换工作在扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;					// 模数转换工作在连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				// ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 8;								// 顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);									// 根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

	// 规则采样通道次序与采样时间

	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_71Cycles5);

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
/*
	ADC_Value[0].offset=0;
	ADC_Value[1].offset=-4;
	ADC_Value[2].offset=70;
	ADC_Value[3].offset=-2;
	ADC_Value[4].offset=0;
	ADC_Value[5].offset=0;
	ADC_Value[6].offset=0;
	ADC_Value[7].offset=0;
	*/
}

// DMA配置

void ADC_DMA_Init(void)
{

	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
   



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

	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium; // DMA优先级，高
	DMA_InitStruct.DMA_BufferSize = 8;				 // DMA缓存大小
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
     
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
        NVIC_Init(&NVIC_InitStructure); 
	
	DMA_ClearITPendingBit(DMA1_IT_TC1);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);                //开启DMA1CH1中断

	DMA_Cmd(DMA1_Channel1, ENABLE);
}

