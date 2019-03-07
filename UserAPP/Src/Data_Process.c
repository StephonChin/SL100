/********************************************************************
  *
  * FILE NAME      Data_Process.c
  *
*********************************************************************/
#include "Data_Process.h"

_Uint8        NormalMode;
_Uint8        ShowMode;
_Uint8        NormalColor;
_Uint8        ShowColor;


EN_ERR_NUM   ErrNum;
_Uint8			  FlagChkTime;
_Flag					FlagChkFlag;
 _Uint8       FlagByte = 0x1;
 
_Sint16       RndSeed;


/********************************************************************
  *
  * FunctionName    User_Data_Init
  *
*********************************************************************/
void User_Data_Init(void)
{
  Flash_Read_Data();

  if (NormalMode > NORMAL_MODE_MAX){
    NormalMode = 1;
    FlashSaveFlag = TRUE;
  }

  if (NormalColor > NORMAL_COLOR_MAX){
    NormalColor = 0;
    FlashSaveFlag = TRUE;
  }

  if (ShowMode > SHOW_MODE_MAX){
    ShowMode = 1;
    FlashSaveFlag = TRUE;
  }

  if (ShowColor > SHOW_COLOR_MAX){
    NormalMode = 1;
    FlashSaveFlag = TRUE;
  }
}



/********************************************************************
  *
  * FunctionName    Data_Process
  *
*********************************************************************/
void Data_Process(void)
{
  RndSeed += 199;
  Key_Process();

  CSR1010_Process();

  Wifi_Process();
  
  Music_Process();
  
  if (RestorCnt >= 3 && ForbidFlag == TRUE){
    RestorCnt = 0;
    
    BtSndData.SndStatus  = SND_BROADCAST;
    BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
    BtSndData.DataBuf[1] = 0x00;
    BtSndData.DataBuf[2] = 0x3F;
    BtSndData.DataBuf[3] = 0xFF;
    BtSndData.DataBuf[4] = 0x03  | (FlagByte << 4);
    BtSndData.DataBuf[5] = 0xA;
    BtSndData.DataBuf[6] = 0;
    BtSndData.DataBuf[7] = 0;
    BtSndData.DataBuf[8] = 0;
    BtSndData.DataBuf[9] = 0;
  }
}


//-------------------------------------------------------------------
// FunctionName     Key_Process
//-------------------------------------------------------------------
static void Key_Process(void)
{
  if (KeyStatus == KEY_1_SHORT)
  {

  }
  else if (KeyStatus == KEY_2_SHORT)
  {

  }
  else
  {
    KeyStatus = KEY_IDLE;
  }
}


//-------------------------------------------------------------------
// FunctionName     CSR1010_Process
//-------------------------------------------------------------------
static void CSR1010_Process(void)
{
  static	_Uint8			TypeReplyTime;
	static 	_Flag       BtInitFlag;               //bluetooth intialization' flag

	/* Initialize the CSR1010's broadcast name after 3 seconds*/
	if (BtInitFlag == FALSE)
	{
		//----------------------------------------------------------------------
		// if device haven't received command FD F0 FE within 1 second after power on,
		// send the type to CSR1010
		//-----------------------------------------------------------------------
		TypeReplyTime++;
		if (TypeReplyTime >= 150)
		{
			BtInitFlag 					= TRUE;
			BtSndData.SndStatus		= SND_TYPE;
		}
	}

	//-----------------------------------------------------------------------
	// if the device haven't received any command within 300ms,
	// clear the flagstatus
	//-----------------------------------------------------------------------
	if (FlagChkFlag)
	{
		FlagChkTime++;
		if (FlagChkTime >= 15)
		{
			FlagChkTime 	= 0;
			FlagChkFlag 	= FALSE;
		}
	}



	/* Check the received flag, when it is false, return */
	if (BtRcvFlag == FALSE)		return;


	/* Received the command from CSR1010
	 * Clear the received flag
	 */
	BtRcvFlag 			= FALSE;
	FlagChkFlag		= TRUE;
	FlagChkTime		= 0;
	BtRcvDataFlag	= TRUE;


	/* CSR1010 Initialize					*/
	if (BtInitFlag == FALSE)
	{
		if (BtRcvData.DataBuf[0] == 0xF0)
		{
      BtInitFlag = TRUE;
			BtSndData.SndStatus		= SND_TYPE;
			return;
		}
	}




}

