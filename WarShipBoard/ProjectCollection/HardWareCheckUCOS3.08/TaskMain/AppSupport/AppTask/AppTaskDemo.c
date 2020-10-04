/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 21:54:18
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-05 00:36:43
**FilePath: \HardWareCheckUCOS3.08\TaskMain\AppSupport\AppTask\AppTaskDemo.c
**ModifyRecord1:    
******************************************************************/
#include "AppTaskConfig.h"

//任务控制块,8字节对齐
#pragma pack(8)
//任务堆栈
__attribute__((section (".RAM_CCM"))) CPU_STK stackBufferTaskAppDemo[STK_SIZE_TASK_APP_DEMO];
#pragma pack()

//任务控制块
__attribute__((section (".RAM_CCM"))) OS_TCB tcbTaskAppDemo;

//更新状态
void ReflushMPU6050State()
{
    uint8_t dmpResult = 0;
    float pitch,roll,yaw;
    //循环读取
    do
    {
        dmpResult = mpu_dmp_get_data(&pitch,&roll,&yaw);
    }while(dmpResult != 0);
    //打印结果
    if(dmpResult != 0)
    {
        //打印相关讯息
        SEGGER_RTT_printf(0,"mpu_dmp_get_data Failed,ErrorCode = 0X%08X\r\n",dmpResult);
        return;
    }
    SEGGER_RTT_printf(0,"pitch: %d, roll: %d, yaw: %d\r\n",(int32_t)(pitch*100),(int32_t)(roll*100),(int32_t)(yaw*100));
}


//任务函数
void TaskFuncAppDemo(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;
    while(1)
    {
        CoreDelayMs(500);
        ReflushMPU6050State();
    }
}














