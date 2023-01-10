/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-17 00:21:42
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-05 15:12:49
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\Plane\HARDWARE\24G.h
 * @Description: 
 * 
 * Copyright (c) 2023 by Headmaster1615, All Rights Reserved. 
 */
#ifndef __24G_H
#define __24G_H
#include "delay.h"

extern struct send_data_ send_Data;
extern struct receive_data_ receive_Data,DMA_receive_Data;
extern u8 uart_time_cnt,controler_offline_cnt,controler_offline_flag;
//frequency 4Hz
struct send_data_//16 Byte
{
	u8 ECC_Code;//sum
    u8 height;// /10
    u8 spd;//km/h

    u8 voltage;//(V-10.6)*50(percent)
    u8 temperature;//(C+100)*2

    
    u8 reserved[3];

    short latitude;//(degree*100)
    short longitude;//(degree*100)
	short pitch;//*100
    short roll;//*100
	short end_of_this;//=0xFFFF
};

//frequency 20Hz
struct receive_data_//10 Byte
{u8 ECC_Code;//sum
    u8 acc;//engine 0-200
    u8 LR;//left-right 0-200
    u8 UD;//up-down 0-200
    u8 HLR;//horizontal wing left-right 0-200
    
	u8 flap;//ΩÛ“Ì
	u8 bits;//0-7 0:LED
    u8 reserved[1];
	short end_of_this;//=0xFFFF
};

void Wireless_Init(void);
void Wireless_UART_Init(u32);
void Wireless_DMA_Init(void);
void Wireless_Send_Data(void);


#endif
