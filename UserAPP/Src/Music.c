/***************************************************************
  * FILE NAME     Music.c
****************************************************************/
#include "Music.h"

//----- < Gloable Paramter >-----
_Flag         MusicSndFlag;
_TypeMusic    MusicMode;
_Flag         MusicEnFlag;
_Flag         ForbidFlag;
uint8         RestorCnt;



uint16        MusicAdCnt;
uint16        MusicAdBuf[MUSIC_SIMPLE_MAX];
uint8         MusicPre = 0xff;
_Flag         MusicFttFlag;
_TypeCompx    FFT_Buf[FFT_N + 1]; 
uint16        FFT_Result[FFT_N + 1];
uint16        MaxCrt;
uint8         MaxPos;
uint16        DutyVal;
uint8         MusicSndTime;
uint8         MusicUpLevel[7];
uint8         FreqDivGain;
uint8         Gain;
sint16        MscRndSeed;

float SIN_TAB[FFT_N/4+1];



_TypeCompx  WN[N]=//??????
{ //???CPU????,??????????
  //?????FFT???N,?????????
  //??????Excel????
  //  WN[k].real=cos(2*PI/N*k);
  //  WN[k].img=-sin(2*PI/N*k);

  {1.00000,0.00000},{0.99518,-0.09802},{0.98079,-0.19509},{0.95694,-0.29028},
  {0.92388,-0.38268},{0.88192,-0.47140},{0.83147,-0.55557},{0.77301,-0.63439},
  {0.70711,-0.70711},{0.63439,-0.77301},{0.55557,-0.83147},{0.47140,-0.88192},
  {0.38268,-0.92388},{0.29028,-0.95694},{0.19509,-0.98079},{0.09802,-0.99518},
  {0.00000,-1.00000},{-0.09802,-0.99518},{-0.19509,-0.98079},{-0.29028,-0.95694},
  {-0.38268,-0.92388},{-0.47140,-0.88192},{-0.55557,-0.83147},{-0.63439,-0.77301},
  {-0.70711,-0.70711},{-0.77301,-0.63439},{-0.83147,-0.55557},{-0.88192,-0.47140},
  {-0.92388,-0.38268},{-0.95694,-0.29028},{-0.98079,-0.19509},{-0.99518,-0.09802},
  {-1.00000,0.00000},{-0.99518,0.09802},{-0.98079,0.19509},{-0.95694,0.29028},
  {-0.92388,0.38268},{-0.88192,0.47140},{-0.83147,0.55557},{-0.77301,0.63439},
  {-0.70711,0.70711},{-0.63439,0.77301},{-0.55557,0.83147},{-0.47140,0.88192},
  {-0.38268,0.92388},{-0.29028,0.95694},{-0.19509,0.98079},{-0.09802,0.99518},
  {0.00000,1.00000},{0.09802,0.99518},{0.19509,0.98079},{0.29028,0.95694},
  {0.38268,0.92388},{0.47140,0.88192},{0.55557,0.83147},{0.63439,0.77301},
  {0.70711,0.70711},{0.77301,0.63439},{0.83147,0.55557},{0.88192,0.47140},
  {0.92388,0.38268},{0.95694,0.29028},{0.98079,0.19509},{0.99518,0.09802}    
};


// save order(0~15)  exp. 0000->0000  0001->1000  0010->0100 ... 1101->1011  1110->0111  1111->1111
uint8 LIST_TAB[FFT_N] = 
{
  0, 32, 16, 48, 8, 40, 24, 56, 4, 36, 20, 52, 12, 44, 28, 60, 2, 34, 18, 50, 10, 42, 26, 58, 6, 38, 22, 54, 14, 46, 30, 62,
  1, 33, 17, 49, 9, 41, 25, 57, 5, 37, 21, 53, 13, 45, 29, 61, 3, 35, 19, 51, 11, 43, 27, 59, 7, 39, 23, 55, 15, 47, 31, 63
};




/**
  * FTT_Init
  */
void FFT_Init(void)
{
  //create_sin_tab(SIN_TAB);
}


/**
  * FunctionName  TMR1_IRQHandler
  */
void TMR1_IRQHandler(void)
{
  uint32_t u32Flag;
  
  TIMER_ClearIntFlag(TIMER1);
  
  u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADIF_INT);
  ADC_CLR_INT_FLAG(ADC, u32Flag);
  
  if (MusicAdCnt < MUSIC_SIMPLE_MAX){
    MusicAdBuf[MusicAdCnt++] = (uint16)ADC_GET_CONVERSION_DATA(ADC, 0);
  }
  else{
    MusicAdCnt = 0;
    TIMER_Stop(TIMER1);
    MusicFttFlag = SET;
  }
  ADC_START_CONV(ADC);
}



