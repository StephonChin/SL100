/********************************************************************
  *
  * FILE NAME      Key_Scan.h
  *
*********************************************************************/
#ifndef __KEY_SCAN_H__
#define __KEY_SCAN_H__

//--------------------< Include File >------------------------------
#include "TypeDef.h"
#include "Config.h"
#include "Music.h"


//--------------------< Type Redefine >-----------------------------
typedef enum _KEY_TYPE
{
  KEY_IDLE,
  KEY_1_SHORT,
  KEY_2_SHORT,
  KEY_1_LONG,
  KEY_2_LONG
}_TypeKey;


//--------------------< Exported Function >-------------------------
void Key_Scan(void);


//--------------------< Exported Paramters >------------------------
extern _TypeKey   KeyStatus;


#endif
