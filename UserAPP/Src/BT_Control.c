/*********************************************************
	*
	* FILE NAME			BT_Control.c
	* BRIEF					Control the bluetooth power
	*
**********************************************************/
#include "BT_Control.h"



//------------------------< Gloable Parameters >-----------------------------//
_TypeBtCon	BtConStatus;
_Flag				BtRcvDataFlag;
_Flag				BtPowerInitFlag;


/**
	* FunctionName		Bt_Work_Detect
	*/
void BT_Work_Detect(void)
{
	static _Uint8		PIO11_HighTime[2];
	static _Uint8		PIO11_LowTime[2];
	
	static _Uint8		BtRcvDataTime[3];
	
	// Detect the PIO11:it should be ouput PWM wave
	// 6S H + 6S L
  
	if (PORT_BT_DT == SET)
	{
		PIO11_LowTime[0] = 0;
		PIO11_LowTime[1] = 0;
		
		PIO11_HighTime[0]++;
		if (PIO11_HighTime[0] >= 50)			//20ms * 50 = 1s
		{	
			PIO11_HighTime[0] = 0;
			PIO11_HighTime[1]++;
			if (PIO11_HighTime[1] >= 10)		//10s
			{
				PIO11_HighTime[1] = 0;
				//BtConStatus 		= BT_RESTART;
				BtPowerInitFlag = TRUE;
			}
		}
	}
	else
	{
		PIO11_HighTime[0] = 0;
		PIO11_HighTime[1] = 0;
		
		PIO11_LowTime[0]++;
		if (PIO11_LowTime[0] >= 50)			//20ms * 50 = 1s
		{	
			PIO11_LowTime[0] = 0;
			PIO11_LowTime[1]++;
			if (PIO11_LowTime[1] >= 10)		//10s
			{
				PIO11_LowTime[1] = 0;
				//BtConStatus 		= BT_RESTART;
				BtPowerInitFlag = TRUE;
			}
		}
	}
	
	
	

	// if the bluetooth haven't receive command without 1hour, restart it.
	if (BtRcvDataFlag == TRUE)
	{
		BtRcvDataFlag = FALSE;
		BtRcvDataTime[0] = 0;
		BtRcvDataTime[1] = 0;
		BtRcvDataTime[2] = 0;
	}
	
	BtRcvDataTime[0]++;
	if (BtRcvDataTime[0] >= 50)	//20 * 50 = 1s
	{
		BtRcvDataTime[0] = 0;
		BtRcvDataTime[1]++;
		if (BtRcvDataTime[1] >= 60)		//1min
		{
			BtRcvDataTime[1] = 0;
			BtRcvDataTime[2]++;
			if (BtRcvDataTime[2] >= 60)		//1hour
			{
				BtRcvDataTime[2] = 0;
				//BtConStatus 		= BT_RESTART;
				BtPowerInitFlag = TRUE;
			}
		}
	}
	
	
	//Control the BT power
	BT_Power_Control();
}




/************************************************************
	*
	* @FunctonName	BT_Power_Control
	* @Brief				Control the power of bluetooth
	* 							
	*/
static void BT_Power_Control(void)
{
	static _Uint8		BtTime;
	
	switch (BtConStatus)
	{
		case BT_IDLE:		break;
		
		case BT_POWER_ON:
		{
			if (BtPowerInitFlag == TRUE)
			{
				BtPowerInitFlag = FALSE;
				BtTime = 0;
				GPIO_SetMode(P1, GP_PIN_PIN2_Msk | GP_PIN_PIN3_Msk, GPIO_MODE_INPUT);
				PORT_BT_CE = SET;
			}
			
			BtTime++;
			if (BtTime > 5)
			{
				BtTime = 0;
				BtConStatus = BT_IDLE;
				SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
				SYS->P1_MFP |= (SYS_MFP_P12_UART0_RXD | SYS_MFP_P13_UART0_TXD);
			}
		} break;
		
		case BT_POWER_OFF:
		{
				BtPowerInitFlag = FALSE;
				BtTime = 0;
				PORT_BT_CE = RESET;
				BtConStatus = BT_IDLE;
				SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
				SYS->P1_MFP |= (SYS_MFP_P12_GPIO | SYS_MFP_P13_GPIO);
				PORT_RX = RESET;
				PORT_TX = RESET;
		} break;
		
		case BT_RESTART:
		{
			if (BtPowerInitFlag == TRUE)
			{
				BtPowerInitFlag = FALSE;
				BtTime = 0;
				PORT_BT_CE = RESET;
				SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
				SYS->P1_MFP |= (SYS_MFP_P12_GPIO | SYS_MFP_P13_GPIO);
				PORT_RX = RESET;
				PORT_TX = RESET;
			}
			
			BtTime++;
			if (BtTime > 5)
			{
				BtTime = 0;
				BtConStatus = BT_POWER_ON;
				BtPowerInitFlag = TRUE;
			}
		} break;
	}
}




