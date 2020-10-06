/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-10-06 14:03:36
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 21:04:28
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardLcd.c
**ModifyRecord1:    
******************************************************************/
#include "BoardLCD.h"

/**LCD背光控制 */
#define	BoardLCD_BackLightSet(n) 		(PBout(15) = n)

//使用NOR/SRAM的 Bank1.sector4,地址位HADDR[27,26]=11
//A6作为数据命令区分线
//注意设置时STM32内部会右移一位对其! 111 1110=0X7E
//LCD基地址
#define BOARD_LCD_BASE_ADDR        ((uint32_t)(0x6C000000 | 0x0000007E))

//LCD地址结构体
typedef struct
{
	//寄存器地址映射
	volatile uint16_t BoardLCD_Reg;
	//数据RAM地址映射
	volatile uint16_t BoardLCD_Ram;
} BoardLCD_TypeDef;

//通过基地址定义寄存器写入地址和缓冲区地址
#define BoardLCD             			((BoardLCD_TypeDef *) BOARD_LCD_BASE_ADDR)

/**写缓存指令,针对5510芯片 */
#define BOARD_LCD_DRIVER_IC_WRITE_RAM_CMD			0X2C00
/**设置X坐标指令,针对5510芯片 */
#define BOARD_LCD_DRIVER_IC_SET_X_CMD				0X2A00
/**设置Y坐标指令,针对5510芯片 */
#define BOARD_LCD_DRIVER_IC_SET_Y_CMD				0X2B00

//m^n函数
//返回值:m^n次方.
static uint32_t BoardLCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)
		result*=m;
	return result;
}

//写寄存器地址函数
static void BoardLCD_WriteRegAddrWithOutData(volatile uint16_t regval)
{
	//使用-O2优化的时候,必须插入的延时
	regval=regval;
	//写入要写的寄存器序号
	BoardLCD->BoardLCD_Reg=regval;
}

//写LCD数据
static void BoardLCD_WriteDataWithOutRegAddr(volatile uint16_t data)
{
	//使用-O2优化的时候,必须插入的延时
	data=data;
	BoardLCD->BoardLCD_Ram=data;
}

//读LCD数据
static uint16_t BoardLCD_ReadDataWithOutRegAddr(void)
{
	//防止被优化
	volatile uint16_t ram;
	ram=BoardLCD->BoardLCD_Ram;
	return ram;
}

/**写指定寄存器地址的指定寄存器数据 */
static void BoardLCD_WriteRegWithData(uint16_t regAddr,uint16_t regValue)
{
	//写入要写的寄存器序号
	BoardLCD->BoardLCD_Reg = regAddr;
	//写入数据
	BoardLCD->BoardLCD_Ram = regValue;
}

/**读指定寄存器数据 */
static uint16_t BoardLCD_ReadDataWithRegAddr(uint16_t redAddr)
{
	//写入要读的寄存器序号
	BoardLCD_WriteRegAddrWithOutData(redAddr);
	CoreDelayUs(LCD_READ_REG_WAIT_TIME);
	//返回读到的值
	return BoardLCD_ReadDataWithOutRegAddr();
}


/**IO初始化 */
static void BoardLCD_IO_Init()
{
	volatile uint32_t i=0;

	GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;
	//使能FSMC时钟
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);

	//PB15 推挽输出,控制背光
	MCU_PortInit(MCU_PIN_B_15, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);

	//PD0,1,4,5,8,9,10,14,15 AF OUT
	MCU_PortInit(MCU_PIN_D_0, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_1, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_4, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_5, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_8, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_9, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_10, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_14, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_D_15, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	//PE7~15,AF OUT
	MCU_PortInit(MCU_PIN_E_7, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_8, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_9, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_10, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_11, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_12, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_13, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_14, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	MCU_PortInit(MCU_PIN_E_15, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	//PF12
	MCU_PortInit(MCU_PIN_F_12, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
	//PG12
	MCU_PortInit(MCU_PIN_G_12, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);

	//PD0,1,4,5,8,9,10,14,15 AF OUT
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);
	//PE7-PE15
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);
	//PF12,PG12
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);

	//地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
	readWriteTiming.FSMC_AddressSetupTime = 0XF;
	//地址保持时间（ADDHLD）模式A未用到
	readWriteTiming.FSMC_AddressHoldTime = 0x00;
	//数据保存时间为60个HCLK	=6*60=360ns
	readWriteTiming.FSMC_DataSetupTime = 60;
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;
	//模式A
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	//地址建立时间（ADDSET）为9个HCLK =54ns
	writeTiming.FSMC_AddressSetupTime =9;
	//地址保持时间（A
	writeTiming.FSMC_AddressHoldTime = 0x00;
	//数据保存时间为6ns*9个HCLK=54ns
	writeTiming.FSMC_DataSetupTime = 8;
	writeTiming.FSMC_BusTurnAroundDuration = 0x00;
	writeTiming.FSMC_CLKDivision = 0x00;
	writeTiming.FSMC_DataLatency = 0x00;
	//模式A
	writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

	//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	// 不复用数据地址
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	// FSMC_MemoryType_SRAM;  //SRAM
	FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
	//存储器数据宽度为16bit
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	// FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	//  存储器写使能
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	// 读写使用不同的时序
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	//写时序
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	//写时序
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;
	//初始化FSMC配置
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	// 使能BANK1
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
	//等待FSMC稳定
 	CoreDelayMs(LCD_FSMC_WAIT_START_TIME);
 	BoardLCD_WriteRegWithData(0x0000,0x0001);
	//等待FSMC稳定
	CoreDelayMs(LCD_FSMC_WAIT_START_TIME);
}


