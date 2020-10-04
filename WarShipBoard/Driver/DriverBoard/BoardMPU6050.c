/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:35:58
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-05 00:53:48
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardMPU6050.c
**ModifyRecord1:    
******************************************************************/
#include "BoardMPU6050.h"

//IIC互斥信号量
static OS_MUTEX mutexBoardMPU6050;

//初始化MPU6050
void BoardMPU6050_Init(void)
{
    OS_ERR err;
    //Port初始化
    MCU_IIC_PortInit();
    //初始化互斥信号量
	OSMutexCreate((OS_MUTEX*	)&mutexBoardMPU6050,
				  (CPU_CHAR*	)"mutexBoardMPU6050",
                  (OS_ERR*		)&err);
}

//配置,自检MCU6050
D_ERROR_CODE BoardMPU6050_ConfigAndCheck(void)
{
    uint8_t deviceID = 0x00;
    D_ERROR_CODE errorCode = D_ERROR_CODE_NONE;
    //配置初始化
    //延时等待复位完成
    CoreDelayMs(20);
    //复位MPU6050
    errorCode = BoardMPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X80);	
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //延时等待复位完成
    CoreDelayMs(100);
    //唤醒MPU6050
	errorCode = BoardMPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X00);	 
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //陀螺仪传感器,±2000dps
	errorCode = BoardMPU6050_SetGyroscopeFullScaleRange(MPU6050_GYRO_DPS_2000);	
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;	
    //加速度传感器,±2g			
	errorCode = BoardMPU6050_SetAccelerometerFullScaleRange(MPU6050_ACCEL_G_2);	
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //设置采样率50Hz				
	errorCode = BoardMPU6050_SetSampleRate(50);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //关闭所有中断						
	errorCode = BoardMPU6050_WriteByte(MPU_INT_EN_REG,0X00);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //I2C主模式关闭	
	errorCode = BoardMPU6050_WriteByte(MPU_USER_CTRL_REG,0X00);	
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //关闭FIFO
	errorCode = BoardMPU6050_WriteByte(MPU_FIFO_EN_REG,0X00);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //INT引脚低电平有效	
	errorCode = BoardMPU6050_WriteByte(MPU_INTBP_CFG_REG,0X80);	
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //获取芯片ID
	errorCode = BoardMPU6050_ReadByte(MPU_DEVICE_ID_REG,&deviceID);
    if(errorCode != D_ERROR_CODE_NONE)return errorCode;
    //器件ID正确
    if(deviceID == BOARD_MPU_ID_CHECK)
	{
        //设置CLKSEL,PLL X轴为参考
		errorCode = BoardMPU6050_WriteByte(MPU_PWR_MGMT1_REG,0X01);	
        if(errorCode != D_ERROR_CODE_NONE)return errorCode;
        //加速度与陀螺仪都工作
		errorCode = BoardMPU6050_WriteByte(MPU_PWR_MGMT2_REG,0X00);	
        if(errorCode != D_ERROR_CODE_NONE)return errorCode;
        //设置采样率为50Hz
		errorCode = BoardMPU6050_SetSampleRate(50);		
        if(errorCode != D_ERROR_CODE_NONE)return errorCode;				
    }
    else
    {
        return D_ERROR_CODE_MPU6050_ID_CHECK;
    }
    return errorCode;
}

//IIC写一个字节
D_ERROR_CODE BoardMPU6050_WriteByte(uint8_t regAddr,uint8_t byteWrite)
{
    D_ERROR_CODE userErrorCode;
    OS_ERR err;
    //申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardMPU6050, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_IIC, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //发送启动
    MCU_IIC_Start();
    //发送设备写入地址
    MCU_IIC_SendByte(BOARD_MPU_ADDR_WRITE);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送寄存器地址
    MCU_IIC_SendByte(regAddr);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送数据
    MCU_IIC_SendByte(byteWrite);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //产生一个停止条件
    MCU_IIC_Stop();
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
	return D_ERROR_CODE_NONE;
}

//IIC读一个字节			
D_ERROR_CODE BoardMPU6050_ReadByte(uint8_t regAddr,uint8_t* byteRead)
{
    D_ERROR_CODE userErrorCode;
    OS_ERR err;
    uint8_t temp = 0;
    //申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardMPU6050, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_IIC, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //发送启动
    MCU_IIC_Start();
    //发送设备写入地址
    MCU_IIC_SendByte(BOARD_MPU_ADDR_WRITE);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送寄存器地址
    MCU_IIC_SendByte(regAddr);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //重新启动
    MCU_IIC_Start();
    //发送读地址
    MCU_IIC_SendByte(BOARD_MPU_ADDR_READ);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //读取值
    temp = MCU_IIC_RecvByte();
    //发送NACK
    MCU_IIC_Nack();
    //产生一个停止条件
    MCU_IIC_Stop();
    *byteRead = temp;
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
    return D_ERROR_CODE_NONE;	
}

