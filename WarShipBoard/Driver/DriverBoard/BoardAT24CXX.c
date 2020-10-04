/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:26:44
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-04 15:37:38
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardAT24CXX.c
**ModifyRecord1:    
******************************************************************/
#include "BoardAT24CXX.h"

//IIC互斥信号量
static OS_MUTEX mutexBoardAT24CXX;

//IIC芯片初始化
void BoardAT24CXX_PortInit(void)
{
    OS_ERR err;
    //Port初始化
    MCU_IIC_PortInit();
    //初始化互斥信号量
	OSMutexCreate((OS_MUTEX*	)&mutexBoardAT24CXX,
				  (CPU_CHAR*	)"mutexBoardAT24CXX",
                  (OS_ERR*		)&err);
}

//IIC芯片自检
D_ERROR_CODE BoardAT24CXX_SelfCheck(void)
{
    D_ERROR_CODE errorCode = D_ERROR_CODE_NONE;
    //生成一个随机地址
    uint16_t randomAddr = MCU_RandomGetNextRangeReal(0,MAX_AT_ADDR_SELF);
    //读取到的源数据
    uint8_t srcDat = 0x00;
    //首先读取源数据
    errorCode = BoardAT24CXX_ReadOneByte(randomAddr,&srcDat);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //将源数据转换一下
    uint8_t convertDat = (~srcDat);
    //写入转换数据
    errorCode = BoardAT24CXX_WriteOneByte(randomAddr,convertDat);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //读取写入的数据
    uint8_t utilDat = 0x00;
    errorCode = BoardAT24CXX_ReadOneByte(randomAddr,&utilDat);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //看看是否写进入了
    if(utilDat != convertDat)
    {
        return D_ERROR_CODE_AT24CXX_CHECK;
    }
    //将源数据写回
    errorCode = BoardAT24CXX_WriteOneByte(randomAddr,srcDat);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //读取写入的数据
    errorCode = BoardAT24CXX_ReadOneByte(randomAddr,&utilDat);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //看看是否写进入了
    if(utilDat != srcDat)
    {
        return D_ERROR_CODE_AT24CXX_CHECK;
    }
    //完成
    return D_ERROR_CODE_NONE;
}

//读取指定指定地址的一个数据
D_ERROR_CODE BoardAT24CXX_ReadOneByte(uint16_t addr, uint8_t *readDat)
{
    D_ERROR_CODE userErrorCode;
    uint8_t temp = 0;
    OS_ERR err;
    //地址检测
    if (addr > MAX_AT_ADDR_SELF)
        return D_ERROR_CODE_AT24CXX_ADDR_RANGE;
    //申请AT24CXX总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardAT24CXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_IIC, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //发送启动
    MCU_IIC_Start();
    //部分芯片需要分两次发送地址
    if (MAX_AT_ADDR_SELF > MAX_ADDR_AT24C16)
    {
        //发送写地址
        MCU_IIC_SendByte(BOARD_AT24CXX_ADDR_WRITE);
        //等待ACK
        userErrorCode = MCU_IIC_WaitAck();
        //ACK超时
        if (D_ERROR_CODE_NONE != userErrorCode)
        {
            //产生一个停止条件
            MCU_IIC_Stop();
            //释放信号量
            if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
            if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
            return userErrorCode;
        }
        //发送高地址
        MCU_IIC_SendByte(addr >> 8);
    }
    else
    {
        //发送器件地址,写数据
        MCU_IIC_SendByte(BOARD_AT24CXX_ADDR_WRITE + ((addr / 256) << 1));
    }
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送低地址
    MCU_IIC_SendByte(addr % 256);
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //读取的二次伪启动
    MCU_IIC_Start();
    //进入接收模式
    MCU_IIC_SendByte(BOARD_AT24CXX_ADDR_READ);
    //等待响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //读取值
    temp = MCU_IIC_RecvByte();
    //发送NACK
    MCU_IIC_Nack();
    //产生一个停止条件
    MCU_IIC_Stop();
    *readDat = temp;
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
    return D_ERROR_CODE_NONE;
}