/**读取芯片的ID */
static uint16_t BoardLCD_ReadID()
{
	uint16_t deviceID = 0;
	deviceID = BoardLCD_ReadDataWithRegAddr(0x0000);
	//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
   	if(deviceID < 0XFF||deviceID == 0XFFFF||deviceID == 0X9300)
	{
		//尝试9341 ID的读取
		BoardLCD_WriteRegAddrWithOutData(0XD3);
		//dummy read
		deviceID = BoardLCD_ReadDataWithOutRegAddr();
		//读到0X00
 		deviceID = BoardLCD_ReadDataWithOutRegAddr();
		//读取93
  		deviceID = BoardLCD_ReadDataWithOutRegAddr();
 		deviceID <<= 8;
		//读取41
		deviceID |= BoardLCD_ReadDataWithOutRegAddr();
		//非9341,尝试是不是6804
		if(deviceID != 0X9341)
		{
			BoardLCD_WriteRegAddrWithOutData(0XBF);
			//dummy read
			deviceID = BoardLCD_ReadDataWithOutRegAddr();
			//读回0X01
	 		deviceID = BoardLCD_ReadDataWithOutRegAddr();
			//读回0XD0
	 		deviceID = BoardLCD_ReadDataWithOutRegAddr();
			//这里读回0X68
	  		deviceID = BoardLCD_ReadDataWithOutRegAddr();
			deviceID <<= 8;
			//这里读回0X04
	  		deviceID |= BoardLCD_ReadDataWithOutRegAddr();
			//也不是6804,尝试看看是不是NT35310
			if(deviceID != 0X6804)
			{
				BoardLCD_WriteRegAddrWithOutData(0XD4);
				//dummy read
				deviceID = BoardLCD_ReadDataWithOutRegAddr();
				//读回0X01
				deviceID = BoardLCD_ReadDataWithOutRegAddr();
				//读回0X53
				deviceID = BoardLCD_ReadDataWithOutRegAddr();
				deviceID <<= 8;
				//这里读回0X10
				deviceID |= BoardLCD_ReadDataWithOutRegAddr();
				//也不是NT35310,尝试看看是不是NT35510
				if(deviceID != 0X5310)
				{
					BoardLCD_WriteRegAddrWithOutData(0XDA00);
					//读回0X00
					deviceID =BoardLCD_ReadDataWithOutRegAddr();
					BoardLCD_WriteRegAddrWithOutData(0XDB00);
					//读回0X80
					deviceID = BoardLCD_ReadDataWithOutRegAddr();
					deviceID <<= 8;
					BoardLCD_WriteRegAddrWithOutData(0XDC00);
					//读回0X00
					deviceID |= BoardLCD_ReadDataWithOutRegAddr();
					//NT35510读回的ID是8000H,为方便区分,我们强制设置为5510
					if(deviceID == 0x8000)
						deviceID = 0x5510;
					//也不是NT5510,尝试看看是不是SSD1963
					if(deviceID != 0X5510)
					{
						BoardLCD_WriteRegAddrWithOutData(0XA1);
						deviceID = BoardLCD_ReadDataWithOutRegAddr();
						//读回0X57
						deviceID = BoardLCD_ReadDataWithOutRegAddr();
						deviceID <<= 8;
						//读回0X61
						deviceID |= BoardLCD_ReadDataWithOutRegAddr();
						//SSD1963读回的ID是5761H,为方便区分,我们强制设置为1963
						if(deviceID == 0X5761)
							deviceID = 0X1963;
					}
				}
			}
		}
	}
	return deviceID;
}

static void Device_LCD_FSMC_SpeedReConfig()
{
	/**设置WR时序最快 */
	//重新配置写时序控制寄存器的时序
	//地址建立时间(ADDSET)清零
	FSMC_Bank1E->BWTR[6]&=~(0XF<<0);
	//数据保存时间清零
	FSMC_Bank1E->BWTR[6]&=~(0XF<<8);
	//地址建立时间(ADDSET)为3个HCLK =18ns
	FSMC_Bank1E->BWTR[6]|=3<<0;
	//数据保存时间(DATAST)为6ns*3个HCLK=18ns
	FSMC_Bank1E->BWTR[6]|=2<<8;
}


