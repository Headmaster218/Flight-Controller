#include <GPS.h>
#include <stdlib.h>
#include <091OLED.h>

struct GGA_DATA gga_data;
extern int CPU_frec_tick, CPU_freq;

u8  USART_RX_BUF[200];
void GPS_DMA_Init(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
	  DMA_InitTypeDef    DMA_Initstructure;

    //时钟
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn; //嵌套通道为DMA1_Channel4_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级为 2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //响应优先级为 7
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能
    NVIC_Init(&NVIC_InitStructure);

    /*开启DMA时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
    DMA_DeInit(DMA1_Channel3); 
    /*DMA配置*/
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART3->DR);
    DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)USART_RX_BUF;
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralSRC;//dev to mem
    DMA_Initstructure.DMA_BufferSize = 200;
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_Initstructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;
    DMA_Initstructure.DMA_Priority = DMA_Priority_High;
    DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3,&DMA_Initstructure);
		
    //开启DMA发送发成中断
		USART_Cmd(USART3,DISABLE);
		DMA_ClearFlag(DMA1_FLAG_TC5);
		DMA_ClearFlag(DMA1_FLAG_HT5);
		
		DMA_Cmd(DMA1_Channel3,ENABLE);
    USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);
		USART_Cmd(USART3,ENABLE);
}



u8 USART_RX_STA=0, temppointer = 0, flag_OLED_refresh = 0;

//IN 30 US
void USART3_IRQHandler(void)   //空闲中断
{
	if(DMA1_Channel3->CNDTR< 25) 
	{
		USART3->SR;USART3->DR;
	DMA1_Channel3->CCR &= 0xFE;//disable dma
	DMA1_Channel3->CNDTR =200;
	DMA1_Channel3->CCR |=1;//enable dma
		return;
	}
	


	GPIOC->ODR ^= GPIO_Pin_13;
			//$GPVTG,,T,,M,0.029,N,0.001,K,D*2C
//		//$GPGGA,060826.00,2236.91284,N,11403.24705,E,2,08,1.03,107.8,M,-2.4,M,,0000*4A
//		//01234567890123456789012345678901234567890123456789012345678901234567890123456
//GPS数据处理
	temppointer = 0;
	
	 	while(USART_RX_BUF[temppointer++]!='N');
		gga_data.speed = atoi(USART_RX_BUF+temppointer+1);	
		while(USART_RX_BUF[temppointer++]!='$' && temppointer < 199);
	  if(temppointer == 199) return;
	
		gga_data.time[0] = (8+(USART_RX_BUF[temppointer+6]-0x30)*10+(USART_RX_BUF[temppointer+7]-0x30));
		gga_data.time[0] -= gga_data.time[0] >=24?24:0;
		gga_data.time[1] = (USART_RX_BUF[temppointer+8]-0x30)*10+(USART_RX_BUF[temppointer+9]-0x30);
		gga_data.time[2] = (USART_RX_BUF[temppointer+10]-0x30)*10+(USART_RX_BUF[temppointer+11]-0x30);
		
			temppointer+=16;
		flag_OLED_refresh = 0;
			gga_data.lon_f = atof(USART_RX_BUF + temppointer);
		flag_OLED_refresh = 0;
		while(USART_RX_BUF[temppointer++]!=',');
		while(USART_RX_BUF[temppointer++]!=',');
			gga_data.lat_f = atof(USART_RX_BUF + temppointer);
		while(USART_RX_BUF[temppointer++]!=',');
		while(USART_RX_BUF[temppointer++]!=',');
		while(USART_RX_BUF[temppointer++]!=',');
		gga_data.num = atoi(USART_RX_BUF+temppointer);

		while(USART_RX_BUF[temppointer++]!=',');
		while(USART_RX_BUF[temppointer++]!=',');
		gga_data.height = atoi(USART_RX_BUF +temppointer);
	
	//restart DMA
	DMA1_Channel3->CCR &= 0xFE;//disable dma
	DMA1_Channel3->CNDTR =200;
	DMA1_Channel3->CCR |=1;//enable dma
 
	flag_OLED_refresh = 1;
	CPU_freq = 3*CPU_frec_tick;
	CPU_frec_tick = 0;
	USART3->SR;USART3->DR;
}

