// POWER4

//avg=6.691625
//max=10.362
//min=1.069

#include "predictor.h"

#define POWER(x) (1<<x)

#define BGHR 12
unsigned short ghr;
unsigned short bht[POWER(BGHR)];

#define BGSR 13
unsigned short gst[POWER(BGSR)];

#define BCHR 12
unsigned short cht[POWER(BCHR)];

#define RESET 0
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

bool predictgshare(unsigned int pc)
{
	unsigned int addr = ((pc ^ ghr) & mask(BGSR));
	unsigned short predict = (gst[addr] & 0x03);

	if(predict > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool predictbimodal(unsigned int pc)
{	
	unsigned short addr = (pc & mask(BGHR));
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

void traingshare(unsigned int pc, bool outcome)
{
	unsigned short addr = ((pc^ghr) & mask(BGSR));
	unsigned short predict = (gst[addr] & 0x03);

	gst[addr] = updatepredict(outcome, predict);
}

void trainbimodal(unsigned int pc, bool outcome)
{	
	unsigned short addr = (pc & mask(BGHR));
	unsigned short predict = (bht[addr] & 0x03);
	
	bht[addr] = updatepredict(outcome, predict);
}

void init_predictor()
{
	ghr = RESET;

	for(int i=0; i<POWER(BGHR); i++)
	{
		bht[i] = NSTATE;
	}

	for(int i=0; i<POWER(BGSR); i++)
	{
		gst[i] = NSTATE;
	}

	for(int i=0; i<POWER(BCHR); i++)
	{
		cht[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	unsigned int addr = ((pc ^ ghr) & mask(BCHR));
	//unsigned int addr = (pc & mask(BCHR));
	unsigned short predict = (cht[addr] & 0x03);

	if(predict > 1)
	{
		return predictgshare(pc);
	}
	else
	{
		return predictbimodal(pc);
	}
}

void train_predictor(unsigned int pc, bool outcome)
{
	unsigned int addr = ((pc^ghr) & mask(BCHR));
	//unsigned int addr = (pc & mask(BCHR));
	unsigned short predict = (cht[addr] & 0x03); 

	bool g = predictgshare(pc);
	bool l = predictbimodal(pc);

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

	cht[addr] = predict;

	traingshare(pc, outcome);
	trainbimodal(pc, outcome);

	//shift PHT table entry right
	//unsigned short r = outcome;
	//r = (r << (BGHR-1));
	//ghr = (ghr / 2);
	//ghr = (ghr | r);	

	//shift PHT table entry left
	ghr = (ghr << 1);
	ghr = (ghr | outcome);
}