//-------------------------------------------------------------------
// FunctionName     Wifi_Process
//-------------------------------------------------------------------
static void Wifi_Process(void)
{
  _Flag           FlagChgFlag = FALSE;
  UART_Cmd_T      CmdBuffer;
  _Uint8          LengthBuffer;
  _Uint8          InfoBuffer;

  if (WifiRcvFlag == FALSE)    return;
  WifiRcvFlag = FALSE;

  CmdBuffer     = (UART_Cmd_T)WifiRcvData.DataBuf[1];
  LengthBuffer  = WifiRcvData.DataBuf[3];
  InfoBuffer    = WifiRcvData.DataBuf[4];

  if (LengthBuffer > 0)
  {
    switch (CmdBuffer)
    {
      //About the wifi status
      case CMD_WIFI_STATUS:{
        if (InfoBuffer == 0x1){
          if (LedMode != LED_WIFI_STANDBY){
            LedMode   = LED_WIFI_STANDBY;
            LedInit   = TRUE;
          }
        }
        else if (InfoBuffer == 0x2){
          if (LedMode != LED_WIFI_DISCONNECT){
            LedMode   = LED_WIFI_DISCONNECT;
            LedInit   = TRUE;
          }
        }
        else if (InfoBuffer == 0x4){
          if (LedMode != LED_WIFI_CONNECTED){
            LedMode   = LED_WIFI_CONNECTED;
            LedInit   = TRUE;
          }
        }
      } break;

      //Setting specific color or theme
      case CMD_THEME_SET:{
        //Error check
        if (InfoBuffer > NORMAL_COLOR_MAX){
          ErrNum = ERR_COLOR_RANGE;
          break;
        }

        if (NormalMode > 0x8){
          NormalMode = 0x1;
        }

        //Change the color
        NormalColor = InfoBuffer;

        BtSndData.SndStatus  = SND_BROADCAST;
        BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
        BtSndData.DataBuf[1] = 0x00;
        BtSndData.DataBuf[2] = 0x3F;
        BtSndData.DataBuf[3] = 0xFF;
        BtSndData.DataBuf[4] = 0x03  | (FlagByte << 4);
        BtSndData.DataBuf[5] = NormalMode;
        BtSndData.DataBuf[6] = NormalColor;
        BtSndData.DataBuf[7] = 0;
        BtSndData.DataBuf[8] = 0;
        BtSndData.DataBuf[9] = 0;

        FlagChgFlag = TRUE;
        FlashSaveFlag = TRUE;
        LedMode       = LED_NORMAL;
        LedInit       = TRUE;
      } break;



      //Setting specific mode
      case CMD_NORMAL_MODE:{
        //Error check
        if (InfoBuffer > NORMAL_MODE_MAX){
          ErrNum = ERR_MODE_RANGE;
          break;
        }

        //Setting mode
        NormalMode = InfoBuffer + 1;

        BtSndData.SndStatus  = SND_BROADCAST;
        BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
        BtSndData.DataBuf[1] = 0x00;
        BtSndData.DataBuf[2] = 0x3F;
        BtSndData.DataBuf[3] = 0xFF;
        BtSndData.DataBuf[4] = 0x03  | (FlagByte << 4);
        BtSndData.DataBuf[5] = NormalMode;
        BtSndData.DataBuf[6] = NormalColor;
        BtSndData.DataBuf[7] = 0;
        BtSndData.DataBuf[8] = 0;
        BtSndData.DataBuf[9] = 0;

        FlagChgFlag = TRUE;
        FlashSaveFlag = TRUE;
        LedMode       = LED_NORMAL;
        LedInit       = TRUE;
      } break;


      //Show
      case CMD_SHOW_MODE:{
        //Error check
        if (InfoBuffer > SHOW_COLOR_MAX){
          ErrNum = ERR_SHOW_RANGE;
          break;
        }

        //Random one mode
        srand(RndSeed);
        ShowMode = ((_Uint8)rand()) % 3 + 1;

        //Setting show mode
        ShowColor = InfoBuffer;

        BtSndData.SndStatus  = SND_BROADCAST;
        BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
        BtSndData.DataBuf[1] = 0x00;
        BtSndData.DataBuf[2] = 0x3F;
        BtSndData.DataBuf[3] = 0xFF;
        BtSndData.DataBuf[4] = 0x0A  | (FlagByte << 4);
        BtSndData.DataBuf[5] = ShowMode;
        BtSndData.DataBuf[6] = ShowColor;
        BtSndData.DataBuf[7] = 0;
        BtSndData.DataBuf[8] = 0;
        BtSndData.DataBuf[9] = 0;

        FlagChgFlag = TRUE;
        FlashSaveFlag = TRUE;
        LedMode       = LED_NORMAL;
        LedInit       = TRUE;
      } break;


      // Setting timer
      case CMD_TIMER_SET:{
        //Err check
//        if (InfoBuffer > 8 || (InfoBuffer % 2 == 1)){
//          ErrNum = ERR_TIMER_INVALID;
//          break;
//        }

        BtSndData.SndStatus  = SND_BROADCAST;
        BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
        BtSndData.DataBuf[1] = 0x00;
        BtSndData.DataBuf[2] = 0x3F;
        BtSndData.DataBuf[3] = 0xFF;
        BtSndData.DataBuf[4] = 0x05 | (FlagByte << 4);
        BtSndData.DataBuf[5] = InfoBuffer;
        BtSndData.DataBuf[6] = 0x0;
        BtSndData.DataBuf[7] = 0;
        BtSndData.DataBuf[8] = 0;
        BtSndData.DataBuf[9] = 0;

        FlagChgFlag = TRUE;
        FlashSaveFlag = TRUE;
        LedMode       = LED_NORMAL;
        LedInit       = TRUE;
      } break;
      
      
      //Music mode
      case CMD_MSC_EN:{
        if (InfoBuffer == 0)    MusicEnFlag = FALSE;
        else                    MusicEnFlag = TRUE;
      } break;

      // Error
      default:
      {
      } break;
    }
  }

  else
  {
    // Turn Off
    if (CmdBuffer == CMD_SOCKET_OFF){
      BtSndData.SndStatus  = SND_BROADCAST;
      BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
      BtSndData.DataBuf[1] = 0x00;
      BtSndData.DataBuf[2] = 0x3F;
      BtSndData.DataBuf[3] = 0xFF;
      BtSndData.DataBuf[4] = 0x03 | (FlagByte << 4);
      BtSndData.DataBuf[5] = 0x0;
      BtSndData.DataBuf[6] = 0x0;
      BtSndData.DataBuf[7] = 0;
      BtSndData.DataBuf[8] = 0;
      BtSndData.DataBuf[9] = 0;

      FlagChgFlag = TRUE;
      LedMode       = LED_NORMAL;
      LedInit       = TRUE;
    }

    // Turn On
    else if (CmdBuffer == CMD_SOCKET_ON){
      BtSndData.SndStatus  = SND_BROADCAST;
      BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
      BtSndData.DataBuf[1] = 0x00;
      BtSndData.DataBuf[2] = 0x3F;
      BtSndData.DataBuf[3] = 0xFF;
      BtSndData.DataBuf[4] = 0x03 | (FlagByte << 4);
      BtSndData.DataBuf[5] = 0xFF;
      BtSndData.DataBuf[6] = 0x0;
      BtSndData.DataBuf[7] = 0;
      BtSndData.DataBuf[8] = 0;
      BtSndData.DataBuf[9] = 0;

      FlagChgFlag = TRUE;
      LedMode       = LED_NORMAL;
      LedInit       = TRUE;
    }

    // Error
    else
    {
      ErrNum = ERR_CMD_INVALID;
    }
  }


  // Error occur
  if (FlagChgFlag)
  {
    FlagByte++;
    if (FlagByte > 16)  FlagByte = 1;
  }
}



