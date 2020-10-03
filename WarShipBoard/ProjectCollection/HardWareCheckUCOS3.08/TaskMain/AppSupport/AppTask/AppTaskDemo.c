/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 21:54:18
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-03 22:54:33
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
//任务函数
void TaskFuncAppDemo(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;
    while(1)
    {
        CoreDelayMs(500);
        BoardLedToogle(BOARD_LED_GREEN);
        //按键检测
        if(SENSOR_VALID == BoardKeyScanGetState(BOARD_KEY_UP))
        {
            SEGGER_RTT_WriteString(0,"BOARD_KEY_UP\r\n");
        }
        if(SENSOR_VALID == BoardKeyScanGetState(BOARD_KEY_DOWN))
        {
            SEGGER_RTT_WriteString(0,"BOARD_KEY_DOWN\r\n");
        }
        if(SENSOR_VALID == BoardKeyScanGetState(BOARD_KEY_LEFT))
        {
            SEGGER_RTT_WriteString(0,"BOARD_KEY_LEFT\r\n");
        }
        if(SENSOR_VALID == BoardKeyScanGetState(BOARD_KEY_RIGHT))
        {
            SEGGER_RTT_WriteString(0,"BOARD_KEY_RIGHT\r\n");
        }
    }
}














