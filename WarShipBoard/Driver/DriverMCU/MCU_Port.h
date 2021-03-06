/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:17:38
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 22:05:08
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_Port.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_PORT_H_
#define __MCU_PORT_H_
#include "MCU_Common.h"


//芯片的输入输出引脚定义
typedef enum MCU_PIN
{
    //PORTA 0-15
    MCU_PIN_A_0,MCU_PIN_A_1,MCU_PIN_A_2,MCU_PIN_A_3,MCU_PIN_A_4,MCU_PIN_A_5,MCU_PIN_A_6,MCU_PIN_A_7,
    MCU_PIN_A_8,MCU_PIN_A_9,MCU_PIN_A_10,MCU_PIN_A_11,MCU_PIN_A_12,MCU_PIN_A_13,MCU_PIN_A_14,MCU_PIN_A_15,
    //PORTB 0-15
    MCU_PIN_B_0,MCU_PIN_B_1,MCU_PIN_B_2,MCU_PIN_B_3,MCU_PIN_B_4,MCU_PIN_B_5,MCU_PIN_B_6,MCU_PIN_B_7,
    MCU_PIN_B_8,MCU_PIN_B_9,MCU_PIN_B_10,MCU_PIN_B_11,MCU_PIN_B_12,MCU_PIN_B_13,MCU_PIN_B_14,MCU_PIN_B_15,
    //PORTC 0-13
    MCU_PIN_C_0,MCU_PIN_C_1,MCU_PIN_C_2,MCU_PIN_C_3,MCU_PIN_C_4,MCU_PIN_C_5,MCU_PIN_C_6,MCU_PIN_C_7,
    MCU_PIN_C_8,MCU_PIN_C_9,MCU_PIN_C_10,MCU_PIN_C_11,MCU_PIN_C_12,MCU_PIN_C_13,
    //PORTD 0-15
    MCU_PIN_D_0,MCU_PIN_D_1,MCU_PIN_D_2,MCU_PIN_D_3,MCU_PIN_D_4,MCU_PIN_D_5,MCU_PIN_D_6,MCU_PIN_D_7,
    MCU_PIN_D_8,MCU_PIN_D_9,MCU_PIN_D_10,MCU_PIN_D_11,MCU_PIN_D_12,MCU_PIN_D_13,MCU_PIN_D_14,MCU_PIN_D_15,
    //PORTE 0-15
    MCU_PIN_E_0,MCU_PIN_E_1,MCU_PIN_E_2,MCU_PIN_E_3,MCU_PIN_E_4,MCU_PIN_E_5,MCU_PIN_E_6,MCU_PIN_E_7,
    MCU_PIN_E_8,MCU_PIN_E_9,MCU_PIN_E_10,MCU_PIN_E_11,MCU_PIN_E_12,MCU_PIN_E_13,MCU_PIN_E_14,MCU_PIN_E_15,
    //PORTF 0-15
    MCU_PIN_F_0,MCU_PIN_F_1,MCU_PIN_F_2,MCU_PIN_F_3,MCU_PIN_F_4,MCU_PIN_F_5,MCU_PIN_F_6,MCU_PIN_F_7,
    MCU_PIN_F_8,MCU_PIN_F_9,MCU_PIN_F_10,MCU_PIN_F_11,MCU_PIN_F_12,MCU_PIN_F_13,MCU_PIN_F_14,MCU_PIN_F_15,
    //PORTG 0-15
    MCU_PIN_G_0,MCU_PIN_G_1,MCU_PIN_G_2,MCU_PIN_G_3,MCU_PIN_G_4,MCU_PIN_G_5,MCU_PIN_G_6,MCU_PIN_G_7,
    MCU_PIN_G_8,MCU_PIN_G_9,MCU_PIN_G_10,MCU_PIN_G_11,MCU_PIN_G_12,MCU_PIN_G_13,MCU_PIN_G_14,MCU_PIN_G_15,
    //PORTH 2-15
    MCU_PIN_H_2,MCU_PIN_H_3,MCU_PIN_H_4,MCU_PIN_H_5,MCU_PIN_H_6,MCU_PIN_H_7,MCU_PIN_H_8,
    MCU_PIN_H_9,MCU_PIN_H_10,MCU_PIN_H_11,MCU_PIN_H_12,MCU_PIN_H_13,MCU_PIN_H_14,MCU_PIN_H_15,
    //PORTI 0-11
    MCU_PIN_I_0,MCU_PIN_I_1,MCU_PIN_I_2,MCU_PIN_I_3,MCU_PIN_I_4,MCU_PIN_I_5,MCU_PIN_I_6,MCU_PIN_I_7,
    MCU_PIN_I_8,MCU_PIN_I_9,MCU_PIN_I_10,MCU_PIN_I_11,MCU_PIN_COUNT,
}MCU_PIN;

//初始化指定引脚
void MCU_PortInit(MCU_PIN pin,GPIOMode_TypeDef mode, GPIOOType_TypeDef oType,GPIOPuPd_TypeDef pupd, GPIOSpeed_TypeDef speed);

//设置指定引脚的值
void MCU_PortWriteSingle(MCU_PIN pin,BitAction value);

//读取指定引脚的值
BitAction MCU_PortReadSingle(MCU_PIN pin);

//翻转指定引脚的值
void MCU_PortToogleSingle(MCU_PIN pin);




#endif




