// A very stupid predictor.  It will always predict not taken.

// Sample code for Percepron Predictor

#define N 7 // No of bits for Global history table
//#define NUM_TABLES (1<<N)
#define NUM_TABLES 256
#define Num_Percept (17+1) // To accomodate w0
#define MAX_WEIGHT 63
#define MIN_WEIGHT -64
#include <iostream>
using namespace std;
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
char w[NUM_TABLES][Num_Percept]; // Char Array of Perceptron ws

 
const int MAX_threshold = (int)((1.93*(Num_Percept-1)+14));
const int MIN_threshold = -MAX_threshold;

int sum;
int GHR=0;// Global history table
bool prediction;


void init_predictor ()
{	
    int i,j; 
    for(i=0;i<NUM_TABLES;i++)
    {
       w[i][0] = 0;
       for(j=1;j<Num_Percept;j++)
	   w[i][j] = 0;			
    }

    GHR = 0;
      
}

bool make_prediction (unsigned int pc)
{
   int tableIndex = pc&0xff;
   int i;
   
   sum = 0;
   int tempGHR = GHR;
   int x=0;
   sum = sum + w[tableIndex][0];
   for(i=1;i<Num_Percept;i++)
   {
       x = (tempGHR&0x1);
      // cout <<"x = "<<x<<endl;
       if(x)
	   sum+=(w[tableIndex][i]);
       else
           sum-=(w[tableIndex][i]);
      tempGHR>>=1;		
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
   int tableIndex = pc&0xff;
   //bool Cond1 = (sum>=0);
   int sign = (result)^(sum>=0);   
   int i,x;
   int tempGHR = GHR;
   if((prediction!=outcome)||((sum<MAX_threshold)&&(sum>MIN_threshold)))
   {
       //cout <<" Hit ";
       if(result)
          w[tableIndex][0] = MIN(MAX_WEIGHT,w[tableIndex][0]+1);
       else
          w[tableIndex][0] = MAX(MIN_WEIGHT,w[tableIndex][0]-1);
     
       for(i=1;i<Num_Percept;i++)
       {
          x = tempGHR&0x1;  
          if(((!x)&&(!result))||((x)&&(result)))
             w[tableIndex][i] = MIN(MAX_WEIGHT,w[tableIndex][i]+1);
          else
             w[tableIndex][i] = MAX(MIN_WEIGHT,w[tableIndex][i]-1);    
         tempGHR>>=1;                
       }
            	

   }
       
    GHR = ((GHR<<1)|(result))&0x7fff;

}
