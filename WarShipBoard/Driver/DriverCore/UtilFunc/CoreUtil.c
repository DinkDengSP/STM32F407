/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 21:46:12
**LastEditors: DengXiaoJun
**LastEditTime: 2020-09-26 22:40:04
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverCore\UtilFunc\CoreUtil.c
**ModifyRecord1:    
**ModifyRecord2:    
**ModifyRecord3:    
**ModifyRecord4:    
**ModifyRecord5:    
******************************************************************/
#include "CoreUtil.h"

void WFI_SET(void)
{
	__asm("WFI");
}

//关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{
	__asm("CPSID   I");
	__asm("BX      LR");
}

//开启所有中断
void INTX_ENABLE(void)
{
	__asm("CPSIE   I");
	__asm("BX      LR");
}

//设置栈顶地址
//addr:栈顶地址
//set Main Stack value
void MSR_MSP(uint32_t addr)
{
	__asm("MSR MSP, r0");
	__asm("BX r14");
}

/*设置默认的中断优先级分组*/
void CoreNVIC_SetGroupDefault(void)
{
    /*系统NVIC中断分组初始化*/
	NVIC_PriorityGroupConfig(SYSTEM_DEFAULT_NVIC_GROUP);
}





