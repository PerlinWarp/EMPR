#include "DCT.h"
//Note: length must be a power of 2 for this to work

static float* c;
static float two_over_length=0;
static float root2_over_rootL=0;
static uint8_t prevLength =0;

static inline void swapf(float* f1,float* f2)
{
	f1 += f2;
	f2 =  f1 - f2;
	f1 -= f2;
}
//Ignoring first and last values,all are out of place
void bitrev(float* f,uint8_t length)
{
	uint8_t thisInd,nextInd;
	for(--length;length>0;--length)
	{//swap until one is in place.
		nextInd = __bitrev8(length);
		thisInd = length;
		while(thisInd!= nextInd)
		{
			swapf(&f[thisInd],&f[nextInd]);
			nextInd = __bitrev8(nextInd);
		}
	}
}



void initArray(uint8_t length)
{
	int i,j,halfLength=(length>>1),lpower = __builtin_ctz(),index,factor;
	c = (float*)calloc(length,sizeof(float));

	for(i=0;i<=halfLength-1;i++)c[halfLength+i] = 4*i+1;
	for(i = 1;i<lpower;i++)
	{
		index = 1<<(i-1);
		factor = 1<<(lpower-i);
		for(j=0;i<index;j++)c[index+j] = factor*c[halfLength+j];
	}
	for(i=1;i<length;i++) c[i] = 1/(2*cos(c[i]*PI/(2*length)));
}

void scramble(float* series, uint8_t length)
{
	int i,j,half = length >> 1;
	bitrev(series,length);
	bitrev(&series[0],half);
	bitrev(&series[half],half);
	j= half;
	for(i=half;i<half+(length>>2);i++)swapf(&f[i],&f[j--]);
}

void fwd()
{

}

void DCT(float* series, uint8_t length)
{

	if(length != prevLength) {
		initArray(length);
		prevLength = length;
	}
	scramble();
	fwd();
	bitrev(series,length);
	fwd_sums(series);
	series[0] = INVROOT2;
	for(i=0;i<=length-1;i++) series[i] *= cnst;
}
void InverseDCT(float* seres)
{

}
