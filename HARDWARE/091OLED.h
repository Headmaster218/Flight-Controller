#ifndef __091OLED_H
#define __091OLED_H

#include <delay.h>
#include <iic.h>

#define RESOLUTION 128*64
#if RESOLUTION == 128*64
#define Max_Row 64
#elif RESOLUTION == 128*32
#define Max_Row 32
#endif

#define BIG_FONT 16
#define SMALL_FONT 8
#define Max_Column 128
#define OLED_CMD 0  //写命令
#define OLED_DATA 1 //写数据
// OLED控制用函数
#define Scroll_Cmd(cmd) OLED_WR_Byte(0x2e|cmd, OLED_CMD) //启动滚动
#define OLED_ShowChinese(x, y, no) OLED_DrawBMP(x, y, 16, 16, no);
void OLED_WR_Byte(unsigned dat, unsigned cmd);
void OLED_Display_Cmd(u8 state);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x, u8 y, u8 t);
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size);
void OLED_ShowNum(u8 x, u8 y, int num, u8 len, u8 size);
void OLED_ShowFloat(u8 x, u8 y, float num, u8 ilen, u8 flen, u8 size2);
void OLED_ShowString(u8 x, u8 y, u8 *p, u8 Char_Size);
void OLED_Set_Pos(u8 x, u8 y);
void OLED_DrawBMP(u8 pos_x, u8 pos_y, u8 pic_x, u8 pic_y, u8 BMP[]);
void Start_Horizontal_Scroll(u8 dir, u8 start, u8 end, u8 interval);
void OLED_Set_Brightness(u8 bright);


#endif
