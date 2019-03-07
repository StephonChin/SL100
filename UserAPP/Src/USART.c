/********************************************************************
  *
  * FILE NAME      USART.c
  *
*********************************************************************/
#include "USART.h"

//--------------------< Gloable Paramters >--------------------------
_TypeStructRcv 	BtRcvData,WifiRcvData;
_TypeStructSnd 	BtSndData,WifiSndData;
_Flag			      BtRcvFlag,WifiRcvFlag;


//--------------------< File Paramters >--------------------------
uint8	BtRcvBuffer[RCV_BYTE_MAX], WifiRcvBuffer[RCV_BYTE_MAX];
enum ENUM_STATUS
{
	RCV_IDLE,
	RCV_STT,
	RCV_END,
	RCV_ERR
}BtRcvByteStatus, WifiRcvByteStatus;
uint8 WifiRcvHoldTime;
uint8 WifiByteCnt;



/**
	* FunctionName	USART_Data_Init
	*/
void USART_Data_Init(void)
{
  _Uint8 			RcvCnt = 0;
	
	for (RcvCnt = 0; RcvCnt < RCV_BYTE_MAX; RcvCnt++)
	{
		BtRcvBuffer[RcvCnt] = UART0->DAT;
		if (UART0->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk)  break;
	}
  
  for (RcvCnt = 0; RcvCnt < RCV_BYTE_MAX; RcvCnt++)
	{
		WifiRcvBuffer[RcvCnt] = UART1->DAT;
		if (UART1->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk)  break;
	}
	
	for (RcvCnt = 0; RcvCnt < RCV_BYTE_MAX; RcvCnt++)
	{
		BtRcvBuffer[RcvCnt] = 0;
    WifiRcvBuffer[RcvCnt] = 0;
	}
}


/**
 	* FunctionName: Uart_Rcv_Interrupt
 	*/
void UART0_IRQHandler(void)
{  
	Csr1010_Rcv_Byte();
}

/**
 	* FunctionName: Uart_Rcv_Interrupt
 	*/
void UART1_IRQHandler(void)
{  
	Wifi_Rcv_Byte();
}


/******************************************************
	*
	* @Function Name	Csr1010_Snd_Byte
	*
****/
static void Csr1010_Snd_Byte(_Uint8 SndBuf)
{
	UART_WRITE(UART0, SndBuf);
}

/**
	* FunctionName	Wifi_Snd_Byte
  */
static void Wifi_Snd_Byte(_Uint8 SndBuf)
{
	UART_WRITE(UART1, SndBuf);
}



/**
	* FunctionName	Csr1010_Rcv_Byte
  */
static void Csr1010_Rcv_Byte(void)
{
	_Uint8 	RcvCnt = 0;
	
	for (RcvCnt = 0; RcvCnt < RCV_BYTE_MAX; RcvCnt++)
	{
		BtRcvBuffer[RcvCnt] = UART0->DAT;
		if (UART0->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk)  break;
	}
	
	switch (RcvCnt)
	{
			/* Power On Command Received : FD F0 FE */
			case 0x2:
			{
				if (BtRcvBuffer[0] == 0xFD && BtRcvBuffer[1] == 0xF0 && BtRcvBuffer[2] == 0xFE)
				{
					BtRcvByteStatus = RCV_END;
				}
				else
				{
					BtRcvByteStatus = RCV_ERR;
				}
			} break;
			/* Normal Command Received */
			case 0x7:
			{
				if (BtRcvBuffer[0] == 0xFD && BtRcvBuffer[7] == 0xFE)
				{
					BtRcvByteStatus = RCV_END;
				}
				else
				{
					BtRcvByteStatus = RCV_ERR;
				}
			} break;
			
			
			default:
			{
				BtRcvByteStatus = RCV_ERR;
				UART->FIFO |= BIT1;		//RXRST = 1, reset
			}break;
		}
}



/**
  * FunctionName  Wifi_Rcv_Byte
  */
static void Wifi_Rcv_Byte(void)
{
  _Uint8		RcvBufferTemp = 0;


	// move receive data to RcvBuffer
	RcvBufferTemp = UART1->DAT;

  WifiRcvHoldTime = 0;

	// start to receive data when receive status is idle
	if (WifiRcvByteStatus == RCV_IDLE && RcvBufferTemp == PROTOCOL_VER)
	{
    WifiRcvByteStatus 	= RCV_STT;
    WifiByteCnt     = 0;
	}

	// receive the data
	if (WifiRcvByteStatus == RCV_STT)
	{
    WifiRcvBuffer[WifiByteCnt++] = RcvBufferTemp;
		if (WifiByteCnt >= RCV_BYTE_MAX)				  WifiRcvByteStatus = RCV_IDLE;								// Rcv Err
	}
}




/**
	* FunctionName	Wifi_Rcv_Data
  */
