/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:18:54
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 20:51:07
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_EXTI.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_EXTI_H_
#define __MCU_EXTI_H_
#include "MCU_Common.h"
#include "MCU_Port.h"

//外部中断回调函数
typedef void (*MCU_EXTI_IntProcessFuncPtr)(void);

//外部中断初始化,设计端口选择,输入模式,输入速度,中断触发模式,回调函数等
D_ERROR_CODE MCU_EXTI_Init(MCU_PIN pin,GPIOOType_TypeDef oType,GPIOPuPd_TypeDef pupd,GPIOSpeed_TypeDef speed,EXTITrigger_TypeDef trigMode,INT_PRE_PRI prePri,INT_SUB_PRI subPri,MCU_EXTI_IntProcessFuncPtr callBack);

//关闭外部中断
D_ERROR_CODE MCU_EXTI_Disable(MCU_PIN pin);

//打开外部中断
D_ERROR_CODE MCU_EXTI_Enable(MCU_PIN pin);

//清除当前设置的外部中断
D_ERROR_CODE MCU_EXTI_Clear(MCU_PIN pin);


#endif




