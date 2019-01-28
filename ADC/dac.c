#include <LPC17xx.h>
#include "lpc17xx_pinsel.h"
#include "lpc_types.h"
#include "lpc17xx_uart.h"		
#include <math.h>
#define PI 3.14159265

int timeMS = 0;
// Function Prototypes
void delayms(int);
void sawtooth();
void sineWave(double amp, double freq);
void sineWave2();
int val2voltage();

//Timings
void SysTick_Handler(void){
	timeMS++;
}

void delayMS(int ms){
	int start = timeMS;
	while(timeMS - start < ms){}
}

int main (void) 
{
  	//Start the timer, 1 interupt every ms
	SysTick_Config(SystemCoreClock / 10000);
	// Configure pins
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 2;
	PinCfg.Portnum = 0;	
	PinCfg.Pinmode = 0;
	PinCfg.OpenDrain = 0;
	// Apply config
	// Pin 18 on the MBED board
	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);
	// Initialise DAC
	DAC_Init(LPC_DAC);

	//Stage 2
	uint32_t start, end;
	end = timeMS + (100000);
	while(timeMS < end) {
		sineWave(1.5, 50);
	}
	end = timeMS + (100000);
	while(timeMS < end) {
		sineWave(1.5, 10);
	}
	end = timeMS + (100000);
	while(timeMS < end) {
		sineWave(3, 10);
	}
}


//Takes in a -1 to 1 value from sin
//Returns a 0-3 Voltage
int val2voltage(double val){
	val = val + 1.0; //Scales to range 0-2
	val = val * 512.0; //Scales to range 0 - 1024
	if(val == 1024){
		val = 1023;
	}
	int vali;
	vali = val;
	return vali;
}

//amp is between 0 and 3
void sineWave(double amp, double freq) {
	double i,s,ratio, period;
	period = 1.0 / freq;
	int tickDelay = (int) (period / 36.0) * 10000.0;

	// sprintf(pBuff, "%10i\r\n", tickDelay);
	// write_usb_serial_blocking(pBuff, 12);
	ratio = amp / 3.3;
	for(i = 0; i < 360; i+=10) {
		s = sin(i * (PI / 180.0))*ratio;
		DAC_UpdateValue(LPC_DAC, val2voltage(s));
		delayMS(tickDelay);
	}

}

void sawtooth(){
	int i;
	for(i = 0; i < 1023; i++){
		DAC_UpdateValue(LPC_DAC, i);
		int j;
		for (j = 0; j < 2000; j++){}
	}
}