//IIC连续写							
D_ERROR_CODE BoardMPU6050_WriteBuffer(uint8_t regAddr,uint8_t* bufWrite,uint8_t dataLength)
{
    D_ERROR_CODE userErrorCode;
    OS_ERR err;
    //申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardMPU6050, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_IIC, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //发送启动
    MCU_IIC_Start();
    //发送设备写入地址
    MCU_IIC_SendByte(BOARD_MPU_ADDR_WRITE);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送寄存器地址
    MCU_IIC_SendByte(regAddr);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    for(uint8_t writeIndexUtil = 0; writeIndexUtil < dataLength; writeIndexUtil++)
    {
        //发送数据
        MCU_IIC_SendByte(bufWrite[writeIndexUtil]);
        //等待ACK响应
        userErrorCode = MCU_IIC_WaitAck();
        //响应超时
        if (D_ERROR_CODE_NONE != userErrorCode)
        {
            //产生一个停止条件
            MCU_IIC_Stop();
            //释放信号量
            if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
            if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
            return userErrorCode;
        }
    }
    //数据发送完成
    //产生一个停止条件
    MCU_IIC_Stop();
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
	return D_ERROR_CODE_NONE;
}

//IIC连续读 
D_ERROR_CODE BoardMPU6050_ReadBuffer(uint8_t regAddr,uint8_t* bufRead,uint8_t dataLength)
{
    D_ERROR_CODE userErrorCode;
    OS_ERR err;
    //申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutexBoardMPU6050, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_IIC, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //发送启动
    MCU_IIC_Start();
    //发送设备写入地址
    MCU_IIC_SendByte(BOARD_MPU_ADDR_WRITE);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //发送寄存器地址
    MCU_IIC_SendByte(regAddr);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //重新启动
    MCU_IIC_Start();
    //发送读地址
    MCU_IIC_SendByte(BOARD_MPU_ADDR_READ);
    //等待ACK响应
    userErrorCode = MCU_IIC_WaitAck();
    //响应超时
    if (D_ERROR_CODE_NONE != userErrorCode)
    {
        //产生一个停止条件
        MCU_IIC_Stop();
        //释放信号量
        if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
        if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
        return userErrorCode;
    }
    //循环读取指定长度数据
    while(dataLength != 0)
    {
        //读取值
        *bufRead = MCU_IIC_RecvByte();
        //判定是不是要读取的最后一个数据
        if(dataLength == 1)
        {
            //发送NACK
            MCU_IIC_Nack();
        }
        else
        {
            //发送ACK
            MCU_IIC_Ack();
        }
        bufRead++;
        dataLength--;
    }
    //产生一个停止条件
    MCU_IIC_Stop();
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexBoardMPU6050, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_IIC, OS_OPT_POST_FIFO, &err);
    return D_ERROR_CODE_NONE;	
}

//MPU6050设置陀螺仪满量程
D_ERROR_CODE BoardMPU6050_SetGyroscopeFullScaleRange(MPU6050_GYRO_RANGE range)
{
    uint8_t rangeLocal = range;
    //寄存器偏移
    rangeLocal <<= 3;
    //设置陀螺仪满量程范围
    return BoardMPU6050_WriteByte(MPU_GYRO_CFG_REG,rangeLocal); 
}

//MPU6050设置加速度传感器满量程
D_ERROR_CODE BoardMPU6050_SetAccelerometerFullScaleRange(MPU6050_ACCEL_RANGE range)
{
    uint8_t rangeLocal = range;
    //寄存器偏移
    rangeLocal <<= 3;
    //设置加速度传感器满量程范围
    return BoardMPU6050_WriteByte(MPU_ACCEL_CFG_REG,rangeLocal);  
}

//设置低通滤波器截止频率
D_ERROR_CODE BoardMPU6050_SetLPF_CutOffFreq(MPU6050_LPF_FREQ cutOffFreq)
{
    //设置数字低通滤波器  
    return BoardMPU6050_WriteByte(MPU_CFG_REG,(uint8_t)cutOffFreq);
}

