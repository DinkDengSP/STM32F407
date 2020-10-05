/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-10-05 21:16:33
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-05 21:18:39
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardSDBase.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_SD_BASE_H_
#define __BOARD_SD_BASE_H_
#include "MCU_Inc.h"

/******************************支持的SD卡类型TYPE定义*****************************************/
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((u32)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((u32)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((u32)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((u32)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((u32)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((u32)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((u32)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((u32)0x00000007)
/******************************支持的SD卡类型TYPE定义*****************************************/


//SD卡CSD寄存器数据		  
typedef struct
{
	uint8_t  CSDStruct;            /*!< CSD structure */
	uint8_t  SysSpecVersion;       /*!< System specification version */
	uint8_t  Reserved1;            /*!< Reserved */
	uint8_t  TAAC;                 /*!< Data read access-time 1 */
	uint8_t  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
	uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency */
	uint16_t CardComdClasses;      /*!< Card command classes */
	uint8_t  RdBlockLen;           /*!< Max. read data block length */
	uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed */
	uint8_t  WrBlockMisalign;      /*!< Write block misalignment */
	uint8_t  RdBlockMisalign;      /*!< Read block misalignment */
	uint8_t  DSRImpl;              /*!< DSR implemented */
	uint8_t  Reserved2;            /*!< Reserved */
	uint32_t DeviceSize;           /*!< Device Size */
	uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
	uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
	uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
	uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
	uint8_t  DeviceSizeMul;        /*!< Device size multiplier */
	uint8_t  EraseGrSize;          /*!< Erase group size */
	uint8_t  EraseGrMul;           /*!< Erase group size multiplier */
	uint8_t  WrProtectGrSize;      /*!< Write protect group size */
	uint8_t  WrProtectGrEnable;    /*!< Write protect group enable */
	uint8_t  ManDeflECC;           /*!< Manufacturer default ECC */
	uint8_t  WrSpeedFact;          /*!< Write speed factor */
	uint8_t  MaxWrBlockLen;        /*!< Max. write data block length */
	uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
	uint8_t  Reserved3;            /*!< Reserded */
	uint8_t  ContentProtectAppli;  /*!< Content protection application */
	uint8_t  FileFormatGrouop;     /*!< File format group */
	uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
	uint8_t  PermWrProtect;        /*!< Permanent write protection */
	uint8_t  TempWrProtect;        /*!< Temporary write protection */
	uint8_t  FileFormat;           /*!< File Format */
	uint8_t  ECC;                  /*!< ECC code */
	uint8_t  CSD_CRC;              /*!< CSD CRC */
	uint8_t  Reserved4;            /*!< always 1*/
} SD_CSD;

//SD卡CID寄存器数据
typedef struct
{
	uint8_t  ManufacturerID;       /*!< ManufacturerID */
	uint16_t OEM_AppliID;          /*!< OEM/Application ID */
	uint32_t ProdName1;            /*!< Product Name part1 */
	uint8_t  ProdName2;            /*!< Product Name part2*/
	uint8_t  ProdRev;              /*!< Product Revision */
	uint32_t ProdSN;               /*!< Product Serial Number */
	uint8_t  Reserved1;            /*!< Reserved1 */
	uint16_t ManufactDate;         /*!< Manufacturing Date */
	uint8_t  CID_CRC;              /*!< CID CRC */
	uint8_t  Reserved2;            /*!< always 1 */
} SD_CID;

//SD卡状态
typedef enum
{
	SD_CARD_READY                  = ((uint32_t)0x00000001),
	SD_CARD_IDENTIFICATION         = ((uint32_t)0x00000002),
	SD_CARD_STANDBY                = ((uint32_t)0x00000003),
	SD_CARD_TRANSFER               = ((uint32_t)0x00000004),
	SD_CARD_SENDING                = ((uint32_t)0x00000005),
	SD_CARD_RECEIVING              = ((uint32_t)0x00000006),
	SD_CARD_PROGRAMMING            = ((uint32_t)0x00000007),
	SD_CARD_DISCONNECTED           = ((uint32_t)0x00000008),
	SD_CARD_ERROR                  = ((uint32_t)0x000000FF)
}SDCardState;	

//SD卡信息,包括CSD,CID等数据
typedef struct
{
  SD_CSD SD_csd;
  SD_CID SD_cid;
  double CardCapacity;  	//SD卡容量,单位:字节,最大支持2^64字节大小的卡.
  uint32_t CardBlockSize; 		//SD卡块大小	
  uint16_t RCA;					//卡相对地址
  uint8_t CardType;				//卡类型
} SD_CardInfo;


#endif




