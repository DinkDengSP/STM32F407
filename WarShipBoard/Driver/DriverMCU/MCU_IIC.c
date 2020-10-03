/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:35:34
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-03 16:41:14
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_IIC.c
**ModifyRecord1:    
**ModifyRecord2:    
**ModifyRecord3:    
**ModifyRecord4:    
**ModifyRecord5:    
******************************************************************/
#include "MCU_IIC.h"
#include "MCU_Port.h"

//IIC总线访问需要获取的信号量
OS_MUTEX mutexMCU_IIC;

//IO口方向控制
//SDA切换为输出模式
#define SDA_DIR_OUT()   GPIOB->MODER &= ~(3 << (9 * 2));GPIOB->MODER |= 1 << 9 * 2
//SDA切换为输入模式
#define SDA_DIR_IN()    GPIOB->MODER &= ~(3 << (9 * 2));GPIOB->MODER |= 0 << 9 * 2
//SDA输出高电平
#define SDA_OUT_HIGH()  MCU_PortWriteSingle(MCU_PIN_B_9, Bit_SET)
//SDA输出低电平
#define SDA_OUT_LOW()   MCU_PortWriteSingle(MCU_PIN_B_9, Bit_RESET)
//SCL输出高电平
#define SCL_OUT_HIGH()  MCU_PortWriteSingle(MCU_PIN_B_8, Bit_SET)
//SCL输出低电平
#define SCL_OUT_LOW()   MCU_PortWriteSingle(MCU_PIN_B_8, Bit_RESET)
//读取IO口状态
#define SDA_IN_READ()   ((uint8_t)(MCU_PortReadSingle(MCU_PIN_B_9)))

//IO口初始化
void MCU_IIC_PortInit(void)
{
    OS_ERR err;
    //PB8 PB9均设置为输出 初始化设置位高电平
    //SCL
    MCU_PortInit(MCU_PIN_B_8, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Fast_Speed);
    MCU_PortWriteSingle(MCU_PIN_B_8, Bit_SET);
    //SDA
    MCU_PortInit(MCU_PIN_B_9, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Fast_Speed);
    MCU_PortWriteSingle(MCU_PIN_B_9, Bit_SET);
    //IIC信号量初始化
	OSMutexCreate((OS_MUTEX*	)&mutexMCU_IIC,
				  (CPU_CHAR*	)"mutexMCU_IIC",
                  (OS_ERR*		)&err);
}

//发送Start信号
void MCU_IIC_Start(void)
{
    //SDA转换为输出
    SDA_DIR_OUT();
    //SDA高电平,SCL高电平
    SDA_OUT_HIGH();
    SCL_OUT_HIGH();
    //延时指定时间
    CoreDelayUs(4);
    //START:when CLK is high,DATA change form high to low
    //SDA转换为低电平
    SDA_OUT_LOW();
    CoreDelayUs(4);
    //SCL钳制总线,准备发送或者接收数据
    SCL_OUT_LOW();
}

//发送停止信号
void MCU_IIC_Stop(void)
{
    //SDA转换为输出
    SDA_DIR_OUT();
    //SDA高电平,SCL高电平
    SCL_OUT_LOW();
    SDA_OUT_LOW();
    //延时指定时间
    CoreDelayUs(4);
    //STOP:when CLK is high DATA change form low to high
    SCL_OUT_HIGH();
    SDA_OUT_HIGH();
    CoreDelayUs(4);
}

//发送一个字节的数据
void MCU_IIC_SendByte(uint8_t byteValue)
{
    uint8_t sendByteUtil;
    //SDA转换为输出
    SDA_DIR_OUT();
    //拉低时钟开始数据传输
    SCL_OUT_LOW();
    for (sendByteUtil = 0; sendByteUtil < 8; sendByteUtil++)
    {
        //输出一个bit,高位优先MSB
        if(((byteValue&0x80)>>7) != 0)SDA_OUT_HIGH();
        else SDA_OUT_LOW();
        //数据移动
        byteValue <<= 1;
        //对TEA5767这三个延时都是必须的
        CoreDelayUs(2);
        SCL_OUT_HIGH();
        CoreDelayUs(2);
        SCL_OUT_LOW();
        CoreDelayUs(2);
    }
}

//读取一个字节的数据
uint8_t MCU_IIC_RecvByte(void)
{
    uint8_t indexUtil = 0;
    uint8_t receiveDat = 0;
    //SDA设置为输入
    SDA_DIR_IN();
    for(indexUtil=0;indexUtil<8;indexUtil++ )
	{
        //拉低时钟开始数据传输
        SCL_OUT_LOW();
        CoreDelayUs(2);
        //释放SCL,数据放到总线上
		SCL_OUT_HIGH();
        receiveDat <<= 1;
        //读取数据
        if(SDA_IN_READ())receiveDat++;   
		CoreDelayUs(1); 
    }	
    //返回数据
    return receiveDat;
}

//发送ACK信息
void MCU_IIC_Ack(void)
{
    SCL_OUT_LOW();
    SDA_DIR_OUT();
    //SDA低电平的时候
    SDA_OUT_LOW();
    //SCL从高电平到低电平
    CoreDelayUs(2);
    SCL_OUT_HIGH();
    CoreDelayUs(2);
    SCL_OUT_LOW();
}

//发送Nack信息
void MCU_IIC_Nack(void)
{
    SCL_OUT_LOW();
    SDA_DIR_OUT();
    //SDA高电平的时候
    SDA_OUT_HIGH();
    //SCL从高电平到低电平
    CoreDelayUs(2);
    SCL_OUT_HIGH();
    CoreDelayUs(2);
    SCL_OUT_LOW();
}

//等待ACK信息返回
D_ERROR_CODE MCU_IIC_WaitAck(void)
{
    uint8_t ucErrTime = 0;
    //SDA设置为输入
    SDA_DIR_IN();
    //SDA高电平,SCL高电平
    SDA_OUT_HIGH();
    CoreDelayUs(1);
    SCL_OUT_HIGH();
    CoreDelayUs(1);
    //等待SDA变成低电平
    while (SDA_IN_READ())
    {
        ucErrTime++;
        if (ucErrTime > 250)
        {
            //时间超时,返回超时
            MCU_IIC_Stop();
            return D_ERROR_CODE_IIC_ACK_TIME_OUT;
        }
    }
    //时钟输出0
    SCL_OUT_LOW();
    return D_ERROR_CODE_NONE;
}



