/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 21:53:16
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-02 23:07:47
**FilePath: \HardWareCheckUCOS3.08\TaskMain\ServiceSupport\ServiceTask\ServiceTaskHeart.c
**ModifyRecord1:    
******************************************************************/
#include "ServiceTaskConfig.h"

//任务控制块,8字节对齐
#pragma pack(8)
//任务堆栈
__attribute__((section (".RAM_CCM"))) CPU_STK stackBufferTaskServiceHeart[STK_SIZE_TASK_SERVICE_HEART];
#pragma pack()

//任务控制块
__attribute__((section (".RAM_CCM"))) OS_TCB tcbTaskServiceHeart;
//任务函数
void TaskFuncServiceHeart(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;
    while(1)
    {
        CoreDelayMs(200);
        BoardLedToogle(BOARD_LED_RED);
        SEGGER_RTT_WriteString(0,"Service Task Heart\r\n");
    }
}


















