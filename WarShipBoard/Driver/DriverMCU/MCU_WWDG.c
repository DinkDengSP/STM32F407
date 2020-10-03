/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-10-03 23:29:57
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-03 23:45:01
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_WWDG.c
**ModifyRecord1:    
******************************************************************/
#include "MCU_WWDG.h"

static MCU_WWDG_CallBackFunc wwdgCallBack = NULL;

//保存WWDG计数器的设置值,默认为最大.
static uint8_t WWDG_CNT = 0X7F;

//窗口看门狗初始化
//初始化窗口看门狗
//tr   :T[6:0],计数器值
//wr   :W[6:0],窗口值
//fprer:分频系数（WDGTB）,仅最低2位有效
//Fwwdg=PCLK1/(4096*2^fprer). 一般PCLK1=42Mhz
//一般 计数器值为7f,窗口寄存器为5f,分频数为8
void MCU_WDG_WindowInit(uint8_t tr, uint8_t wr, uint32_t fprer,MCU_WWDG_CallBackFunc callBack)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //使能窗口看门狗时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    //复位
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG,ENABLE);
    //取消复位
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG,DISABLE);

    //初始化WWDG_CNT.
    WWDG_CNT = tr & WWDG_CNT;
    //设置分频值
    WWDG_SetPrescaler(fprer);
    //设置窗口值
    WWDG_SetWindowValue(wr);
    //开启看门狗
    WWDG_Enable(WWDG_CNT);

    //窗口看门狗中断
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    //设置优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRE_PRI_0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = INT_SUB_PRI_0;
    //使能窗口看门狗通道
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //初始化
    NVIC_Init(&NVIC_InitStructure);

    //清除提前唤醒中断标志位
    WWDG_ClearFlag();
    //开启提前唤醒中断
    WWDG_EnableIT();
    
    wwdgCallBack = callBack;
}

//窗口看门狗喂狗,在中断中喂狗,所以注意中断关闭的时间
void MCU_WDG_WindowFeed(void)
{
    //重设窗口看门狗值
    WWDG_SetCounter(WWDG_CNT);
    //清除提前唤醒中断标志位
    WWDG_ClearFlag();
}


//窗口看门狗中断服务程序
void WWDG_IRQHandler(void)
{
    //重设窗口看门狗值
    WWDG_SetCounter(WWDG_CNT);
    //清除提前唤醒中断标志位
    WWDG_ClearFlag();
    if(wwdgCallBack != NULL)
    {
        wwdgCallBack();
    }
}
