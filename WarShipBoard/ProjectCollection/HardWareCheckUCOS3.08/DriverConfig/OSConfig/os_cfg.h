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
*                                          CONFIGURATION FILE
*
* Filename : os_cfg.h
* Version  : V3.08.00
*********************************************************************************************************
*/

#ifndef OS_CFG_H
#define OS_CFG_H

/*使能APP回调函数*/                                               /* --------------------------- MISCELLANEOUS --------------------------- */
#define OS_CFG_APP_HOOKS_EN                        1u           /* Enable (1) or Disable (0) application specific hooks                  */
/*使能参数检查*/
#define OS_CFG_ARG_CHK_EN                          1u           /* Enable (1) or Disable (0) argument checking                           */
/*使能中断中调用系统API检查*/
#define OS_CFG_CALLED_FROM_ISR_CHK_EN              1u           /* Enable (1) or Disable (0) check for called from ISR                   */
/*使能操作系统调试*/
#define OS_CFG_DBG_EN                              1u           /* Enable (1) or Disable (0) debug code/variables                        */
/*使能Tick时钟*/
#define OS_CFG_TICK_EN                             1u           /* Enable (1) or Disable (0) the kernel tick                             */
/*使能动态时钟*/
#define OS_CFG_DYN_TICK_EN                         0u           /* Enable (1) or Disable (0) the Dynamic Tick                            */
/*使能不支持的系统调用检查*/
#define OS_CFG_INVALID_OS_CALLS_CHK_EN             1u           /* Enable (1) or Disable (0) checks for invalid kernel calls             */
/*使能对象类型检查*/
#define OS_CFG_OBJ_TYPE_CHK_EN                     1u           /* Enable (1) or Disable (0) object type checking                        */
/*使能时间帧计数器*/
#define OS_CFG_TS_EN                               1u           /* Enable (1) or Disable (0) time stamping                               */
/*设置最大系统优先级*/
#define OS_CFG_PRIO_MAX                           32u           /* Defines the maximum number of task priorities (see OS_PRIO data type) */
/*系统调度关断时间测量*/
#define OS_CFG_SCHED_LOCK_TIME_MEAS_EN             1u           /* Include code to measure scheduler lock time                           */
/*同级优先级调度*/
#define OS_CFG_SCHED_ROUND_ROBIN_EN                1u           /* Include code for Round-Robin scheduling                               */
/*最小堆栈大小*/
#define OS_CFG_STK_SIZE_MIN                       128u           /* Minimum allowable task stack size                                     */


/*标志位*/                                                       /* --------------------------- EVENT FLAGS ----------------------------- */
#define OS_CFG_FLAG_EN                             1u           /* Enable (1) or Disable (0) code generation for EVENT FLAGS             */
#define OS_CFG_FLAG_DEL_EN                         1u           /*     Include code for OSFlagDel()                                      */
#define OS_CFG_FLAG_MODE_CLR_EN                    1u           /*     Include code for Wait on Clear EVENT FLAGS                        */
#define OS_CFG_FLAG_PEND_ABORT_EN                  1u           /*     Include code for OSFlagPendAbort()                                */


/*内存管理*/                                                     /* ------------------------ MEMORY MANAGEMENT -------------------------  */
#define OS_CFG_MEM_EN                              0u           /* Enable (1) or Disable (0) code generation for the MEMORY MANAGER      */


/*互斥信号量*/                                                   /* ------------------- MUTUAL EXCLUSION SEMAPHORES --------------------  */
#define OS_CFG_MUTEX_EN                            1u           /* Enable (1) or Disable (0) code generation for MUTEX                   */
#define OS_CFG_MUTEX_DEL_EN                        1u           /*     Include code for OSMutexDel()                                     */
#define OS_CFG_MUTEX_PEND_ABORT_EN                 1u           /*     Include code for OSMutexPendAbort()                               */


