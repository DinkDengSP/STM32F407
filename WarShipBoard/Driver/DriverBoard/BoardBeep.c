/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:22:11
**LastEditors: DengXiaoJun
**LastEditTime: 2020-09-28 23:44:51
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardBeep.c
**ModifyRecord1:    
******************************************************************/
#include "BoardBeep.h"

#define VALID_LEVEL_BEEP_PORT       Bit_SET

//蜂鸣器初始化
void BoardBeepInit(OUTPUT_STATE initState)
{
    BitAction ioState;
    //确定写入状态
    ioState = (initState == OUTPUT_INVALID) ? ((BitAction)!VALID_LEVEL_BEEP_PORT):VALID_LEVEL_BEEP_PORT;
    //初始化IO口
    MCU_PortInit(MCU_PIN_F_8, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP,GPIO_Fast_Speed);
    MCU_PortWriteSingle(MCU_PIN_F_8, ioState);
}

//设置蜂鸣器状态
void BoardBeepSetState(OUTPUT_STATE state)
{
    BitAction ioState;
    ioState = (state == OUTPUT_INVALID) ?((BitAction)!VALID_LEVEL_BEEP_PORT):VALID_LEVEL_BEEP_PORT;
    MCU_PortWriteSingle(MCU_PIN_F_8, ioState);
}
