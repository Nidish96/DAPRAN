#ifndef COLUMNREAD_DEFD
#define COLUMNREAD_DEFD

#include<stdio.h>

double* ColumnRead(FILE* F,char* delim,int* n,int g,int c,double* freq);
double** ColumnRead2(FILE* F,char* delim,int* n,int g,int c1,int c2,double* freq);
double** ColumnReadn(FILE* F,char* delim,int* n,int g,int cn,int *c,double* freq);

#endif
