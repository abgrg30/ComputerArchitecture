// BiMode Predictor
/*      7.091
1000*wrong_cc_predicts/total insts 1000 *   242042 / 29499987 =   8.205
1000*wrong_cc_predicts/total insts 1000 *   281151 / 29499985 =   9.531
1000*wrong_cc_predicts/total insts 1000 *   120029 / 29499988 =   4.069
1000*wrong_cc_predicts/total insts 1000 *    35140 / 29499869 =   1.191
1000*wrong_cc_predicts/total insts 1000 *   282793 / 29499979 =   9.587
1000*wrong_cc_predicts/total insts 1000 *   335406 / 29499970 =  11.370
1000*wrong_cc_predicts/total insts 1000 *   184255 / 29499316 =   6.246
1000*wrong_cc_predicts/total insts 1000 *   192588 / 29499198 =   6.529
*/

#include "predictor.h"

#define POWER(x) (1<<x)

#define MOD 5501
#define BGHR 13
unsigned short ghr;
unsigned short cht[MOD];

#define BPHTT 13
unsigned short phtt[MOD];

#define BPHNT 13
unsigned short phnt[MOD];

#define SET 1
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

bool predictdirectionT(unsigned int pc)
{
	unsigned int addr = ((pc ^ ghr) % MOD);
	unsigned short predict = (phtt[addr] & 0x03);

	if(predict > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool predictdirectionNT(unsigned int pc)
{	
	unsigned short addr = ((pc ^ ghr) % MOD);
	unsigned short predict = (phnt[addr] & 0x03);

	if(predict > 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void traindirectionT(unsigned int pc, bool outcome)
{
	unsigned short addr = ((pc^ghr) % MOD);
	unsigned short predict = (phtt[addr] & 0x03);

	phtt[addr] = updatepredict(outcome, predict);
}

void traindirectionNT(unsigned int pc, bool outcome)
{	
	unsigned short addr = ((pc^ghr) % MOD);
	unsigned short predict = (phnt[addr] & 0x03);
	
	phnt[addr] = updatepredict(outcome, predict);
}

void init_predictor()
{
	cout<<endl<<"bits used : "<<(BGHR + 6*MOD)<<endl;
	ghr = RESET;

	for(int i=0; i<MOD; i++)
	{
		cht[i] = NSTATE;
	}

	for(int i=0; i<MOD; i++)
	{
		phtt[i] = NSTATE;
	}

	for(int i=0; i<MOD; i++)
	{
		phnt[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	unsigned int addr = (pc % MOD);
	unsigned short predict = (cht[addr] & 0x03);

	if(predict > 1)
	{
		return predictdirectionT(pc);
	}
	else
	{
		return predictdirectionNT(pc);
	}
}

void train_predictor(unsigned int pc, bool outcome)
{
	unsigned int addr = (pc % MOD);
	unsigned short predict = (cht[addr] & 0x03);

	bool directionpredict = false;	

	if(predict > 1)
	{
		directionpredict = predictdirectionT(pc);
		traindirectionT(pc, outcome);
	}
	else
	{
		directionpredict = predictdirectionNT(pc);
		traindirectionNT(pc, outcome);
	}
	
	if((predict != outcome) && (directionpredict == outcome))
	{
		//choice predictor not updated
	}
	else
	{
		cht[addr] = updatepredict(outcome, predict);
	}

	//shift PHT table entry right
	unsigned short r = outcome;
	r = (r << (BGHR-1));
	ghr = (ghr / 2);
	ghr = (ghr | r);	

	//shift PHT table entry left
	//ghr = (ghr << 1);
	//ghr = (ghr | outcome);
}
