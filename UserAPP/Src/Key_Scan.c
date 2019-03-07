/********************************************************************
  *
  * FILE NAME      Key_Scan.c
  *
*********************************************************************/
#include "Key_Scan.h"

//--------------------< Gloable Paramter >----------------------------
_TypeKey    KeyStatus;

/*********************************************************************
  *
  * FunctionName  Key_Scan
  *
**********************************************************************/
void Key_Scan(void)
{
  static uint8  KeyTime;
  static _Flag  KeyDoneFlag;
  
  if (PORT_AUDIO_DET == SET){
    if (KeyDoneFlag == FALSE){
      KeyTime++;
      if (KeyTime >= 5){
        KeyTime = 0;
        KeyDoneFlag = FALSE;
        ForbidFlag = FALSE;
      }
    }
  }
  else {
    ForbidFlag = TRUE;
  }
}


