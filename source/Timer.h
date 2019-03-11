/***********************************************************************//**
 * @file		Timer.h
 * @brief		Contains all functions
 * 				for PWM on LPC1768
 * @version		1.0
 * @date		14. November. 2018
 * @author		Lackshan Senkodan
 **************************************************************************/
#ifndef TIMER2_H_
#define TIMER2_H_
/* Includes ------------------------------------------------------------------- */
#include "LPC17xx.h"

/* Public Functions ----------------------------------------------------------- */
extern void initTimer(LPC_TIM_TypeDef* Timer);
extern void delayMS(unsigned int milliseconds, LPC_TIM_TypeDef* Timer);

#endif
/* --------------------------------- End Of File ------------------------------ */

// Need to implement Match Register stuff so each timer can generate 4 delays each.