/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:36:57
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-05 20:46:59
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardRS232.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_RS232_H_
#define __BOARD_RS232_H_
#include "MCU_Inc.h"

typedef enum RS232_NAME
{
    RS232_COM2      = 0X01,//UART2
    RS232_COM3      = 0X02,//UART3
}RS232_NAME;

//串口初始化
void BoardRS232_Init(RS232_NAME rsPort,uint32_t baud, MCU_UART_LENGTH length, MCU_UART_STOPBIT stopBit,MCU_UART_CHECK_MODE checkMode, MCU_UART_HARD_CONTROL hardWareControl, MCU_UartRecvIntProcFunc rxCallBack);

//串口发送数组
void BoardRS232_SendBuffer(RS232_NAME rsPort,uint8_t* bufferStartPtr,uint16_t sendLength);

//串口发送字符串
void BoardRS232_SendString(RS232_NAME rsPort,uint8_t* stringStartPtr);


#endif



