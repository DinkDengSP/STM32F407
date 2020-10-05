/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:37:07
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-05 20:52:50
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardRS485.c
**ModifyRecord1:    
******************************************************************/
#include "BoardRS485.h"

//切换到发送
#define RS485_DIR_TX()  MCU_PortWriteSingle(MCU_PIN_G_8,Bit_SET)
#define RS485_DIR_RX()  MCU_PortWriteSingle(MCU_PIN_G_8,Bit_RESET)

//串口初始化
void BoardRS485_Init(uint32_t baud, MCU_UART_LENGTH length, MCU_UART_STOPBIT stopBit,MCU_UART_CHECK_MODE checkMode, MCU_UART_HARD_CONTROL hardWareControl, MCU_UartRecvIntProcFunc rxCallBack)
{
    //IO口初始化
    MCU_PortInit(MCU_PIN_G_8, GPIO_Mode_OUT, GPIO_OType_PP,GPIO_PuPd_UP,GPIO_Speed_100MHz);
    //设定为接收形式
    RS485_DIR_TX();
    //初始化串口
    MCU_Uart2Init(baud,length,stopBit,checkMode,hardWareControl,rxCallBack);
}

//串口发送数组
void BoardRS485_SendBuffer(uint8_t* bufferStartPtr,uint16_t sendLength)
{
    //转为发送
    RS485_DIR_TX();
    //发送数据
    MCU_Uart2SendBufferWhileOver(bufferStartPtr,sendLength);
    //切换为接收
    RS485_DIR_RX();
}

//串口发送字符串
void BoardRS485_SendString(uint8_t* stringStartPtr)
{
    //转为发送
    RS485_DIR_TX();
    //发送数据
    MCU_Uart2SendStringWhileOver(stringStartPtr);
    //切换为接收
    RS485_DIR_RX();
}

