/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 21:45:10
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-09-26 22:34:35
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverCore\UtilFunc\CoreDelay.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __CORE_DELAY_H_
#define __CORE_DELAY_H_
#include "stm32f4xx.h"
#include "os.h"

//延时函数初始化
void CoreDelayInit(void);

//延时函数延时毫秒
void CoreDelayMs(uint16_t nms);

//延时函数延时微秒
void CoreDelayUs(uint16_t nus);

//延时操作系统最小时间基数
void CoreDelayMinTick(void);


#endif





