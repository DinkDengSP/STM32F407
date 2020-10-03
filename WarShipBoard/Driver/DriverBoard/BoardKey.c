/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:23:58
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-03 23:13:53
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
            return SENSOR_INVALID;
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
            return SENSOR_INVALID;
    }
    //读取
    if(keyValidState == MCU_PortReadSingle(pinIndex))return SENSOR_VALID;
    else return SENSOR_INVALID;
}

//按键中断初始化
void BoardKeyIntInit(BOARD_KEY_NAME keyName,INT_PRE_PRI prePri,INT_SUB_PRI subPri,MCU_EXTI_IntProcessFuncPtr callBack)
{
    //计算引脚IO
    MCU_PIN pinIndex;
    GPIOOType_TypeDef ooType = GPIO_OType_PP;
    GPIOPuPd_TypeDef pupdType = GPIO_PuPd_UP;
    GPIOSpeed_TypeDef speedType = GPIO_Low_Speed;
    EXTITrigger_TypeDef trigMode = EXTI_Trigger_Falling;
    switch(keyName)
    {
        case BOARD_KEY_UP:
            pinIndex = MCU_PIN_A_0;
            pupdType = GPIO_PuPd_DOWN;
            trigMode = EXTI_Trigger_Rising;
            break;
        case BOARD_KEY_DOWN:
            pinIndex = MCU_PIN_E_3;
            pupdType = GPIO_PuPd_UP;
            trigMode = EXTI_Trigger_Falling;
            break;
        case BOARD_KEY_LEFT:
            pinIndex = MCU_PIN_E_2;
            pupdType = GPIO_PuPd_UP;
            trigMode = EXTI_Trigger_Falling;
            break;
        case BOARD_KEY_RIGHT:
            pinIndex = MCU_PIN_E_4;
            pupdType = GPIO_PuPd_UP;
            trigMode = EXTI_Trigger_Falling;
            break;
        default:
            return;
    }
    //先清除以前的注册的中断
    MCU_EXTI_Clear(pinIndex);
    //注册中断
    MCU_EXTI_Init(pinIndex,ooType,pupdType,speedType,trigMode,prePri,subPri,callBack);
}
