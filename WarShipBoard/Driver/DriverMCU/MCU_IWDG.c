/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-10-03 23:29:14
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-03 23:34:40
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_IWDG.c
**ModifyRecord1:    
******************************************************************/
#include "MCU_IWDG.h"

//独立看门狗初始化
//prer:分频数:0~7(只有低3位有效!)
//rlr:自动重装载值,0~0XFFF.
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).
//分频数为64(4),重载值为500,溢出时间为1s
void MCU_IWDG_Init(uint8_t prer, uint16_t rlr)
{
    //使能对IWDG->PR IWDG->RLR的写
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    //设置IWDG分频系数
    IWDG_SetPrescaler(prer);
    //设置IWDG装载值
    IWDG_SetReload(rlr);
    //reload
    IWDG_ReloadCounter();
    //使能看门狗
    IWDG_Enable();
}

//独立看门狗喂狗,打开之后必须定时喂狗
void MCU_IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}



