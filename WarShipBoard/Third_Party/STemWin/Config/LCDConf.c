/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-10-06 20:45:32
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 22:07:02
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Third_Party\STemWin\Config\LCDConf_FlexColor_Template.c
**ModifyRecord1:    
******************************************************************/
#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "BoardLCD.h"

#define XSIZE_PHYS  BOARD_LCD_WIDTH // To be adapted to x-screen size
#define YSIZE_PHYS  BOARD_LCD_HEIGHT // To be adapted to y-screen size

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   GUICC_565
  #error Color conversion not defined!
#endif
#ifndef   GUIDRV_FLEXCOLOR
  #error No display driver defined!
#endif


//创建显示驱动器件
void LCD_X_Config(void) 
{
  GUI_DEVICE_CreateAndLink(&GUIDRV_Template_API, GUICC_M565, 0, 0); //创建显示驱动器件
	LCD_SetSizeEx    (0, XSIZE_PHYS, YSIZE_PHYS);
	LCD_SetVSizeEx   (0, XSIZE_PHYS, YSIZE_PHYS);
  
}

//显示驱动的回调函数
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) 
{
  int r;
  (void) LayerIndex;
  (void) pData;
  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    //当初始化的时候被调用,主要是设置显示控制器,如果显示控制器在外部初始化则需要用户初始化
	  //	TFTLCD_Init(); //初始化LCD 已经在开始初始化了,所以此处不需要初始化。
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/