/**5510芯片的初始化 */
/**配置SSD芯片 */
static void BoardLCD_SSD_5510_Init(void)
{
	BoardLCD_WriteRegWithData(0xF000,0x55);
	BoardLCD_WriteRegWithData(0xF001,0xAA);
	BoardLCD_WriteRegWithData(0xF002,0x52);
	BoardLCD_WriteRegWithData(0xF003,0x08);
	BoardLCD_WriteRegWithData(0xF004,0x01);
	//AVDD Set AVDD 5.2V
	BoardLCD_WriteRegWithData(0xB000,0x0D);
	BoardLCD_WriteRegWithData(0xB001,0x0D);
	BoardLCD_WriteRegWithData(0xB002,0x0D);
	//AVDD ratio
	BoardLCD_WriteRegWithData(0xB600,0x34);
	BoardLCD_WriteRegWithData(0xB601,0x34);
	BoardLCD_WriteRegWithData(0xB602,0x34);
	//AVEE -5.2V
	BoardLCD_WriteRegWithData(0xB100,0x0D);
	BoardLCD_WriteRegWithData(0xB101,0x0D);
	BoardLCD_WriteRegWithData(0xB102,0x0D);
	//AVEE ratio
	BoardLCD_WriteRegWithData(0xB700,0x34);
	BoardLCD_WriteRegWithData(0xB701,0x34);
	BoardLCD_WriteRegWithData(0xB702,0x34);
	//VCL -2.5V
	BoardLCD_WriteRegWithData(0xB200,0x00);
	BoardLCD_WriteRegWithData(0xB201,0x00);
	BoardLCD_WriteRegWithData(0xB202,0x00);
	//VCL ratio
	BoardLCD_WriteRegWithData(0xB800,0x24);
	BoardLCD_WriteRegWithData(0xB801,0x24);
	BoardLCD_WriteRegWithData(0xB802,0x24);
	//VGH 15V (Free pump)
	BoardLCD_WriteRegWithData(0xBF00,0x01);
	BoardLCD_WriteRegWithData(0xB300,0x0F);
	BoardLCD_WriteRegWithData(0xB301,0x0F);
	BoardLCD_WriteRegWithData(0xB302,0x0F);
	//VGH ratio
	BoardLCD_WriteRegWithData(0xB900,0x34);
	BoardLCD_WriteRegWithData(0xB901,0x34);
	BoardLCD_WriteRegWithData(0xB902,0x34);
	//VGL_REG -10V
	BoardLCD_WriteRegWithData(0xB500,0x08);
	BoardLCD_WriteRegWithData(0xB501,0x08);
	BoardLCD_WriteRegWithData(0xB502,0x08);
	BoardLCD_WriteRegWithData(0xC200,0x03);
	//VGLX ratio
	BoardLCD_WriteRegWithData(0xBA00,0x24);
	BoardLCD_WriteRegWithData(0xBA01,0x24);
	BoardLCD_WriteRegWithData(0xBA02,0x24);
	//VGMP/VGSP 4.5V/0V
	BoardLCD_WriteRegWithData(0xBC00,0x00);
	BoardLCD_WriteRegWithData(0xBC01,0x78);
	BoardLCD_WriteRegWithData(0xBC02,0x00);
	//VGMN/VGSN -4.5V/0V
	BoardLCD_WriteRegWithData(0xBD00,0x00);
	BoardLCD_WriteRegWithData(0xBD01,0x78);
	BoardLCD_WriteRegWithData(0xBD02,0x00);
	//VCOM
	BoardLCD_WriteRegWithData(0xBE00,0x00);
	BoardLCD_WriteRegWithData(0xBE01,0x64);
	//Gamma Setting
	BoardLCD_WriteRegWithData(0xD100,0x00);
	BoardLCD_WriteRegWithData(0xD101,0x33);
	BoardLCD_WriteRegWithData(0xD102,0x00);
	BoardLCD_WriteRegWithData(0xD103,0x34);
	BoardLCD_WriteRegWithData(0xD104,0x00);
	BoardLCD_WriteRegWithData(0xD105,0x3A);
	BoardLCD_WriteRegWithData(0xD106,0x00);
	BoardLCD_WriteRegWithData(0xD107,0x4A);
	BoardLCD_WriteRegWithData(0xD108,0x00);
	BoardLCD_WriteRegWithData(0xD109,0x5C);
	BoardLCD_WriteRegWithData(0xD10A,0x00);
	BoardLCD_WriteRegWithData(0xD10B,0x81);
	BoardLCD_WriteRegWithData(0xD10C,0x00);
	BoardLCD_WriteRegWithData(0xD10D,0xA6);
	BoardLCD_WriteRegWithData(0xD10E,0x00);
	BoardLCD_WriteRegWithData(0xD10F,0xE5);
	BoardLCD_WriteRegWithData(0xD110,0x01);
	BoardLCD_WriteRegWithData(0xD111,0x13);
	BoardLCD_WriteRegWithData(0xD112,0x01);
	BoardLCD_WriteRegWithData(0xD113,0x54);
	BoardLCD_WriteRegWithData(0xD114,0x01);
	BoardLCD_WriteRegWithData(0xD115,0x82);
	BoardLCD_WriteRegWithData(0xD116,0x01);
	BoardLCD_WriteRegWithData(0xD117,0xCA);
	BoardLCD_WriteRegWithData(0xD118,0x02);
	BoardLCD_WriteRegWithData(0xD119,0x00);
	BoardLCD_WriteRegWithData(0xD11A,0x02);
	BoardLCD_WriteRegWithData(0xD11B,0x01);
	BoardLCD_WriteRegWithData(0xD11C,0x02);
	BoardLCD_WriteRegWithData(0xD11D,0x34);
	BoardLCD_WriteRegWithData(0xD11E,0x02);
	BoardLCD_WriteRegWithData(0xD11F,0x67);
	BoardLCD_WriteRegWithData(0xD120,0x02);
	BoardLCD_WriteRegWithData(0xD121,0x84);
	BoardLCD_WriteRegWithData(0xD122,0x02);
	BoardLCD_WriteRegWithData(0xD123,0xA4);
	BoardLCD_WriteRegWithData(0xD124,0x02);
	BoardLCD_WriteRegWithData(0xD125,0xB7);
	BoardLCD_WriteRegWithData(0xD126,0x02);
	BoardLCD_WriteRegWithData(0xD127,0xCF);
	BoardLCD_WriteRegWithData(0xD128,0x02);
	BoardLCD_WriteRegWithData(0xD129,0xDE);
	BoardLCD_WriteRegWithData(0xD12A,0x02);
	BoardLCD_WriteRegWithData(0xD12B,0xF2);
	BoardLCD_WriteRegWithData(0xD12C,0x02);
	BoardLCD_WriteRegWithData(0xD12D,0xFE);
	BoardLCD_WriteRegWithData(0xD12E,0x03);
	BoardLCD_WriteRegWithData(0xD12F,0x10);
	BoardLCD_WriteRegWithData(0xD130,0x03);
	BoardLCD_WriteRegWithData(0xD131,0x33);
	BoardLCD_WriteRegWithData(0xD132,0x03);
	BoardLCD_WriteRegWithData(0xD133,0x6D);
	BoardLCD_WriteRegWithData(0xD200,0x00);
	BoardLCD_WriteRegWithData(0xD201,0x33);
	BoardLCD_WriteRegWithData(0xD202,0x00);
	BoardLCD_WriteRegWithData(0xD203,0x34);
	BoardLCD_WriteRegWithData(0xD204,0x00);
	BoardLCD_WriteRegWithData(0xD205,0x3A);
	BoardLCD_WriteRegWithData(0xD206,0x00);
	BoardLCD_WriteRegWithData(0xD207,0x4A);
	BoardLCD_WriteRegWithData(0xD208,0x00);
	BoardLCD_WriteRegWithData(0xD209,0x5C);
	BoardLCD_WriteRegWithData(0xD20A,0x00);
	BoardLCD_WriteRegWithData(0xD20B,0x81);
	BoardLCD_WriteRegWithData(0xD20C,0x00);
	BoardLCD_WriteRegWithData(0xD20D,0xA6);
	BoardLCD_WriteRegWithData(0xD20E,0x00);
	BoardLCD_WriteRegWithData(0xD20F,0xE5);
	BoardLCD_WriteRegWithData(0xD210,0x01);
	BoardLCD_WriteRegWithData(0xD211,0x13);
	BoardLCD_WriteRegWithData(0xD212,0x01);
	BoardLCD_WriteRegWithData(0xD213,0x54);
	BoardLCD_WriteRegWithData(0xD214,0x01);
	BoardLCD_WriteRegWithData(0xD215,0x82);
	BoardLCD_WriteRegWithData(0xD216,0x01);
	BoardLCD_WriteRegWithData(0xD217,0xCA);
	BoardLCD_WriteRegWithData(0xD218,0x02);
	BoardLCD_WriteRegWithData(0xD219,0x00);
	BoardLCD_WriteRegWithData(0xD21A,0x02);
	BoardLCD_WriteRegWithData(0xD21B,0x01);
	BoardLCD_WriteRegWithData(0xD21C,0x02);
	BoardLCD_WriteRegWithData(0xD21D,0x34);
	BoardLCD_WriteRegWithData(0xD21E,0x02);
	BoardLCD_WriteRegWithData(0xD21F,0x67);
	BoardLCD_WriteRegWithData(0xD220,0x02);
	BoardLCD_WriteRegWithData(0xD221,0x84);
	BoardLCD_WriteRegWithData(0xD222,0x02);
	BoardLCD_WriteRegWithData(0xD223,0xA4);
	BoardLCD_WriteRegWithData(0xD224,0x02);
	BoardLCD_WriteRegWithData(0xD225,0xB7);
	BoardLCD_WriteRegWithData(0xD226,0x02);
	BoardLCD_WriteRegWithData(0xD227,0xCF);
	BoardLCD_WriteRegWithData(0xD228,0x02);
	BoardLCD_WriteRegWithData(0xD229,0xDE);
	BoardLCD_WriteRegWithData(0xD22A,0x02);
	BoardLCD_WriteRegWithData(0xD22B,0xF2);
	BoardLCD_WriteRegWithData(0xD22C,0x02);
	BoardLCD_WriteRegWithData(0xD22D,0xFE);
	BoardLCD_WriteRegWithData(0xD22E,0x03);
	BoardLCD_WriteRegWithData(0xD22F,0x10);
	BoardLCD_WriteRegWithData(0xD230,0x03);
	BoardLCD_WriteRegWithData(0xD231,0x33);
	BoardLCD_WriteRegWithData(0xD232,0x03);
	BoardLCD_WriteRegWithData(0xD233,0x6D);
	BoardLCD_WriteRegWithData(0xD300,0x00);
	BoardLCD_WriteRegWithData(0xD301,0x33);
	BoardLCD_WriteRegWithData(0xD302,0x00);
	BoardLCD_WriteRegWithData(0xD303,0x34);
	BoardLCD_WriteRegWithData(0xD304,0x00);
	BoardLCD_WriteRegWithData(0xD305,0x3A);
	BoardLCD_WriteRegWithData(0xD306,0x00);
	BoardLCD_WriteRegWithData(0xD307,0x4A);
	BoardLCD_WriteRegWithData(0xD308,0x00);
	BoardLCD_WriteRegWithData(0xD309,0x5C);
	BoardLCD_WriteRegWithData(0xD30A,0x00);
	BoardLCD_WriteRegWithData(0xD30B,0x81);
	BoardLCD_WriteRegWithData(0xD30C,0x00);
	BoardLCD_WriteRegWithData(0xD30D,0xA6);
	BoardLCD_WriteRegWithData(0xD30E,0x00);
	BoardLCD_WriteRegWithData(0xD30F,0xE5);
	BoardLCD_WriteRegWithData(0xD310,0x01);
	BoardLCD_WriteRegWithData(0xD311,0x13);
	BoardLCD_WriteRegWithData(0xD312,0x01);
	BoardLCD_WriteRegWithData(0xD313,0x54);
	BoardLCD_WriteRegWithData(0xD314,0x01);
	BoardLCD_WriteRegWithData(0xD315,0x82);
	BoardLCD_WriteRegWithData(0xD316,0x01);
	BoardLCD_WriteRegWithData(0xD317,0xCA);
	BoardLCD_WriteRegWithData(0xD318,0x02);
	BoardLCD_WriteRegWithData(0xD319,0x00);
	BoardLCD_WriteRegWithData(0xD31A,0x02);
	BoardLCD_WriteRegWithData(0xD31B,0x01);
	BoardLCD_WriteRegWithData(0xD31C,0x02);
	BoardLCD_WriteRegWithData(0xD31D,0x34);
	BoardLCD_WriteRegWithData(0xD31E,0x02);
	BoardLCD_WriteRegWithData(0xD31F,0x67);
	BoardLCD_WriteRegWithData(0xD320,0x02);
	BoardLCD_WriteRegWithData(0xD321,0x84);
	BoardLCD_WriteRegWithData(0xD322,0x02);
	BoardLCD_WriteRegWithData(0xD323,0xA4);
	BoardLCD_WriteRegWithData(0xD324,0x02);
	BoardLCD_WriteRegWithData(0xD325,0xB7);
	BoardLCD_WriteRegWithData(0xD326,0x02);
	BoardLCD_WriteRegWithData(0xD327,0xCF);
	BoardLCD_WriteRegWithData(0xD328,0x02);
	BoardLCD_WriteRegWithData(0xD329,0xDE);
	BoardLCD_WriteRegWithData(0xD32A,0x02);
	BoardLCD_WriteRegWithData(0xD32B,0xF2);
	BoardLCD_WriteRegWithData(0xD32C,0x02);
	BoardLCD_WriteRegWithData(0xD32D,0xFE);
	BoardLCD_WriteRegWithData(0xD32E,0x03);
	BoardLCD_WriteRegWithData(0xD32F,0x10);
	BoardLCD_WriteRegWithData(0xD330,0x03);
	BoardLCD_WriteRegWithData(0xD331,0x33);
	BoardLCD_WriteRegWithData(0xD332,0x03);
	BoardLCD_WriteRegWithData(0xD333,0x6D);
	BoardLCD_WriteRegWithData(0xD400,0x00);
	BoardLCD_WriteRegWithData(0xD401,0x33);
	BoardLCD_WriteRegWithData(0xD402,0x00);
	BoardLCD_WriteRegWithData(0xD403,0x34);
	BoardLCD_WriteRegWithData(0xD404,0x00);
	BoardLCD_WriteRegWithData(0xD405,0x3A);
	BoardLCD_WriteRegWithData(0xD406,0x00);
	BoardLCD_WriteRegWithData(0xD407,0x4A);
	BoardLCD_WriteRegWithData(0xD408,0x00);
	BoardLCD_WriteRegWithData(0xD409,0x5C);
	BoardLCD_WriteRegWithData(0xD40A,0x00);
	BoardLCD_WriteRegWithData(0xD40B,0x81);
	BoardLCD_WriteRegWithData(0xD40C,0x00);
	BoardLCD_WriteRegWithData(0xD40D,0xA6);
	BoardLCD_WriteRegWithData(0xD40E,0x00);
	BoardLCD_WriteRegWithData(0xD40F,0xE5);
	BoardLCD_WriteRegWithData(0xD410,0x01);
	BoardLCD_WriteRegWithData(0xD411,0x13);
	BoardLCD_WriteRegWithData(0xD412,0x01);
	BoardLCD_WriteRegWithData(0xD413,0x54);
	BoardLCD_WriteRegWithData(0xD414,0x01);
	BoardLCD_WriteRegWithData(0xD415,0x82);
	BoardLCD_WriteRegWithData(0xD416,0x01);
	BoardLCD_WriteRegWithData(0xD417,0xCA);
	BoardLCD_WriteRegWithData(0xD418,0x02);
	BoardLCD_WriteRegWithData(0xD419,0x00);
	BoardLCD_WriteRegWithData(0xD41A,0x02);
	BoardLCD_WriteRegWithData(0xD41B,0x01);
	BoardLCD_WriteRegWithData(0xD41C,0x02);
	BoardLCD_WriteRegWithData(0xD41D,0x34);
	BoardLCD_WriteRegWithData(0xD41E,0x02);
	BoardLCD_WriteRegWithData(0xD41F,0x67);
	BoardLCD_WriteRegWithData(0xD420,0x02);
	BoardLCD_WriteRegWithData(0xD421,0x84);
	BoardLCD_WriteRegWithData(0xD422,0x02);
	BoardLCD_WriteRegWithData(0xD423,0xA4);
	BoardLCD_WriteRegWithData(0xD424,0x02);
	BoardLCD_WriteRegWithData(0xD425,0xB7);
	BoardLCD_WriteRegWithData(0xD426,0x02);
	BoardLCD_WriteRegWithData(0xD427,0xCF);
	BoardLCD_WriteRegWithData(0xD428,0x02);
	BoardLCD_WriteRegWithData(0xD429,0xDE);
	BoardLCD_WriteRegWithData(0xD42A,0x02);
	BoardLCD_WriteRegWithData(0xD42B,0xF2);
	BoardLCD_WriteRegWithData(0xD42C,0x02);
	BoardLCD_WriteRegWithData(0xD42D,0xFE);
	BoardLCD_WriteRegWithData(0xD42E,0x03);
	BoardLCD_WriteRegWithData(0xD42F,0x10);
	BoardLCD_WriteRegWithData(0xD430,0x03);
	BoardLCD_WriteRegWithData(0xD431,0x33);
	BoardLCD_WriteRegWithData(0xD432,0x03);
	BoardLCD_WriteRegWithData(0xD433,0x6D);
	BoardLCD_WriteRegWithData(0xD500,0x00);
	BoardLCD_WriteRegWithData(0xD501,0x33);
	BoardLCD_WriteRegWithData(0xD502,0x00);
	BoardLCD_WriteRegWithData(0xD503,0x34);
	BoardLCD_WriteRegWithData(0xD504,0x00);
	BoardLCD_WriteRegWithData(0xD505,0x3A);
	BoardLCD_WriteRegWithData(0xD506,0x00);
	BoardLCD_WriteRegWithData(0xD507,0x4A);
	BoardLCD_WriteRegWithData(0xD508,0x00);
	BoardLCD_WriteRegWithData(0xD509,0x5C);
	BoardLCD_WriteRegWithData(0xD50A,0x00);
	BoardLCD_WriteRegWithData(0xD50B,0x81);
	BoardLCD_WriteRegWithData(0xD50C,0x00);
	BoardLCD_WriteRegWithData(0xD50D,0xA6);
	BoardLCD_WriteRegWithData(0xD50E,0x00);
	BoardLCD_WriteRegWithData(0xD50F,0xE5);
	BoardLCD_WriteRegWithData(0xD510,0x01);
	BoardLCD_WriteRegWithData(0xD511,0x13);
	BoardLCD_WriteRegWithData(0xD512,0x01);
	BoardLCD_WriteRegWithData(0xD513,0x54);
	BoardLCD_WriteRegWithData(0xD514,0x01);
	BoardLCD_WriteRegWithData(0xD515,0x82);
	BoardLCD_WriteRegWithData(0xD516,0x01);
	BoardLCD_WriteRegWithData(0xD517,0xCA);
	BoardLCD_WriteRegWithData(0xD518,0x02);
	BoardLCD_WriteRegWithData(0xD519,0x00);
	BoardLCD_WriteRegWithData(0xD51A,0x02);
	BoardLCD_WriteRegWithData(0xD51B,0x01);
	BoardLCD_WriteRegWithData(0xD51C,0x02);
	BoardLCD_WriteRegWithData(0xD51D,0x34);
	BoardLCD_WriteRegWithData(0xD51E,0x02);
	BoardLCD_WriteRegWithData(0xD51F,0x67);
	BoardLCD_WriteRegWithData(0xD520,0x02);
	BoardLCD_WriteRegWithData(0xD521,0x84);
	BoardLCD_WriteRegWithData(0xD522,0x02);
	BoardLCD_WriteRegWithData(0xD523,0xA4);
	BoardLCD_WriteRegWithData(0xD524,0x02);
	BoardLCD_WriteRegWithData(0xD525,0xB7);
	BoardLCD_WriteRegWithData(0xD526,0x02);
	BoardLCD_WriteRegWithData(0xD527,0xCF);
	BoardLCD_WriteRegWithData(0xD528,0x02);
	BoardLCD_WriteRegWithData(0xD529,0xDE);
	BoardLCD_WriteRegWithData(0xD52A,0x02);
	BoardLCD_WriteRegWithData(0xD52B,0xF2);
	BoardLCD_WriteRegWithData(0xD52C,0x02);
	BoardLCD_WriteRegWithData(0xD52D,0xFE);
	BoardLCD_WriteRegWithData(0xD52E,0x03);
	BoardLCD_WriteRegWithData(0xD52F,0x10);
	BoardLCD_WriteRegWithData(0xD530,0x03);
	BoardLCD_WriteRegWithData(0xD531,0x33);
	BoardLCD_WriteRegWithData(0xD532,0x03);
	BoardLCD_WriteRegWithData(0xD533,0x6D);
	BoardLCD_WriteRegWithData(0xD600,0x00);
	BoardLCD_WriteRegWithData(0xD601,0x33);
	BoardLCD_WriteRegWithData(0xD602,0x00);
	BoardLCD_WriteRegWithData(0xD603,0x34);
	BoardLCD_WriteRegWithData(0xD604,0x00);
	BoardLCD_WriteRegWithData(0xD605,0x3A);
	BoardLCD_WriteRegWithData(0xD606,0x00);
	BoardLCD_WriteRegWithData(0xD607,0x4A);
	BoardLCD_WriteRegWithData(0xD608,0x00);
	BoardLCD_WriteRegWithData(0xD609,0x5C);
	BoardLCD_WriteRegWithData(0xD60A,0x00);
	BoardLCD_WriteRegWithData(0xD60B,0x81);
	BoardLCD_WriteRegWithData(0xD60C,0x00);
	BoardLCD_WriteRegWithData(0xD60D,0xA6);
	BoardLCD_WriteRegWithData(0xD60E,0x00);
	BoardLCD_WriteRegWithData(0xD60F,0xE5);
	BoardLCD_WriteRegWithData(0xD610,0x01);
	BoardLCD_WriteRegWithData(0xD611,0x13);
	BoardLCD_WriteRegWithData(0xD612,0x01);
	BoardLCD_WriteRegWithData(0xD613,0x54);
	BoardLCD_WriteRegWithData(0xD614,0x01);
	BoardLCD_WriteRegWithData(0xD615,0x82);
	BoardLCD_WriteRegWithData(0xD616,0x01);
	BoardLCD_WriteRegWithData(0xD617,0xCA);
	BoardLCD_WriteRegWithData(0xD618,0x02);
	BoardLCD_WriteRegWithData(0xD619,0x00);
	BoardLCD_WriteRegWithData(0xD61A,0x02);
	BoardLCD_WriteRegWithData(0xD61B,0x01);
	BoardLCD_WriteRegWithData(0xD61C,0x02);
	BoardLCD_WriteRegWithData(0xD61D,0x34);
	BoardLCD_WriteRegWithData(0xD61E,0x02);
	BoardLCD_WriteRegWithData(0xD61F,0x67);
	BoardLCD_WriteRegWithData(0xD620,0x02);
	BoardLCD_WriteRegWithData(0xD621,0x84);
	BoardLCD_WriteRegWithData(0xD622,0x02);
	BoardLCD_WriteRegWithData(0xD623,0xA4);
	BoardLCD_WriteRegWithData(0xD624,0x02);
	BoardLCD_WriteRegWithData(0xD625,0xB7);
	BoardLCD_WriteRegWithData(0xD626,0x02);
	BoardLCD_WriteRegWithData(0xD627,0xCF);
	BoardLCD_WriteRegWithData(0xD628,0x02);
	BoardLCD_WriteRegWithData(0xD629,0xDE);
	BoardLCD_WriteRegWithData(0xD62A,0x02);
	BoardLCD_WriteRegWithData(0xD62B,0xF2);
	BoardLCD_WriteRegWithData(0xD62C,0x02);
	BoardLCD_WriteRegWithData(0xD62D,0xFE);
	BoardLCD_WriteRegWithData(0xD62E,0x03);
	BoardLCD_WriteRegWithData(0xD62F,0x10);
	BoardLCD_WriteRegWithData(0xD630,0x03);
	BoardLCD_WriteRegWithData(0xD631,0x33);
	BoardLCD_WriteRegWithData(0xD632,0x03);
	BoardLCD_WriteRegWithData(0xD633,0x6D);
	//LV2 Page 0 enable
	BoardLCD_WriteRegWithData(0xF000,0x55);
	BoardLCD_WriteRegWithData(0xF001,0xAA);
	BoardLCD_WriteRegWithData(0xF002,0x52);
	BoardLCD_WriteRegWithData(0xF003,0x08);
	BoardLCD_WriteRegWithData(0xF004,0x00);
	//Display control
	BoardLCD_WriteRegWithData(0xB100, 0xCC);
	BoardLCD_WriteRegWithData(0xB101, 0x00);
	//Source hold time
	BoardLCD_WriteRegWithData(0xB600,0x05);
	//Gate EQ control
	BoardLCD_WriteRegWithData(0xB700,0x70);
	BoardLCD_WriteRegWithData(0xB701,0x70);
	//Source EQ control (Mode 2)
	BoardLCD_WriteRegWithData(0xB800,0x01);
	BoardLCD_WriteRegWithData(0xB801,0x03);
	BoardLCD_WriteRegWithData(0xB802,0x03);
	BoardLCD_WriteRegWithData(0xB803,0x03);
	//Inversion mode (2-dot)
	BoardLCD_WriteRegWithData(0xBC00,0x02);
	BoardLCD_WriteRegWithData(0xBC01,0x00);
	BoardLCD_WriteRegWithData(0xBC02,0x00);
	//Timing control 4H w/ 4-delay
	BoardLCD_WriteRegWithData(0xC900,0xD0);
	BoardLCD_WriteRegWithData(0xC901,0x02);
	BoardLCD_WriteRegWithData(0xC902,0x50);
	BoardLCD_WriteRegWithData(0xC903,0x50);
	BoardLCD_WriteRegWithData(0xC904,0x50);
	BoardLCD_WriteRegWithData(0x3500,0x00);
	//16-bit/pixel
	BoardLCD_WriteRegWithData(0x3A00,0x55);
	BoardLCD_WriteRegAddrWithOutData(0x1100);
	CoreDelayMs(LCD_WAIT_CONFIG_TIME);
	BoardLCD_WriteRegAddrWithOutData(0x2900);
}


