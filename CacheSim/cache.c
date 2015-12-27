#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define LOAD 0
#define STORE 1

//we'll assume a constant miss penalty of 35
#define miss_penalty 35

// You can change the parameters here.  Your program should work for any 
//     reasonable values of CACHESIZE, BLOCKSIZE, or associativity.


#ifdef SET_ASSOCIATIVE
#define CACHESIZE 1024*32
#define BLOCKSIZE 64
#define associativity 1
#else
#define CACHESIZE 1024*64
#define BLOCKSIZE 32
#define associativity 1
#endif

// a good place to declare your storage for tags, etc.  Obviously,
//   you don't need to actually store the data.

  long hits=0, misses=0, readhits=0, readmisses=0;
  int is_cache_miss(int loadstore, long address, int cycles) ;


typedef struct set_{
  
  int tag[associativity];  	
  int cyc[associativity];
  int valid[associativity];	
}Set;




int blockOffset;
int setMask;
int NumOfSets; 


Set *set;

int main(
int argc,char *argv[]) {

  long address, references;
  int loadstore, icount;
  char marker;
  long cycles = 0;
  long basecycles = 0;

  int i,j;
  
  FILE *fp = fopen(argv[1],"r");
	
  
  //a good place to initialize your structures.
  NumOfSets = (CACHESIZE)/((BLOCKSIZE)*(associativity));
  set = (Set*)calloc(NumOfSets,sizeof(Set));
  
#ifdef SET_ASSOCIATIVE
  blockOffset = 6; 
#else
  blockOffset = 5; 
#endif
  setMask = NumOfSets-1;	
     
  for(i=0;i<NumOfSets;i++)
  {
    for(j=0;j<associativity;j++)
    { 
       set[i].tag[j]=0;
       set[i].cyc[j]=0;
       set[i].valid[j] = 0;  
    }
  } 

  fprintf(stderr,"Num of sets = %d\n",NumOfSets);
  fprintf(stderr,"Block Offset = %d\n",blockOffset);
  fprintf(stderr,"set Mask = %d\n",setMask);
  
  printf("Cache parameters:\n");
  printf("\tCache size %d\n", CACHESIZE);
  printf("\tCache block size %d\n", BLOCKSIZE);
  printf("\tCache associativity %d\n", associativity);

  // the format of the trace is
  //    # loadstore address instcount
  //    where loadstore is 0 (load) or 1 (store)
  //          address is the address of the memory access
  //          instcount is the number of instructions (including the load
  //            or store) between the previous access and this one.

  while (fscanf(fp,"%c %d %lx %d\n",&marker,&loadstore,&address,&icount) != EOF) {
    
    if (marker == '#')
      references++;
    else { 
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

int is_cache_miss(int loadstore, long address, int cycles) {

  int hit;
  int setId    = (address>>blockOffset)&setMask;	
  int currTag  = (address>>blockOffset)/NumOfSets;
  int i,j; 
  bool IsConflict=true;   
  int lruIdx=0;
  int cycMin; 
   




//   fprintf(stderr,"address = %d\t setId = %d \t currTag = %d\t \n",address,setId,currTag);



 
  hit = 0;
  for(i=0;i<associativity;i++)
  {
     if(set[setId].tag[i]==currTag)  
     {    hit=1;
          set[setId].cyc[i]=cycles; 
          IsConflict = false;
          break;
     }

     else if(set[setId].valid[i]==0)
     {
          set[setId].tag[i]=currTag;  
          set[setId].cyc[i]=cycles; 
          IsConflict = false;
          set[setId].valid[i] = 1;
          break;
     }
  }
        
  if((!hit)&&(IsConflict))
  {
     
#ifdef SET_ASSOCIATIVE    
     lruIdx = 0;
     cycMin = set[setId].cyc[0]; 
     //int *ptr = &(set[setId].cyc[0]);   
      for(i=1;i<associativity;i++)
     {
         if(set[setId].cyc[i]<cycMin)
            lruIdx = i;
     }
                        
     set[setId].tag[lruIdx] = currTag;
     set[setId].cyc[lruIdx] = cycles;  
     set[setId].valid[lruIdx] = 1;  

#else
     set[setId].tag[0] = currTag;
     set[setId].cyc[0] = cycles;
     set[setId].valid[0]  = 1;

#endif  

  }

        
// fprintf(stderr,"address = %d\t setId = %d \t currTag = %d\t hit= %d\n",address,setId,currTag,hit);


  if (hit) {
      hits++;
      if (loadstore == LOAD) 
	readhits++;
      return 0;
  }

  /* miss */
  misses++;
  if (loadstore == LOAD) 
    readmisses++;
  return 1;
  


}

