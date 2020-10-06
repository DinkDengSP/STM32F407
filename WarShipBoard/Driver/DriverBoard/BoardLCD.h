/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-10-06 14:03:44
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-06 14:37:26
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardLcd.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_LCD_H_
#define __BOARD_LCD_H_
#include "MCU_Inc.h"

/**芯片ID定义,只支持该芯片的驱动 */
#define BOARD_LCD_DRIVER_IC_ID                 0X5510
//读取LCD寄存器的时候的等待时间 单位为US
#define LCD_READ_REG_WAIT_TIME		            5
//等待FSMC稳定时间 单位MS
#define LCD_FSMC_WAIT_START_TIME	            50
//等待芯片配置时间 单位MS
#define LCD_WAIT_CONFIG_TIME		            120

/**屏幕宽高设置 */
#define BOARD_LCD_HEIGHT                       800
#define BOARD_LCD_WIDTH                        480

/***************************扫描方向定义*******************************/
#define L2R_U2D  0//从左到右,从上到下
#define L2R_D2U  1//从左到右,从下到上
#define R2L_U2D  2//从右到左,从上到下
#define R2L_D2U  3//从右到左,从下到上
#define U2D_L2R  4//从上到下,从左到右
#define U2D_R2L  5//从上到下,从右到左
#define D2U_L2R  6//从下到上,从左到右
#define D2U_R2L  7//从下到上,从右到左

//默认的扫描方向 从左到右 从上到下
#define DFT_SCAN_DIR  L2R_U2D

/****************************颜色宏定义*************************/
#define WHITE         	 0xFFFF//白色
#define BLACK         	 0x0000//黑色
#define BLUE         	 0x001F//蓝色
#define BRED             0XF81F//蓝红
#define GRED 			 0XFFE0//红绿
#define GBLUE			 0X07FF//蓝黄
#define RED           	 0xF800//红色
#define MAGENTA       	 0xF81F//紫色
#define GREEN         	 0x07E0//绿色
#define CYAN          	 0x7FFF//灰色
#define YELLOW        	 0xFFE0//黄色
#define BROWN 			 0XBC40//棕色
#define BRRED 			 0XFC07//棕红色
#define GRAY  			 0X8430//灰色


/**LCD初始化 */
D_ERROR_CODE BoardLCD_Init(void);
/**LCD用指定颜色清除界面 */
void BoardLCD_Clear(uint16_t color);
/**开启显示 */
void BoardLCD_DisplayOn(void);
/**LCD关闭显示 */
void BoardLCD_DisplayOff(void);
//读取个某点的颜色值 
uint16_t BoardLCD_ReadPoint(u16 x,u16 y);
//画点
void BoardLCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
//快速画点
void BoardLCD_FastDrawPoint(uint16_t x,uint16_t y,uint16_t color);
//在指定区域内填充单个颜色
void BoardLCD_FillSingleColor(uint16_t startX,uint16_t startY,uint16_t endX,uint16_t endY,uint16_t color);


#endif

