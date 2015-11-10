// MY SKEW predictor
/*
avg=6.58475
max=10.476
min=1.044

Branches		 4184792
Incorrect		  232862
1000*wrong_cc_predicts/total insts 1000 *   232862 / 29499987 =   7.894
Branches		 2866495
Incorrect		  257508
1000*wrong_cc_predicts/total insts 1000 *   257508 / 29499985 =   8.729
Branches		 2213673
Incorrect		  116650
1000*wrong_cc_predicts/total insts 1000 *   116650 / 29499988 =   3.954
Branches		 1792835
Incorrect		   30802
1000*wrong_cc_predicts/total insts 1000 *    30802 / 29499869 =   1.044
Branches		 2229289
Incorrect		  259747
1000*wrong_cc_predicts/total insts 1000 *   259747 / 29499979 =   8.805
Branches		 3809780
Incorrect		  309043
1000*wrong_cc_predicts/total insts 1000 *   309043 / 29499970 =  10.476
Branches		 3660616
Incorrect		  170225
1000*wrong_cc_predicts/total insts 1000 *   170225 / 29499316 =   5.771
Branches		 3537562
Incorrect		  177154
1000*wrong_cc_predicts/total insts 1000 *   177154 / 29499198 =   6.005

*/

#include "predictor.h"

#define POWER(x) (1<<x)

#define BGHR 13
#define MOD 5501
unsigned int ghr;
unsigned int t0[MOD];
unsigned int t1[MOD];
unsigned int t2[MOD];

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
	cout<<endl<<"bits used : "<<(3*2*MOD + BGHR)<<endl;
	ghr = RESET;

	for(int i=0; i<MOD; i++)
	{
		t0[i] = NSTATE;
		t1[i] = NSTATE;
		t2[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	unsigned int addr = (f0(pc) % MOD);
	unsigned short predict0 = (t0[addr] & 0x03);

	addr = (f1(pc) % MOD);
	unsigned short predict1 = (t1[addr] & 0x03);

	addr = (f2(pc) % MOD);
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
	unsigned int addr0 = (f0(pc) % MOD);
	unsigned short predict0 = (t0[addr0] & 0x03);

	unsigned int addr1 = (f1(pc) % MOD);
	unsigned short predict1 = (t1[addr1] & 0x03);

	unsigned int addr2 = (f2(pc) % MOD);
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