//设置采样频率
D_ERROR_CODE BoardMPU6050_SetSampleRate(uint16_t sampleRate)
{
    uint8_t writeData = 0;
    D_ERROR_CODE errorCode = D_ERROR_CODE_NONE;
    //限制采样率在指定范围内
    if(sampleRate > 1000)sampleRate = 1000;
	if(sampleRate < 4)sampleRate = 4;
    //通过采样率计算出值
    writeData = (1000/sampleRate)-1;
    //写入采样率
    errorCode = BoardMPU6050_WriteByte(MPU_SAMPLE_RATE_REG,writeData);
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    //设置LPF为采样率的一半以下
    MPU6050_LPF_FREQ cutOffFreq;
    sampleRate /= 2;
    if(sampleRate >= 188)
        cutOffFreq = MPU6050_LPF_FREQ_HZ_188;
    else if(sampleRate >= 98)
        cutOffFreq = MPU6050_LPF_FREQ_HZ_98;
    else if(sampleRate >= 42)
        cutOffFreq = MPU6050_LPF_FREQ_HZ_42;
    else if(sampleRate >= 20)
        cutOffFreq = MPU6050_LPF_FREQ_HZ_20;
    else if(sampleRate >= 10)
        cutOffFreq = MPU6050_LPF_FREQ_HZ_10;
    else 
        cutOffFreq = MPU6050_LPF_FREQ_HZ_5;
    //设置滤波器
    errorCode = BoardMPU6050_SetLPF_CutOffFreq(cutOffFreq);
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    return errorCode;
}

//读取当前温度
D_ERROR_CODE BoardMPU6050_ReadCurrentTemperatureDat(float* tempValue)
{
    uint8_t readBuf[2]; 
    D_ERROR_CODE errorCode = D_ERROR_CODE_NONE;
    short srcDat;
    //读取数据
	errorCode = BoardMPU6050_ReadBuffer(MPU_TEMP_OUTH_REG,readBuf,2); 
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    //转换成温度
    srcDat = (short)((((uint16_t)readBuf[0])<<8)|readBuf[1]);  
    *tempValue = (float)(36.53+(((double)srcDat)/340.0));  
    return errorCode;
}

//获取陀螺仪当前原始数据
D_ERROR_CODE BoardMPU6050_ReadCurrentGyroscopeDat(short *gx,short *gy,short *gz)
{
    uint8_t readBuf[6];  
    D_ERROR_CODE errorCode = D_ERROR_CODE_NONE;
    //读取数据
	errorCode = BoardMPU6050_ReadBuffer(MPU_GYRO_XOUTH_REG,readBuf,6);
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    //数据转化
	*gx = (short)((((uint16_t)readBuf[0])<<8)|readBuf[1]);  
    *gy = (short)((((uint16_t)readBuf[2])<<8)|readBuf[3]);  
    *gz = (short)((((uint16_t)readBuf[4])<<8)|readBuf[5]);	
    return errorCode;
}

//获取加速度计当前原始数据
D_ERROR_CODE BoardMPU6050_ReadCurrentAccelerometerDat(short *ax,short *ay,short *az)
{
    uint8_t readBuf[6];  
    D_ERROR_CODE errorCode = D_ERROR_CODE_NONE;
    //读取数据
	errorCode = BoardMPU6050_ReadBuffer(MPU_ACCEL_XOUTH_REG,readBuf,6);
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    //数据转化
	*ax = (short)((((uint16_t)readBuf[0])<<8)|readBuf[1]);  
    *ay = (short)((((uint16_t)readBuf[2])<<8)|readBuf[3]);  
    *az = (short)((((uint16_t)readBuf[4])<<8)|readBuf[5]);	
    return errorCode;
}

//获取MPU6050当前全部源数据
D_ERROR_CODE BoardMPU6050_ReadCurrentAllDat(DAT_MPU6050_SRC* datSrc)
{
    D_ERROR_CODE errorCode = D_ERROR_CODE_NONE;
    //读取温度
    errorCode = BoardMPU6050_ReadCurrentTemperatureDat(&(datSrc->tempValue));
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    //读取陀螺仪
    errorCode = BoardMPU6050_ReadCurrentGyroscopeDat(&(datSrc->gyroscopeAxisX),&(datSrc->gyroscopeAxisY),&(datSrc->gyroscopeAxisZ));
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    //读取加速度传感器
    errorCode = BoardMPU6050_ReadCurrentAccelerometerDat(&(datSrc->accelerometerAxisX),&(datSrc->accelerometerAxisY),&(datSrc->accelerometerAxisZ));
    if(errorCode != D_ERROR_CODE_NONE)
    {
        return errorCode;
    }
    //回传完成
    return errorCode;
}

