/********************************************************************
  *
  * FILE NAME      Data_Process.h
  *
*********************************************************************/
#ifndef __DATA_PROCESS_H__
#define __DATA_PROCESS_H__

//--------------------< Include Files >-------------------------------
#include "USART.h"
#include "Key_Scan.h"
#include "BT_Control.h"
#include "Flash_Prgm.h"
#include "stdlib.h"
#include "Led.h"
#include "Music.h"

//--------------------< Const Value Define >--------------------------
#define     NORMAL_COLOR_MAX           0x1C
#define     NORMAL_MODE_MAX            0x09
#define     SHOW_MODE_MAX              0x02
#define     SHOW_COLOR_MAX             0x06


//Type Redefine
typedef enum{
  ERR_NONE                  = 0x0,
  ERR_CMD_INVALID           = 0x30,         //command is invalid
  ERR_COLOR_RANGE           = 0x31,         //theme value overflow maximum
  ERR_MODE_RANGE            = 0x32,         //mode value overflow maximum
  ERR_SHOW_RANGE            = 0x33,         //show mode overflow maximum
  ERR_TIMER_INVALID         = 0x34,         //timer value is invalid
}EN_ERR_NUM;



//--------------------< Exported Function >---------------------------
void User_Data_Init(void);
void Data_Process(void);

//--------------------< File Functions >------------------------------
static void Key_Process(void);
static void CSR1010_Process(void);
static void Wifi_Process(void);
static void Music_Process(void);


//Exported arguments
extern _Uint8        NormalMode;
extern _Uint8        ShowMode;
extern _Uint8        NormalColor;
extern _Uint8        ShowColor;



#endif
