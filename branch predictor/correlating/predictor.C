// GHR Predictor

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

#define BBHR 14

#define SET 1
#define RESET 0
#define TSTATE 2
#define NSTATE 1

#define MAX      ((1<<BPHT)-1)
#define POWER(x) (1<<x)

unsigned short bhr;
unsigned short bht[POWER(BBHR)];

int mask(int x)
{
	x = (1<<x);
	x--;
	return x;
}

void init_predictor()
{	
	bhr = (sizeof(short) * 8);
	bhr = mask(bhr);

	//bhr = RESET;

	for(int i=0; i<(POWER(BBHR)/2); i++)
	{
		bht[i] = TSTATE;
	}

	for(int i=(POWER(BBHR)/2); i<POWER(BBHR); i++)
	{
		bht[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	unsigned short addr = (bhr & mask(BBHR));

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
	unsigned short addr = (bhr & mask(BBHR));

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

	//shift PHT table entry right
	unsigned short r = outcome;
	r = (r << (BBHR-1));
	bhr = (bhr / 2);
	bhr = (bhr | r);	

	//shift PHT table entry left
	//bhr = (bhr << 1);
	//bhr = (bhr | outcome);
}
