/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 21:53:02
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-09-27 21:56:43
 *FilePath: \HardWareCheckUCOS3.08\TaskMain\ServiceSupport\ServiceTask\ServiceTaskConfig.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __SERVICE_TASK_CONFIG_H_
#define __SERVICE_TASK_CONFIG_H_
#include "BoardInc.h"
#include "os.h"
#include "GUI.h"
#include "GUIDEMO.h"

/**************************************************辅助任务***************************************************/
//任务名称
#define NAME_TASK_SERVICE_HEART                                "Heart"
//任务优先级
#define PRIO_TASK_SERVICE_HEART                                4
//任务堆栈大小
#define STK_SIZE_TASK_SERVICE_HEART                            8 * 32
//任务堆栈
extern CPU_STK stackBufferTaskServiceHeart[STK_SIZE_TASK_SERVICE_HEART];
//任务控制块
extern OS_TCB tcbTaskServiceHeart;
//任务函数
void TaskFuncServiceHeart(void *p_arg);



#endif
























