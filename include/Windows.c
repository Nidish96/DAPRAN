#include"Windows.h"
#include<stdlib.h>

double Rectangle(int n,int N,double a,double b)
{
  return (fabs(n)<(double)N/2)?1.:0;
}
double Triangle(int n,int N,double a,double b)
{
  return (fabs(n)<(double)N/2)?1.-fabs(n)/(N/2):0;
}

double Hann(int n,int N,double a,double b)
{
  return (fabs(n)<(double)N/2)?0.5*(1.-cos(2.*M_PI*(n-N/2)/(N-1))):0;
}
double Hamming(int n,int N,double a,double b)
{
  return (fabs(n)<(double)N/2)?0.53836-0.46164*cos(2.*M_PI*(n-N/2)/(N-1)):0;
}
double GHamming(int n,int N,double a,double b)
{
  return (fabs(n)<(double)N/2)?a-b*cos(2.*M_PI*(n-N/2)/(N-1)):0;
}
double GBlackman(int n,int N,double a,double b)
{
  return (fabs(n)<(double)N/2)?((1.-a)-cos(2.*M_PI*(n-N/2)/(N-1))+a*cos(4.*M_PI*(n-N/2)/(N-1)))/2:0;
}
double TBlackman(int n,int N,double a,double b)
{
  return (fabs(n)<(double)N/2)?(.42-.5*cos(2.*M_PI*(n-N/2)/(N-1))+.08*cos(4.*M_PI*(n-N/2)/(N-1))):0;
}
double EBlackman(int n,int N,double a,double b)
{
  (fabs(n)<(double)N/2)?(7938.-9240.*cos(2.*M_PI*(n-N/2)/(N-1))+1430.*cos(4.*M_PI*(n-N/2)/(N-1)))/18608.:0;
}
							      
double* Windowed(double* data,window W,int at,int L,int Nw)
/* data : pointer to array of data
   window : pointer to window specs array (3 member)
   at : center point for window function
   L : Length of data
   Nw : Window length
*/
{
  double* ret = malloc(L*sizeof(double));
  double (*windowfn)(int,int,double,double);
  switch(W.w){
  case 'r':
    windowfn = Rectangle;
    break;
  case 'v':
    windowfn = Triangle;
      break;
  case 'm':
    windowfn = Hamming;
      break;
  case 'n':
    windowfn = Hann;
    break;
  case 'H':
    windowfn = GHamming;
      break;
  case 'b':
    windowfn = EBlackman;
      break;
  case 'T':
    windowfn = TBlackman;
      break;
  case 'B':
    windowfn = GBlackman;
      break;
  }

  int i;
  for( i=0;i<L;i++ )
    ret[i] = data[i]*(*windowfn)(i-at,Nw,W.a,W.b);

  return ret;
}
