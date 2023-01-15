/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-05-17 00:21:42
 * @LastEditors: Headmaster1615(Mac)  e-mail:hm-218@qq.com
 * @LastEditTime: 2023-01-15 20:02:38
 * @FilePath: \USERd:\Program_Data\STM32\Flight-Controler\Controller\HARDWARE\24G.h
 * @Description:
 *
 * Copyright (c) 2023 by Headmaster1615, All Rights Reserved.
 */
#ifndef __24G_H
#define __24G_H
#include "delay.h"

extern u8 uart_time_cnt;
extern struct send_data_ send_Data;
extern struct receive_data_ receive_Data, DMA_receive_Data;

// frequency 4Hz
struct receive_data_ // 18 Byte
{
    u8 ECC_Code;    // sum
    u8 height;      // /10

    u8 spd;         // km/h
    u8 voltage;     //(V-10.6)*50(percent)

    u8 temperature; // C=(tem-100)/3
    u8 bits; // 0-7 0:GPS Offlie flag 1:GPS locate state 2:MPU6050 Offlie flag
    
    u16 distance;    // M
    short latitude;    //(degree*100)
    short longitude;   //(degree*100)
    short pitch;       //*100
    short roll;        //*100
    short end_of_this; //=0xFFFF
};

// frequency 20Hz
struct send_data_ // 10 Byte
{
    u8 ECC_Code; // sum
    u8 acc;      // engine 0-200
    u8 LR;       // left-right 0-200
    u8 UD;       // up-down 0-200
    u8 HLR;      // horizontal wing right-left 0-200

    u8 flap; // ΩÛ“Ì
    u8 bits; // 0-7 0:LED line	1:Auto Back	2:Auto Fly	3:
    u8 reserved[1];
    short end_of_this; //=0xFFFF
};

void OLED_Receive_Refresh(void);
void Wireless_Init(void);
void Wireless_UART_Init(u32);
void Wireless_DMA_Init(void);
void Wireless_Send_Data(void);

#endif
