/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-25 14:52:32
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-06 00:37:35
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
u8 OLED_Buffer[8][128]=
{{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,
98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,
124,125,126,127,128},{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,
98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,
124,125,126,127,128}};
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
	/*OLED_ShowString(0,0,"   cTime:  :  :",16);
	OLED_ShowString(0,0,"   cTime:  :  :",16);
	OLED_ShowString(0,2,"H:   M  GPSx",16);*/
	__set_PRIMASK(0);//Enable all interrupt

	while(1)//没有中断时3,000,000/s  2,800,000
	{
		CPU_Free_Time++;
		OLED_Refresh(*OLED_Buffer);
		
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
