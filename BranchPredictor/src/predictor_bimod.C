// A very stupid predictor.  It will always predict not taken.
#include <stdio.h>
#define TLEN 12 // Taken BHT's length
#define NLEN 12 // Not Taken BHT's length
#define CLEN 12 // Choice table length
#define TSIZE (1<<(TLEN))
#define NSIZE (1<<(NLEN))
#define CSIZE (1<<(CLEN))
#define TMASK (TSIZE-1)
#define NMASK (NSIZE-1)
#define CMASK (CSIZE-1)





int NTaken[NSIZE];
int Taken[TSIZE];
int Choice[CSIZE];
int shiftReg;
int phtAddress = 0;
int GshareAddress=0;
int BMAddress;
int address;
//#define CorPred
// Let's consider a 1K Local Pattern Table or 32 addresses
int Gsharevalue ;
int BMvalue ;
bool Tpredict,NTpredict;
int addressTaken=0,addressNTaken=0;
void table_init()
{
    int i;
    // Initialise Pattern Table For Gshare
    for(i=0;i<TSIZE;i++)
        Taken[i] = 0;
    for(i=0;i<NSIZE;i++)
        NTaken[i] = 0;
    for(i=0;i<CSIZE;i++)
        Choice[i] = 0;
    
    shiftReg = 0;
}


void sr_add(char i)
{
    shiftReg = (shiftReg<<1)|i;
    shiftReg = shiftReg&TMASK;
    
}

int pattern_address()
{

    return shiftReg;    
}




void init_predictor ()
{
    table_init();
}

bool make_prediction (unsigned int pc)
{
  
      
 addressTaken  = pattern_address()^(pc&TMASK); 
 addressNTaken = pattern_address()^(pc&NMASK); 
 
 Tpredict =  (bool)(Taken[addressTaken]>>1);
 NTpredict = (bool)(NTaken[addressNTaken]>>1);

    
  int choose = Choice[(pc&CMASK)];
 	
  if(choose<2)
    return NTpredict;
  else
    return Tpredict;  
}

void train_predictor (unsigned int pc, bool outcome)
{
   
   int outInt = (int)outcome;
   int add = (pc&CMASK);
   int choose =Choice[add]; 
   if(Tpredict==outcome)
   {
      if((Tpredict == true)&&(choose<3))  // Avoid partial updates
      	 Choice[add] = choose+1;
      //if((choose<3))  // Not Avoiding partial updates
      // Choice[(pc&CMASK)] = choose+1;
   }       
   if(NTpredict==outcome)
   {
      if((NTpredict == false)&&(choose>0)) // Avoid Partial updates
     	 Choice[add] = choose-1;
     // if((choose>0)) // Avoid Partial updates
     //	 Choice[(pc&CMASK)] = choose-1;
   }       
 
   sr_add(outInt);

   if(choose>1)
  
   {
       if((!outcome)&&(Taken[addressTaken]>0))  
           Taken[addressTaken]--;
        
       if((outcome)&&(Taken[addressTaken]<3))  
           Taken[addressTaken]++;
	

    } 
 
    else
   {

       if((!outcome)&&(NTaken[addressNTaken]>0))  
           NTaken[addressNTaken]--;
        
       if((outcome)&&(NTaken[addressNTaken]<3))  
           NTaken[addressNTaken]++;

   }
   
   
     

}
