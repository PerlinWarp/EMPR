#include "PWM.h"

void initPWM(PWM_Pin_Type Pin) {
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	*Pin.PINSEL_Register |= Pin.PINCON; //Selects output pin
	LPC_PWM1->PCR = 0x0; 				//Selects PWM mode: Single or Double edged
	LPC_PWM1->PR = PWMPRESCALE; 		//Clock cycles to increment by
	LPC_PWM1->MR0 = 125; 				//Set period duration - 1000us = 1ms period duration
	LPC_PWM1->MCR = (1<<1)|(1<<0); 			//Reset PWM TC on PWM1MR0 match
	LPC_PWM1->LER = (1<<0) | (1<<1); 	//update values in MR0 and MR1 respectively
										// IMPLEMENT METHOD OF CHOOSING VALUES FOR MR1-MR6!!
	LPC_PWM1->PCR = (1<<9); 			//enable PWM output
	LPC_PWM1->TCR = (1<<1); 			//Reset PWM TC & PR

	LPC_PWM1->TCR = (1<<0) | (1<<3); 	//enable counters and PWM Mode
										//PWM Generation goes active now!
}

void updatePulseWidth(PWM_Pin_Type Pin, unsigned int pulseWidth) {
	*Pin.Match_Register = pulseWidth; 	//Update MR1 with new value
	LPC_PWM1->LER = (1<<1); 			//Load the MR1 new value at start of next cycle
}