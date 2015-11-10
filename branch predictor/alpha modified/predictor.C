// Alpha Predictor

//RESULTS
/*                  rshift      lshift
DIST-INT-1          8.975       8.649
DIST-INT-2          9.574	9.577
DIST-FP-1           4.498	4.125
DIST-FP-2           1.071	1.138
DIST-MM-1           9.328	9.476
DIST-MM-2           11.241	11.453
DIST-SERV-1         7.556       7.755
DIST-SERV-2         7.797       8.007
*/

#include "predictor.h"

#define POWER(x) (1<<x)

#define BGHR 12
#define BGHT 2

unsigned short ghr;
unsigned short ght[POWER(BGHR)];

#define BBHR 10
#define BPHT 10
#define BBHT 2

unsigned short bhr;
unsigned short pht[POWER(BBHR)];
unsigned short bht[POWER(BPHT)];

#define BCHR 12
#define BCHT 2

unsigned short chr;
unsigned short cht[POWER(BCHR)];

#define SET 1
#define RESET 0
#define TSTATE 2
#define NSTATE 1

#define MAX      ((1<<BPHT)-1)

int mask(int x)
{
	x = (1<<x);
	x--;
	return x;
}

unsigned short updatepredict(bool outcome, unsigned short predict)
{
	if(outcome)
	{
		if(predict < 3)
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

	return predict;
}

bool predictglobal(unsigned int pc)
{
	unsigned short addr = ((ghr^pc) & mask(BGHR));
	unsigned short predict = (ght[addr] & 0x03);

	if(predict > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool predictlocal(unsigned int pc)
{	
	bhr = (pc & mask(BBHR));
	unsigned short addr = (pht[bhr] & mask(BPHT));
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

void trainglobal(unsigned int pc, bool outcome)
{
	unsigned short addr = ((ghr^pc) & mask(BGHR));
	unsigned short predict = (ght[addr] & 0x03);

	ght[addr] = updatepredict(outcome, predict);

	//shift PHT table entry right
	unsigned short r = outcome;
	r = (r << (BGHR-1));
	ghr = (ghr / 2);
	ghr = (ghr | r);	

	//shift PHT table entry left
	//bhr = (bhr << 1);
	//bhr = (bhr | outcome);
}

void trainlocal(unsigned int pc, bool outcome)
{	
	bhr = (pc & mask(BBHR));
	unsigned short addr = (pht[bhr] & mask(BPHT));
	unsigned short predict = (bht[addr] & 0x03);
	
	bht[addr] = updatepredict(outcome, predict);

	//shift PHT table entry right
	unsigned short r = outcome;
	r = (r << (BPHT-1));
	pht[bhr] = (pht[bhr] / 2);
	pht[bhr] = (pht[bhr] | r);	

	//shift PHT table entry left
	//bhr = (bhr << 1);
	//bhr = (bhr | outcome);
}

void init_predictor()
{
	//bhr = (sizeof(short) * 8);
	//bhr = (mask(BBHR) / 2);
	bhr = RESET;
	ghr = RESET;
	chr = RESET;
	//ghr = (mask(BGHR) / 2);
	//chr = (mask(BCHR) / 2);

	for(int i=0; i<(POWER(BGHR)/2); i++)
	{
		ght[i] = NSTATE;
	}

	for(int i=(POWER(BGHR)/2); i<POWER(BGHR); i++)
	{
		ght[i] = NSTATE;
	}

	for(int i=0; i<POWER(BBHR); i++)
	{
		pht[i] = bhr;
	}

	for(int i=0; i<(POWER(BPHT)/2); i++)
	{
		bht[i] = NSTATE;
	}

	for(int i=(POWER(BPHT)/2); i<POWER(BPHT); i++)
	{
		bht[i] = NSTATE;
	}

	for(int i=0; i<(POWER(BCHR)/2); i++)
	{
		cht[i] = NSTATE;
	}

	for(int i=(POWER(BCHR)/2); i<POWER(BCHR); i++)
	{
		cht[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	chr = ((ghr^pc) & mask(BCHR));
	unsigned short predict = (cht[chr] & 0x03);

	if(predict > 1)
	{
		return predictglobal(pc);
	}
	else
	{
		return predictlocal(pc);
	}
}

void train_predictor(unsigned int pc, bool outcome)
{
	chr = ((ghr^pc) & mask(BCHR));
	unsigned short predict = (cht[chr] & 0x03);

	bool g = predictglobal(pc);
	bool l = predictlocal(pc);
	
	if(g != outcome)
	{
		if(l == outcome)
		{
			if(predict > 0)
			{
				predict--;
			}
		}
	}
	else
	{
		if(l != outcome)
		{
			if(predict < 3)
			{
				predict++;
			}
		}
	}

	cht[chr] = predict;

	trainglobal(pc, outcome);
	trainlocal(pc, outcome);
}
