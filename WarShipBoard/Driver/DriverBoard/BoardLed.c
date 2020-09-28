/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:17:52
**LastEditors: DengXiaoJun
**LastEditTime: 2020-09-28 23:42:54
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardLed.c
**ModifyRecord1:    
******************************************************************/
#include "BoardLed.h"


//函数指针类型
//初始化函数指针
typedef void (*BoardLedInitFuncPtr)(OUTPUT_STATE initState);
//写入状态函数指针
typedef void (*BoardLedWriteFuncPtr)(OUTPUT_STATE state);
//状态翻转函数指针
typedef void (*BoardLedToogleFuncPtr)(void);

//LED有效信号
#define LED_RED_VALID_OUT_LEVEL         Bit_RESET
#define LED_GREEN_VALID_OUT_LEVEL       Bit_RESET

//RED
static void BoardLedInitRed(OUTPUT_STATE initState)
{
    BitAction ioStatus;
    //确定写入状态
    ioStatus = (initState == OUTPUT_INVALID) ? (BitAction)(!LED_RED_VALID_OUT_LEVEL):LED_RED_VALID_OUT_LEVEL;
    //初始化IO口
    MCU_PortInit(MCU_PIN_F_9, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Fast_Speed);
    MCU_PortWriteSingle(MCU_PIN_F_9, ioStatus);
}

static void BoardLedWriteRed(OUTPUT_STATE state)
{
    BitAction ioStatus;
    ioStatus = (state == OUTPUT_INVALID) ? (BitAction)(!LED_RED_VALID_OUT_LEVEL):LED_RED_VALID_OUT_LEVEL;
    MCU_PortWriteSingle(MCU_PIN_F_9, ioStatus);
}

static void BoardLedToogleRed()
{
    MCU_PortToogleSingle(MCU_PIN_F_9);
}

//green
static void BoardLedInitGreen(OUTPUT_STATE initState)
{
    BitAction ioStatus;
    //确定写入状态
    ioStatus = (initState == OUTPUT_INVALID) ? (BitAction)(!LED_GREEN_VALID_OUT_LEVEL):LED_GREEN_VALID_OUT_LEVEL;
    //初始化IO口
    MCU_PortInit(MCU_PIN_F_10, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Fast_Speed);
    MCU_PortWriteSingle(MCU_PIN_F_10, ioStatus);
}

static void BoardLedWriteGreen(OUTPUT_STATE state)
{
    BitAction ioStatus;
    ioStatus = (state == OUTPUT_INVALID) ? (BitAction)(!LED_GREEN_VALID_OUT_LEVEL):LED_GREEN_VALID_OUT_LEVEL;
    MCU_PortWriteSingle(MCU_PIN_F_10, ioStatus);
}

static void BoardLedToogleGreen()
{
    MCU_PortToogleSingle(MCU_PIN_F_10);
}


//初始化函数数组
static const BoardLedInitFuncPtr BoardLedInitFuncPtrArray[] = {
    BoardLedInitRed,
    BoardLedInitGreen,
};

//写入数值函数数组
static const BoardLedWriteFuncPtr BoardLedWriteFuncPtrArray[] = {
    BoardLedWriteRed,
    BoardLedWriteGreen,
};  

//翻转状态函数数组
static const BoardLedToogleFuncPtr BoardLedToogleFuncPtrArray[] = {
    BoardLedToogleRed,
    BoardLedToogleGreen,
};




//led初始化
void BoardLedInit(BOARD_LED_NAME name,OUTPUT_STATE initState)
{
    if(name > BOARD_LED_COUNT)
    {
        return;
    }
    BoardLedInitFuncPtrArray[name](initState);
}

//LED设置状态
void BoardLedWrite(BOARD_LED_NAME name,OUTPUT_STATE state)
{
    if(name > BOARD_LED_COUNT)
    {
        return;
    }
    BoardLedWriteFuncPtrArray[name](state);
}

//LED切换状态
void BoardLedToogle(BOARD_LED_NAME name)
{
    if(name > BOARD_LED_COUNT)
    {
        return;
    }
    BoardLedToogleFuncPtrArray[name]();
}




