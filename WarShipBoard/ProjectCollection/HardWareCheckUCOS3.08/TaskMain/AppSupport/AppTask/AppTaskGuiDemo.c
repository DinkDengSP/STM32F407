/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-10-06 22:33:21
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 22:36:58
**FilePath: \HardWareCheckUCOS3.08\TaskMain\AppSupport\AppTask\AppTaskGuiDemo.c
**ModifyRecord1:    
******************************************************************/
#include "AppTaskConfig.h"

//任务控制块,8字节对齐
#pragma pack(8)
//任务堆栈
__attribute__((section (".RAM_CCM"))) CPU_STK stackBufferTaskAppGuiDemo[STK_SIZE_TASK_APP_GUI_DEMO];
#pragma pack()

//任务控制块
__attribute__((section (".RAM_CCM"))) OS_TCB tcbTaskAppGuiDemo;



//任务函数
void TaskFuncAppGuiDemo(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;
    while(1)
    {
        GUIDEMO_Main(); 
    }
}
