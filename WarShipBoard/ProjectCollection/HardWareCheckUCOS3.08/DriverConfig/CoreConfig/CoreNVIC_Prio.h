/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 21:50:02
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-06 13:40:08
 *FilePath: \HardWareCheckUCOS3.08\DriverConfig\CoreConfig\CoreNVIC_Prio.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __CORE_NVIC_PRIO_H_
#define __CORE_NVIC_PRIO_H_
#include "stm32f4xx.h"

//因为系统的中断四位,所以,慎重起见,将中断感知级别设置为7 ,也就是抢占优先级1,子优先级3
//只有在这之上的中断,才能调用系统函数

//系统内部NVIC中断的优先级设定
/*****************************不会调用系统内核函数的中断优先级************************************/
//中断抢占优先级,DMA 串口1
#define SDIO_PREEMPT_PRIORITY               INT_PRE_PRI_0
//中断子优先级
#define SDIO_SUB_PRIORITY                   INT_SUB_PRI_1

//中断抢占优先级,DMA 串口1
#define MCU_DMA2_STREAM7_PREE_PRI           INT_PRE_PRI_1
//中断子优先级
#define MCU_DMA2_STREAM7_SUB_PRI            INT_SUB_PRI_0

//中断抢占优先级,DMA 串口2
#define MCU_DMA1_STREAM6_PREE_PRI           INT_PRE_PRI_1
//中断子优先级
#define MCU_DMA1_STREAM6_SUB_PRI            INT_SUB_PRI_1


/************************************会调用系统内核函数的中断优先级********************************/
//中断优先级RX CAN
#define CAN1_RX_INT_PREE_PRI                INT_PRE_PRI_2
#define CAN1_RX_INT_SUB_PRI                 INT_SUB_PRI_0

//中断优先级 TX
#define CAN1_TX_INT_PREE_PRI                INT_PRE_PRI_2
#define CAN1_TX_INT_SUB_PRI                 INT_SUB_PRI_1

//中断抢占优先级,串口1
#define MCU_UART1_DMA_PREE_PRI              INT_PRE_PRI_3
//中断子优先级
#define MCU_UART1_DMA_SUB_PRI               INT_SUB_PRI_0

//中断抢占优先级 串口2
#define MCU_UART2_DMA_PREE_PRI              INT_PRE_PRI_3
//中断子优先级
#define MCU_UART2_DMA_SUB_PRI               INT_SUB_PRI_1

//中断抢占优先级 串口3
#define MCU_UART3_PREE_PRI                  INT_PRE_PRI_3
//中断子优先级
#define MCU_UART3_SUB_PRI                   INT_SUB_PRI_1


//中断抢占优先级 RTC ALARM
#define MCU_RTC_ALARM_PREE_PRI              INT_PRE_PRI_3
//中断子优先级 RTC
#define MCU_RTC_ALARM_SUB_PRI               INT_SUB_PRI_3

//中断抢占优先级 RTC WAKEUP
#define MCU_RTC_WAKEUP_PREE_PRI             INT_PRE_PRI_3
//中断子优先级 RTC
#define MCU_RTC_WAKEUP_SUB_PRI              INT_SUB_PRI_3

#endif






