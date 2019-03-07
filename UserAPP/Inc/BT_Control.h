/*************************************************************************
	*
	* FILE NAME			BT_Control.h
	* BRIEF					To control the power of bluetooth
	*
**************************************************************************/
#ifndef __BT_CONTROL_H__
#define __BT_CONTROL_H__

//--------------------------< Include Files >---------------------------//
#include "TypeDef.h"
#include "Config.h"


//--------------------------< Type Redefine >--------------------------//
typedef enum EN_BT_CON
{
	BT_IDLE,
	BT_POWER_ON,
	BT_POWER_OFF,
	BT_RESTART
}_TypeBtCon;
   

//-----------------------< Exported Functions >----------------------//
void BT_Work_Detect(void);



//-----------------------< Exported Parameters >---------------------//
extern _TypeBtCon			BtConStatus;
extern _Flag					BtRcvDataFlag;
extern _Flag					BtPowerInitFlag;


//-----------------------< File Parameters >------------------------//
static void BT_Power_Control(void);





#endif
