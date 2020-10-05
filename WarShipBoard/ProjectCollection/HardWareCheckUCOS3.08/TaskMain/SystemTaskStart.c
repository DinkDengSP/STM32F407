/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:03:05
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 01:24:33
**FilePath: \HardWareCheckUCOS3.08\TaskMain\SystemTaskStart.c
**ModifyRecord1:    
**ModifyRecord2:    
**ModifyRecord3:    
**ModifyRecord4:    
**ModifyRecord5:    
******************************************************************/
#include "SystemTaskConfig.h"
#include "bsp_user.h"

//任务控制块,8字节对齐
#pragma pack(8)
//任务堆栈
CPU_STK stackBufferTaskStart[STK_SIZE_TASK_START];
#pragma pack()

//启动任务的堆栈
OS_TCB tcbTaskStart;

//板上外设初始化
void BoardDeviceInit(void);


//服务任务的配置
const static OS_TASK_CREATE_CONFIG serviceTaskConfigArray[] = {
    //心跳任务
    {&tcbTaskServiceHeart,NAME_TASK_SERVICE_HEART,TaskFuncServiceHeart,PRIO_TASK_SERVICE_HEART,&stackBufferTaskServiceHeart[0],STK_SIZE_TASK_SERVICE_HEART / 10,STK_SIZE_TASK_SERVICE_HEART,2},    
};
//服务任务的数量
#define COUNT_SERVICE_TASK (sizeof(serviceTaskConfigArray) / sizeof(serviceTaskConfigArray[0]))

//应用任务的配置
const static OS_TASK_CREATE_CONFIG appTaskConfigArray[] = {
    //应用层任务Demo
    {&tcbTaskAppDemo,NAME_TASK_APP_DEMO,TaskFuncAppDemo,PRIO_TASK_APP_DEMO,&stackBufferTaskAppDemo[0],STK_SIZE_TASK_APP_DEMO / 10,STK_SIZE_TASK_APP_DEMO,2},
};
//应用任务的数量
#define COUNT_APP_TASK (sizeof(appTaskConfigArray) / sizeof(appTaskConfigArray[0]))

//IPC数据结构初始化
void ServiceIpcDataInit(void);

