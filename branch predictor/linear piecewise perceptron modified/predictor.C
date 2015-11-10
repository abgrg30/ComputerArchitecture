// Modified Linear Piecewise Perceptron Predictor

/*
ghr=18, pc=4, ga=4, w=7
avg=5.858375
max=8.968
min=1.073

1000*wrong_cc_predicts/total insts 1000 *   172014 / 29499987 =   5.831
1000*wrong_cc_predicts/total insts 1000 *   232439 / 29499985 =   7.880
1000*wrong_cc_predicts/total insts 1000 *    81413 / 29499988 =   2.760
1000*wrong_cc_predicts/total insts 1000 *    31644 / 29499869 =   1.073
1000*wrong_cc_predicts/total insts 1000 *   215431 / 29499979 =   7.303
1000*wrong_cc_predicts/total insts 1000 *   264542 / 29499970 =   8.968
1000*wrong_cc_predicts/total insts 1000 *   187717 / 29499316 =   6.364
1000*wrong_cc_predicts/total insts 1000 *   197286 / 29499198 =   6.688
*/

#include "predictor.h"

#define POWER(x) (1<<x)

#define BGHR 18
#define BGA 4
#define BPC 4
#define BW 7

unsigned int ghr;
unsigned int gat[BGHR];
int prt[POWER(BPC)][POWER(BGA)][(BGHR)];
int w0;

#define THRESHOLD ((2.14 * (BGHR+1)) + 20.58)
#define RESET 0
#define NSTATE 1

#define MAX (POWER(BW-1)-1)
#define MIN (-1*(MAX+1))

int mask(int x)
{
	x = (1<<x);
	x--;
	return x;
}

bool checkthreshold(int y)
{
	if((y > THRESHOLD) || (y < (-1 * THRESHOLD)))
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
	cout<<endl<<"Total bits consumed : "<<(BGHR + BGHR*BGA + POWER(BPC)*POWER(BGA)*(BGHR)*BW + BW)<<endl;
	//cout<<MAX<<endl<<MIN<<endl<<THRESHOLD<<endl;

	ghr = RESET;
	w0 = 0;	

	for(int i=0; i<(BGHR); i++)
	{
		gat[i] = RESET;
	}

	for(int i=0; i<POWER(BPC); i++)
	{
		for(int j=0; j<POWER(BGA); j++)
		{
			for(int k=0; k<(BGHR); k++)
			{
				prt[i][j][k] = RESET;
			}
		}
	}
}

bool make_prediction(unsigned int pc)
{
	unsigned int addr = (pc & mask(BPC));
	unsigned int temp = (ghr & mask(BGHR));
	int y = w0;
	int w = 0;
	//cout<<y<<endl;

	for(int j=0; j<BGHR; j++)
	{
		w = prt[addr][(gat[j] & mask(BGA))][j];

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
	unsigned int addr = (pc & mask(BPC));
	unsigned int temp = (ghr & mask(BGHR));
	int y = w0;
	int w = 0;
	int t = ((outcome)?1:-1);

	for(int j=0; j<BGHR; j++)
	{
		w = prt[addr][(gat[j] & mask(BGA))][j];

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
			w = prt[addr][(gat[j] & mask(BGA))][j];

			if(temp&0x01)
			{
				w = limitweight(w + t);
			}
			else
			{
				w = limitweight(w - t);
			}

			prt[addr][gat[j]][j] = w;

			temp = (temp>>1);
		}
	}

	for(int i=BGHR; i>1; i--)
	{
		gat[i-1] = gat[i-2];
	}

	gat[0] = (pc & mask(BGA));

	//shift PHT table entry right
	//unsigned int r = outcome;
	//r = (r << (BGHR-1));
	//ghr = (ghr / 2);
	//ghr = (ghr | r);	

	//shift PHT table entry left
	ghr = (ghr << 1);
	ghr = (ghr | outcome);
}
