// Perceptron Predictor

/*
pc=8, ghr=21, w=6

avg=6.273
max=9.565
min=0.996

Branches		 4184792
Incorrect		  178074
1000*wrong_cc_predicts/total insts 1000 *   178074 / 29499987 =   6.037
Branches		 2866495
Incorrect		  270420
1000*wrong_cc_predicts/total insts 1000 *   270420 / 29499985 =   9.167
Branches		 2213673
Incorrect		   81022
1000*wrong_cc_predicts/total insts 1000 *    81022 / 29499988 =   2.747
Branches		 1792835
Incorrect		   29392
1000*wrong_cc_predicts/total insts 1000 *    29392 / 29499869 =   0.996
Branches		 2229289
Incorrect		  229115
1000*wrong_cc_predicts/total insts 1000 *   229115 / 29499979 =   7.767
Branches		 3809780
Incorrect		  282150
1000*wrong_cc_predicts/total insts 1000 *   282150 / 29499970 =   9.565
Branches		 3660616
Incorrect		  200157
1000*wrong_cc_predicts/total insts 1000 *   200157 / 29499316 =   6.785
Branches		 3537562
Incorrect		  210026
1000*wrong_cc_predicts/total insts 1000 *   210026 / 29499198 =   7.120
*/

#include "predictor.h"

#define POWER(x) (1<<x)

#define BGHR 21
unsigned int ghr;
int threshold;

#define BPC 8
int prt[POWER(BPC)][BGHR];
int w0;

#define BW 6

#define SET 1
#define RESET 0
#define NSTATE 1

#define MAX (POWER(BW)-1)
#define MIN (-1*(MAX+1))

int mask(int x)
{
	x = (1<<x);
	x--;
	return x;
}

bool checkthreshold(int y)
{
	if((y > threshold) || (y < (-1 * threshold)))
	{
		return false;
	}
	else
	{
		return true;
	}
}


int limitweight(int w)
{
	if(w > MAX)
	{
		w = MAX;
	}

	if(w < MIN)
	{
		w = MIN;
	}
	
	return w;
}

void init_predictor()
{
	ghr = RESET;
	w0 = RESET;
	threshold = ((1.93 * BGHR) + 14);

	for(int i=0; i<POWER(BPC); i++)
	{
		for(int j=0; j<BGHR; j++)
		{
			prt[i][j] = RESET;
		}
	}
}

bool make_prediction(unsigned int pc)
{
	//unsigned int addr = (pc % POWER(BPC));
	unsigned int addr = ((pc^ghr) % POWER(BPC));
	unsigned int temp = (ghr & mask(BGHR));
	int w = 0;
	int y = w0;
	//cout<<y<<endl;

	for(int j=0; j<BGHR; j++)
	{
		w = prt[addr][j];

		if(temp&0x01)
		{
			y = y + w;
		}
		else
		{
			y = y - w;
		}

		temp = (temp>>1);
	}

	//cout<<y<<endl;

	if(y >= 0)
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
	//unsigned int addr = (pc % POWER(BPC));
	unsigned int addr = ((pc^ghr) % POWER(BPC));
	unsigned int temp = (ghr & mask(BGHR));
	int y = w0;
	int w = 0;
	int t = ((outcome)?1:-1);

	for(int j=0; j<BGHR; j++)
	{
		w = prt[addr][j];

		if(temp&0x01)
		{
			y = y + w;
		}
		else
		{
			y = y - w;
		}

		temp = (temp>>1);
	}

	w = ((y>=0)?1:-1);
	temp = (ghr & mask(BGHR));

	if((w != t) || checkthreshold(y))
	{
		w0 = limitweight(w0 + t);

		for(int j=0; j<BGHR; j++)
		{
			w = prt[addr][j];

			if(temp&0x01)
			{
				prt[addr][j] = limitweight(w + t);
			}
			else
			{
				prt[addr][j] = limitweight(w - t);
			}

			temp = (temp>>1);
		}
	}

	//shift PHT table entry right
	//unsigned int r = outcome;
	//r = (r << (BGHR-1));
	//ghr = (ghr / 2);
	//ghr = (ghr | r);	

	//shift PHT table entry left
	ghr = (ghr << 1);
	ghr = (ghr | outcome);
}
