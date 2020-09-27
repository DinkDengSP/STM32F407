/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 21:53:59
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-09-27 22:07:44
 *FilePath: \HardWareCheckUCOS3.08\TaskMain\AppSupport\AppTask\AppTaskConfig.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __APP_TASK_CONFIG_H_
#define __APP_TASK_CONFIG_H_
#include "BoardInc.h"
#include "os.h"

/**************************************************辅助任务***************************************************/
//任务名称
#define NAME_TASK_APP_DEMO                                "Demo"
//任务优先级
#define PRIO_TASK_APP_DEMO                                25
//任务堆栈大小
#define STK_SIZE_TASK_APP_DEMO                            8 * 32
//任务堆栈
extern CPU_STK stackBufferTaskAppDemo[STK_SIZE_TASK_APP_DEMO];
//任务控制块
extern OS_TCB tcbTaskAppDemo;
//任务函数
void TaskFuncAppDemo(void *p_arg);




#endif