/*消息队列*/                                                     /* -------------------------- MESSAGE QUEUES --------------------------  */
#define OS_CFG_Q_EN                                1u           /* Enable (1) or Disable (0) code generation for QUEUES                  */
#define OS_CFG_Q_DEL_EN                            1u           /*     Include code for OSQDel()                                         */
#define OS_CFG_Q_FLUSH_EN                          1u           /*     Include code for OSQFlush()                                       */
#define OS_CFG_Q_PEND_ABORT_EN                     1u           /*     Include code for OSQPendAbort()                                   */


/*信号量*/                                                       /* ---------------------------- SEMAPHORES ----------------------------- */
#define OS_CFG_SEM_EN                              1u           /* Enable (1) or Disable (0) code generation for SEMAPHORES              */
#define OS_CFG_SEM_DEL_EN                          1u           /*     Include code for OSSemDel()                                       */
#define OS_CFG_SEM_PEND_ABORT_EN                   1u           /*     Include code for OSSemPendAbort()                                 */
#define OS_CFG_SEM_SET_EN                          1u           /*     Include code for OSSemSet()                                       */


/*系统统计任务*/                                                 /* -------------------------- TASK MANAGEMENT -------------------------- */
#define OS_CFG_STAT_TASK_EN                        1u           /* Enable (1) or Disable (0) the statistics task                         */
#define OS_CFG_STAT_TASK_STK_CHK_EN                1u           /*     Check task stacks from the statistic task                         */

#define OS_CFG_TASK_CHANGE_PRIO_EN                 1u           /* Include code for OSTaskChangePrio()                                   */
#define OS_CFG_TASK_DEL_EN                         1u           /* Include code for OSTaskDel()                                          */
#define OS_CFG_TASK_IDLE_EN                        1u           /* Include the idle task                                                 */
#define OS_CFG_TASK_PROFILE_EN                     1u           /* Include variables in OS_TCB for profiling                             */
#define OS_CFG_TASK_Q_EN                           1u           /* Include code for OSTaskQXXXX()                                        */
#define OS_CFG_TASK_Q_PEND_ABORT_EN                1u           /* Include code for OSTaskQPendAbort()                                   */
#define OS_CFG_TASK_REG_TBL_SIZE                   1u           /* Number of task specific registers                                     */

#define OS_CFG_TASK_STK_REDZONE_EN                 1u           /* Enable (1) or Disable (0) stack redzone                               */
#define OS_CFG_TASK_STK_REDZONE_DEPTH              8u           /* Depth of the stack redzone                                            */

#define OS_CFG_TASK_SEM_PEND_ABORT_EN              1u           /* Include code for OSTaskSemPendAbort()                                 */
#define OS_CFG_TASK_SUSPEND_EN                     1u           /* Include code for OSTaskSuspend() and OSTaskResume()                   */


/*任务寄存器*/                                                    /* ------------------ TASK LOCAL STORAGE MANAGEMENT -------------------  */
#define OS_CFG_TLS_TBL_SIZE                        0u           /* Include code for Task Local Storage (TLS) registers                   */


/*系统时间管理*/                                                  /* ------------------------- TIME MANAGEMENT --------------------------  */
#define OS_CFG_TIME_DLY_HMSM_EN                    1u           /* Include code for OSTimeDlyHMSM()                                      */
#define OS_CFG_TIME_DLY_RESUME_EN                  1u           /* Include code for OSTimeDlyResume()                                    */


/*系统定时任务*/                                                 /* ------------------------- TIMER MANAGEMENT -------------------------- */
#define OS_CFG_TMR_EN                              0u           /* Enable (1) or Disable (0) code generation for TIMERS                  */
#define OS_CFG_TMR_DEL_EN                          1u           /* Enable (1) or Disable (0) code generation for OSTmrDel()              */


/*使能系统状态追踪*/                                              /* ------------------------- TRACE RECORDER ---------------------------- */
#define OS_CFG_TRACE_EN                            0u           /* Enable (1) or Disable (0) uC/OS-III Trace instrumentation             */
#define OS_CFG_TRACE_API_ENTER_EN                  0u           /* Enable (1) or Disable (0) uC/OS-III Trace API enter instrumentation   */
#define OS_CFG_TRACE_API_EXIT_EN                   0u           /* Enable (1) or Disable (0) uC/OS-III Trace API exit  instrumentation   */

#endif
