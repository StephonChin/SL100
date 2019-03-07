/********************************************************************
  *
  * FILE NAME      WifiBt.c
  *
  * Modify by      DeshengChin
  *                07.25  2018
  *                Update the protocol
  *
  * Ver0.4.0
  * Modify by DeshengChin at 29 OCt 2018
  *   MCU update to MINI58FDE
  *
  * Ver0.4.1
  * Modify by DeshengChin at 30 OCt 2018
  *   Add music mode
  *
  *
  * Ver0.4.2
  * FFT simple frequency changed to 64 count
  *
*********************************************************************/
#include "WifiBt.h"



//-------------------------------------------------------------------
// FunctionName     main()
// Brief            program entrance
//-------------------------------------------------------------------
int main(void)
{
  MCU_Config();

  USART_Data_Init();

  User_Data_Init();

  while (1)
	{
		if (TIMER_GetIntFlag(TIMER0))
		{
			TIMER_ClearIntFlag(TIMER0);
			
			// Clear the WDT count number
			SYS_UnlockReg();
			WDT->CTL &= WDT_CTL_RSTCNT_Msk;			//Clear WDT Cnt
			SYS_LockReg();

      Csr1010_Snd_Data();

      Csr1010_Rcv_Data();

      Wifi_Snd_Data();

      Wifi_Rcv_Data();

      Key_Scan();
      
      Music_AD();

      Data_Process();

      BT_Work_Detect();

      Byte_Write_To_APROM();

      Led_Control();
    }
  }
}
