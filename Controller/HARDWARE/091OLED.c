/*
 * @Author: Headmaster1615  e-mail:hm-218@qq.com
 * @Date: 2022-11-24 22:00:53
 * @LastEditors: error: git config user.name && git config user.email & please set dead value or install git
 * @LastEditTime: 2023-01-15 15:59:03
 * @FilePath: \USERd:\Program_Data\STM32\Flight-Controler\Controller\HARDWARE\091OLED.c
 * @Description:
 * OLED library
 * Copyright (c) 2022 by Headmaster1615, All Rights Reserved.
 */

#include <091OLED.h>
#include <OLEDFONT.h>

// 链接底层iic
/**
 * @description: link mcu's iic
 * @param {unsigned} dat
 * @param {unsigned} cmd OLED_CMD/OLED_DATA
 * @return {*}
 */
void OLED_WR_Byte(u8 dat, u8 cmd)
{
	OLED_Multi_WR_Byte(&dat, cmd, 1);
}

/**
 * @description: link mcu's iic
 * @param {unsigned} dat
 * @param {unsigned} cmd OLED_CMD/OLED_DATA
 * @return {*}
 */
void OLED_Multi_WR_Byte(u8 *dat, u8 cmd, u16 len)
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
	u8 dat[3];
	dat[0] = 0xb0 + y;
	dat[1] = ((x & 0xf0) >> 4) | 0x10;
	dat[2] = (x & 0x0f);

	OLED_Multi_WR_Byte(dat, OLED_CMD, 3);
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
/**
 * @description: 		OLED清屏
 * @return {*}
 */
void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);	  // 设置显示位置—列低地址
		OLED_WR_Byte(0x10, OLED_CMD);	  // 设置显示位置—列高地址
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0, OLED_DATA);
	}
}

