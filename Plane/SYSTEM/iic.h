/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2023-01-05 12:54:14
 * @LastEditors: Headmaster1615(Mac)  e-mail:hm-218@qq.com
 * @LastEditTime: 2023-01-25 13:57:49
 * @FilePath: \USERd:\Program_Data\STM32\Flight-Controler\Plane\SYSTEM\iic.h
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
void Hard_IIC_Init(void);
void I2C1_Hard_Delay(void);
int I2C1_Hard_Start(void);
void I2C1_Hard_Stop(void);
void I2C1_Hard_Ack(void);
void I2C1_Hard_NoAck(void);
int I2C1_Hard_WaitAck(void); // ����:=1��ACK,=0��ACK
void I2C1_Hard_SendByte(u8 SendByte);
u8 I2C1_Hard_ReadByte(void);

int I2C1_Hard_Single_Write(u8 SlaveAddress, u8 REG_Address, u8 REG_data);
int I2C1_Hard_Mult_Write(u8 SlaveAddress, u8 REG_Address, u8 *ptChar, u8 size);
int I2C1_Hard_Single_Read(u8 SlaveAddress, u8 REG_Address);
int I2C1_Hard_Mult_Read(u8 SlaveAddress, u8 REG_Address, u8 *ptChar, u8 size);

void IIC2_Init(void);

#endif
