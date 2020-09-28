/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:20:06
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-09-28 23:29:46
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_Common.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_COMMON_H_
#define __MCU_COMMON_H_
#include "CoreInc.h"
#include "os.h"
#include "ErrorCode.h"

//传感器状态
typedef enum SENSOR_STATE
{
    SENSOR_INVALID = 0X00,//传感器无效
    SENSOR_VALID   = 0X01,//传感器有效
}SENSOR_STATE;

//输出设备状态
typedef enum OUTPUT_STATE
{
    OUTPUT_INVALID = 0X00,//输出状态无效
    OUTPUT_VALID   = 0X01,//输出状态有效
}OUTPUT_STATE;

#endif




