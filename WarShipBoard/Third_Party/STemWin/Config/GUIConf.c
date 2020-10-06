/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-10-06 20:45:32
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 21:19:51
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Third_Party\STemWin\Config\GUIConf.c
**ModifyRecord1:    
******************************************************************/
#include "GUI.h"
#include "UserMemManage.h"

//定义GUI使用的内存空间大小
#define GUI_NUMBYTES  (100*1024)

//初始化的时候调用,用于设置GUI使用的内存
void GUI_X_Config(void) 
{
    //申请内存
    uint32_t* aMemory = UserMemMalloc(MEM_SRAM_IS62,GUI_NUMBYTES);
    //给GUI设定显存
    GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
    //设置默认字体
    GUI_SetDefaultFont(GUI_FONT_6X8);
}

/*************************** End of file ****************************/
