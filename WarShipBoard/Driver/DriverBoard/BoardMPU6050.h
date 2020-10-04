/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:36:05
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-04 22:46:11
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardMPU6050.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_MPU6050_H_
#define __BOARD_MPU6050_H_
#include "BoardMPU6050Base.h"

//初始化MPU6050
void BoardMPU6050_Init(void); 	
//配置,自检MCU6050
D_ERROR_CODE BoardMPU6050_ConfigAndCheck(void);
//IIC写一个字节
D_ERROR_CODE BoardMPU6050_WriteByte(uint8_t regAddr,uint8_t byteWrite);	
//IIC读一个字节			
D_ERROR_CODE BoardMPU6050_ReadByte(uint8_t regAddr,uint8_t* byteRead);
//IIC连续写							
D_ERROR_CODE BoardMPU6050_WriteBuffer(uint8_t regAddr,uint8_t* bufWrite,uint8_t dataLength);
//IIC连续读 
D_ERROR_CODE BoardMPU6050_ReadBuffer(uint8_t regAddr,uint8_t* bufRead,uint8_t dataLength); 
//MPU6050设置陀螺仪满量程
D_ERROR_CODE BoardMPU6050_SetGyroscopeFullScaleRange(MPU6050_GYRO_RANGE range);
//MPU6050设置加速度传感器满量程
D_ERROR_CODE BoardMPU6050_SetAccelerometerFullScaleRange(MPU6050_ACCEL_RANGE range);
//设置低通滤波器截止频率
D_ERROR_CODE BoardMPU6050_SetLPF_CutOffFreq(MPU6050_LPF_FREQ cutOffFreq);
//设置采样频率
D_ERROR_CODE BoardMPU6050_SetSampleRate(uint16_t sampleRate);
//读取当前温度
D_ERROR_CODE BoardMPU6050_ReadCurrentTemperatureDat(float* tempValue);
//获取陀螺仪当前原始数据
D_ERROR_CODE BoardMPU6050_ReadCurrentGyroscopeDat(short *gx,short *gy,short *gz);
//获取加速度计当前原始数据
D_ERROR_CODE BoardMPU6050_ReadCurrentAccelerometerDat(short *ax,short *ay,short *az);
//获取MPU6050当前全部源数据
D_ERROR_CODE BoardMPU6050_ReadCurrentAllDat(DAT_MPU6050_SRC* datSrc);


#endif



