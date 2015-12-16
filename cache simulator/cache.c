#include <stdio.h>
#include <math.h>

#define LOAD 0
#define STORE 1

//we'll assume a constant miss penalty of 35
#define miss_penalty 35

// You can change the parameters here.  Your program should work for any 
// reasonable values of CACHESIZE, BLOCKSIZE, or associativity.
//#define CACHESIZE (1024*32)
//#define BLOCKSIZE (64)
//#define associativity (4)

#ifdef SET
#define CACHESIZE (1024*256)
#define BLOCKSIZE (64)
#define associativity (4)
#else
#define CACHESIZE (1024*64)
#define BLOCKSIZE (32)
#define associativity (1)
#endif

#define pow(a) (1<<a)
#define MAX(a,b) ((a>b)?a:b) //
#define indexes (CACHESIZE/(BLOCKSIZE * MAX(1,associativity))) //
#define tags ((MAX(1,associativity) == 1)?indexes:indexes * associativity) //

// a good place to declare your storage for tags, etc.  Obviously,
//   you don't need to actually store the data.

long hits=0, misses=0, readhits=0, readmisses=0;

long lru[tags]; //
long tag[tags];//
long valid[tags];//

int power(long num)
{
    int result = 0;
    
    if(num<=0)
    {
        return 0;
    }
    
    while(num)
    {
        result++;
        num= num/2;
    }    
    
    return result; 
}


//main(int argc, char* argv[])
main() 
{
  //freopen(argv[1], "r" , stdin);
  //printf("\nfile is : %s \n", argv[1]);
  
  long address, references;
  int loadstore, icount;
  char marker;
  long cycles = 0;
  long basecycles = 0;

  int i;
  
  //a good place to initialize your structures.
  
  for(i=0;i<tags;i++)
  {
    lru[i] = 0;
    tag[i] = 0;
    valid[i] = 0;
  }
  
  printf("Cache parameters:\n");
  printf("\tCache size %d\n", CACHESIZE);
  printf("\tCache block size %d\n", BLOCKSIZE);
  printf("\tCache associativity %d\n", associativity);
  
  printf("\tcache lines %d\n", indexes);
  printf("\tcache tags %d\n", tags);
  
  // the format of the trace is
  //    # loadstore address instcount
  //    where loadstore is 0 (load) or 1 (store)
  //          address is the address of the memory access
  //          instcount is the number of instructions (including the load
  //            or store) between the previous access and this one.


  
  while (scanf("%c %d %lx %d\n",&marker,&loadstore,&address,&icount) != EOF) 
  {    
    if (marker == '#')
    {
        //printf("\nreference:%ld", ++references);
    }      
    else 
    { 
      printf("Oops\n");
      continue;
    }

    // for (crude) performance modeling, we will assume a base CPI of 1,
    //     thus every instruction takes one cycle, plus memory access time.
    cycles += icount;
    basecycles += icount;
    cycles += is_cache_miss(loadstore,address,cycles) * miss_penalty;
  }

  printf("Simulation results:\n");
  printf("\texecution cycles %ld cycles\n",cycles);
  printf("\tinstructions %ld\n", basecycles);
  printf("\tmemory accesses %ld\n", hits+misses);
  printf("\toverall miss rate %.2f%%\n", 100.0 * (float) misses / ((float) (hits + misses)) );
  printf("\tread miss rate %.2f%%\n", 100.0 * (float) readmisses / ((float) (readhits + readmisses)) );
  printf("\tmemory CPI %.2f\n", (float) (cycles - basecycles) / (float) basecycles);
  printf("\ttotal CPI %.2f\n", (float) 1.0 + (cycles - basecycles) / (float) basecycles);
  printf("\taverage memory access time %.2f cycles\n",  (float) (cycles - basecycles) / (float) (hits + misses));
  printf("load_misses %ld\n", readmisses);
  printf("store_misses %ld\n", misses - readmisses);
  printf("load_hits %ld\n", readhits);
  printf("store_hits %ld\n", hits - readhits);
}


// you will complete this function.  Notice that we pass the 
//    cycle count to this routine as an argument.  That may make
//    it easier to implement lru.
// this routine returns either 0 (hit) or 1 (miss)

int is_cache_miss(int loadstore, long address, int cycles) 
{
    int hit = 0;
  
    long offset = 0;
    long index = 0;
    long pctag = 0;
    
    //long addressbackup = address;
  
    offset = address&(BLOCKSIZE-1);
    //printf("\n%d", power(BLOCKSIZE-1));
    address = address>>(power(BLOCKSIZE-1));
    index = address&(indexes-1);
    //printf("\n%d", power(indexes-1));
    address = address>>(power(indexes-1));
    
    long tagbits = 32-(power(indexes-1))-(power(BLOCKSIZE-1));
    pctag = address&(pow(tagbits)-1);

    if(associativity == 0 || associativity == 1)
    {
        if(tag[index] == pctag && valid[index] == 1)
        {
            hit = 1;
        }
        else
        {
            //MISS
            tag[index] = pctag;
            valid[index] = 1;
        }
    }
    else
    {
        long start = index * associativity;
        long minlru = cycles;
        long ind = start;
        long replaceflag = 1;
        
        int i;
        for(i=0;i<associativity;i++)
        {
            if(tag[start+i] == pctag && valid[start+i] == 1)
            {
                hit=1;
                lru[start+i] = cycles;
                replaceflag = 0;
                break;
            }
            else
            {                
                if(valid[start+i] == 0)
                {
                    ind = start+i;
                    break;
                }
                else
                {
                    if(minlru > lru[start+i])
                    {
                        minlru = lru[start+i];
                        ind = start+i;
                    }
                }
            }
        }
        
        if(replaceflag)
        {
            lru[ind] = cycles;
            tag[ind] = pctag;
            valid[ind] = 1;
        }
    }
  
  
  
    if (hit) 
    {
      hits++;
      if (loadstore == LOAD) 
        readhits++;
      return 0;
    }

    /* miss */
    //printf("\nmiss @ : %ld", addressbackup);
    misses++;
    if (loadstore == LOAD) 
        readmisses++;
    return 1;
}