static void BoardLCD_InitScanDir(uint8_t scanDir)
{
	uint16_t regValue = 0;
	switch(scanDir)
	{
		case L2R_U2D://从左到右,从上到下
				regValue|=(0<<7)|(0<<6)|(0<<5);
				break;
		case L2R_D2U://从左到右,从下到上
				regValue|=(1<<7)|(0<<6)|(0<<5);
				break;
		case R2L_U2D://从右到左,从上到下
				regValue|=(0<<7)|(1<<6)|(0<<5);
				break;
		case R2L_D2U://从右到左,从下到上
				regValue|=(1<<7)|(1<<6)|(0<<5);
				break;
		case U2D_L2R://从上到下,从左到右
				regValue|=(0<<7)|(0<<6)|(1<<5);
				break;
		case U2D_R2L://从上到下,从右到左
				regValue|=(0<<7)|(1<<6)|(1<<5);
				break;
		case D2U_L2R://从下到上,从左到右
				regValue|=(1<<7)|(0<<6)|(1<<5);
				break;
		case D2U_R2L://从下到上,从右到左
				regValue|=(1<<7)|(1<<6)|(1<<5);
				break;
	}
	/**扫描方向设置 */
	BoardLCD_WriteRegWithData(0X3600,regValue);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD);
	BoardLCD_WriteDataWithOutRegAddr(0);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD+1);
	BoardLCD_WriteDataWithOutRegAddr(0);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD+2);
	BoardLCD_WriteDataWithOutRegAddr((BOARD_LCD_WIDTH-1)>>8);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD+3);
	BoardLCD_WriteDataWithOutRegAddr((BOARD_LCD_WIDTH-1)&0XFF);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD);
	BoardLCD_WriteDataWithOutRegAddr(0);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD+1);
	BoardLCD_WriteDataWithOutRegAddr(0);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD+2);
	BoardLCD_WriteDataWithOutRegAddr((BOARD_LCD_HEIGHT-1)>>8);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD+3);
	BoardLCD_WriteDataWithOutRegAddr((BOARD_LCD_HEIGHT-1)&0XFF);
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
static void BoardLCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD);
	BoardLCD_WriteDataWithOutRegAddr(Xpos>>8);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD+1);
	BoardLCD_WriteDataWithOutRegAddr(Xpos&0XFF);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD);
	BoardLCD_WriteDataWithOutRegAddr(Ypos>>8);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD+1);
	BoardLCD_WriteDataWithOutRegAddr(Ypos&0XFF);
}