void Wifi_Rcv_Data(void)
{
	_Uint8		TempCnt = 0;


	/* Exit when receive status is idle or working */
	if (WifiRcvByteStatus == RCV_IDLE)
	{
		WifiRcvHoldTime = 0;
		return;
	}


	/* Hold time after received, if timer > 1second, received err, then clear the data */
	if (WifiRcvByteStatus == RCV_STT)
	{
		WifiRcvHoldTime++;
		if (WifiRcvHoldTime > 5) // 50ms
		{
			WifiRcvHoldTime 	= 0;

        WifiRcvByteStatus = RCV_IDLE;
        WifiRcvFlag	= TRUE;
        for (TempCnt = 0; TempCnt < RCV_BYTE_MAX; TempCnt++)									// move the buffer to rcvdata
        {
          WifiRcvData.DataBuf[TempCnt]	= WifiRcvBuffer[TempCnt];
          WifiRcvBuffer[TempCnt] 			= 0;
        }
		}
		return;
	}
}




/**
	* FunctionName	Csr1010_Rcv_Data
	*/
void Csr1010_Rcv_Data(void)
{
	 _Uint8		TempCnt = 0;
	
	switch(BtRcvByteStatus)
	{
		/* Exit when receive status is idle or working */
		default:
		{
			
		}break;
		
		/* received error */
		case RCV_ERR:
		{
			for (TempCnt = 0; TempCnt < RCV_BYTE_MAX; TempCnt++)		BtRcvBuffer[TempCnt]	= 0;			// clear the buffer
			BtRcvByteStatus = RCV_IDLE;																									// clear the status
		} break;
	
		/* received success */
		case RCV_END:
		{
			BtRcvFlag	= TRUE;
			for (TempCnt = 0; TempCnt < 6; TempCnt++)									// move the buffer to rcvdata
			{
				BtRcvData.DataBuf[TempCnt]	= BtRcvBuffer[TempCnt + 1];
			}
			for (TempCnt = 0; TempCnt < RCV_BYTE_MAX; TempCnt++)
			{
				BtRcvBuffer[TempCnt] = 0;
			}
			BtRcvByteStatus = RCV_IDLE;																	// clear status
		} break;
	}
}









/***********************************************************
	*
	* FunctionName	Csr1010_Snd_Data
	*
************************************************************/
void Csr1010_Snd_Data(void)
{
	_Uint8	TempCnt = 0;

	/* when snd data is idle , exit */
	if (BtSndData.SndStatus == SND_IDLE)
	{
		return;
	}

	/* type send LA1 */
	if (BtSndData.SndStatus == SND_TYPE)
	{
		// Send type command "LA1"
		Csr1010_Snd_Byte(0xFC);
		Csr1010_Snd_Byte('V');
		Csr1010_Snd_Byte('W');
		Csr1010_Snd_Byte('B');
    Csr1010_Snd_Byte('0');
    Csr1010_Snd_Byte('0');
    Csr1010_Snd_Byte('0');
    Csr1010_Snd_Byte('0');
		Csr1010_Snd_Byte(0xFE);

		// Send power on command after type command sent completed
		BtSndData.SndStatus = SND_IDLE;
		return;
	}


	/* Send normally */
	if (BtSndData.SndStatus == SND_NORMAL)
	{
		Csr1010_Snd_Byte(0xFD);
		for (TempCnt = 0; TempCnt < 8; TempCnt++)		Csr1010_Snd_Byte(BtSndData.DataBuf[TempCnt]);
		Csr1010_Snd_Byte(0xFE);
		BtSndData.SndStatus = SND_IDLE;

    return;
	}

  /* Send Broadcast */
	if (BtSndData.SndStatus == SND_BROADCAST)
	{
		Csr1010_Snd_Byte(0xF9);
		for (TempCnt = 0; TempCnt < 10; TempCnt++)		Csr1010_Snd_Byte(BtSndData.DataBuf[TempCnt]);
		Csr1010_Snd_Byte(0xFE);
		BtSndData.SndStatus = SND_IDLE;

    return;
	}
  
  BtSndData.SndStatus = SND_IDLE;
  for (TempCnt = 0; TempCnt < 10; TempCnt++)		BtSndData.DataBuf[TempCnt] = 0;
}





/***********************************************************
	*
	* FunctionName	Wifi_Snd_Data
	*
************************************************************/
void Wifi_Snd_Data(void)
{
	_Uint8	TempCnt = 0;

	/* when snd data is idle , exit */
	if (WifiSndData.SndStatus == SND_IDLE)
	{
		return;
	}

	/* type send LA1 */
	if (WifiSndData.SndStatus == SND_TYPE)
	{
		// Send type command "LA1"
		Wifi_Snd_Byte(0xFC);
		Wifi_Snd_Byte(0xFE);

		// Send power on command after type command sent completed
		WifiSndData.SndStatus = SND_NORMAL;
		return;
	}


	/* Send normally */
	if (WifiSndData.SndStatus == SND_NORMAL)
	{
		Wifi_Snd_Byte(0xFD);
		for (TempCnt = 0; TempCnt < 8; TempCnt++)		Wifi_Snd_Byte(WifiSndData.DataBuf[TempCnt]);
		Wifi_Snd_Byte(0xFE);
		WifiSndData.SndStatus = SND_IDLE;
	}
}



