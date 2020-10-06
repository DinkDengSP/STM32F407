/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-10-06 20:45:32
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-06 21:15:34
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Third_Party\STemWin\Config\GUIConf.h
 *ModifyRecord1:    
******************************************************************/
#ifndef GUICONF_H
#define GUICONF_H

//显示的最大层数
#define GUI_NUM_LAYERS            2    // Maximum number of available layers

//是否支持操作系统
#define GUI_OS                    (1)  // Compile with multitasking support

//是否支持触摸
#define GUI_SUPPORT_TOUCH       (0)  // Support touchscreen

//默认字体
#define GUI_DEFAULT_FONT          &GUI_Font6x8

//鼠标支持
#define GUI_SUPPORT_MOUSE             (1)    /* Support a mouse */
//窗口管理支持
#define GUI_WINSUPPORT                (1)    /* Use window manager */
//存储设备支持
#define GUI_SUPPORT_MEMDEV            (1)    /* Memory device package available */
//设备指针支持
#define GUI_SUPPORT_DEVICES           (1)    /* Enable use of device pointers */

#endif  /* Avoid multiple inclusion */
