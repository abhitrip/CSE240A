// A very stupid predictor.  It will always predict not taken.

// Sample code for Percepron Predictor

//#define NUM_TABLES (1<<N)
#include <iostream>
#include "predictor.h"
using namespace std;

char w[NUM_TABLES][GA_LEN][Num_Percept]; // Char Array of Perceptron ws (7bit signed)
char bias[NUM_TABLES]; // Char array of bias enntries per pc . (7bit signed )
//int GHR[GHR_LEN];
int GHR; // GHR Shift register 
int GA[GHR_LEN]; // Global Address array for keeping past GHR_LEN addresses

inline int MAX(int a,int b)
{
   if(a>b)
      return a;
   else
      return b;   
}

inline int MIN(int a,int b)
{
   if(a<b)
      return a;
   else
      return b;   
}

 

int sum;
bool prediction;


void init_predictor()
{	
    int i,j,k; 
    for(i=0;i<NUM_TABLES;i++)
    {
        bias[i] = 0;              // Initialise Bias Values to zero 
        for(j=0;j<GA_LEN;j++)
        {    
	   for(k=0;k<GHR_LEN;k++) // Initialise perceptron weight Values to zero 
            w[i][j][k] = 0;			
        }
    }
	
      GHR = 0;  // GHR is initialised to zero
     for(i=0;i<GA_LEN;i++)
      GA[i] = 0;   // Initialise the GA address array to zero. This should contain only 4 bit values 0-15. 
}

bool make_prediction (unsigned int pc)
{
   int tableIndex = pc&0xf; // Fetch the table Index from lower 4 bits of pc 
   int i;
   
   sum = 0;
   int tempGHR = GHR; // Make a local copy of GHR to get bit at positions 0-17 
   int x=0;
   sum = bias[tableIndex]; // Initialise sum with Bias of that Table index
   for(i=0;i<GHR_LEN;i++)
   {
        x = tempGHR&0x1;  
       if(x)
	   sum+=(w[tableIndex][GA[i]][i]); // If GHR bit is 1 , sum = sum +weight so max positive means branch taken
       else
	   sum-=(w[tableIndex][GA[i]][i]);  // If GHR bit is 0 , sum = sum - weight so negative sum means ranch not taken
	tempGHR>>=1; // Right shift to get the next bit by anding with 1
   }
  
  prediction = (sum>=0); // If summation(w*x+b)>=0 then 1 else zero , where x = GHR bit , w = weight

  if(prediction)
     return true;  
  else
     return false;
}

void train_predictor (unsigned int pc, bool outcome)
{
   int result = outcome;
   int tableIndex = pc&0xf;
   int i,x;
   int tempGHR = GHR;
   if((prediction!=outcome)||((sum<MAX_threshold)&&(sum>MIN_threshold))) // Update only when Prediction and outcome mismatch or abs(sum)<Threshold
   {
       if(result)
          bias[tableIndex] = MIN(MAX_WEIGHT,bias[tableIndex]+1); // If prediction and outcome match add 1 to weight
       else
          bias[tableIndex] = MAX(MIN_WEIGHT,bias[tableIndex]-1);  // If prediction and outcome don't match subtract 1 from weight to tend towards negative
     
       for(i=0;i<GHR_LEN;i++)
       { 
          x = tempGHR&0x1;            
          if(x==outcome)
          {
                w[tableIndex][GA[i]][i] = MIN(MAX_WEIGHT,w[tableIndex][GA[i]][i]+1);// If prediction and outcome match add 1 to weight
          }
             else
          {
               w[tableIndex][GA[i]][i] = MAX(MIN_WEIGHT,w[tableIndex][GA[i]][i]-1);   // If prediction and outcome don't match subtract 1 from weight to tend towards negative  
          }
          
	 tempGHR>>=1;
       }
   }

       for(i=GA_LEN-1;i>=1;i--)
           GA[i] = GA[i-1]; // Shift the global addresses so that recent address should be at the first
       GA[0] = pc&0xf;
 
          GHR = (GHR<<1)&0x1ffff; // Shift GHR as a shift register
          GHR |= outcome;          


  

}
