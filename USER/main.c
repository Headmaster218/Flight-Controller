/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-25 14:52:32
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2022-12-31 13:49:48
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
#include "spi.h"
#include "24G.h"
#include "serio.h"
#include "timer.h"
#include "adc.h"


short MPU_data[7],temp,a[3];

extern u8 USART_RX_BUF[200], flag_OLED_refresh;
extern struct GGA_DATA gga_data;
extern struct _Mpu_Data Mpu_Data;

int CPU_frec_tick = 0, CPU_freq = 0;
u16 pwm=75;
int voltage,tmpvol,tmp=32;
int main(void)
{
	__set_PRIMASK(1);//Close all interrupt
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//TIM1_Int_Init(720,1000);
	delay_init();
	Soft_IIC1_Init();
	//UART_GPS_Init(9600); //GPS
	//GPS_DMA_Init();
	//HardwareInit
	delay_ms(300);
	//PWM_Init(1000,1440);//50hz
	//Adc_Init();
	//ADC_DMA_Init();
	//RF24L01_Init();
	MPU_Init();
	OLED_Init();
	OLED_ShowString(0,0,"   cTime:  :  :",12);
	OLED_ShowString(0,1,"H:   M  GPSx",12);
	USART3->SR;USART3->DR;
	DMA1_Channel3->CCR &= 0xFE;//disable dma
	DMA1_Channel3->CNDTR =200;
	DMA1_Channel3->CCR |=1;//enable dma

		__set_PRIMASK(0);//Enable all interrupt
	//
	while(1)
	{
		
		Get_Bat_Vol();
		delay_ms(10);
		MPU_Get_Raw_Data();
		//MPU_Set_Offset_Data();
		//CPU_frec_tick++;
		TIM2->CCR1 = pwm;//25-125
		TIM2->CCR2 = pwm;
		TIM3->CCR1 = pwm;
		TIM3->CCR3 = pwm;
		TIM3->CCR4 = pwm;

		//OLED_refresh,3hz
	}
 }

 float abc = 100;
//100Hz
//定时�?3中断服务程序
void TIM1_UP_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断�? 
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理�?:TIM 中断�? 
	
	MPU_Get_Raw_Data();
	MPU_My_Calculate();
			if(flag_OLED_refresh)
		{
			flag_OLED_refresh = 0;
			OLED_ShowNum(0,0 ,Mpu_Data.temp ,4,12);
			
			OLED_ShowNum(76,0,gga_data.time[0],2,12);
			OLED_ShowNum(98,0,gga_data.time[1],2,12);
			OLED_ShowNum(116,0,gga_data.time[2],2,12);
				
			
			OLED_ShowNum(11,1,gga_data.height,5,12);
			OLED_ShowNum(90,1,gga_data.num,2,12);
			
			OLED_ShowFloat(0,2,gga_data.lon_f ,5,5,12);
			OLED_ShowFloat(0,3,gga_data.lat_f ,5,5,12);
			
			OLED_ShowNum(90,2,gga_data.speed,3,12);
			
			OLED_ShowNum(90,3,CPU_freq, 6,12);
	}
}

