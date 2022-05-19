#include <iic.h>
#define RCC_I2C1	     RCC_APB2Periph_GPIOB
#define I2C1_PORT      GPIOB
#define I2C1_Pin_SCL   GPIO_Pin_6
#define I2C1_Pin_SDA   GPIO_Pin_7

u8 I2C_RX_BUF[20], I2C_TX_BUF[20];

void Soft_IIC1_Init(void)
{					     
	u8 i = 0;
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		for(;i<10;i++)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_6);
			delay_us(10);
			GPIO_ResetBits(GPIOB, GPIO_Pin_6);
			delay_us(10);	
		}
		
	GPIO_DeInit(GPIOB);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable; 
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_ClockSpeed = 300000; 
	I2C_InitStructure.I2C_OwnAddress1 = 0x01;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd (I2C1,ENABLE);
		
	IIC2_DMA_Init();
}


void IIC2_DMA_Init(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
	  DMA_InitTypeDef    DMA_Initstructure;

    //时钟
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn; //嵌套通道为DMA1_Channel4_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //抢占优先级为 2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //响应优先级为 7
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能
    NVIC_Init(&NVIC_InitStructure);

    /*开启DMA时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
    DMA_DeInit(DMA1_Channel6); 
		DMA_DeInit(DMA1_Channel7); 
    /*DMA配置*///Tx
    DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&I2C1->DR);//外设地址
		DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)I2C_TX_BUF;//存储器地址
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralDST;//mem to dev
    DMA_Initstructure.DMA_BufferSize = 20;
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
    DMA_Initstructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//mem pointer ++
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;
    DMA_Initstructure.DMA_Priority = DMA_Priority_High;
    DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6,&DMA_Initstructure);
		//Rx
		DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&I2C1->DR);
    DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)I2C_TX_BUF;
    DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_Initstructure.DMA_BufferSize = 20;
    DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_Initstructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;
    DMA_Initstructure.DMA_Priority = DMA_Priority_High;
    DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7,&DMA_Initstructure);
		
		
		//DMA_Cmd(DMA1_Channel6,ENABLE);
		//DMA_Cmd(DMA1_Channel7,ENABLE);
		I2C_DMACmd(I2C1, ENABLE);
}


//0 send, 1 receive
//单字节写入
int I2C1_Soft_Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data)		
{
	u32 delay = 0;
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	I2C_Send7bitAddress(I2C1,SlaveAddress<<1,I2C_Direction_Transmitter);//okkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	I2C_SendData(I2C1,REG_Address);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	delay = 0;
	I2C_SendData(I2C1,REG_data);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	I2C_GenerateSTOP(I2C1,ENABLE);
	return SUCCESS;
	}
	
	int I2C1_Soft_Mult_Write(u8 SlaveAddress,u8 REG_Address,u8 * ptChar,u8 size)
	{
			u32 delay = 0;
		u8 i = 0;
	I2C_GenerateSTART(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	I2C_Send7bitAddress(I2C1,SlaveAddress<<1,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	I2C_SendData(I2C1,REG_Address);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	
	for(i = 0;i < size;i++)
	{
	I2C_SendData(I2C1,ptChar[i]);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
		if(delay > TIME_OUT)
		return ERROR;
		delay++;
	}
	delay = 0;
	}
	I2C_GenerateSTOP(I2C1,ENABLE);
	return SUCCESS;
}



//单字节读取
int I2C1_Soft_Single_Read(u8 SlaveAddress,u8 REG_Address)
{
	return I2C1_Soft_Mult_Read(SlaveAddress, REG_Address, &REG_Address, 1);
}

//多字节读取
	int I2C1_Soft_Mult_Read(u8 SlaveAddress,u8 REG_Address,u8 * ptChar,u8 size)
{
		uint32_t delay = 0;
	I2C_GenerateSTART(I2C1,ENABLE);//起始信号
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	I2C_Send7bitAddress(I2C1,SlaveAddress<<1,I2C_Direction_Transmitter);//发送设备地址+写信号
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	I2C_SendData(I2C1,REG_Address);//发送存储单元地址，从0开始
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING))//////////////////////////ed,ing
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}
	delay = 0;
	if(size != 1)
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	
	
	I2C_GenerateSTART(I2C1,ENABLE);//起始信号
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))
	{
	if(delay > TIME_OUT)	
		return ERROR;
	delay++;
	}
		
	delay = 0;
	I2C_Send7bitAddress(I2C1,SlaveAddress<<1,I2C_Direction_Receiver);//发送设备地址+读信号
	
		if(size == 1)
	{
		while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))//EV6
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}	
	I2C1->SR1;//EV6_1
	I2C1->SR2;
	delay = I2C_ReceiveData(I2C1);
	I2C_GenerateSTOP(I2C1,ENABLE);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED));//EV7
	return delay;
	}
	
	
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
	if(delay > TIME_OUT)
		return ERROR;
	delay++;
	}	
	

	for(;size > 0; size--) 
	{
		delay_us(30);
		*ptChar++ = I2C_ReceiveData(I2C1);
	}
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	I2C_GenerateSTOP(I2C1,ENABLE);
	Soft_IIC1_Init();
	return delay;
}	
