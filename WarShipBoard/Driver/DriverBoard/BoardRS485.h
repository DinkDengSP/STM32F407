/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:37:16
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-05 20:47:34
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardRS485.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_RS485_H_
#define __BOARD_RS485_H_
#include "MCU_Inc.h"

//串口初始化
void BoardRS485_Init(uint32_t baud, MCU_UART_LENGTH length, MCU_UART_STOPBIT stopBit,MCU_UART_CHECK_MODE checkMode, MCU_UART_HARD_CONTROL hardWareControl, MCU_UartRecvIntProcFunc rxCallBack);

//串口发送数组
void BoardRS485_SendBuffer(uint8_t* bufferStartPtr,uint16_t sendLength);

//串口发送字符串
void BoardRS485_SendString(uint8_t* stringStartPtr);

#endif



