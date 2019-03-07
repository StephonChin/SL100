/********************************************************************
  *
  * FILE NAME      Flash_Prgm.c
  *
*********************************************************************/
#include "Flash_Prgm.h"

/* @Gloable variables */
_Flag               FlashSaveFlag;


/*****************************************************************************
	*
	* @FunctionName Byte_Write_To_APROM
	* @Output 			None
	* @Input				None
	*
****/
void Byte_Write_To_APROM(void)
{
  _Uint32			AddStart = FMC_LDROM_BASE;
  
  if (FlashSaveFlag == TRUE)
	{
		FlashSaveFlag = FALSE;
		
		SYS_UnlockReg();

		FMC_Open();
		
		FMC_ENABLE_LD_UPDATE();
		
		FMC_Erase(AddStart);
		FMC_Write(AddStart, (_Uint32)NormalMode);
		AddStart += 4;
		FMC_Write(AddStart, (_Uint32)NormalColor);
		AddStart += 4;
		FMC_Write(AddStart, (_Uint32)ShowMode);
		AddStart += 4;
		FMC_Write(AddStart, (_Uint32)ShowColor);
		
		SYS_LockReg();
	}
}


/*
 * FunctionName		Flash_Read_Data
 */
void Flash_Read_Data(void)
{
	_Uint32			AddStart = FMC_LDROM_BASE;
	
	SYS_UnlockReg();

	FMC_Open();
	
	FMC_ENABLE_LD_UPDATE();
	
	NormalMode 	= (_Uint8)FMC_Read(AddStart);
	AddStart += 4;
	NormalColor = (_Uint8)FMC_Read(AddStart);
	AddStart += 4;
	ShowMode		= (_Uint8)FMC_Read(AddStart);
	AddStart += 4;
	ShowColor 	= (_Uint8)FMC_Read(AddStart);

	SYS_LockReg();
}









