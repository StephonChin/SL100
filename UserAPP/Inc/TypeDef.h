/********************************************************************
  *
  * FILE NAME      TypeDef.h
  *
*********************************************************************/
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

//----------------------------------------------------------------
// Include Files
//----------------------------------------------------------------
#include "Mini58Series.h"


//----------------------------------------------------------------
// Data type redefine
//----------------------------------------------------------------
typedef 		unsigned char					_Uint8,_Flag,uint8;
typedef 		unsigned short				_Uint16,uint16;
typedef			unsigned int					_Uint32,uint32;
typedef 		signed char						_Sint8,sint8;
typedef			signed short					_Sint16,sint16;
typedef			signed int						_Sint32,sint32;


//--------------------------< Const Value Define >---------------//
#ifndef SET
  #define     SET       (1)
  #define     RESET     (0)
#endif

#ifndef TRUE
  #define     TRUE        (1)
  #define     FALSE       (0)
#endif


#endif
