// SKEW predictor
/*
avg=6.948
max=10.510
min=1.054

Branches		 4184792
Incorrect		  241481
1000*wrong_cc_predicts/total insts 1000 *   241481 / 29499987 =   8.186
Branches		 2866495
Incorrect		  271498
1000*wrong_cc_predicts/total insts 1000 *   271498 / 29499985 =   9.204
Branches		 2213673
Incorrect		  119298
1000*wrong_cc_predicts/total insts 1000 *   119298 / 29499988 =   4.044
Branches		 1792835
Incorrect		   31106
1000*wrong_cc_predicts/total insts 1000 *    31106 / 29499869 =   1.054
Branches		 2229289
Incorrect		  259484
1000*wrong_cc_predicts/total insts 1000 *   259484 / 29499979 =   8.796
Branches		 3809780
Incorrect		  310034
1000*wrong_cc_predicts/total insts 1000 *   310034 / 29499970 =  10.510
Branches		 3660616
Incorrect		  197824
1000*wrong_cc_predicts/total insts 1000 *   197824 / 29499316 =   6.706
Branches		 3537562
Incorrect		  208984
1000*wrong_cc_predicts/total insts 1000 *   208984 / 29499198 =   7.084
*/

#include "predictor.h"

#define POWER(x) (1<<x)

#define BGHR 12
unsigned int ghr;
unsigned int t0[POWER(BGHR)];
unsigned int t1[POWER(BGHR)];
unsigned int t2[POWER(BGHR)];

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

bool givepredict(unsigned short predict)
{
	if(predict > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

unsigned int invhash(unsigned int var)
{
	var = (var & mask(BGHR));
	int n = 1 << (BGHR);
	n = n&var;
	n = n >> (BGHR-1);
	n = n^(var&0x01);
	var = var << 1;
	var = var|n;
	return var;		
}

unsigned int hash(unsigned int var)
{
	var = (var & mask(BGHR));
	int n = var & 0x01;
	n = n << (BGHR-1);
	n = n^var;	
	var = var >> 1;
	var = var|n;
	return var;
}

unsigned int f0(unsigned int pc)
{
	unsigned int res = (hash(ghr) ^ invhash(pc));
	res = ((res ^ pc) & mask(BGHR));
	return res;
}

unsigned int f1(unsigned int pc)
{
	unsigned int res = (hash(ghr) ^ invhash(pc));
	res = ((res ^ ghr) & mask(BGHR));
	return res;
}

unsigned int f2(unsigned int pc)
{
	unsigned int res = (invhash(ghr) ^ hash(pc));
	res = ((res ^ pc) & mask(BGHR));
	return res;
}

void init_predictor()
{
	cout<<endl<<"bits used : "<<(3*2*POWER(BGHR) + BGHR)<<endl;
	ghr = RESET;

	for(int i=0; i<POWER(BGHR); i++)
	{
		t0[i] = NSTATE;
		t1[i] = NSTATE;
		t2[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	unsigned int addr = (f0(pc) & mask(BGHR));
	unsigned short predict0 = (t0[addr] & 0x03);

	addr = (f1(pc) & mask(BGHR));
	unsigned short predict1 = (t1[addr] & 0x03);

	addr = (f2(pc) & mask(BGHR));
	unsigned short predict2 = (t2[addr] & 0x03);

	if((givepredict(predict0)&givepredict(predict1)) || (givepredict(predict0)&givepredict(predict2)) || (givepredict(predict1)&givepredict(predict2)))
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
	unsigned int addr0 = (f0(pc) & mask(BGHR));
	unsigned short predict0 = (t0[addr0] & 0x03);

	unsigned int addr1 = (f1(pc) & mask(BGHR));
	unsigned short predict1 = (t1[addr1] & 0x03);

	unsigned int addr2 = (f2(pc) & mask(BGHR));
	unsigned short predict2 = (t2[addr2] & 0x03);

	bool overallpredict = false;

	if((givepredict(predict0)&givepredict(predict1)) || (givepredict(predict0)&givepredict(predict2)) || (givepredict(predict1)&givepredict(predict2)))
	{
		overallpredict = true;
	}
	else
	{
		overallpredict = false;
	}

	if(overallpredict == outcome)  //partial update
	{
		if(givepredict(predict0) == outcome)
		{
			t0[addr0] = updatepredict(outcome, predict0);
		}

		if(givepredict(predict1) == outcome)
		{
			t1[addr1] = updatepredict(outcome, predict1);
		}

		if(givepredict(predict2) == outcome)
		{
			t2[addr2] = updatepredict(outcome, predict2);
		}
	}
	else //all update
	{
		t0[addr0] = updatepredict(outcome, predict0);
		t1[addr1] = updatepredict(outcome, predict1);
		t2[addr2] = updatepredict(outcome, predict2);
	}

	//shift PHT table entry right
	//unsigned short r = outcome;
	//r = (r << (BGHR-1));
	//ghr = (ghr / 2);
	//ghr = (ghr | r);	

	//shift PHT table entry left
	ghr = (ghr << 1);
	ghr = (ghr | outcome);
}