//开始写GRAM
static void BoardLCD_WriteRAM_Prepare(void)
{
 	BoardLCD->BoardLCD_Reg = BOARD_LCD_DRIVER_IC_WRITE_RAM_CMD;
}

//当mdk -O1时间优化时需要设置
//延时i
static void opt_delay(u8 i)
{
	while(i--);
}

//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
uint16_t BoardLCD_ReadPoint(u16 x,u16 y)
{
	uint16_t r=0,g=0,b=0;
	//坐标超过了范围,直接返回
	if(x >= BOARD_LCD_WIDTH || y >= BOARD_LCD_HEIGHT)
		return 0;
	BoardLCD_SetCursor(x,y);
	BoardLCD_WriteRegAddrWithOutData(0X2E00);
	//dummy Read
 	r=BoardLCD_ReadDataWithOutRegAddr();
	opt_delay(2);
	//实际坐标颜色
 	r=BoardLCD_ReadDataWithOutRegAddr();
	opt_delay(2);
	b=BoardLCD_ReadDataWithOutRegAddr();
	//对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
	g=r&0XFF;
	g<<=8;
	return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
}














//创建LCD访问信号量
OS_MUTEX mutex_LCD;

/**LCD初始化 */
D_ERROR_CODE BoardLCD_Init(void)
{
    uint16_t deviceID = 0;
	OS_ERR err;
	//创建一个互斥信号量,用于flash读写
    OSMutexCreate((OS_MUTEX*	)&mutex_LCD,
				  (CPU_CHAR*	)"mutex_LCD",
                  (OS_ERR*		)&err);
	/**IO初始化 */
	BoardLCD_IO_Init();
	deviceID = BoardLCD_ReadID();
	/**ID匹配 */
	if(deviceID == BOARD_LCD_DRIVER_IC_ID)
	{
		/**设置FSMC总线 */
		Device_LCD_FSMC_SpeedReConfig();
		/**初始化驱动IC */
		BoardLCD_SSD_5510_Init();
		/**设定屏幕尺寸和扫描方向 */
		BoardLCD_InitScanDir(DFT_SCAN_DIR);
		/**点亮背光 */
		BoardLCD_BackLightSet(1);
		/**清除屏幕 */
		BoardLCD_Clear(WHITE);
		return D_ERROR_CODE_NONE;
	}
	else
	{
		return D_ERROR_CODE_BOARD_LCD_ID_CHECK;
	}
}

