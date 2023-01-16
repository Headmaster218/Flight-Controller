/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-25 14:52:32
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-14 23:41:54
 * @FilePath: \USER\main.c
 * @Description:
 *
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved.
 */
#include "stm32f10x.h"
#include "MPU6050.h"
#include "091OLED.h"
#include "delay.h"
#include "usart.h"
#include "GPS.h"
#include "24G.h"
#include "serio.h"
#include "timer.h"
#include "adc.h"
#include "led.h"
#include "sys.h"

int main(void)
{
	__set_PRIMASK(1); // Close all interrupt

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	delay_init();
	Soft_IIC1_Init();
	PWM_Init(10000, 144); // 50hz
	Adc_Init();
	ADC_DMA_Init();
	Wireless_Init();
	GPS_Init();
	TIM1_Int_Init(720, 1000);
	delay_ms(100);
	MPU_Init(1);
	// OLED_Init();
	controler_offline_cnt=51;
	GPS_Data.offline_cnt=100;
	MPU_Data.offline_cnt=100;
	__set_PRIMASK(0); // Enable all interrupt

	while (1)
	{
		delay_ms(10);
	}
}
u8 cnt = 0;
u16 tmppp;
// 100Hz
void TIM1_UP_IRQHandler(void) // TIM3中断
{
	////////////////////////串口DMA重新设置//////////////////////////////////
	if (uart_time_cnt++ > 1)
	{
		DMA1_Channel5->CCR &= 0xFE; // disable dma
		DMA1_Channel5->CNDTR = sizeof(DMA_receive_Data);
		DMA1_Channel5->CCR |= 1; // enable dma
	}

	////////////////////////掉线检测//////////////////////////////////////
	controler_offline_cnt++;
	if (controler_offline_cnt > 50)
	{
		controler_offline_cnt = 60;
		controler_offline_flag = 1;
		PCout(14) = 1; // 遥控器掉线开灯
	}
	else
	{
		controler_offline_flag = 0;
	}

	GPS_Data.offline_cnt++;
	if (GPS_Data.offline_cnt > 70)
	{
		GPS_Data.offline_cnt = 80;
		GPS_Data.offline_flag = 1;
	}
	else
	{
		GPS_Data.offline_flag = 0;
	}

	MPU_Data.offline_cnt++;
	if (MPU_Data.offline_cnt > 70)
	{
		MPU_Data.offline_cnt = 80;
		MPU_Data.offline_flag = 1;
		MPU_Init(0);
	}
	else
	{
		MPU_Data.offline_flag = 0;
	}

	////////////////////////Loop清零，1Hz//////////////////////////////////////
	if (cnt == 99)
	{
		cnt = 0;
	}
	else
		cnt++;

	////////////////////////100Hz/////////////////////////////////////////
	Get_Adc();
	if (MPU_Get_Raw_Data())
	{
		MPU_My_Calculate();
		MPU_Data.offline_cnt = 0;
	}

	/////////////////////////////////////////////////////////////////////
	if (cnt % 2) // 50Hz
	{
		PWM_Output();
	}
	if (cnt % 25 == 0) // 4Hz
	{
		tmppp++;
		Wireless_Send_Data();
	}

	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) // 检查指定的TIM中断发生与否:TIM 中断�?
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);	   // 清除TIMx的中断待处理�?:TIM 中断�?
}
