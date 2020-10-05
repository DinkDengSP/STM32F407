/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:21:10
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-05 15:37:08
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_Uart.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_UART1_H_
#define __MCU_UART1_H_
#include "MCU_Common.h"

//变参宏定义,调试printf
#define ServicePrintf(format,args...)            MCU_Uart1Printf(format,##args)	

//串口接收中断处理函数
typedef void (*MCU_UartRecvIntProcFunc)(uint8_t dat);

/*********************************串口相关枚举定义**************************************/
//字长
typedef enum MCU_UART_LENGTH
{
    MCU_UART_LENGTH8 = USART_WordLength_8b,
    MCU_UART_LENGTH9 = USART_WordLength_9b,
} MCU_UART_LENGTH;

//停止位
typedef enum MCU_UART_STOPBIT
{
    MCU_UART_STOPBIT0_5 = USART_StopBits_0_5,
    MCU_UART_STOPBIT1 = USART_StopBits_1,
    MCU_UART_STOPBIT1_5 = USART_StopBits_1_5,
    MCU_UART_STOPBIT2 = USART_StopBits_2,
} MCU_UART_STOPBIT;

//校验位
typedef enum MCU_UART_CHECK_MODE
{
    MCU_UART_CHECK_MODE_NONE = USART_Parity_No,
    MCU_UART_CHECK_MODE_EVEN = USART_Parity_Even,
    MCU_UART_CHECK_MODE_ODD = USART_Parity_Odd,
} MCU_UART_CHECK_MODE;

//硬件流控制
typedef enum MCU_UART_HARD_CONTROL
{
    MCU_UART_HARD_CONTROL_NONE = USART_HardwareFlowControl_None,
    MCU_UART_HARD_CONTROL_RTS = USART_HardwareFlowControl_RTS,
    MCU_UART_HARD_CONTROL_CTS = USART_HardwareFlowControl_CTS,
    MCU_UART_HARD_CONTROL_RTS_CTS = USART_HardwareFlowControl_RTS_CTS,
} MCU_UART_HARD_CONTROL;

/**************************************串口1************************************************/
//发送数据的串口缓冲区的长度
#define LENGTH_UART1_BUFFER_WITH_SEND       1024
//使用DMA单块发送数据,单块的长度
#define LENGTH_UART1_BUFFER_WITH_BLOCK      256
//串口用于printf的缓冲区的长度
#define LENGTH_UART1_BUFFER_WITH_FORMAT     256

//串口初始化
void MCU_Uart1Init(uint32_t baud, MCU_UART_LENGTH length, MCU_UART_STOPBIT stopBit,MCU_UART_CHECK_MODE checkMode, MCU_UART_HARD_CONTROL hardWareControl, MCU_UartRecvIntProcFunc rxCallBack);
//串口发送数组
void MCU_Uart1SendBuffer(uint8_t* bufferStartPtr,uint16_t sendLength);
//串口发送字符串
void MCU_Uart1SendString(uint8_t* stringStartPtr);
//串口发送字符串,带格式化
int MCU_Uart1Printf(const char *format, ...);



#endif




