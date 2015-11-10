// GShare Predictor
/* lshift        7.460875
1000*wrong_cc_predicts/total insts 1000 *   254894 / 29499987 =   8.641
1000*wrong_cc_predicts/total insts 1000 *   278874 / 29499985 =   9.454
1000*wrong_cc_predicts/total insts 1000 *   121320 / 29499988 =   4.113
1000*wrong_cc_predicts/total insts 1000 *    33118 / 29499869 =   1.123
1000*wrong_cc_predicts/total insts 1000 *   277041 / 29499979 =   9.392
1000*wrong_cc_predicts/total insts 1000 *   337746 / 29499970 =  11.449
1000*wrong_cc_predicts/total insts 1000 *   225312 / 29499316 =   7.638
1000*wrong_cc_predicts/total insts 1000 *   232372 / 29499198 =   7.877

rshift          7.4345
1000*wrong_cc_predicts/total insts 1000 *   263413 / 29499987 =   8.930
1000*wrong_cc_predicts/total insts 1000 *   278906 / 29499985 =   9.455
1000*wrong_cc_predicts/total insts 1000 *   132302 / 29499988 =   4.485
1000*wrong_cc_predicts/total insts 1000 *    30910 / 29499869 =   1.048
1000*wrong_cc_predicts/total insts 1000 *   272832 / 29499979 =   9.249
1000*wrong_cc_predicts/total insts 1000 *   331241 / 29499970 =  11.229
1000*wrong_cc_predicts/total insts 1000 *   218988 / 29499316 =   7.424
1000*wrong_cc_predicts/total insts 1000 *   225855 / 29499198 =   7.656
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
	cout<<endl<<"bits used : "<<(2*POWER(BBHR) + BBHR)<<endl;
	bhr = RESET;

	for(int i=0; i<POWER(BBHR); i++)
	{
		bht[i] = NSTATE;
	}
}

bool make_prediction(unsigned int pc)
{
	unsigned short addr = ((pc^bhr) & mask(BBHR));
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
	unsigned short addr = ((pc^bhr) & mask(BBHR));
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
