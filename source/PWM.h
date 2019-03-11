/***********************************************************************//**
 * @file		PWM.h
 * @brief		Contains all macro definitions and function prototypes
 * 				support for PWM on LPC1768
 * @version		1.0
 * @date		14. November. 2018
 * @author		Lackshan Senkodan
 **************************************************************************/
#ifndef PWM_H_
#define PWM_H_
/* Includes ------------------------------------------------------------------- */
#include "LPC17xx.h"

/* Public Macros -------------------------------------------------------------- */
#define PWMPRESCALE (25-1) //25 PCLK cycles to increment TC by 1 i.e. 1 Micro-second

/* Global Variables -------------------------------------------------------------- */
//volatile uint32_t MR_1 = &LPC_PWM1->MR1;
//static LPC_PWM_TypeDef (* const MR[7]) = { &LPC_PWM1->MR0, &LPC_PWM1->MR1, &LPC_PWM1->MR2, &LPC_PWM1->MR3, &LPC_PWM1->MR4, &LPC_PWM1->MR5, &LPC_PWM1->MR6 };

/* Public Types --------------------------------------------------------------- */
typedef struct {
	volatile uint32_t* PINSEL_Register;
	int PINCON;
	volatile uint32_t* Match_Register;
} PWM_Pin_Type;

/* Public Functions ----------------------------------------------------------- */
extern void initPWM(PWM_Pin_Type Pin);
extern void updatePulseWidth(PWM_Pin_Type Pin, unsigned int pulseWidth);
#endif
/* --------------------------------- End Of File ------------------------------ */