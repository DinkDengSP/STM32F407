/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:19:21
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 15:37:42
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_Random.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_RANDOM_H_
#define __MCU_RANDOM_H_
#include "MCU_Common.h"

//锁定操作系统调度
#define MCU_RANDOM_LOCK_SCHED()     OS_ERR  p_err;OSSchedLock(&p_err)
//解锁操作系统调度
#define MCU_RANDOM_UNLOCK_SCHED()   OSSchedUnlock(&p_err);

//随机数初始化
void MCU_RandomInit(void);

//得到随机数
uint32_t MCU_RandomGetNext(void);

//生成指定范围的随机数
int MCU_RandomGetNextRange(int min, int max);

//正确计算随机数的方法
int MCU_RandomGetNextRangeReal(int min, int max);



#endif




