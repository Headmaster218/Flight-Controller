/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2023-01-05 12:54:14
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-05 13:31:03
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\Controller\SYSTEM\iic.h
 * @Description:
 *
 * Copyright (c) 2023 by Headmaster1615, All Rights Reserved.
 */
#ifndef __IIC_H
#define __IIC_H

#include "sys.h"
#include <delay.h>

#define SCL1_H I2C1_PORT->BSRR = I2C1_Pin_SCL
#define SCL1_L I2C1_PORT->BRR = I2C1_Pin_SCL
#define SDA1_H I2C1_PORT->BSRR = I2C1_Pin_SDA
#define SDA1_L I2C1_PORT->BRR = I2C1_Pin_SDA
#define SDA1_read I2C1_PORT->IDR &I2C1_Pin_SDA
#define TIME_OUT 0x100

void IIC2_DMA_Init(void);
void Soft_IIC1_Init(void);
void I2C1_Soft_Delay(void);
int I2C1_Soft_Start(void);
void I2C1_Soft_Stop(void);
void I2C1_Soft_Ack(void);
void I2C1_Soft_NoAck(void);
int I2C1_Soft_WaitAck(void); // ·µ»Ø:=1ÓÐACK,=0ÎÞACK
void I2C1_Soft_SendByte(u8 SendByte);
u8 I2C1_Soft_ReadByte(void);

int I2C1_Soft_Single_Write(u8 SlaveAddress, u8 REG_Address, u8 REG_data);
int I2C1_Soft_Mult_Write(u8 SlaveAddress, u8 REG_Address, u8 *ptChar, u16 size);
int I2C1_Soft_Single_Read(u8 SlaveAddress, u8 REG_Address);
int I2C1_Soft_Mult_Read(u8 SlaveAddress, u8 REG_Address, u8 *ptChar, u16 size);

void IIC2_Init(void);

#endif
