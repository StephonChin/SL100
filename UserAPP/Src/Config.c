/********************************************************************
  *
  * FILE NAME      Config.c
  *
*********************************************************************/
#include "Config.h"

uint8 HseStatus;

/********************************************************************
  *
  * FunctioName    MCU_Config()
  *
*********************************************************************/
void MCU_Config(void)
{

  CLK_Init();
  
  GPIO_Init();
  
  TIM_Init();
  
  USART_Init();
  
  ADC_Init();
  
  //WDT_Init();
}



/**
	* FunctionName		CLK_Init
	*/
static void CLK_Init(void)
{
	/*---------------------------------------------------------------------------------------------------------*/
	/* Init System Clock                                                                                       */
	/*---------------------------------------------------------------------------------------------------------*/
	/* Unlock protected registers */
	SYS_UnlockReg();
	
	/* Set P5 multi-function pins for crystal output/input */
	SYS->P5_MFP &= ~(SYS_MFP_P50_Msk | SYS_MFP_P51_Msk);
	SYS->P5_MFP |= (SYS_MFP_P50_XT1_IN | SYS_MFP_P51_XT1_OUT);

	/* Enable HXT clock (external XTAL 12MHz) */
	CLK_EnableXtalRC(CLK_PWRCTL_XTLEN_HXT);

	/* Wait for HXT clock ready */
	HseStatus = (_Uint8)CLK_WaitClockReady(CLK_STATUS_XTLSTB_Msk);
	
	if (HseStatus == 0) // Change the HCLK to HIRC if HXT clock doesn't work
	{
		/* Enable HIRC clock (Internal RC 22.1184MHz) */
		CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
		/* Wait for HIRC clock ready */
		CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
		/* Select HCLK clock source as HIRC and and HCLK source divider as 1 */
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV_HCLK(1));
	}
	
	/* Set core clock as PLL_CLOCK from PLL */
	CLK_SetCoreClock(PLL_CLOCK);
	
	
//	// Enable LIRC for WDT
//	CLK->PWRCTL = CLK_PWRCTL_LIRCEN_Msk;
//	// Wait for LIRC
//	CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);
//	// Enable WDT IP clock
//	CLK->APBCLK = CLK_APBCLK_WDTCKEN_Msk;
//	CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_WDTSEL_Msk) | CLK_CLKSEL1_WDTSEL_IRC10K;
//	
	
	
	/* Reset UART clock */
  CLK->APBCLK &= ~(CLK_APBCLK_UART0CKEN_Msk | CLK_APBCLK_UART1CKEN_Msk);
	CLK->APBCLK |= (CLK_APBCLK_UART0CKEN_Msk | CLK_APBCLK_UART1CKEN_Msk);
	/* Select UART clock source from external crystal*/
	CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UARTSEL_Msk) | CLK_CLKSEL1_UARTSEL_XTAL;
	/* Set P1 multi-function pins for UART1 RXD and TXD  */
	SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
	SYS->P1_MFP |= (SYS_MFP_P12_UART0_RXD | SYS_MFP_P13_UART0_TXD);
	SYS->P2_MFP &= ~(SYS_MFP_P24_Msk | SYS_MFP_P25_Msk);
	SYS->P2_MFP |= (SYS_MFP_P24_UART1_RXD | SYS_MFP_P25_UART1_TXD);
  
  /* Enable ADC clock */
  CLK_EnableModuleClock(ADC_MODULE);
  
  /*Select ADC clock source */
  CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_XTAL,CLK_CLKDIV_ADC(6));
  
  /* Set P1 multi-function pins for UART RXD, TXD and ADC channel 5 input*/
  SYS->P1_MFP |= SYS_MFP_P14_ADC_CH4;

  /* Analog pin OFFD to prevent leakage */
  P1->DINOFF |= (1 << 4) << GP_DINOFF_DINOFF0_Pos;
	
	
	
	
	/* Enable Timer 0 clock */
  CLK->APBCLK |= CLK_APBCLK_TMR0CKEN_Msk;
	/* Select TIMER0 clock source from external crystal*/
	CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_TMR0SEL_Msk) | CLK_CLKSEL1_TMR0SEL_XTAL;
	
	
	
	/* Enable Timer 1 clock */
  CLK->APBCLK |= CLK_APBCLK_TMR1CKEN_Msk;
	/* Select TIMER0 clock source from LIRC*/
	CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_TMR1SEL_Msk) | CLK_CLKSEL1_TMR1SEL_XTAL;
	
	
	/* Update System Core Clock */
	/* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CycylesPerUs automatically. */
	SystemCoreClockUpdate();
	
	/* Lock protected registers */
  SYS_LockReg();
}





/**
	* FunctionName		WDT_Init
	*/
static void WDT_Init(void)
{
	SYS_UnlockReg();
	
	WDT->CTL = 0x00000300;		// 2^10 = 1024 = 102.4ms
	WDT->CTL |= WDT_CTL_RSTEN_Msk;
	WDT->ALTCTL = 0x00000001;	// Reset after 130 * WDT_CLK = 13ms
	WDT->CTL |= WDT_CTL_WDTEN_Msk;		//Enable
	
	SYS_LockReg();
}




/**
	* Function Name		TIM_Init
	*/
static void TIM_Init(void)
{
	// Set timer 0 working 50Hz in periodic mode
	TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 100);
	TIMER_Start(TIMER0);
  
  // Set timer 0 working 50Hz in periodic mode
	TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 6000);
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
}

/**
  * Function Name: GPIO_Init
	*/
static void GPIO_Init(void)
{
	// PIO11_T-P05	VCC CE-P06	COMM-P07
	GPIO_SetMode(P0, GP_PIN_PIN5_Msk, GPIO_MODE_QUASI);
	GPIO_SetMode(P0, GP_PIN_PIN6_Msk | GP_PIN_PIN7_Msk, GPIO_MODE_OUTPUT);
	PORT_COMM 	= RESET;
	PORT_BT_CE 	= SET;
  
  //P35 audio detect
  GPIO_SetMode(P3, GP_PIN_PIN5_Msk, GPIO_MODE_QUASI);
}


/**
 	* FunctionName: USART_Init
 	*/
static void USART_Init(void)
{ 
	/* Enable Interrupt and install the call back function */
	UART_SetTimeoutCnt(UART0, 30);
	UART_ENABLE_INT(UART0, UART_INTEN_RXTOIEN_Msk);
  NVIC_EnableIRQ(UART0_IRQn);
	UART_Open(UART0, 9600);
  
  
  /* Enable Interrupt and install the call back function */
	UART_SetTimeoutCnt(UART1, 30);
	UART_ENABLE_INT(UART1, UART_INTEN_RDAIEN_Msk);
  NVIC_EnableIRQ(UART1_IRQn);
	UART_Open(UART1, 9600);
}

/**
  * FunctionName  ADC_Init
  */
static void ADC_Init(void)
{
  // Enable channel 4
  ADC_Open(ADC, 0, 0, 0x01 << 4);

  // Power on ADC
  ADC_POWER_ON(ADC);
  
  ADC_START_CONV(ADC);
}


