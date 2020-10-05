/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-10-05 19:57:01
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-05 20:11:18
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_Uart3.c
**ModifyRecord1:    
******************************************************************/
#include "MCU_Uart.h"
#include "MCU_Port.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"

//串口中断接收数据处理函数
static MCU_UartRecvIntProcFunc uart3RecvFuncPtr = NULL;
//串口互斥信号量
static OS_MUTEX mutexMCU_Uart3;
//循环队列
static ADT_CIRCLE_BYTE_QUEUE circleByteQueueUart3Send = {0};
//用于循环队列存储数据的数组
static uint8_t bufferWithUart3Send[LENGTH_UART3_BUFFER_WITH_SEND] = {0};
//用于printf的缓冲区
static uint8_t blockBufferWithUart3Printf[LENGTH_UART3_BUFFER_WITH_FORMAT] = {0};
//串口中断发送正在运行标志
static uint8_t flagUart3TC = 0;



//串口初始化
void MCU_Uart3Init(uint32_t baud, MCU_UART_LENGTH length, MCU_UART_STOPBIT stopBit,MCU_UART_CHECK_MODE checkMode, MCU_UART_HARD_CONTROL hardWareControl, MCU_UartRecvIntProcFunc rxCallBack)
{
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    OS_ERR err;

    ADT_CircleByteQueueInit(&circleByteQueueUart3Send, bufferWithUart3Send, LENGTH_UART3_BUFFER_WITH_SEND);

    //使能USART3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    //复位串口
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE);
    //取消复位
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE);
    //复位串口3
    USART_DeInit(USART3);  

    //USART3端口配置
    MCU_PortInit(MCU_PIN_B_10, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    MCU_PortInit(MCU_PIN_B_11, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);

    //UART3复用
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); 

    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = length;
    USART_InitStructure.USART_StopBits = stopBit;
    USART_InitStructure.USART_Parity = checkMode;
    USART_InitStructure.USART_HardwareFlowControl = hardWareControl;
    //发送接收同时打开
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    //使能串口3
    USART_Cmd(USART3, ENABLE);

    //STM32在使能串口的时候会发送一帧空闲帧作为起始,所以要等待第一帧发送完成
    USART_ClearFlag(USART3, USART_FLAG_TC);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
    {
        __asm("nop"); //等待空闲帧发送完成后  再清零发送标志
    }
    USART_ClearFlag(USART3, USART_FLAG_TC);

    //开启相关中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_TC, ENABLE);

    //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MCU_UART3_PREE_PRI;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = MCU_UART3_SUB_PRI;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //创建一个互斥信号量,用于DMA串口发送
	OSMutexCreate((OS_MUTEX*	)&mutexMCU_Uart3,
				  (CPU_CHAR*	)"mutexMCU_Uart3",
                  (OS_ERR*		)&err);	

    uart3RecvFuncPtr = rxCallBack;
    flagUart3TC = 0;
}

//串口发送数组
void MCU_Uart3SendBuffer(uint8_t* bufferStartPtr,uint16_t sendLength)
{
    uint16_t currentBufferHadSendLength = 0;
    uint16_t currentBufferWaitSendLength = sendLength;
    uint16_t realWriteDataCount = 0;
    OS_ERR err;
    uint8_t maybeNeedSendByte = 0;
    if(sendLength == 0)
    {
        return;
    }
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexMCU_Uart3, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    do
    {
        realWriteDataCount = 0;
        //循环写入
        ADT_CircleByteQueueWriteBuffer(&circleByteQueueUart3Send,(bufferStartPtr+currentBufferHadSendLength),currentBufferWaitSendLength,&realWriteDataCount);
        currentBufferHadSendLength += realWriteDataCount;
        currentBufferWaitSendLength -= realWriteDataCount;
        if(flagUart3TC == 0)
        {
            //一个字节一个字节读取
            ADT_CircleByteQueueReadBuffer(&circleByteQueueUart3Send,&maybeNeedSendByte,1,&realWriteDataCount);
            if(realWriteDataCount > 0)
            {
                USART_SendData(USART3,maybeNeedSendByte);
                flagUart3TC = 1;
            }
        }
        if(currentBufferHadSendLength != sendLength)       
        {
            CoreDelayMinTick();
        }                       
    }while(currentBufferHadSendLength != sendLength);
    //释放互斥信号量,禁止在释放信号量的时候出发任务调度
    if (OSRunning)OSMutexPost(&mutexMCU_Uart3, OS_OPT_POST_NO_SCHED, &err);
}

//串口发送字符串
void MCU_Uart3SendString(uint8_t* stringStartPtr)
{
    int stringLength = strlen((char const *)stringStartPtr);
    if (stringLength <= 0)
    {
        return;
    }
    MCU_Uart3SendBuffer(stringStartPtr, (uint16_t)stringLength);
}

//串口发送字符串,带格式化
int MCU_Uart3Printf(const char *format, ...)
{
    //转换
    va_list arg;
    int rv;
    OS_ERR err;
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexMCU_Uart3, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    va_start(arg, format);
    rv = vsnprintf((char *)blockBufferWithUart3Printf, LENGTH_UART3_BUFFER_WITH_FORMAT, format, arg);
    va_end(arg);
    if ((rv > 0) && (rv <= LENGTH_UART3_BUFFER_WITH_FORMAT))
    {
        MCU_Uart3SendBuffer(blockBufferWithUart3Printf, (uint16_t)rv);
    }
    else
    {
        //转换失败
    }
    //释放互斥信号量,禁止在释放信号量的时候出发任务调度
    if (OSRunning)OSMutexPost(&mutexMCU_Uart3, OS_OPT_POST_NO_SCHED, &err);
    return rv;
}


void USART3_IRQHandler(void)
{
    uint8_t intData;
    uint8_t readDat;
    uint16_t realReadDataCount;
    
    //中断内核感知,需要
    CPU_SR_ALLOC();
    CPU_CRITICAL_ENTER();
    //此时要禁止操作系统调度打断中断
    OSIntEnter();
    //关闭了就要打开
    CPU_CRITICAL_EXIT();

    //获取当前是否存在接收中断
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        intData = USART_ReceiveData(USART3);
        if (uart3RecvFuncPtr != NULL)
        {
            uart3RecvFuncPtr(intData);
        }
    }
    //发送完成中断
    if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)
    {
        USART_ClearFlag(USART3, USART_FLAG_TC);
        ADT_CircleByteQueueReadBuffer(&circleByteQueueUart3Send,&readDat,1,&realReadDataCount);
        if(realReadDataCount > 0)
        {
            USART_SendData(USART3,readDat);
        }
        else
        {
            flagUart3TC = 0;
        }
    }
    //退出中断,启用调度
    OSIntExit();
}  








