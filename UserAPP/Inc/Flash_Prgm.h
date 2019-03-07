/********************************************************************
  *
  * FILE NAME      Flash_Prgm.h
  *
*********************************************************************/
#ifndef __FLASH_PRGM_H__
#define __FLASH_PRGM_H__


/* @Include Files */
#include "TypeDef.h"
#include "Data_Process.h"


/* @Define */
#define	APROM_ERASE			0x22
#define APROM_WRITE			0x21
#define	APROM_READ			0x00
#define	LDROM_ERASE			0x62
#define	LDROM_WRITE			0x61
#define	LDROM_READ			0x40
#define CONFIG_ERASE		0xE2
#define	CONFIG_WRITE		0xE1
#define	CONFIG_READ			0xC0


/* @Type Define */
typedef enum _EPROM_SAVE_TYPE
{
	APROM_NORMAL_MODE = 0,
	APROM_NORMAL_COLOR,
	APROM_SHOW_MODE,
  APROM_SHOW_COLOR
}_TypeEnumEprom;



/* @Exported Functions */
void Byte_Write_To_APROM(void);
void Flash_Read_Data(void);


/* @Exported variables */
extern _Flag                FlashSaveFlag;






#endif