void TaskFuncStart(void *p_arg)
{
    p_arg = p_arg;
    //辅助变量
        uint16_t indexUtil = 0;
    //临界段变量
        OS_ERR  os_err;
        CPU_SR_ALLOC();
    //默认为空的追踪函数
        OS_TRACE_INIT(); 
    //使能定时器以及启动定时器运行
        BSP_OS_TickEnable();
    //状态初始化
        #if OS_CFG_STAT_TASK_EN > 0u
            OSStatTaskCPUUsageInit(&os_err);                           
        #endif
    //中断时间测量初始化
        #ifdef CPU_CFG_INT_DIS_MEAS_EN
            CPU_IntDisMeasMaxCurReset();
        #endif
    //板上设备初始化
        BoardDeviceInit();
    //进入临界区
        CPU_CRITICAL_ENTER();
    /**************************************************创建系统服务任务**************************************************/
    for (indexUtil = 0; indexUtil < COUNT_SERVICE_TASK; indexUtil++)
    {
        OSTaskCreate((OS_TCB *)serviceTaskConfigArray[indexUtil].tcbTaskPtr,           //任务控制块
                     (CPU_CHAR *)serviceTaskConfigArray[indexUtil].taskNamePtr,        //任务名字
                     (OS_TASK_PTR)serviceTaskConfigArray[indexUtil].taskFuncPtr,       //任务函数
                     (void *)0,                                                        //传递给任务函数的参数
                     (OS_PRIO)serviceTaskConfigArray[indexUtil].taskPrio,              //任务优先级
                     (CPU_STK *)serviceTaskConfigArray[indexUtil].taskStkBasePtr,      //任务堆栈基地址
                     (CPU_STK_SIZE)serviceTaskConfigArray[indexUtil].taskStkSizeLimit, //任务堆栈深度限位
                     (CPU_STK_SIZE)serviceTaskConfigArray[indexUtil].taskStkSize,      //任务堆栈大小
                     (OS_MSG_QTY)serviceTaskConfigArray[indexUtil].msgQueenSize,       //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                     (OS_TICK)0,                                                       //当使能时间片轮转时的时间片长度，为0时为默认长度，
                     (void *)0,                                                        //用户补充的存储区
                     (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,                //任务选项
                     (OS_ERR *)&os_err);                                               //存放该函数错误时的返回值
    }
    /******************************************************创建应用服务任务***********************************************/
    for (indexUtil = 0; indexUtil < COUNT_APP_TASK; indexUtil++)
    {
        OSTaskCreate((OS_TCB *)appTaskConfigArray[indexUtil].tcbTaskPtr,           //任务控制块
                     (CPU_CHAR *)appTaskConfigArray[indexUtil].taskNamePtr,        //任务名字
                     (OS_TASK_PTR)appTaskConfigArray[indexUtil].taskFuncPtr,       //任务函数
                     (void *)0,                                                    //传递给任务函数的参数
                     (OS_PRIO)appTaskConfigArray[indexUtil].taskPrio,              //任务优先级
                     (CPU_STK *)appTaskConfigArray[indexUtil].taskStkBasePtr,      //任务堆栈基地址
                     (CPU_STK_SIZE)appTaskConfigArray[indexUtil].taskStkSizeLimit, //任务堆栈深度限位
                     (CPU_STK_SIZE)appTaskConfigArray[indexUtil].taskStkSize,      //任务堆栈大小
                     (OS_MSG_QTY)appTaskConfigArray[indexUtil].msgQueenSize,       //任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                     (OS_TICK)0,                                                   //当使能时间片轮转时的时间片长度，为0时为默认长度，
                     (void *)0,                                                    //用户补充的存储区
                     (OS_OPT)OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,            //任务选项
                     (OS_ERR *)&os_err);                                           //存放该函数错误时的返回值
    }
    //服务层IPC数据结构初始化
        ServiceIpcDataInit();
    //退出临界区
        CPU_CRITICAL_EXIT();
    //删除start_task任务自身
        OSTaskDel((OS_TCB *)0, &os_err);
}


//板上外设初始化
void BoardDeviceInit(void)
{
    D_ERROR_CODE deviceInitResult = D_ERROR_CODE_NONE;
    //初始化LED灯
        BoardLedInit(BOARD_LED_RED,OUTPUT_VALID);
        BoardLedInit(BOARD_LED_GREEN,OUTPUT_VALID);
    //初始化Beep
        BoardBeepInit(OUTPUT_INVALID);
    //打开蜂鸣器
        BoardBeepSetState(OUTPUT_VALID);
    //延时
        CoreDelayMs(500);
    //关闭蜂鸣器
        BoardBeepSetState(OUTPUT_INVALID);
    //串口初始化
        MCU_Uart1Init(115200,MCU_UART_LENGTH8,MCU_UART_STOPBIT1,MCU_UART_CHECK_MODE_NONE,MCU_UART_HARD_CONTROL_NONE,NULL);
    //初始化随机数发生器
        MCU_RandomInit();
    //内部内存管理和CCM内存管理初始化
        UserMemInit(MEM_SRAM_IN);
        UserMemInit(MEM_SRAM_CCM);
    //初始化SRAM,并进行自检
        BoardSRAM_Init();
        do
        {
            deviceInitResult = BoardSRAM_SelfCheck();
            if(deviceInitResult != D_ERROR_CODE_NONE)
            {
                //红灯闪烁
                BoardLedToogle(BOARD_LED_RED);
                //输出日志
                SEGGER_RTT_printf(0,"BoardSRAM_SelfCheck Failed,ErrorCode = 0X%08X\r\n",deviceInitResult);
                //延时等待
                CoreDelayMs(500);
            }
        } while (deviceInitResult != D_ERROR_CODE_NONE);
    //SRAM内存管理初始化
        UserMemInit(MEM_SRAM_IS62);
    //初始化AT24CXX IIC接口,并进行自检
        BoardAT24CXX_PortInit();
        do
        {
            deviceInitResult = BoardAT24CXX_SelfCheck();
            if(deviceInitResult != D_ERROR_CODE_NONE)
            {
                //红灯闪烁
                BoardLedToogle(BOARD_LED_RED);
                //输出日志
                SEGGER_RTT_printf(0,"BoardAT24CXX_SelfCheck Failed,ErrorCode = 0X%08X\r\n",deviceInitResult);
                //延时等待
                CoreDelayMs(500);
            }
        } while (deviceInitResult != D_ERROR_CODE_NONE);
    //初始化MPU6050,并运行自检
        BoardMPU6050_Init();
        do
        {
            deviceInitResult = BoardMPU6050_ConfigAndCheck();
            if(deviceInitResult != D_ERROR_CODE_NONE)
            {
                //红灯闪烁
                BoardLedToogle(BOARD_LED_RED);
                //输出日志
                SEGGER_RTT_printf(0,"BoardMPU6050_ConfigAndCheck Failed,ErrorCode = 0X%08X\r\n",deviceInitResult);
                //延时等待
                CoreDelayMs(500);
            }
        } while (deviceInitResult != D_ERROR_CODE_NONE);
        //启动DMP姿态计算功能
        uint8_t dmpResult;
        do
        {
            dmpResult = mpu_dmp_init();
            if(dmpResult != 0)
            {
                //红灯闪烁
                BoardLedToogle(BOARD_LED_RED);
                //输出日志
                SEGGER_RTT_printf(0,"mpu_dmp_init Failed,ErrorCode = 0X%08X\r\n",dmpResult);
                //延时等待
                CoreDelayMs(500);
            }
        } while (dmpResult != 0);
    //SPI Flash W25QXX初始化,同时自检
        W25QXX_Init();
        do
        {
            deviceInitResult = W25QXX_SelfCheck();
            if(deviceInitResult != D_ERROR_CODE_NONE)
            {
                //红灯闪烁
                BoardLedToogle(BOARD_LED_RED);
                //输出日志
                SEGGER_RTT_printf(0,"W25QXX_SelfCheck Failed,ErrorCode = 0X%08X\r\n",deviceInitResult);
                //延时等待
                CoreDelayMs(500);
            }
        } while (deviceInitResult != D_ERROR_CODE_NONE);
    //SD卡初始化并打印卡讯息
        SD_ERROR sdErrorCode;
        BoardSD_PortInit();
        do
        {
            sdErrorCode = BoardSD_Init();
            if(sdErrorCode != SD_OK)
            {
                //红灯闪烁
                BoardLedToogle(BOARD_LED_RED);
                //输出日志
                SEGGER_RTT_printf(0,"BoardSD_Init Failed,ErrorCode = 0X%08X\r\n",sdErrorCode);
                //延时等待
                CoreDelayMs(500);
            }
        } while (sdErrorCode != SD_OK);
        ConsoleSendCardMessage();
    //独立看门狗中断
        MCU_IWDG_Init(MCU_IWDG_DEFAULT_PRER,MCU_IWDG_DEFAULT_RLR);
        

    //系统初始化完成,关闭灯光
        BoardLedWrite(BOARD_LED_RED,OUTPUT_INVALID);
        BoardLedWrite(BOARD_LED_GREEN,OUTPUT_INVALID);
    //系统启动完成  
        SEGGER_RTT_WriteString(0,"BoardDeviceInit Success\r\n");
        ServicePrintf("Board SystemStart1\r\n");
}


//IPC数据结构初始化
void ServiceIpcDataInit(void)
{
    
}








