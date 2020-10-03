/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:23:58
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-03 22:49:17
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardKey.c
**ModifyRecord1:    
******************************************************************/
#include "BoardKey.h"

//按键轮询初始化
void BoardKeyScanInit(void)
{
    //UP
    MCU_PortInit(MCU_PIN_A_0, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_DOWN,GPIO_Low_Speed);
    //DOWN
    MCU_PortInit(MCU_PIN_E_3, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Low_Speed);
    //LEFT
    MCU_PortInit(MCU_PIN_E_2, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Low_Speed);
    //RIGHT
    MCU_PortInit(MCU_PIN_E_4, GPIO_Mode_IN, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Low_Speed);
}

//按键轮询获取按键状态
SENSOR_STATE BoardKeyScanGetState(BOARD_KEY_NAME keyName)
{
    BitAction keyValidState = Bit_RESET;
    //计算有效信号
    switch(keyName)
    {
        case BOARD_KEY_DOWN:
        case BOARD_KEY_LEFT:
        case BOARD_KEY_RIGHT:
            keyValidState = Bit_RESET;
            break;
        case BOARD_KEY_UP:
            keyValidState = Bit_SET;
            break;
        default:
            keyValidState = Bit_RESET;
            break;
    }
    //计算引脚IO
    MCU_PIN pinIndex;
    switch(keyName)
    {
        case BOARD_KEY_UP:
            pinIndex = MCU_PIN_A_0;
            break;
        case BOARD_KEY_DOWN:
            pinIndex = MCU_PIN_E_3;
            break;
        case BOARD_KEY_LEFT:
            pinIndex = MCU_PIN_E_2;
            break;
        case BOARD_KEY_RIGHT:
            pinIndex = MCU_PIN_E_4;
            break;
        default:
            pinIndex = MCU_PIN_A_0;
            break;
    }
    //读取
    if(keyValidState == MCU_PortReadSingle(pinIndex))return SENSOR_VALID;
    else return SENSOR_INVALID;
}

