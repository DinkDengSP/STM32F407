/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:35:40
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 16:35:17
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_IIC.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_IIC_H_
#define __MCU_IIC_H_
#include "MCU_Common.h"

//IIC总线访问需要获取的信号量
extern OS_MUTEX mutexMCU_IIC;

//IO口初始化
void MCU_IIC_PortInit(void);

//发送Start信号
void MCU_IIC_Start(void);

//发送停止信号
void MCU_IIC_Stop(void);

//发送一个字节的数据
void MCU_IIC_SendByte(uint8_t byteValue);

//读取一个字节的数据
uint8_t MCU_IIC_RecvByte(void);

//发送ACK信息
void MCU_IIC_Ack(void);

//发送Nack信息
void MCU_IIC_Nack(void);

//等待ACK信息返回
D_ERROR_CODE MCU_IIC_WaitAck(void);


#endif




