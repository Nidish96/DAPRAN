#include"ColumnRead.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<assert.h>

double* ColumnRead(FILE* F,char* delim,int* n,int g,int c,double* freq)
/* 
   F : File pointer
   delim : delimiter string
   n : pointer to number of lines
   g : number of line
   c : Column
  freq : pointer to sampling frequency; if negative, abs(*freq) is the time column
*/
{
  int i,j,N=*n;
  int tcol = (*freq<0)?-*freq-1:-1;
  rewind(F);
  c=(c<1)?1:c;
  char *line = NULL;
  size_t len = 0;
  char *buff = NULL;
  double* dat;
  dat = malloc(sizeof(double));
  double time=0,ptime=0,pptime=0;
  for( i=1;i<g;i++ )
    assert(getline(&line,&len,F));
  /*  if( *freq<0 ){
    for( i=0;i<2;i++){
      ptime = time;
      do{
	getline(&line,&len,F);}while(line[0]=='#');
      buff=strtok(line,delim);
      for( j=1;j<-*freq;j++ ){
	buff=strtok(NULL,delim);
	if(buff==NULL) break;}
      time = (buff==NULL)?0:atof(buff);
    }
    rewind(F);
    for(i=1;i<g;i++)
      getline(&line,&len,F);
    *freq = (time==ptime)?1:1.0/(time-ptime);
  }*/
  
  if( N!=-1 ){
    for( i=0;i<N;i++ )
    {
      if(getline(&line,&len,F)==EOF)
      {N=i;	break;}
      if( line[0]=='#' ) continue;
      pptime = ptime;
      ptime = time;
      
      dat = realloc(dat,(i+1)*sizeof(double));
      buff = strtok(line,delim);
      if( tcol==0 )
	time = atof(buff);
      for( j=1;j<c;j++ ){
	buff = strtok(NULL,delim);
	if( tcol==j )
	  time = atof(buff);
	if(buff==NULL) break;}
      dat[i] = (buff!=NULL)?atof(buff):0;
    }
  }
  else{
    N=0;
    while(getline(&line,&len,F)!=EOF){
      if( line[0]=='#' ) continue;
      pptime = ptime;
      ptime = time;
      N+=1;
      dat = realloc(dat,N*sizeof(double));
      buff = strtok(line,delim);
      if( tcol==0 )
	time = atof(buff);
      for( i=1;i<c;i++ ){
	buff = strtok(NULL,delim);
	if( tcol==i )
	  time = atof(buff);
	if(buff==NULL) break;}
      dat[N-1] = (buff!=NULL)?atof(buff):0;
    }
  }
  *freq = (*freq<0)?1.0/(ptime-pptime):*freq;
  //  fprintf(stderr,"tcol : %d\ntime : %lf\nptime : %lf\n",tcol,ptime,pptime);
  free(line);
  *n = N-g;

  return dat;
}

double** ColumnRead2(FILE* F,char* delim,int* n,int g,int c1,int c2,double* freq)
/*
  F : File pointer
  delim : delimiter string
  n : pointer to number of lines
  g : number of lines to ignore
  c1 : column 1
  c2 : column 2
  freq : pointer to sampling frequency; if negative, abs(*freq) is the frequency column
*/
{
  int i,j,N=*n,fcol;
  fcol = (*freq<0)?-*freq-1:0;
  rewind(F);
  c1=(c1<1)?1:c1;
  c2=(c2<1)?1:c2;
  char *line = NULL;
  size_t len = 0;
  char *buff = NULL;
  double** dat;
  dat = malloc(2*sizeof(double));
  dat[0] = malloc(sizeof(double));
  dat[1] = malloc(sizeof(double));  
  double time=0,ptime=0,pptime=0;
  int flag;
  for( i=1;i<g;i++ )
    assert(getline(&line,&len,F));
  
  if( N!=-1 ){
    for( i=0;i<N;i++ )
    {
      if(getline(&line,&len,F)==EOF)
      {N=i;	break;}
      if( line[0]=='#' ) continue;
      
      dat[0] = realloc(dat[0],(i+1)*sizeof(double));
      dat[1] = realloc(dat[1],(i+1)*sizeof(double));      
      buff = strtok(line,delim);
      if( fcol==0 ) *freq = 2*atof(buff);
      for( j=1;j<c1;j++ ){
	if( j==fcol )
	  *freq = 2*atof(buff);
	buff = strtok(NULL,delim);
	if(buff==NULL) break;}
      dat[0][i] = (buff!=NULL)?atof(buff):0;

      buff = strtok(line,delim);
      for( j=1;j<c2;j++ ){
	buff = strtok(NULL,delim);
	if(buff==NULL) break;}
      dat[1][i] = (buff!=NULL)?atof(buff):0;      
    }
  }
  else{
    N=0;
    while(getline(&line,&len,F)!=EOF){
      if( line[0]=='#' ) continue;
      N+=1;
      dat[0] = realloc(dat[0],N*sizeof(double));
      dat[1] = realloc(dat[1],N*sizeof(double));      
      flag=0;
      i=-1;
      buff=strtok(line,delim);
      
      do{
	i++;
	if( i==fcol ){
	  *freq = 2*atof(buff);
	}
	if(i==c1-1){
	  dat[0][N-1] = atof(buff);
	  flag++;
	}
	if(i==c2-1){
	  dat[1][N-1] = atof(buff);
	  flag++;
	}
	buff = strtok(NULL,delim);
      }while(flag!=2 && buff!=NULL);
      dat[0][N-1]=(flag==0)?0:dat[0][N-1];
      dat[1][N-1]=(flag==0)?0:dat[1][N-1];
    }
  }
  *n = N-g;

  return dat;
}

