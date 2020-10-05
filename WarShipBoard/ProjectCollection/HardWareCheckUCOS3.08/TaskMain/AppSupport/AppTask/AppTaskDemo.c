/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 21:54:18
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 01:46:48
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

//SD卡读取数据
void BoardSD_ReadTest()
{
    uint8_t* readBuf = UserMemMalloc(MEM_SRAM_IS62,512);
    uint16_t sectorAddr = MCU_RandomGetNextRangeReal(0,1000);
    ServicePrintf("**************************1****************************** \r\n");
    //读取数据
    if(0 == BoardSD_ReadDisk(readBuf,sectorAddr,1))
    {
        ServicePrintf("BoardSD_ReadDisk Success,Sector = %d\r\n",sectorAddr);
        for(uint16_t indexUtil = 0; indexUtil < 512; indexUtil++)
        {
            ServicePrintf("%d : 0X%02X ",indexUtil,readBuf[indexUtil]);
        }
        ServicePrintf("\r\n");
    }
    else
    {
        ServicePrintf("BoardSD_ReadDisk Failed,Sector = %d\r\n",sectorAddr);
    }
    ServicePrintf("**************************2****************************** \r\n");
    //数据进行随机处理
    for(uint16_t indexUtil = 0; indexUtil < 512; indexUtil++)
    {
        readBuf[indexUtil] = MCU_RandomGetNextRangeReal(0,255);
    }
    ServicePrintf("Random Sector = %d\r\n",sectorAddr);
    for(uint16_t indexUtil = 0; indexUtil < 512; indexUtil++)
    {
        ServicePrintf("%d : 0X%02X ",indexUtil,readBuf[indexUtil]);
    }
    //数据写入
    if(0 != BoardSD_WriteDisk(readBuf,sectorAddr,1))
    {
        ServicePrintf("BoardSD_WriteDisk Failed,Sector = %d\r\n",sectorAddr);
    }
    ServicePrintf("**************************3****************************** \r\n");
    UserMemSet(readBuf,0,512);
    if(0 == BoardSD_ReadDisk(readBuf,sectorAddr,1))
    {
        ServicePrintf("BoardSD_ReadDisk Success,Sector = %d\r\n",sectorAddr);
        for(uint16_t indexUtil = 0; indexUtil < 512; indexUtil++)
        {
            ServicePrintf("%d : 0X%02X ",indexUtil,readBuf[indexUtil]);
        }
        ServicePrintf("\r\n");
    }
    else
    {
        ServicePrintf("BoardSD_ReadDisk Failed,Sector = %d\r\n",sectorAddr);
    }
    //释放内存
    UserMemFree(MEM_SRAM_IS62,readBuf);
}


//任务函数
void TaskFuncAppDemo(void *p_arg)
{
    OS_ERR err;
    p_arg = p_arg;
    BoardSD_ReadTest();
    while(1)
    {
        CoreDelayMs(500);
        ReflushMPU6050State();
        
    }
}














