// A very stupid predictor.  It will always predict not taken.
#include <stdio.h>
#define BLEN 12
#define CLEN 12
#define GBITS 13
#define M 14 //No of bits of PC 
#define N 0 // L-N = no of bits or GHR 
#define TABSIZE (1<<(GBITS))
#define BMTL  (1<<BLEN)
#define GMASK (TABSIZE-1)
#define BMASK (BMTL-1)





int PTable[TABSIZE];
int shiftReg;
int phtAddress = 0;
int GshareAddress=0;
int BMAddress;
int Choose[BMTL];
int *ptrTable = PTable;
int address;
int BMT[BMTL]; // Pattern History Table
//#define CorPred
// Let's consider a 1K Local Pattern Table or 32 addresses
int Gsharevalue ;
int BMvalue ;
bool Gpredict,BMpredict;

void table_init()
{
    int i;
    // Initialise Pattern Table For Gshare
    for(i=0;i<TABSIZE;i++)
        PTable[i] = 0;
    // Initialise the Bimodal and Choose Tables
    for(i=0;i<BMTL;i++)
    {    BMT[i] =0;
         Choose[i] =0;
    }
    
    shiftReg = 0;
}


void sr_add(char i)
{
    shiftReg = (shiftReg<<1)&GMASK;
    shiftReg|=i;
    
}

int pattern_address()
{
    int i;
    int add=0;
    add = shiftReg;

    return add;    
}




void init_predictor ()
{
    table_init();
}

bool make_prediction (unsigned int pc)
{
  
 address  = pattern_address();
 address   = address^((pc&GMASK)); 
 Gsharevalue = PTable[address];

 BMAddress = pc&(BMASK);
 BMvalue = BMT[BMAddress];
      
     
  int choice; 
  Gpredict = (Gsharevalue<2)?false:true;
  BMpredict = (BMvalue<2)?false:true;
 	
  choice = Choose[address&BMASK];
 
  if(choice<2)
    return BMpredict;
  else
    return Gpredict;  
}

void train_predictor (unsigned int pc, bool outcome)
{
   
   int outInt = (int)outcome;
   
   int choosePC =Choose[address&BMASK]; 
   if(Gpredict!=BMpredict)
   {
     if(Gpredict==outcome)
        choosePC = (choosePC>2)?3:choosePC+1;
    else if(BMpredict==outcome) 
        choosePC = (choosePC<1)?0:choosePC-1;
    Choose[address&BMASK] = choosePC;
   }       
 
   sr_add(outInt);
   if(outcome==0)
   {
     
         
           	PTable[address] = ((Gsharevalue-1)<0)?0:Gsharevalue-1;
                BMT[BMAddress] = ((BMvalue-1)<0)?0:BMvalue-1;
         

   }
   else
   {
         
          	PTable[address] = ((Gsharevalue+1)>3)?3:Gsharevalue+1;
       		BMT[BMAddress] = ((BMvalue+1)>3)?3:BMvalue+1;
         
   }

}
