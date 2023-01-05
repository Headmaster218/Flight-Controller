#ifndef __SPI_H
#define __SPI_H

#include "sys.h"  

//SPI引脚定义
#define SPI_CLK_GPIO_PORT			GPIOB
#define SPI_CLK_GPIO_CLK			RCC_APB2Periph_GPIOB
#define SPI_CLK_GPIO_PIN			GPIO_Pin_13

#define SPI_MISO_GPIO_PORT		GPIOB
#define SPI_MISO_GPIO_CLK			RCC_APB2Periph_GPIOB
#define SPI_MISO_GPIO_PIN			GPIO_Pin_14

#define SPI_MOSI_GPIO_PORT		GPIOB
#define SPI_MOSI_GPIO_CLK			RCC_APB2Periph_GPIOB
#define SPI_MOSI_GPIO_PIN			GPIO_Pin_14

#define SPI_NSS_GPIO_PORT			GPIOB
#define SPI_NSS_GPIO_CLK			RCC_APB2Periph_GPIOB
#define SPI_NSS_GPIO_PIN			GPIO_Pin_12


#define spi_set_nss_high( )			SPI_NSS_GPIO_PORT->ODR |= SPI_NSS_GPIO_PIN								//片选置高
#define spi_set_nss_low( )			SPI_NSS_GPIO_PORT->ODR &= (uint32_t)( ~((uint32_t)SPI_NSS_GPIO_PIN ))	//片选置低

//SPI接口定义
#define SPI_PORT					SPI2				//SPI接口
#define SPI_PORT_CLK				RCC_APB1Periph_SPI2			//SPI时钟

void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length );
u8 spi_read_write_byte( uint8_t TxByte );
void SPI1_Init(void);
#endif
