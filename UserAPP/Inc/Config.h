/********************************************************************
  *
  * FILE NAME      Config.h
  *
*********************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__


//----------------------< Include Fiels >---------------------------
#include "TypeDef.h"

//Clock frequency define
#define		PLL_CLOCK					48000000


//--------------------< PORT Define >------------------------------
#define PORT_COMM       P07
#define PORT_BT_DT      P05
#define PORT_BT_CE      P06
#define PORT_TX         P13
#define PORT_RX         P12
#define PORT_AUDIO_DET  P35


//--------------------< Exported Functions >----------------------
void MCU_Config(void);


//--------------------< File Functions >---------------------------
static void CLK_Init(void);
static void WDT_Init(void);
static void GPIO_Init(void);
static void TIM_Init(void);
static void USART_Init(void);
static void ADC_Init(void);



#endif
