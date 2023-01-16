/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-12-30 22:09:25
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-05 15:07:08
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\Plane\HARDWARE\serio.c
 * @Description:
 *
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved.
 */
#include "serio.h"
#include "24G.h"
#include "sys.h"
#include "GPS.h"
#include "MPU6050.h"

struct serio_Data_ serio_Data;

void PWM_Output()
{ // 25-125
	// 50-150 250-750
	if((receive_Data.bits&4 && !MPU_Data.offline_flag)||controler_offline_flag)
	{//Auto Fly
		
		if (GPS_Data.speed > 30 && !GPS_Data.no_locate_flag)
		{
			if(controler_offline_flag)
			{
				serio_Data.pwm_output[0] = (short)LIMIT(((short)60/*0-200*/-(short)GPS_Data.speed/*km/h*/)*20,0,500);
			}
			else
			{
				serio_Data.pwm_output[0] = (short)LIMIT(((short)receive_Data.acc/*0-200*/-(short)GPS_Data.speed/*km/h*/)*20,0,500);
			}
		}
		else if(!controler_offline_flag)
			serio_Data.pwm_output[0] = receive_Data.acc * 2.5;
		else
			serio_Data.pwm_output[0] = 0;

		
		serio_Data.pwm_output[1] = (short)MPU_Data.roll*1.5;
		serio_Data.pwm_output[2] = (short)MPU_Data.roll*1.5;//+-150
		serio_Data.pwm_output[3] = -(short)MPU_Data.pitch*1.5;
		serio_Data.pwm_output[4] = 100;
	}
	else
	{
		serio_Data.pwm_output[0] = receive_Data.acc * 2.5;
		serio_Data.pwm_output[1] = LIMIT((((short)receive_Data.LR - 100) * -1.5 - ((short)receive_Data.flap - 100)), -150, 150); //+-150
		serio_Data.pwm_output[2] = LIMIT((((short)receive_Data.LR - 100) * -1.5 + ((short)receive_Data.flap - 100)), -150, 150); //+-150
		serio_Data.pwm_output[3] = ((short)receive_Data.UD - 100) * 1.5;
		serio_Data.pwm_output[4] = ((short)receive_Data.HLR - 100) * 1.5;
	}

	// 电调，左翼，右翼，平尾，垂尾
	TIM_SetCompare2(TIM2, serio_Data.pwm_output_offset[0] + serio_Data.pwm_output[0]);
	TIM_SetCompare4(TIM3, serio_Data.pwm_output_offset[1] + serio_Data.pwm_output[1]);
	TIM_SetCompare3(TIM3, serio_Data.pwm_output_offset[2] + serio_Data.pwm_output[2]);
	TIM_SetCompare1(TIM3, serio_Data.pwm_output_offset[3] + serio_Data.pwm_output[3]);
	TIM_SetCompare1(TIM2, serio_Data.pwm_output_offset[4] + serio_Data.pwm_output[4]);
}

// TIM3 PWM部分初始化
// PWM输出初始化
// arr：自动重装值
// psc：时钟预分频数
void PWM_Init(u16 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM2, ENABLE);						   // 使能定时器3时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能GPIO外设和AFIO复用功能模块时钟

	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE); // Timer3部分重映射  TIM3_CH2->PB5
	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	// 设置该引脚为复用输出功能,输出TIM3 CH2的PWM脉冲波形	GPIOB.5

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3; // TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;												  // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			// 初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;		// TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIO

	// 初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr;						// 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					// 设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				// 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				// 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	// TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	// NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	// NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	// 初始化TIM3 Channel2 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			  // 选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;	  // 输出极性:TIM输出比较极性高

	TIM_OC1Init(TIM3, &TIM_OCInitStructure); // 根据T指定的参数初始化外设TIM3 OC2
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // 使能TIM3在CCR2上的预装载寄存器
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

	serio_Data.pwm_output_offset[0] = 500;
	serio_Data.pwm_output_offset[1] = 750 + 40;
	serio_Data.pwm_output_offset[2] = 750 - 60;
	serio_Data.pwm_output_offset[3] = 750 + 25;
	serio_Data.pwm_output_offset[4] = 750 + 40;

	serio_Data.pwm_output[0] = 0;
	serio_Data.pwm_output[1] = 0;
	serio_Data.pwm_output[2] = 0;
	serio_Data.pwm_output[3] = 0;
	serio_Data.pwm_output[4] = 0;

	TIM_SetCompare2(TIM2, serio_Data.pwm_output_offset[0] + serio_Data.pwm_output[0]);
	TIM_SetCompare4(TIM3, serio_Data.pwm_output_offset[1] + serio_Data.pwm_output[1]);
	TIM_SetCompare3(TIM3, serio_Data.pwm_output_offset[2] + serio_Data.pwm_output[2]);
	TIM_SetCompare1(TIM3, serio_Data.pwm_output_offset[3] + serio_Data.pwm_output[3]);
	TIM_SetCompare1(TIM2, serio_Data.pwm_output_offset[4] + serio_Data.pwm_output[4]);

	TIM_Cmd(TIM3, ENABLE); // 使能TIM3
	TIM_Cmd(TIM2, ENABLE); // 使能TIM3
}
