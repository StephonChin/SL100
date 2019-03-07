/***********************************************
  * FILE NAME     Music.h
************************************************/
#ifndef __MUSIC_H__
#define __MUSIC_H__

//include files
#include "TypeDef.h"
#include "Led.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//----- < Const Value Define >-----


//#define     FFT_16    1
//#define     FFT_32    1
#define     FFT_64    1


#ifdef FFT_16
  #define     FFT_N                 16
  #define     FFT_LOG_N             4
#endif

#ifdef FFT_32
  #define     FFT_N                 32
  #define     FFT_LOG_N             5 
  #define     HIGH_LEVEL            250
  #define     TREE_DIV              15
  #define     STAR_DIV              7
  
  #define     MUSIC_SIMPLE_MAX      33
#endif

#ifdef FFT_64
  #define     FFT_N                 64
  #define     FFT_LOG_N             6
  #define     N                     64
  #define     log2N                 6
  
  #define     HIGH_LEVEL            145
  #define     TREE_DIV              9
  #define     STAR_HIGH             135
  #define     STAR_DIV              20
  
  #define     MUSIC_SIMPLE_MAX      65
#endif




#ifndef PI
#define PI             (3.1429265)
#endif


//type redefine
typedef struct{
  float real;
  float imag;
}_TypeCompx;

typedef union{
  uint8 All;
  struct{
    uint8   High : 4;
    uint8   Low : 4;
  }Buf;
}_TypeUnMusic;


typedef struct MUSIC_MODE{
  uint8 TotalMode;
  uint8 TotalModePre;
  _TypeUnMusic Byte1;
  _TypeUnMusic Byte2;
  _TypeUnMusic Byte3;
}_TypeMusic;


//----- < Exported Functions >-----
void Music_AD(void);
void FFT_Init(void);


//file functions
_TypeCompx add(_TypeCompx a,_TypeCompx b);
_TypeCompx sub(_TypeCompx a,_TypeCompx b);
static _TypeCompx mul(_TypeCompx a,_TypeCompx b);
void FFT(void);
static uint16 Max_Value_Get(uint16 val1, uint16 val2, uint16 val3, uint16 val4, uint16 val5);


//----- < Exported Paramter >-----
extern _Flag         MusicSndFlag;
extern _TypeMusic    MusicMode;
extern _Flag         MusicEnFlag;
extern _Flag         ForbidFlag;
extern uint8         RestorCnt;

#endif






