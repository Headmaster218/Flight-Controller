/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-11-24 22:00:53
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-04 12:50:14
 * @FilePath: \USERd:\STM32\My Project\Flight Controller\HARDWARE\091OLED.c
 * @Description: 
 * OLED library
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved. 
 */

#include <091OLED.h>
#include <OLEDFONT.h>

//u8 OLED_Buffer[8][128]={1,2,3,4,5,6,7,8,9};

//链接底层iic
/**
 * @description: link mcu's iic
 * @param {unsigned} dat
 * @param {unsigned} cmd OLED_CMD/OLED_DATA
 * @return {*}
 */
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
	if (cmd)
		I2C1_Soft_Single_Write(0x3c, 0x40, dat);
	else
		I2C1_Soft_Single_Write(0x3c, 0x00, dat);
}

/**
 * @description: link mcu's iic
 * @param {unsigned} dat
 * @param {unsigned} cmd OLED_CMD/OLED_DATA
 * @return {*}
 */
void OLED_Multi_WR_Byte(u8 *dat, unsigned cmd, u8 len)
{
	if (cmd)
		I2C1_Soft_Mult_Write(0x3c, 0x40, dat, len);
	else
		I2C1_Soft_Mult_Write(0x3c, 0x00, dat, len);
}


/**
 * @description: 内容坐标设置
 * @param {u8} x坐标
 * @param {u8} y坐标
 * @return {*}
 */
void OLED_Set_Pos(u8 x, u8 y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

/**
 * @description: cmd state of OLED
 * @param {u8} state 0/1
 * @return {*}
 */
void OLED_Display_Cmd(u8 state)
{
	OLED_WR_Byte(0X8D, OLED_CMD);			  // SET DCDC命令
	OLED_WR_Byte(0X10 + 4 * state, OLED_CMD); // DCDC ON
	OLED_WR_Byte(0XAE | state, OLED_CMD);	  // DISPLAY ON
}

void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); //设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);	  //设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);	  //设置显示位置—列高地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0, OLED_DATA);
	}
}

//在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// mode:0,反白显示;1,正常显示
// size:选择字体 16/8
/**
 * @description: 
 * @param {u8} x 0-127 pixel
 * @param {u8} y 0-7 page
 * @param {u8} chr char
 * @param {u8} Char_Size BIG_FONT/SMALL_FONT
 * @return {*}
 */
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
	u8 c = 0;
	c = chr - ' '; //得到偏移后的值
	if (x > Max_Column - 1)
	{
		x -= Max_Column;
		y = y + 1;
	}
	if (Char_Size == 16)
	{
		OLED_Set_Pos(x, y);
		OLED_Multi_WR_Byte((u8*)&F8X16[c * 16], OLED_DATA, 8);
		OLED_Set_Pos(x, y + 1);
		OLED_Multi_WR_Byte((u8*)&F8X16[c * 16 + 8], OLED_DATA, 8);
	}
	else
	{
		OLED_Set_Pos(x, y);
		OLED_Multi_WR_Byte((u8*)&F6x8[c][0], OLED_DATA, 6);
	}
}
// m^n函数
u32 oled_pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}

//显示小数
// x,y :起点坐标
// ilen :整数的位数
// flen :小数的位数
// size:字体大小16/12
void OLED_ShowFloat(u8 x, u8 y, float num, u8 ilen, u8 flen, u8 size2)
{
	OLED_ShowChar(x + size2 / 2 * ilen, y, '.', size2);
	OLED_ShowNum(x, y, num, ilen, size2);
	ilen = flen;
	while (ilen--)
		num *= 10;
	OLED_ShowNum(x + size2 / 2 * flen + 6, y, num, flen, size2);
}

