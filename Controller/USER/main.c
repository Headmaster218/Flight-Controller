/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-25 14:52:32
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-05 13:41:16
 * @FilePath: \USER\main.c
 * @Description: 
 * 
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved. 
 */
#include "stm32f10x.h"
#include "MPU6050.h"
#include "091OLED.h"
#include "delay.h"
#include "24G.h"
#include "serio.h"
#include "timer.h"
#include "adc.h"

unsigned long CPU_Free_Time=0;

int main(void)
{
	__set_PRIMASK(1);//Close all interrupt
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	Soft_IIC1_Init();
	//PWM_Init(10000,144);//50hz
	Adc_Init();
	ADC_DMA_Init();
	Wireless_Init();
	TIM1_Int_Init(720,1000);
	//HardwareInit
	delay_ms(100);
	//MPU_Init();
	OLED_Init();
	OLED_ShowString(0,0,"   cTime:  :  :",16);
	OLED_ShowString(0,2,"H:   M  GPSx",16);
	__set_PRIMASK(0);//Enable all interrupt

	while(1)//没有中断时3,000,000/s  2,800,000
	{
		CPU_Free_Time++;
	}
 }

 extern int times;
 int freq;
	u8 cnt=0;
//100Hz
void TIM1_UP_IRQHandler(void)   //TIM3中断
{
	freq=times*10;
	times=0;

	if(cnt==100)
		cnt=0;
	else
		cnt++;
	MPU_Get_Raw_Data();
	MPU_My_Calculate();
	if(cnt%2)//50Hz
	{
		PWM_Output();
	}
	if(cnt%25==0)//4Hz
	{
		Wireless_Send_Data();
	}
	
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断�? 
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理�?:TIM 中断�? 

}