/**
  * FunctionName    Music_Process
  */
static void Music_Process(void)
{
  static uint8  SendTypeCnt;
  
  if (MusicSndFlag == TRUE){
    MusicSndFlag = FALSE;
    
    BtSndData.SndStatus  = SND_BROADCAST;
    BtSndData.DataBuf[0] = 0x3  | (FlagByte << 4);
    BtSndData.DataBuf[1] = 0x00;
    if (SendTypeCnt > 0){
      SendTypeCnt = 0;
      BtSndData.DataBuf[2] = 0x08;
      BtSndData.DataBuf[3] = 0x00;
    }
    else{
      SendTypeCnt = 1;
      BtSndData.DataBuf[2] = 0x3F;
      BtSndData.DataBuf[3] = 0xFF;
    }
    BtSndData.DataBuf[4] = 0x0B | (FlagByte << 4);
    BtSndData.DataBuf[5] = MusicMode.TotalMode;
    BtSndData.DataBuf[6] = MusicMode.Byte1.All;
    BtSndData.DataBuf[7] = MusicMode.Byte2.All;
    BtSndData.DataBuf[8] = MusicMode.Byte3.All;
    BtSndData.DataBuf[9] = 0;
    
    
    FlagByte++;
    if (FlagByte > 16)  FlagByte = 1;
  }
}
