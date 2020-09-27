/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:17:59
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-09-27 22:37:20
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardLed.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_LED_H_
#define __BOARD_LED_H_
#include "MCU_Inc.h"

//LED灯序号
typedef enum BOARD_LED_NAME
{
    BOARD_LED_RED,
    BOARD_LED_GREEN,
    BOARD_LED_COUNT = BOARD_LED_GREEN,
}BOARD_LED_NAME;

//LED灯状态
typedef enum BOARD_LED_STATE
{
    BOARD_LED_DARK,
    BOARD_LED_LIGHT,
}BOARD_LED_STATE;

//led初始化
void BoardLedInit(BOARD_LED_NAME name,BOARD_LED_STATE initState);

//LED设置状态
void BoardLedWrite(BOARD_LED_NAME name,BOARD_LED_STATE setState);

//LED切换状态
void BoardLedToogle(BOARD_LED_NAME name);

#endif



