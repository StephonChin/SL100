/********************************************************************
  *
  * FILE NAME      Led.h
  *
*********************************************************************/
#ifndef __LED_H__
#define __LED_H__

//--------------------< Include Files >------------------------------
#include "TypeDef.h"
#include "Config.h"
#include "Music.h"

//define
#define   LED_OFF                   0
#define   LED_WIFI_STANDBY          1
#define   LED_WIFI_DISCONNECT       2
#define   LED_WIFI_CONNECTED        3
#define   LED_NORMAL                4
#define   LED_ERROR                 5

#define   LED_TOTAL                 10
#define   COLOR_TOTAL               30

//type redefine
typedef union{
  _Uint8 SendBuf;
  struct{
    _Uint8  DutyG;
    _Uint8  DutyR;
    _Uint8  DutyB;
  }Buffer[LED_TOTAL];
}_TypeLed_U;

//Exported arguments
extern _Uint8     LedMode;
extern _Flag      LedInit;
extern _Flag      UpdateFlag;

//Exported Functions
void Led_Control(void);

//File Functions

void Led_Update(void);
static void Led_Off(void);
static void Wifi_Standby(void);
static void Wifi_Disconnect(void);
static void Wifi_Connected(void);
static void Led_Normal(void);
static void Led_Err(void);


#endif
