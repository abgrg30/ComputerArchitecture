// Linear Piecewise Perceptron Predictor
/*
Branches		 4184792
Incorrect		  170889
1000*wrong_cc_predicts/total insts 1000 *   170889 / 29499987 =   5.793
Branches		 2866495
Incorrect		  230669
1000*wrong_cc_predicts/total insts 1000 *   230669 / 29499985 =   7.820
Branches		 2213673
Incorrect		   81388
1000*wrong_cc_predicts/total insts 1000 *    81388 / 29499988 =   2.759
Branches		 1792835
Incorrect		   31606
1000*wrong_cc_predicts/total insts 1000 *    31606 / 29499869 =   1.071
Branches		 2229289
Incorrect		  215086
1000*wrong_cc_predicts/total insts 1000 *   215086 / 29499979 =   7.291
Branches		 3809780
Incorrect		  264862
1000*wrong_cc_predicts/total insts 1000 *   264862 / 29499970 =   8.979
Branches		 3660616
Incorrect		  186176
1000*wrong_cc_predicts/total insts 1000 *   186176 / 29499316 =   6.311
Branches		 3537562
Incorrect		  196814
1000*wrong_cc_predicts/total insts 1000 *   196814 / 29499198 =   6.672
*/
#include "predictor.h"

unsigned int ghr;          //global history register
unsigned int gat[BGHR];    //global address table

int w0[POWER(BPC)];          //table for weights0
int prt[POWER(BPC)][POWER(BGA)][BGHR]; //table of weights
int y;                  //output

int mask(int x)          //to get mask of x bits
{
	x = (1<<x);
	x--;
	return x;
}

bool checkthreshold(int y)        //check limits of output (|y| < limit)
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


int limitweight(int w)       //check limits of weight (|w| < limit)
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
	//cout<<endl<<"Total bits consumed : "<<(16 + BGHR + BGHR*BGA + POWER(BPC)*BW + POWER(BPC)*POWER(BGA)*(BGHR)*BW)<<endl;
	//cout<<MAX<<endl<<MIN<<endl<<THRESHOLD<<endl;

	ghr = RESET;
	y = RESET;	

	for(int i=0; i<(BGHR); i++)
	{
		gat[i] = RESET;
	}

	for(int i=0; i<POWER(BPC); i++)
	{
		w0[i] = RESET;

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
	unsigned int addr = (pc & mask(BPC));           // pc masked address
	unsigned int temp = (ghr & mask(BGHR));         // ghr masked value
	y = w0[addr];                                 //y=y+w0;
	int w = 0;

	for(int j=0; j<BGHR; j++)
	{
		w = prt[addr][(gat[j] & mask(BGA))][j];      //accessing weight table to get value

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

	return ((y >= 0)?true:false);
}

void train_predictor(unsigned int pc, bool outcome)
{
	unsigned int addr = (pc & mask(BPC));
	unsigned int temp = (ghr & mask(BGHR));

	int t = ((outcome)?1:-1);
	int w = ((y>=0)?1:-1);

	if((w != t) || checkthreshold(y))
	{
		w0[addr] = limitweight(w0[addr] + t);         //w0=w0+t

		for(int j=0; j<BGHR; j++)
		{
			w = prt[addr][(gat[j] & mask(BGA))][j];

			if(temp&0x01)
			{
				w = limitweight(w + t);            //w=w+t
			}
			else
			{
				w = limitweight(w - t);
			}

			prt[addr][gat[j]][j] = w;

			temp = (temp>>1);
		}
	}

	for(int i=BGHR; i>1; i--)         //shifting addresses in Global address table
	{
		gat[i-1] = gat[i-2];
	}

	gat[0] = (pc & mask(BGA));	//first index of global address table used for latest pc address

	//shift PHT table entry left
	ghr = (ghr << 1);                 //ghr left shifted
	ghr = (ghr | outcome);
}
