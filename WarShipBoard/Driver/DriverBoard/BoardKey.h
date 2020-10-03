/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:24:06
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 23:04:58
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardKey.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_KEY_H_
#define __BOARD_KEY_H_
#include "MCU_Inc.h"

//电路板上按键状态
typedef enum BOARD_KEY_NAME
{
    BOARD_KEY_UP    = 0X00,
    BOARD_KEY_DOWN  = 0X01,
    BOARD_KEY_LEFT  = 0X02,
    BOARD_KEY_RIGHT = 0X03,
}BOARD_KEY_NAME;

//按键轮询初始化
void BoardKeyScanInit(void);

//按键轮询获取按键状态
SENSOR_STATE BoardKeyScanGetState(BOARD_KEY_NAME keyName);

//按键中断初始化
void BoardKeyIntInit(BOARD_KEY_NAME keyName,INT_PRE_PRI prePri,INT_SUB_PRI subPri,MCU_EXTI_IntProcessFuncPtr callBack);

#endif



