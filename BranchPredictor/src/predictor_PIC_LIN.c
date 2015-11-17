// A very stupid predictor.  It will always predict not taken.

// Sample code for Percepron Predictor

#define N 7 // No of bits for Global history table
//#define NUM_TABLES (1<<N)
#define NUM_TABLES 16
#define Num_Percept (15+1) // To accomodate w0
#define GA_LEN  16
#define GA_MASK 0xf
#define GHR_LEN 15
#define MAX_WEIGHT 127
#define MIN_WEIGHT -128
#include <iostream>
using namespace std;
char w[NUM_TABLES][GA_LEN][Num_Percept]; // Char Array of Perceptron ws
int GHR[GHR_LEN];
int GA[GHR_LEN];

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

 
//const int MAX_threshold = (int)((1.93*(Num_Percept-1)+14));// For PErceptron
const int MAX_threshold = (int)((2.14*(Num_Percept)+20.58)); // For piecewise linear
const int MIN_threshold = -MAX_threshold;

int sum;
bool prediction;


void init_predictor ()
{	
    int i,j,k; 
    for(i=0;i<NUM_TABLES;i++)
    {
        for(j=0;j<GA_LEN;j++)
        {     
	   for(k=0;k<GHR_LEN+1;k++)
            w[i][j][k] = 0;			
        }
    }
	
     for(i=0;i<GHR_LEN;i++)
      GHR[i] = 0; 
     for(i=0;i<GHR_LEN;i++)
      GA[i] = 0; 
}

bool make_prediction (unsigned int pc)
{
   int tableIndex = pc&0xf;
   int i;
   
   sum = 0;
   //int tempGHR = GHR;
   int x=0;
   sum = w[tableIndex][0][0];
   for(i=0;i<GHR_LEN;i++)
   {
       if(GHR[i])
	   sum+=(w[tableIndex][GA[i]][i+1]);
       else
	   sum-=(w[tableIndex][GA[i]][i+1]);
   }
  
  prediction = (sum>=0);

  if(prediction)
     return true;  
  else
     return false;
}

void train_predictor (unsigned int pc, bool outcome)
{
   int result = outcome;
   int tableIndex = pc&0xf;
   //bool Cond1 = (sum>=0);
   int i,x;
  // int tempGHR = GHR;
   if((prediction!=outcome)||((sum<MAX_threshold)&&(sum>MIN_threshold)))
   {
       //cout <<" Hit ";
       if(result)
          w[tableIndex][0][0] = MIN(MAX_WEIGHT,w[tableIndex][0][0]+1);
       else
          w[tableIndex][0][0] = MAX(MIN_WEIGHT,w[tableIndex][0][0]-1);
     
       for(i=0;i<GHR_LEN;i++)
       { 
          
          if(GHR[i]==outcome)
          {
                w[tableIndex][GA[i]][i+1] = MIN(MAX_WEIGHT,w[tableIndex][GA[i]][i+1]+1);
          }
             else
                w[tableIndex][GA[i]][i+1] = MAX(MIN_WEIGHT,w[tableIndex][GA[i]][i+1]-1);    
          
       }
   }

       for(i=GHR_LEN-1;i>=1;i--)
           GA[i] = GA[i-1];
       GA[0] = pc&0xf;
 
       for(i=GHR_LEN-1;i>=1;i--)      
            GHR[i]=GHR[i-1];
        GHR[0] = outcome;  	



  
       
    //GHR = ((GHR<<1)|(result))&0x7fff;

}
