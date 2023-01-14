#include "timer.h"

void TIM1_Int_Init(u16 arr, u16 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); // 时钟使能

  TIM_TimeBaseStructure.TIM_Period = arr;                     // 设置自动重装载寄存器周期值
  TIM_TimeBaseStructure.TIM_Prescaler = (psc - 1);            // 设置预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;                // 设置时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            // 重复计数设置
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);             // 参数初始化
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);                       // 清中断标志位

  TIM_ITConfig(           // 使能或者失能指定的TIM中断
      TIM1,               // TIM1
      TIM_IT_Update |     // TIM 更新中断源
          TIM_IT_Trigger, // TIM 触发中断源
      ENABLE              // 使能
  );

  // 设置优先级
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // 先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 从优先级0级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM1, ENABLE); // 使能TIMx外设
}
