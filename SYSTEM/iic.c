#include <iic.h>
#define RCC_I2C2	     RCC_APB2Periph_GPIOB
#define I2C2_PORT      GPIOB
#define I2C2_Pin_SCL   GPIO_Pin_10
#define I2C2_Pin_SDA   GPIO_Pin_11


void Soft_IIC1_Init(void)
{					     
	u8 i = 0;
GPIO_InitTypeDef GPIO_InitStructure;
I2C_InitTypeDef I2C_InitStructure;
	
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	for(;i<10;i++)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
		delay_us(10);
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		delay_us(10);	
	}
	
GPIO_DeInit(GPIOB);
RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOB, &GPIO_InitStructure);
	
I2C_DeInit(I2C2);
I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ; 
I2C_InitStructure.I2C_Ack = I2C_Ack_Disable; 
I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
I2C_InitStructure.I2C_ClockSpeed = 100000; 
I2C_InitStructure.I2C_OwnAddress1 = 0x01;
I2C_Init(I2C2, &I2C_InitStructure);
I2C_Cmd (I2C2,ENABLE);
}


//0 send, 1 receive
//单字节写入
int I2C1_Soft_Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data)		
{
	u32 delay = 0;
I2C_GenerateSTART(I2C2,ENABLE);
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_Send7bitAddress(I2C2,SlaveAddress<<1,I2C_Direction_Transmitter);//okkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_SendData(I2C2,REG_Address);
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
delay = 0;
I2C_SendData(I2C2,REG_data);
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_GenerateSTOP(I2C2,ENABLE);
return SUCCESS;
}

int I2C2_Soft_Mult_Write(u8 SlaveAddress,u8 REG_Address,u8 * ptChar,u8 size)
{
		u32 delay = 0;
	u8 i = 0;
I2C_GenerateSTART(I2C2,ENABLE);
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_Send7bitAddress(I2C2,SlaveAddress<<1,I2C_Direction_Transmitter);
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_SendData(I2C2,REG_Address);
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;

for(i = 0;i < size;i++)
{
 I2C_SendData(I2C2,ptChar[i]);
 while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
 {
  if(delay > 0x10000)
   return ERROR;
  delay++;
 }
 delay = 0;
}
I2C_GenerateSTOP(I2C2,ENABLE);
return SUCCESS;
}


//单字节读取
int I2C1_Soft_Single_Read(u8 SlaveAddress,u8 REG_Address)
{   
uint32_t delay = 0;
I2C_GenerateSTART(I2C2,ENABLE);//起始信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_Send7bitAddress(I2C2,SlaveAddress<<1,I2C_Direction_Transmitter);//发送设备地址+写信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_SendData(I2C2,REG_Address);//发送存储单元地址，从0开始
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING))//////////////////////////ed,ing
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_GenerateSTART(I2C2,ENABLE);//起始信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))//May be can delete
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_Send7bitAddress(I2C2,SlaveAddress<<1,I2C_Direction_Receiver);//发送设备地址+读信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}	
delay = 0;
//while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED))
//{
// if(delay > 0x10000)
//  return ERROR;
// delay++;
//}
//while((I2C2->CR2 & 0X40 ));
delay_us(100);
delay = I2C_ReceiveData(I2C2);
I2C_GenerateSTOP(I2C2,ENABLE);
	return delay;
}

//多字节读取
	int I2C1_Soft_Mult_Read(u8 SlaveAddress,u8 REG_Address,u8 * ptChar,u8 size)
{
		uint32_t delay = 0;
I2C_GenerateSTART(I2C2,ENABLE);//起始信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_Send7bitAddress(I2C2,SlaveAddress<<1,I2C_Direction_Transmitter);//发送设备地址+写信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_SendData(I2C2,REG_Address);//发送存储单元地址，从0开始
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING))//////////////////////////ed,ing
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}
delay = 0;
I2C_AcknowledgeConfig(I2C2, ENABLE);
I2C_GenerateSTART(I2C2,ENABLE);//起始信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))
{
 if(delay > 0x10000)	
  return ERROR;
 delay++;
}
  
delay = 0;
I2C_Send7bitAddress(I2C2,SlaveAddress<<1,I2C_Direction_Receiver);//发送设备地址+读信号
while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
{
 if(delay > 0x10000)
  return ERROR;
 delay++;
}	

 for(;size > 0; size--) *ptChar++ = I2C_ReceiveData(I2C2);
I2C_AcknowledgeConfig(I2C2,DISABLE);
I2C_GenerateSTOP(I2C2,ENABLE);
return delay;
}	