/*????*/

_TypeCompx add(_TypeCompx a,_TypeCompx b)

{

 _TypeCompx c;

 c.real=a.real+b.real;

 c.imag=a.imag+b.imag;

 return c;

}

/*????*/

_TypeCompx sub(_TypeCompx a,_TypeCompx b)

{

 _TypeCompx c;

 c.real=a.real-b.real;

 c.imag=a.imag-b.imag;

 return c;

}
  
/**
  * FunctionName    EE
  */
static _TypeCompx mul(_TypeCompx a,_TypeCompx b)
{

 _TypeCompx c;

 c.real=a.real*b.real - a.imag*b.imag;

 c.imag=a.real*b.imag + a.imag*b.real;

 return c;

}


void Reverse(void)

{

 unsigned int i,j,k;

 unsigned int t;

 _TypeCompx temp;

 for(i=0;i<N;i++)

 {

  k=i;//???i???

  j=0;//????????,?????0

  for(t=0;t<log2N;t++)//???t?,??log2N?????????

  {

   j<<=1;

   j|=(k&1);//j????????k????

   k>>=1;//k????,????????

  }

  if(j>i)//??????????,????????????(??j>i?????????)

  {

   temp=FFT_Buf[i];

   FFT_Buf[i]=FFT_Buf[j];

   FFT_Buf[j]=temp;

  }

 }

}

void FFT(void)
{

 unsigned int i,j,k,l;

 _TypeCompx top,bottom,xW;

 Reverse(); //????

 for(i=0;i<log2N;i++)   /*?log2N?*/

 {    //??????

  l=1<<i;//l??2?i??

  for(j=0;j<N;j+=2*l)  /*?L??????,???N/2L?*/

  {   //??????

   for(k=0;k<l;k++)   /*???L?*/

   {  //??????

     xW=mul(FFT_Buf[j+k+l],WN[N/(2*l)*k]); //????l

     top=add(FFT_Buf[j+k],xW); //????k???

     bottom=sub(FFT_Buf[j+k],xW);

     FFT_Buf[j+k]=top;

     FFT_Buf[j+k+l]=bottom;

   }

  }

 }

}





