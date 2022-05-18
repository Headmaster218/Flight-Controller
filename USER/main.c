#include "stm32f10x.h"
#include <MPU6050.h>
#include <091OLED.h>
#include <delay.h>
#include <usart.h>
#include <GPS.h>
#include <spi.h>
#include <24G.h>




short MPU_data[7],temp,a[3];

extern u8 USART_RX_BUF[200], flag_OLED_refresh;
extern struct GGA_DATA gga_data;

int CPU_frec_tick = 0, CPU_freq = 0;

int main(void)
{
	__set_PRIMASK(1);//Close all interrupt
		if(1){
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//��ʹ������IO PORTBʱ�� 
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	 // �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��GPIO 
		}
	//SystemInit
	delay_init();
	Soft_IIC1_Init();
	SPI1_Init();
	uart_init(9600); //GPS
	//HardwareInit
	delay_ms(300);
	RF24L01_Init();
	MPU_Init();
	GPS_DMA_Init();
	OLED_Init();
	OLED_ShowString(0,0,"   cTime:  :  :",12);
	OLED_ShowString(0,1,"H:   M  GPSx",12);
	USART3->SR;USART3->DR;
	DMA1_Channel3->CCR &= 0xFE;//disable dma
	DMA1_Channel3->CNDTR =200;
	DMA1_Channel3->CCR |=1;//enable dma

		__set_PRIMASK(0);//Enable all interrupt
	while(1)
	{
		CPU_frec_tick++;
		//NRF24L01_Read_Reg(0x05);
		
		//OLED_refresh,3hz
		if(flag_OLED_refresh)
		{
			MPU_Get_Raw_Data(MPU_data);
			flag_OLED_refresh = 0;
			OLED_ShowNum(0,0 ,MPU_data[3],4,12);
			
			OLED_ShowNum(76,0,gga_data.time[0],2,12);
			OLED_ShowNum(98,0,gga_data.time[1],2,12);
			OLED_ShowNum(116,0,gga_data.time[2],2,12);
				
			
			OLED_ShowNum(11,1,gga_data.height,5,12);
			OLED_ShowNum(90,1,gga_data.num,2,12);
			
			OLED_ShowFloat(0,2,gga_data.lon_f ,5,5,12);
			OLED_ShowFloat(0,3,gga_data.lat_f ,5,5,12);
			
			OLED_ShowNum(90,2,gga_data.speed,3,12);
			
			OLED_ShowNum(90,3,CPU_freq, 6,12);
	}
	
	
	
	
	
	}
 }