/**
 * @description:	显示一个字符
 * @param {u8} x	起点x坐标	0-127
 * @param {u8} y 	起点y坐标	0-63
 * @param {u8} chr	字符
 * @param {u8} Char_Size BIG_FONT/SMALL_FONT
 * @return {*}
 */
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
	u8 c = 0;
	c = chr - ' '; // 得到偏移后的值
	if (x > Max_Column - 1)
	{
		x -= Max_Column;
		y = y + 1;
	}
	if (Char_Size == 16)
	{
		OLED_Set_Pos(x, y);
		OLED_Multi_WR_Byte((u8 *)&F8X16[c * 16], OLED_DATA, 8);
		OLED_Set_Pos(x, y + 1);
		OLED_Multi_WR_Byte((u8 *)&F8X16[c * 16 + 8], OLED_DATA, 8);
	}
	else
	{
		OLED_Set_Pos(x, y);
		OLED_Multi_WR_Byte((u8 *)&F6x8[c][0], OLED_DATA, 6);
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

/**
 * @description: 		显示小数(负数时丢失一位小数精度)
 * @param {u8} x		起点x坐标	0-127
 * @param {u8} y		起点y坐标	0-7
 * @param {float} num	要显示的数
 * @param {u8} ilen		整数位数
 * @param {u8} flen		小数位数
 * @param {u8} size2	字体大小 BIG_FONT/SMALL_FONT
 * @return {*}
 */
void OLED_ShowFloat(u8 x, u8 y, float num, u8 ilen, u8 flen, u8 size2)
{
	u8 i;
	size2 = size2 == 16 ? 16 : 12;
	if(num < 0)
	{
		num = -num;
		OLED_ShowChar(x, y, '-', size2);
		flen--;
		x += size2 / 2;
	}
	OLED_ShowChar(x + size2 / 2 * ilen, y, '.', size2);
	OLED_ShowNum(x, y, num, ilen, size2);
	i = flen;
	while (i--)
		num *= 10;
	OLED_ShowNum(x + size2 / 2 * ilen + size2 / 4, y, num, flen, size2);
}


/**
 * @description:		显示数字
 * @param {u8} x		起点x坐标	0-127
 * @param {u8} y		起点y页坐标	0-7
 * @param {int} num		数字		(int)
 * @param {u8} len		数字位数
 * @param {u8} size2	字体大小	BIG_FONT/SMALL_FONT
 * @return {*}
 */
void OLED_ShowNum(u8 x, u8 y, int num, u8 len, u8 size2)
{
	u8 t, temp;
	u8 enshow = 0;
	if (num < 0)
	{
		num = -num;
		OLED_ShowChar(x, y, '-', size2);
		x += size2 / 2;
		len--;
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

/**
 * @description: 			显示字符串
 * @param {u8} x			起始坐标	0-127 pixel
 * @param {u8} y			起始页		0-7 page
 * @param {u8} *chr			字符串
 * @param {u8} Char_Size	字号		BIG_FONT/SMALL_FONT
 * @return {*}
 */
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

/**
 * @description: 			显示BMP图片
 * @param {u8} pos_x		起始点坐标x		0～127	
 * @param {u8} pos_y		起始点页坐标y		0～7
 * @param {u8} pic_x		图片宽度		0～127
 * @param {u8} pic_y		图片高度		0～63
 * @param {u8} BMP			图片首地址
 * @return {*}
 */
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

/**
 * @description:	全屏幕刷新
 * @param {u8} BMP	缓冲区
 * @return {*}
 */
void OLED_Refresh(u8 BMP[]) // 38fps
{
	u8 dat[3]={0xb0,0,0x10};
	
	OLED_Multi_WR_Byte(dat, OLED_CMD, 3);
	OLED_Multi_WR_Byte(BMP, OLED_DATA, 1024);
}

/**
 * @description: 
 * @return {*}
 */
#define Scroll_Cmd(cmd) OLED_WR_Byte(0x2e | cmd, OLED_CMD) // 启动滚动

/**
 * @description: 		屏幕内容垂直滚动
 * @param {u8} dir		方向		'L' or 'R'
 * @param {u8} start	起始页		0-7
 * @param {u8} end		结束页		0-7
 * @param {u8} interval	滚动速度	0-7
 * @return {*}
 */
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

/**
 * @description: 		控制屏幕亮度
 * @param {u8} bright	亮度值	0-255
 * @return {*}
 */
void OLED_Set_Brightness(u8 bright)
{
	OLED_WR_Byte(0x81, OLED_CMD); // 设置对比度
	OLED_WR_Byte(bright, OLED_CMD);
};

// 初始化SSD1306
void OLED_Init(void)
{
	OLED_WR_Byte(0xAE | 0, OLED_CMD); // Set Display OFF

	OLED_Set_Brightness(0x50);

	OLED_WR_Byte(0xa6 | 0, OLED_CMD); // Set Normal/Inverse Display

	OLED_WR_Byte(0xB0, OLED_CMD);	  // Set Page Start Address for Page Addressing Mode
	OLED_WR_Byte(0x00, OLED_CMD);
	OLED_WR_Byte(0x10, OLED_CMD);
	
	//OLED_WR_Byte(0x40, OLED_CMD);	  // Set Display Start Line
	OLED_WR_Byte(0xa0 | 1, OLED_CMD); // 屏幕反向

	OLED_WR_Byte(0xC8, OLED_CMD); // C0/C8Set COM Output Scan Direction

	OLED_WR_Byte(0xd3, OLED_CMD); // Set Display Offset
	OLED_WR_Byte(0x00, OLED_CMD);

	OLED_WR_Byte(0xda, OLED_CMD); // Set COM Pins Hardware Configuration

#if RESOLUTION == 128 * 64
	OLED_WR_Byte(0x12, OLED_CMD); // 128*64
#elif RESOLUTION == 128 * 32
	OLED_WR_Byte(0x02, OLED_CMD); // 128*32
#endif

	OLED_WR_Byte(0xa8, OLED_CMD); // 设置驱动路数
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
