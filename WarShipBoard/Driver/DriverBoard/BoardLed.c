/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:17:52
**LastEditors: DengXiaoJun
**LastEditTime: 2020-09-26 23:58:32
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardLed.c
**ModifyRecord1:    
******************************************************************/
#include "BoardLed.h"


//函数指针类型
//初始化函数指针
typedef void (*BoardLedInitFuncPtr)(BOARD_LED_STATE initStatus);
//写入状态函数指针
typedef void (*BoardLedWriteFuncPtr)(BOARD_LED_STATE status);
//状态翻转函数指针
typedef void (*BoardLedToogleFuncPtr)(void);

//LED有效信号
#define LED_RED_VALID_OUT_LEVEL         Bit_RESET
#define LED_GREEN_VALID_OUT_LEVEL       Bit_RESET

//RED
static void BoardLedInitRed(BOARD_LED_STATE initStatus)
{
    BitAction ioStatus;
    //确定写入状态
    ioStatus = (initStatus == BOARD_LED_DARK) ? (BitAction)(!LED_RED_VALID_OUT_LEVEL):LED_RED_VALID_OUT_LEVEL;
    //初始化IO口
    MCU_PortInit(MCU_PIN_F_9, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Fast_Speed);
    MCU_PortWriteSingle(MCU_PIN_F_9, ioStatus);
}

static void BoardLedWriteRed(BOARD_LED_STATE status)
{
    BitAction ioStatus;
    ioStatus = (status == BOARD_LED_DARK) ? (BitAction)(!LED_RED_VALID_OUT_LEVEL):LED_RED_VALID_OUT_LEVEL;
    MCU_PortWriteSingle(MCU_PIN_F_9, ioStatus);
}

static void BoardLedToogleRed()
{
    MCU_PortToogleSingle(MCU_PIN_F_9);
}

//green
static void BoardLedInitGreen(BOARD_LED_STATE initStatus)
{
    BitAction ioStatus;
    //确定写入状态
    ioStatus = (initStatus == BOARD_LED_DARK) ? (BitAction)(!LED_GREEN_VALID_OUT_LEVEL):LED_GREEN_VALID_OUT_LEVEL;
    //初始化IO口
    MCU_PortInit(MCU_PIN_F_10, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Fast_Speed);
    MCU_PortWriteSingle(MCU_PIN_F_10, ioStatus);
}

static void BoardLedWriteGreen(BOARD_LED_STATE status)
{
    BitAction ioStatus;
    ioStatus = (status == BOARD_LED_DARK) ? (BitAction)(!LED_GREEN_VALID_OUT_LEVEL):LED_GREEN_VALID_OUT_LEVEL;
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
void BoardLedInit(BOARD_LED_NAME name,BOARD_LED_STATE initStatus)
{
    if(name > BOARD_LED_COUNT)
    {
        return;
    }
    BoardLedInitFuncPtrArray[name](initStatus);
}

//LED设置状态
void BoardLedWrite(BOARD_LED_NAME name,BOARD_LED_STATE status)
{
    if(name > BOARD_LED_COUNT)
    {
        return;
    }
    BoardLedWriteFuncPtrArray[name](status);
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




