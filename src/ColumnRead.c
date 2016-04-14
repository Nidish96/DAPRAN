#include"ColumnRead.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

double* ColumnRead(FILE* F,char* delim,int* n,int g,int c,double* freq)
{
  int i,j,N=*n;
  rewind(F);
  c=(c<1)?1:c;
  char *line = NULL;
  size_t len = 0;
  char *buff = NULL;
  double* dat;
  dat = malloc(sizeof(double));
  double time=0,ptime=0;
  for( i=1;i<g;i++ )
    getline(&line,&len,F);
  if( *freq<0 ){
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
  }
  
  if( N!=-1 ){
    for( i=0;i<N;i++ )
    {
      if(getline(&line,&len,F)==EOF)
      {N=i;	break;}
      if( line[0]=='#' ) continue;
      
      dat = realloc(dat,(i+1)*sizeof(double));
      buff = strtok(line,delim);
      for( j=1;j<c;j++ ){
	buff = strtok(NULL,delim);
	if(buff==NULL) break;}
      dat[i] = (buff!=NULL)?atof(buff):0;
    }
  }
  else{
    N=0;
    while(getline(&line,&len,F)!=EOF){
      if( line[0]=='#' ) continue;
      N+=1;
      dat = realloc(dat,N*sizeof(double));
      buff = strtok(line,delim);
      for( i=1;i<c;i++ ){
	buff = strtok(NULL,delim);
	if(buff==NULL) break;}
      dat[N-1] = (buff!=NULL)?atof(buff):0;
    }
  }
  *freq = (*freq==-1)?1.0/(time-ptime):*freq;
  *n = N-g;

  return dat;
}

double** ColumnRead2(FILE* F,char* delim,int* n,int g,int c1,int c2,double* freq)
{
  int i,j,N=*n;
  rewind(F);
  c1=(c1<1)?1:c1;
  c2=(c2<1)?1:c2;
  char *line = NULL;
  size_t len = 0;
  char *buff = NULL;
  double** dat;
  char* saveptr;
  dat = malloc(2*sizeof(double));
  dat[0] = malloc(sizeof(double));
  dat[1] = malloc(sizeof(double));  
  double time=0,ptime=0;
  int flag;
  for( i=1;i<g;i++ )
    getline(&line,&len,F);
  if( *freq<=0 ){
    *freq = (*freq==0)?1.:*freq;
    while(getline(&line,&len,F)!=EOF);
    buff=strtok(line,delim);
    for( j=1;j<-*freq;j++ ){
      buff=strtok(NULL,delim);
      if(buff==NULL) break;}
    *freq = (buff==NULL)?0:atof(buff);
    *freq *= 2;
    rewind(F);
    for(i=1;i<g;i++)
      getline(&line,&len,F);
  }
  
  if( N!=-1 ){
    for( i=0;i<N;i++ )
    {
      if(getline(&line,&len,F)==EOF)
      {N=i;	break;}
      if( line[0]=='#' ) continue;
      
      dat[0] = realloc(dat[0],(i+1)*sizeof(double));
      dat[1] = realloc(dat[1],(i+1)*sizeof(double));      
      buff = strtok(line,delim);
      for( j=1;j<c1;j++ ){
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
      /*      buff = strtok_r(line,delim,saveptr);
      for( i=1;i<c1;i++ ){
	buff = strtok_r(NULL,delim,saveptr);
	if(buff==NULL) break;}
      dat[0][N-1] = (buff!=NULL)?atof(buff):0;

      buff = strtok_r(line,delim,saveptr);
      for( i=1;i<c2;i++ ){
	buff = strtok_r(NULL,delim,saveptr);
	if(buff==NULL) break;}
	dat[1][N-1] = (buff!=NULL)?atof(buff):0;      */
      flag=0;
      i=-1;
      buff=strtok(line,delim);
      do{
	i++;
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
      dat[1][N-1]=(flag==1)?0:dat[1][N-1];
    }
  }
  *n = N-g;

  return dat;
}
