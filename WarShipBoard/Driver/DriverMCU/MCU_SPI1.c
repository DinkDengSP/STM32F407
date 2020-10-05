/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:26:59
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-05 10:48:36
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_SPI1.c
**ModifyRecord1:    
**ModifyRecord2:    
**ModifyRecord3:    
**ModifyRecord4:    
**ModifyRecord5:    
******************************************************************/
#include "MCU_SPI.h"
#include "MCU_Port.h"

/**系统内置SPI初始化特性 */
static SPI_InitTypeDef  SPI1_InitStructure;
//SPI模块已经初始化的标识
static uint8_t spi1InitFlag = 0;
//SPI模块的互斥信号量
OS_MUTEX mutexMCU_SPI1;

//SPI 初始化
void MCU_SPI1_Init(MCU_SPI_SPEED speed,MCU_SPI_CPOL cpol,MCU_SPI_CPHA cpha)
{
    OS_ERR err;
    GPIO_InitTypeDef  GPIO_InitStructure;
    if(spi1InitFlag == 0)
    {
        //SPI信号量初始化
        OSMutexCreate((OS_MUTEX*	)&mutexMCU_SPI1,
                    (CPU_CHAR*	)"mutexMCU_SPI1",
                    (OS_ERR*		)&err);
                    
        //使能SPI1时钟
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

        //IO口初始化
        //PB3 SCK
        MCU_PortInit(MCU_PIN_B_3, GPIO_Mode_AF, GPIO_OType_PP,GPIO_PuPd_UP,GPIO_Speed_100MHz);
        //PB4 MISO
        MCU_PortInit(MCU_PIN_B_4, GPIO_Mode_AF, GPIO_OType_PP,GPIO_PuPd_UP,GPIO_Speed_100MHz);
        //PB5 MOSI
        MCU_PortInit(MCU_PIN_B_5, GPIO_Mode_AF, GPIO_OType_PP,GPIO_PuPd_UP,GPIO_Speed_100MHz);

        //复用初始化
        GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);

        //复位
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);
        //取消复位
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);
        //禁止SPI外设
        SPI_Cmd(SPI1, DISABLE);

        //初始化SPI模块
        //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
        SPI1_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        //设置SPI工作模式:设置为主SPI
        SPI1_InitStructure.SPI_Mode = SPI_Mode_Master;
        //设置SPI的数据大小:SPI发送接收8位帧结构
        SPI1_InitStructure.SPI_DataSize = SPI_DataSize_8b;
        //串行同步时钟的空闲状态为高电平
        SPI1_InitStructure.SPI_CPOL = cpol;
        //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
        SPI1_InitStructure.SPI_CPHA = cpha;
        //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
        SPI1_InitStructure.SPI_NSS = SPI_NSS_Soft;
        //定义波特率预分频的值:波特率预分频值为256
        SPI1_InitStructure.SPI_BaudRatePrescaler = speed;
        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
        SPI1_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
        //CRC值计算的多项式
        SPI1_InitStructure.SPI_CRCPolynomial = 7;
        //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
        SPI_Init(SPI1, &SPI1_InitStructure);

        //使能SPI外设
        SPI_Cmd(SPI1, ENABLE);
        //执行一次传输启动
        MCU_SPI1_WriteRead(0xff);
    }
    //设置标志
    spi1InitFlag = 1;
}

//读写数据
uint8_t MCU_SPI1_WriteRead(uint8_t writeDat)
{
    uint16_t waitCount = 0;
    //等待发送区空
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	//通过外设SPIx发送一个byte数据
	SPI_I2S_SendData(SPI1, writeDat);
	//等待接收完一个byte
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
    {
        waitCount++;
        if(waitCount > MCU_SPI_WAIT_COUNT_MAX)
        {
            //超时退出
            return 0;
        }
    }
	//返回通过SPIx最近接收的数据
	return SPI_I2S_ReceiveData(SPI1);
}

//设置速度
void MCU_SPI1_SetSpeed(MCU_SPI_SPEED speed)
{
    //禁止SPI外设
	SPI_Cmd(SPI1, DISABLE);
    //定义波特率预分频的值:波特率预分频值为256
	SPI1_InitStructure.SPI_BaudRatePrescaler = speed;
    //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Init(SPI1, &SPI1_InitStructure);
    //使能SPI1
	SPI_Cmd(SPI1,ENABLE);
}

//设置信号极性
void MCU_SPI1_SetPolarity(MCU_SPI_CPOL cpol,MCU_SPI_CPHA cpha)
{
    //禁止SPI外设
	SPI_Cmd(SPI1, DISABLE);
    //串行同步时钟的空闲状态为高电平
	SPI1_InitStructure.SPI_CPOL = cpol;
	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI1_InitStructure.SPI_CPHA = cpha;
    //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Init(SPI1, &SPI1_InitStructure);
    //使能SPI1
	SPI_Cmd(SPI1,ENABLE);
}



