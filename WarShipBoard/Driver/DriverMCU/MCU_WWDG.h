/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-10-03 23:30:05
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 23:43:36
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_WWDG.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __MCU_WWDG_H_
#define __MCU_WWDG_H_
#include "MCU_Common.h"

//默认计数值
#define MCU_WWDG_DEFAULT_TR         0X7F
//默认窗口值
#define MCU_WWDG_DEFAULT_WR         0X5F
//默认分频系数
#define MCU_WWDG_DEFAULT_FPRER      8

//窗口看门狗中断回调函数
//注意,该中断程序不得调用系统API
typedef void (*MCU_WWDG_CallBackFunc)(void);

//窗口看门狗初始化
//初始化窗口看门狗
//tr   :T[6:0],计数器值
//wr   :W[6:0],窗口值
//fprer:分频系数（WDGTB）,仅最低2位有效
//Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz
//一般 计数器值为7f,窗口寄存器为5f,分频数为8
void MCU_WDG_WindowInit(uint8_t tr, uint8_t wr, uint32_t fprer,MCU_WWDG_CallBackFunc callBack);

//窗口看门狗喂狗,在中断中喂狗,所以注意中断关闭的时间
void MCU_WDG_WindowFeed(void);


#endif


