/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-25 14:52:32
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-05 23:20:21
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
	__set_PRIMASK(1);//Close all interrupt
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	delay_init();
	Soft_IIC1_Init();
	PWM_Init(10000,144);//50hz
	Adc_Init();
	ADC_DMA_Init();
	Wireless_Init();
	GPS_Init();
	TIM1_Int_Init(720,1000);
	//HardwareInit
	delay_ms(100);
	MPU_Init();
	OLED_Init();
	OLED_ShowString(0,0,"   cTime:  :  :",16);
	OLED_ShowString(0,2,"H:   M  GPSx",16);
	/*
	USART3->SR;USART3->DR;
	DMA1_Channel3->CCR &= 0xFE;//disable dma
	DMA1_Channel3->CNDTR =200;
	DMA1_Channel3->CCR |=1;//enable dma
*/

	__set_PRIMASK(0);//Enable all interrupt

	while(1)
	{
		
		
		delay_ms(10);


	}
 }
u8 cnt=0;
//100Hz
void TIM1_UP_IRQHandler(void)   //TIM3中断
{

	if(cnt==99)
		cnt=0;
	else
		cnt++;
	Get_Adc();
	MPU_Get_Raw_Data();
	MPU_My_Calculate();
	if(cnt%2)//50Hz
	{
		PWM_Output();
	}
	if(cnt%25==0)//4Hz
	{
		PCout(14)=!PCout(14);
		Wireless_Send_Data();
	}
	
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断�? 
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理�?:TIM 中断�? 
	

			OLED_ShowNum(0,0 ,Mpu_Data.temp ,4,12);
			
			OLED_ShowNum(76,0,GPS_Data.time[0],2,12);
			OLED_ShowNum(98,0,GPS_Data.time[1],2,12);
			OLED_ShowNum(116,0,GPS_Data.time[2],2,12);
				
			
			OLED_ShowNum(11,1,GPS_Data.height,5,12);
			OLED_ShowNum(90,1,GPS_Data.num,2,12);
			
			OLED_ShowFloat(0,2,GPS_Data.lon_f ,5,5,12);
			OLED_ShowFloat(0,3,GPS_Data.lat_f ,5,5,12);
			
			OLED_ShowNum(90,2,GPS_Data.speed,3,12);

}