//写入指定地址的一个数据
D_ERROR_CODE BoardAT24CXX_WriteOneByte(uint16_t addr, uint8_t writeDat)
{
    D_ERROR_CODE userErrorCode;
    OS_ERR err;
    //参数检查
    if (addr > MAX_AT_ADDR_SELF)
        return D_ERROR_CODE_AT24CXX_ADDR_RANGE;
    //申请IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardAT24CXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_IIC, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //发送启动
    MCU_IIC_Start();
    if (MAX_AT_ADDR_SELF > MAX_ADDR_AT24C16)
    {
        //发送写命令
        MCU_IIC_SendByte(BOARD_AT24CXX_ADDR_WRITE);
        userErrorCode = MCU_IIC_WaitAck();
        //响应超时
        if (D_ERROR_CODE_NONE != userErrorCode)
        {
            //产生一个停止条件
            MCU_IIC_Stop();
            //释放信号量
            if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
            if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
            return userErrorCode;
        }
        //发送高地址
        MCU_IIC_SendByte(addr >> 8);
    }
    else
    {
        //发送器件地址0XA0,写数据
        MCU_IIC_SendByte(BOARD_AT24CXX_ADDR_WRITE + ((addr / 256) << 1));
    }
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送低地址
    MCU_IIC_SendByte(addr % 256);
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送字节
    MCU_IIC_SendByte(writeDat);
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //产生一个停止条件
    MCU_IIC_Stop();
    //先释放IIC总线,可能别的设备也需要用
    if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
    //延时等待,两次写入之间的间隔
    CoreDelayMs(TIMS_MS_WRITE_AT24CXX_INTERVAL);
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
    return D_ERROR_CODE_NONE;
}

//读取指定区间的数据
D_ERROR_CODE BoardAT24CXX_ReadBufferLength(uint16_t startAddr, uint16_t length, uint8_t *readBuffer)
{
    OS_ERR err;
    //检查数据范围
    if ((startAddr + length) > MAX_AT_ADDR_SELF)
    {
        return D_ERROR_CODE_AT24CXX_ADDR_RANGE;
    }
    //开始读取数据,一个字节一个字节的读取
    uint16_t readIndex = 0;
    D_ERROR_CODE userErrorCode;
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardAT24CXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    for (readIndex = 0; readIndex < length; readIndex++)
    {
        userErrorCode = BoardAT24CXX_ReadOneByte(startAddr + readIndex, (uint8_t *)(readBuffer + readIndex));
        if (D_ERROR_CODE_NONE != userErrorCode)
        {
            //释放信号量
            if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
            return userErrorCode;
        }
    }
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
    //读取成功,返回完成
    return D_ERROR_CODE_NONE;
}

//写入指定区间的数据
D_ERROR_CODE BoardAT24CXX_WriteBufferLength(uint16_t startAddr, uint16_t length, uint8_t *writeBuffer)
{
    OS_ERR err;
    //检查数据范围
    if ((startAddr + length) > MAX_AT_ADDR_SELF)
    {
        return D_ERROR_CODE_AT24CXX_ADDR_RANGE;
    }
    //开始写入数据,一个字节一个字节的写入
    uint16_t writeIndex = 0;
    D_ERROR_CODE userErrorCode;
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardAT24CXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    for (writeIndex = 0; writeIndex < length; writeIndex++)
    {
        userErrorCode = BoardAT24CXX_WriteOneByte(startAddr + writeIndex, (*(writeBuffer + writeIndex)));
        if (D_ERROR_CODE_NONE != userErrorCode)
        {
            //释放信号量
            if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
            return userErrorCode;
        }
    }
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardAT24CXX, OS_OPT_POST_FIFO, &err);
    //写入完成
    return D_ERROR_CODE_NONE;
}