//显示数字
// x,y :起点坐标
// len :数字的位数
// size:字体大小16/12
// mode:模式	0,填充模式;1,叠加模式
// num:数值(-32767~32766);
void OLED_ShowNum(u8 x, u8 y, int num, u8 len, u8 size2)
{
	u8 t, temp;
	u8 enshow = 0;
	if (num < 0)
	{
		num = -num;
		OLED_ShowChar(x, y, '-', size2);
		x += size2 / 2;
	}
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
	}
}
//显示一个字符号串
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
	u8 j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += Char_Size == 16 ? 8 : 6;
		if (x > 120)
		{
			x = 0;
			y += Char_Size == 16 ? 2 : 1;
		}
		j++;
	}
}
/*
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
	u8 j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size);
		x += Char_Size == 16 ? 8 : 6;
		if (x > 120)
		{
			x = 0;
			y += Char_Size == 16 ? 2 : 1;
		}
		j++;
	}
}*/

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(u8 pos_x, u8 pos_y, u8 pic_x, u8 pic_y, u8 BMP[])
{
	unsigned int j = 0;
	u8 x, y;
	pic_y /= 8;
	for (y = pos_y; y < pos_y + pic_y; y++)
	{
		OLED_Set_Pos(pos_x, y);
		for (x = pos_x; x < pos_x + pic_x; x++)
		{
			OLED_WR_Byte(BMP[j++], OLED_DATA);
		}
	}
}

// dir:L,R
// start:0-7
// len:0-7
// end:0-7
// interval:0-7
#define Scroll_Cmd(cmd) OLED_WR_Byte(0x2e|cmd, OLED_CMD) //启动滚动
void Start_Horizontal_Scroll(u8 dir, u8 start, u8 end, u8 interval)
{
	Scroll_Cmd(0);
	if (dir == 'R')
		OLED_WR_Byte(0x26, OLED_CMD);
	else
		OLED_WR_Byte(0x27, OLED_CMD);

	OLED_WR_Byte(0x00, OLED_CMD);
	OLED_WR_Byte(start, OLED_CMD);
	OLED_WR_Byte(interval, OLED_CMD);
	OLED_WR_Byte(end, OLED_CMD);
	OLED_WR_Byte(0x00, OLED_CMD);
	OLED_WR_Byte(0xFF, OLED_CMD);
	Scroll_Cmd(1);
}

void OLED_Set_Brightness(u8 bright)
{
	OLED_WR_Byte(0x81, OLED_CMD); //设置对比度
	OLED_WR_Byte(bright, OLED_CMD);
};

//初始化SSD1306
void OLED_Init(void)
{
	OLED_WR_Byte(0xAE | 0, OLED_CMD); // Set Display OFF

	OLED_Set_Brightness(0x2);

	OLED_WR_Byte(0xa6 | 0, OLED_CMD); // Set Normal/Inverse Display

	OLED_WR_Byte(0xB0, OLED_CMD);	  // Set Page Start Address for Page Addressing Mode
	OLED_WR_Byte(0x40, OLED_CMD);	  // Set Display Start Line
	OLED_WR_Byte(0xa0 | 1, OLED_CMD); //屏幕反向

	OLED_WR_Byte(0xC8, OLED_CMD); // C0/C8Set COM Output Scan Direction

	OLED_WR_Byte(0xd3, OLED_CMD); // Set Display Offset
	OLED_WR_Byte(0x00, OLED_CMD);

	OLED_WR_Byte(0xda, OLED_CMD); // Set COM Pins Hardware Configuration

#if RESOLUTION == 128 * 64
	OLED_WR_Byte(0x12, OLED_CMD); // 128*64
#elif RESOLUTION == 128 * 32
	OLED_WR_Byte(0x02, OLED_CMD); // 128*32
#endif

	OLED_WR_Byte(0xa8, OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0x3f, OLED_CMD);

	// Set Display Clock Divide Ratio/Oscillator Frequency
	OLED_WR_Byte(0xd5, OLED_CMD);
	OLED_WR_Byte(0xf0, OLED_CMD);

	OLED_WR_Byte(0xd9, OLED_CMD); // Set Pre-charge Period
	OLED_WR_Byte(0x22, OLED_CMD);

	OLED_WR_Byte(0xdb, OLED_CMD); // Set VCOMH Deselect Level
	OLED_WR_Byte(0x49, OLED_CMD);

	OLED_WR_Byte(0x8d, OLED_CMD);	  // 8Dh ; Charge Pump Setting
	OLED_WR_Byte(0x14, OLED_CMD);	  // 14h ; Enable Charge Pump
	OLED_WR_Byte(0xaE | 1, OLED_CMD); // AFh; Display ON
	OLED_Clear();
}
