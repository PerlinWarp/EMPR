#include "Timer.h"

void initTimer(LPC_TIM_TypeDef* Timer)
{
	Timer->CTCR = 0x0;
	Timer->PR = 25000-1; //Increment TC at every 24999+1 clock cycles
	//25000 clock cycles @25Mhz = 1 mS
	Timer->TCR = 0x02; //Reset Timer
}

void delayMS(unsigned int milliseconds, LPC_TIM_TypeDef* Timer)
{
	Timer->TCR = 0x02; //Reset Timer
	Timer->TCR = 0x01; //Enable timer

	while(Timer->TC < milliseconds); //wait until timer counter reaches the desired delay

	Timer->TCR = 0x00; //Disable timer
}