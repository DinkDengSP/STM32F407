/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:22:17
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-09-28 23:37:24
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardBeep.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_BEEP_H_
#define __BOARD_BEEP_H_
#include "MCU_Inc.h"

//蜂鸣器初始化
void BoardBeepInit(OUTPUT_STATE initState);

//设置蜂鸣器状态
void BoardBeepSetState(OUTPUT_STATE state);

#endif