double** ColumnReadn(FILE* F,char* delim,int* n,int g,int cn,int *c,double* freq)
/*
  F : File pointer
  delim : delimiter string
  n : pointer to number of lines (-1 if full file)
  g : number of lines to ignore
  cn : number of columns
  c : pointer to column indices
  freq : pointer to sampling frequency; if negative, abs(*freq) is the time column
*/
{
  int i,j,N=*n,tcol,k;
  tcol = (freq==NULL)?0:(*freq<0)?-*freq-1:0;
  rewind(F);

  char *line = NULL,*saveptr,*tmp=(char*)malloc(100*sizeof(char));
  size_t len = 0;
  char *buff = NULL;
  int dsiz = 0;
  double** dat = (double**)malloc(sizeof(double*));
  dat[0] = (double*)malloc(cn*sizeof(double));
  for( k=0;k<cn;k++ ){
    c[k] = abs(c[k]);
  }

  double time=0,ptime=0,pptime=0;
  int flag;
  for( i=1;i<g;i++ )
    assert(getline(&line,&len,F));
  
  if( N!=-1 ){
    for( i=0;i<N;i++ ){
      pptime = ptime;
      ptime  = time;
      if(getline(&line,&len,F)==EOF)
  	{N=i;	break;}
      if( line[0]=='#' ) continue;
      
      dsiz++;
      dat = (double**)realloc(dat,dsiz*sizeof(double*));
      dat[dsiz-1] = (double*)malloc(cn*sizeof(double));
      
      for( k=0;k<cn;k++ ){
	strcpy(tmp,line);
  	buff = strtok(tmp,delim);
  	if( tcol==0 )
  	  time=atof(buff);
  	for( j=1;j<c[k];j++ ){
  	  buff = strtok(NULL,delim);
  	  if( j==tcol )
  	    time=atof(buff);
  	  if(buff==NULL) break;}
  	dat[dsiz-1][k] = (buff!=NULL)?atof(buff):0;
      }
    }
  }
  else{
    N=0;
    while(getline(&line,&len,F)!=EOF){
      if( line[0]=='#' ) continue;
	
      pptime = ptime;
      ptime = time;
      N+=1;
      dsiz++;
      dat = (double**)realloc(dat,dsiz*sizeof(double*));
      dat[dsiz-1] = (double*)malloc(cn*sizeof(double));
      
      for( k=0;k<cn;k++ ){
      	strcpy(tmp, line);
      	buff = strtok_r(tmp,delim,&saveptr);
      	if( tcol==0 )
      	  time=atof(buff);
      	for( j=1;j<c[k];j++ ){
      	  buff = strtok_r(NULL,delim,&saveptr);
      	  if( j==tcol )
      	    time=atof(buff);
      	  if(buff==NULL) break;}
      	dat[dsiz-1][k] = (buff!=NULL)?atof(buff):0;
      }
    }
  }
  if( freq!=NULL )
    *freq = (*freq<0)?(time==ptime)?1.0:1.0/(ptime-pptime):*freq;
  *n = N-g;

  return dat;
}
