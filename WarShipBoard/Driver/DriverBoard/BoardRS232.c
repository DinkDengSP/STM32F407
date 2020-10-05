/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:36:49
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-05 20:41:51
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardRS232.c
**ModifyRecord1:    
******************************************************************/
#include "BoardRS232.h"

//串口初始化
void BoardRS_Init(RS232_NAME rsPort,uint32_t baud, MCU_UART_LENGTH length, MCU_UART_STOPBIT stopBit,MCU_UART_CHECK_MODE checkMode, MCU_UART_HARD_CONTROL hardWareControl, MCU_UartRecvIntProcFunc rxCallBack)
{
    if(rsPort == RS232_COM2)
    {
        MCU_Uart2Init(baud,length,stopBit,checkMode,hardWareControl,rxCallBack);
    }
    else
    {
        MCU_Uart3Init(baud,length,stopBit,checkMode,hardWareControl,rxCallBack);
    }
}

//串口发送数组
void BoardRS_SendBuffer(RS232_NAME rsPort,uint8_t* bufferStartPtr,uint16_t sendLength)
{
    if(rsPort == RS232_COM2)
    {
        MCU_Uart2SendBuffer(bufferStartPtr,sendLength);
    }
    else
    {
        MCU_Uart3SendBuffer(bufferStartPtr,sendLength);
    }
}

//串口发送字符串
void BoardRS_SendString(RS232_NAME rsPort,uint8_t* stringStartPtr)
{
    if(rsPort == RS232_COM2)
    {
        MCU_Uart2SendString(stringStartPtr);
    }
    else
    {
        MCU_Uart3SendString(stringStartPtr);
    }
}


