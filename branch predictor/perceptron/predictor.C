// Perceptron Predictor

/*
pc=8, ghr=20, w=6
avg=6.320375
max=9.560
min=0.993

Branches		 4184792
Incorrect		  181159
1000*wrong_cc_predicts/total insts 1000 *   181159 / 29499987 =   6.141
Branches		 2866495
Incorrect		  270756
1000*wrong_cc_predicts/total insts 1000 *   270756 / 29499985 =   9.178
Branches		 2213673
Incorrect		   90256
1000*wrong_cc_predicts/total insts 1000 *    90256 / 29499988 =   3.060
Branches		 1792835
Incorrect		   29300
1000*wrong_cc_predicts/total insts 1000 *    29300 / 29499869 =   0.993
Branches		 2229289
Incorrect		  228451
1000*wrong_cc_predicts/total insts 1000 *   228451 / 29499979 =   7.744
Branches		 3809780
Incorrect		  282003
1000*wrong_cc_predicts/total insts 1000 *   282003 / 29499970 =   9.560
Branches		 3660616
Incorrect		  200135
1000*wrong_cc_predicts/total insts 1000 *   200135 / 29499316 =   6.784
Branches		 3537562
Incorrect		  209526
1000*wrong_cc_predicts/total insts 1000 *   209526 / 29499198 =   7.103
*/

#include "predictor.h"

#define POWER(x) (1<<x)

#define BGHR 20
unsigned int ghr;
int threshold;

#define BPC 8
int prt[POWER(BPC)][(BGHR)+1];
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
	threshold = ((1.93 * BGHR) + 14);

	for(int i=0; i<POWER(BPC); i++)
	{
		for(int j=0; j<(BGHR+1); j++)
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
	int y = prt[addr][BGHR];
	int w = 0;
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
	int y = prt[addr][BGHR];
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
		prt[addr][BGHR] = limitweight(prt[addr][BGHR] + t);

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