/***************************************************************
  *
  * FunctionName    Music_AD
  *
****************************************************************/
void Music_AD(void)
{
  uint8    temp = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
  uint16    modeTemp = 0;
  
//  if (MusicEnFlag == FALSE || ForbidFlag == TRUE){
//    UpdateFlag = TRUE;
//    return;
//  }
  
  if (ForbidFlag == TRUE){
    if (LedMode != LED_OFF)   UpdateFlag = TRUE;
    return;
  }
  
  if (MusicFttFlag == RESET)   {
    MusicSndTime++;
    if (LedMode != LED_OFF)   UpdateFlag = TRUE;
    if (MusicSndTime >= 15){
      MusicSndTime = 0;
      UpdateFlag = FALSE;
      TIMER_Start(TIMER1);
    }
    return;
  }
  MusicFttFlag = RESET;  
  
 

  
  // Simple Data and Clear arguments
  for (temp = 0; temp < FFT_N; temp++)
  {                                                                                                    
    FFT_Buf[LIST_TAB[temp]].real = MusicAdBuf[temp];
    FFT_Buf[temp].imag   = 0;
  }
  // Caculator the FFT
  FFT();
  
  // Valve Value
  MaxCrt = 0;
  for (temp = 0; temp < FFT_N; temp++)
  {
    FFT_Result[temp] = (uint16)sqrt(FFT_Buf[temp].imag * FFT_Buf[temp].imag + FFT_Buf[temp].real * FFT_Buf[temp].real);
    if (temp > 0 && temp < 32){
      if (MaxCrt < FFT_Result[temp]){
        MaxCrt = FFT_Result[temp];
        MaxPos = temp;
      }
    }
  }
  
  
  
  //amp adjust
  if (MaxCrt < 50 && MaxCrt > 15)   Gain = 100 / MaxCrt;
  else                              Gain = 1;
  
  modeTemp = Max_Value_Get(FFT_Result[1],FFT_Result[2],FFT_Result[3],FFT_Result[4],FFT_Result[5]) * Gain;
  if (modeTemp > HIGH_LEVEL)   modeTemp = HIGH_LEVEL;
  MusicMode.Byte1.Buf.High = (uint8)(modeTemp / TREE_DIV);
  if (MusicMode.Byte1.Buf.High < 3 && MaxCrt > 15){
    MscRndSeed += 1023;
    MusicMode.Byte1.Buf.High = (uint8)rand() % 3 + (Gain * MaxCrt) / 25;
  }
  
  modeTemp = Max_Value_Get(FFT_Result[6],FFT_Result[7],FFT_Result[8],FFT_Result[9],FFT_Result[10]) * Gain;
  if (modeTemp > HIGH_LEVEL)   modeTemp = HIGH_LEVEL;
  MusicMode.Byte1.Buf.Low= (uint8)(modeTemp / TREE_DIV);
  if (MusicMode.Byte1.Buf.Low < 3 && MaxCrt > 15){
    MscRndSeed += 1023;
    MusicMode.Byte1.Buf.Low = (uint8)rand() % 3 + (Gain * MaxCrt) / 25;
  }
  
  modeTemp = Max_Value_Get(FFT_Result[11],FFT_Result[12],FFT_Result[13],FFT_Result[14],FFT_Result[15]) * Gain;
  if (modeTemp > HIGH_LEVEL)   modeTemp = HIGH_LEVEL;
  MusicMode.Byte2.Buf.High = (uint8)(modeTemp / TREE_DIV);
  if (MusicMode.Byte2.Buf.High < 3 && MaxCrt > 15){
    MscRndSeed += 1023;
    MusicMode.Byte2.Buf.High = (uint8)rand() % 3 + (Gain * MaxCrt) / 25;
  }
  
  modeTemp = Max_Value_Get(FFT_Result[16],FFT_Result[17],FFT_Result[18],FFT_Result[19],FFT_Result[20]) * Gain;
  if (modeTemp > HIGH_LEVEL)   modeTemp = HIGH_LEVEL;
  MusicMode.Byte2.Buf.Low = (uint8)(modeTemp / TREE_DIV);
  if (MusicMode.Byte2.Buf.Low < 3 && MaxCrt > 15){
    MscRndSeed += 1023;
    MusicMode.Byte2.Buf.Low = (uint8)rand() % 3 + (Gain * MaxCrt) / 25;
  }
  
  
  modeTemp = Max_Value_Get(FFT_Result[21],FFT_Result[22],FFT_Result[23],FFT_Result[24],FFT_Result[25]) * Gain;
  if (modeTemp > HIGH_LEVEL)   modeTemp = HIGH_LEVEL;
  MusicMode.Byte3.Buf.High = (uint8)(modeTemp / TREE_DIV);
  if (MusicMode.Byte3.Buf.High < 3 && MaxCrt > 15){
    MscRndSeed += 1023;
    MusicMode.Byte3.Buf.High = (uint8)rand() % 3 + (Gain * MaxCrt) / 25;
  }
  
  modeTemp = Max_Value_Get(FFT_Result[26],FFT_Result[27],FFT_Result[28],FFT_Result[29],FFT_Result[30]) * Gain;
  if (modeTemp > HIGH_LEVEL)   modeTemp = HIGH_LEVEL;
  MusicMode.Byte3.Buf.Low = (uint8)(modeTemp / TREE_DIV);
  if (MusicMode.Byte3.Buf.Low < 3 && MaxCrt > 15){
    MscRndSeed += 1023;
    MusicMode.Byte3.Buf.Low = (uint8)rand() % 3 + (Gain * MaxCrt) / 25;
  }
  
  
  if (MaxCrt > 15){
    MaxPos = (MaxPos - 1) / 5;
    modeTemp = MaxPos * 5 + MusicUpLevel[MaxPos];
    MusicUpLevel[MaxPos]++;
    if (MusicUpLevel[MaxPos] > 4) MusicUpLevel[MaxPos] = 0;
    MusicMode.TotalMode = modeTemp;
    MusicSndFlag = TRUE;
    
    if (RestorCnt < 50)   RestorCnt++;
  }
}


/**
  * FunctionName    Max_Value_Get
  */
static uint16 Max_Value_Get(uint16 val1, uint16 val2, uint16 val3, uint16 val4, uint16 val5)
{
  uint16 valBuf = 0;
  
  if (valBuf < val1)    valBuf = val1;
  if (valBuf < val2)    valBuf = val2;
  if (valBuf < val3)    valBuf = val3;
  if (valBuf < val4)    valBuf = val4;
  if (valBuf < val5)    valBuf = val5;
  
  return valBuf;
}