/**LCD用指定颜色清除界面 */
void BoardLCD_Clear(uint16_t color)
{
	OS_ERR err;
    uint32_t index=0;
	uint32_t totalpoint=BOARD_LCD_WIDTH;
	//请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_LCD, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//得到总点数
	totalpoint*=BOARD_LCD_HEIGHT;
	BoardLCD_SetCursor(0x00,0x0000);
	//开始写入GRAM
	BoardLCD_WriteRAM_Prepare();
	for(index=0;index<totalpoint;index++)
	{
		BoardLCD->BoardLCD_Ram=color;
	}
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_LCD, OS_OPT_POST_FIFO, &err);
}

/**开启显示 */
void BoardLCD_DisplayOn(void)
{
	OS_ERR err;
	//请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_LCD, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    BoardLCD_WriteRegAddrWithOutData(0X2900);
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_LCD, OS_OPT_POST_FIFO, &err);
}

/**LCD关闭显示 */
void BoardLCD_DisplayOff(void)
{
	OS_ERR err;
	//请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_LCD, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    BoardLCD_WriteRegAddrWithOutData(0X2800);
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_LCD, OS_OPT_POST_FIFO, &err);
}

//画点
void BoardLCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	OS_ERR err;
	//请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_LCD, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    //设置光标位置
	BoardLCD_SetCursor(x,y);
	//开始写入GRAM
	BoardLCD_WriteRAM_Prepare();
	BoardLCD->BoardLCD_Ram=color;
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_LCD, OS_OPT_POST_FIFO, &err);
}

