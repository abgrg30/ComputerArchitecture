// Local Predictor

#include "predictor.h"

#define BBHR 16
#define BPHT BBHR

#define SET 1
#define RESET 0
#define TSTATE 2
#define NSTATE 1

#define MAX      ((1<<BPHT)-1)
#define POWER(x) (1<<x)

unsigned short bhr;
unsigned short pht[POWER(BBHR)];
unsigned short bht[POWER(BPHT)];

int mask(int x)
{
	x = (1<<x);
	x--;
	return x;
}

void init_predictor()
{
	//setting PHT members as MAX
	bhr = (sizeof(short) * 8);
	bhr = mask(bhr);
	
	//setting PHT members as 0
	//bhr = RESET;

	for(int i=0; i<POWER(BBHR); i++)
	{
		pht[i] = bhr;
	}

	bhr = RESET;

	for(int i=0; i<(POWER(BPHT)/2); i++)
	{
		bht[i] = TSTATE;
	}

	for(int i=(POWER(BPHT)/2); i<POWER(BPHT); i++)
	{
		bht[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	bhr = (pc & mask(BBHR));
	
	unsigned short addr = pht[bhr];
	addr = (addr & mask(BPHT));

	unsigned short predict = (bht[addr] & 0x03);

	if(predict > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void train_predictor(unsigned int pc, bool outcome)
{
	bhr = (pc & mask(BBHR));
	
	unsigned short addr = pht[bhr];
	addr = (addr & mask(BPHT));

	unsigned short predict = (bht[addr] & 0x03);

	if(outcome)
	{
		if(predict <= 2)
		{
			predict++;
		}
	}
	else
	{
		if(predict > 0)
		{
			predict--;
		}
	}

	bht[addr] = predict;

	//shift PHT entry right
	unsigned short r = outcome;
	r = (r << (BPHT-1));
	pht[bhr] = (pht[bhr] >> 1);
	pht[bhr] = (pht[bhr] + r);

	//shift PHT table entry left
	//pht[bhr] = (pht[bhr] << 1);
	//pht[bhr] = (pht[bhr] | outcome);
}
