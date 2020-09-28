/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 21:31:01
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-09-28 23:19:12
 *FilePath: \HardWareCheckUCOS3.08\DriverConfig\OSConfig\os_cfg_app.h
 *ModifyRecord1:    
******************************************************************/
/*
*********************************************************************************************************
*                                              uC/OS-III
*                                        The Real-Time Kernel
*
*                    Copyright 2009-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                               OS CONFIGURATION (APPLICATION SPECIFICS)
*
* Filename : os_cfg_app.h
* Version  : V3.08.00
*********************************************************************************************************
*/

#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

/*
**************************************************************************************************************************
*                                                      CONSTANTS
**************************************************************************************************************************
*/
                                                                /* ------------------ MISCELLANEOUS ------------------- */
/*中断任务堆栈*/                                                                
#define  OS_CFG_ISR_STK_SIZE                                512u/* Stack size of ISR stack (number of CPU_STK elements) */
/*系统消息池*/                                                              
#define  OS_CFG_MSG_POOL_SIZE                               128u/* Maximum number of messages                           */
/*堆栈安全比例*/                                                               
#define  OS_CFG_TASK_STK_LIMIT_PCT_EMPTY                    10u/* Stack limit position in percentage to empty          */
/*空闲任务堆栈*/
#define  OS_CFG_IDLE_TASK_STK_SIZE                          128u/* Stack size (number of CPU_STK elements)              */
 /*状态统计任务的优先级和堆栈*/                                                       
#define  OS_CFG_STAT_TASK_PRIO                              ((OS_PRIO)(OS_CFG_PRIO_MAX-2u))
#define  OS_CFG_STAT_TASK_STK_SIZE                          256u
/*状态统计任务调度频率*/                                                               
#define  OS_CFG_STAT_TASK_RATE_HZ                           10u
/*系统调度频率*/                                                            
#define  OS_CFG_TICK_RATE_HZ                                1000u
/*系统定时任务的优先级*/                                                             
#define  OS_CFG_TMR_TASK_PRIO                               ((OS_PRIO)(OS_CFG_PRIO_MAX-3u))
/*系统定时任务的堆栈*/                                                              
#define  OS_CFG_TMR_TASK_STK_SIZE                           128u

/* DEPRECATED - Rate for timers (10 Hz Typ.)            */
/* The timer task now calculates its timeouts based     */
/* on the timers in the list. It no longer runs at a    */
/* static frequency.                                    */
/* This define is included for compatibility reasons.   */
/* It will determine the period of a timer tick.        */
/* We recommend setting it to OS_CFG_TICK_RATE_HZ       */
/* for new projects.                                    */
/*系统定时任务的运行频率*/
#define  OS_CFG_TMR_TASK_RATE_HZ                            1u


#endif
