#ifndef WINDOWS_DEFD
#define WINDOWS_DEFD

#include<math.h>

typedef struct{int w; double a,b;}window;

/* Window function macros */
double Rectangle(int n,int N,double a,double b);
double Triangle(int n,int N,double a,double b); 

double Hann(int n,int N,double a,double b); 
double Hamming(int n,int N,double a,double b);
double GHamming(int n,int N,double a,double b);

double GBlackman(int n,int N,double a,double b);
double TBlackman(int n,int N,double a,double b); 
double EBlackman(int n,int N,double a,double b); 

double* Windowed(double* data,window W,int at,int L,int Nw);

#endif