//快速画点
void BoardLCD_FastDrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	OS_ERR err;
	//请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_LCD, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD);
	BoardLCD_WriteDataWithOutRegAddr(x>>8);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_X_CMD+1);
	BoardLCD_WriteDataWithOutRegAddr(x&0XFF);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD);
	BoardLCD_WriteDataWithOutRegAddr(y>>8);
	BoardLCD_WriteRegAddrWithOutData(BOARD_LCD_DRIVER_IC_SET_Y_CMD+1);
	BoardLCD_WriteDataWithOutRegAddr(y&0XFF);
	BoardLCD->BoardLCD_Reg=BOARD_LCD_DRIVER_IC_WRITE_RAM_CMD;
	BoardLCD->BoardLCD_Ram=color;
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_LCD, OS_OPT_POST_FIFO, &err);
}

//在指定区域内填充单个颜色
void BoardLCD_FillSingleColor(uint16_t startX,uint16_t startY,uint16_t endX,uint16_t endY,uint16_t color)
{
	OS_ERR err;
    uint16_t i,j;
	uint16_t xlen=0;
	//请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_LCD, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	xlen=endX-startX+1;
	for(i=startY;i<=endY;i++)
	{
		//设置光标位置
		BoardLCD_SetCursor(startX,i);
		//开始写入GRAM
		BoardLCD_WriteRAM_Prepare();
		//显示颜色
		for(j=0;j<xlen;j++)
			BoardLCD->BoardLCD_Ram=color;
	}
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_LCD, OS_OPT_POST_FIFO, &err);
}
































