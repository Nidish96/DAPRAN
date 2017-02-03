#include<stdio.h>
#include<stdlib.h>
#include<ColumnRead.h>
#include<getopt.h>
#include<string.h>
#include<assert.h>
#include<math.h>

char* progname;
const struct option lops[] = {
  { "help", 0, NULL, 'h' },
  { "infile", 1, NULL, 'i' },
  { "ignore", 1, NULL, 'g' },
  { "lines", 1, NULL, 'N' },
  { "delim", 1, NULL, 'd' },
  { "dimensions", 1, NULL, 'M' },
  { "columns", 1, NULL, 'c' },
  { "plane", 1, NULL, 'n' },
  { "offset", 1, NULL, 'e' },
  { "direction", 1, NULL, 'x' },
  { "outfile", 1, NULL, 'o' },
  { NULL, 0, NULL, 0}
};
int nops = 11;
const char* const dops[] = {
  "  -  \t Print this help message",
  " str \t Specify input file (stdin by default)",
  " int \t Specify number of lines to ignore",
  " int \t Specify total number of lines to consider",
  " str \t Set field delimiter <tab,spc>[defaults to spc] - other delimiters may be typed as is",
  " int \t Dimensions of data",
  " i,..\t Specify columns to read from file",
  " d,..\t Define plane of cut (0's appended)",
  " dbl \t Define offset of plane",
  " 1,-1\t Specify direction of cut",
  " str \t Specify output file (stdout by default)"
};
const char* const sops = "hd:M:n:e:c:i:o:x:N:g:";

void usage();
double planeoffset(double* data,double* n,double e,int M);

int main( int nargs,char* sargs[] )
{
  progname = sargs[0];
  int opn;
  char *delim = NULL,*saveptr,*tmp;
  int M = 2,i,k;
  double* n = (double*)calloc(M,sizeof(double));
  int* c = (int*)malloc(M*sizeof(int));
  c[0] = 0; c[1] = 1;
  n[0] = 1; n[1] = 0;
  double e = 0;
  int dxn = 1;
  FILE *FIN = stdin,*FOUT = stdout;
  int N = -1,g = 0;

  do{
    opn = getopt_long(nargs,sargs,sops,lops,NULL);

    switch(opn){
    case 'h': usage(); exit(1);
    case 'd': delim = (strcmp(optarg,"tab")==0)?"\t":(strcmp(optarg,"spc")==0)?" ":optarg; break;
    case 'M': M = atoi(optarg);
      assert(M>1); break;
    case 'n': n = (double*)realloc(n,M*sizeof(double));
      tmp = strtok_r(optarg, ",", &saveptr);
      if( tmp==NULL ) n[0] = 0;
      else n[0] = atof(tmp);
      for( i=1;i<M;i++ ){
	tmp = strtok_r(NULL,",",&saveptr);
	if( tmp==NULL ) n[i] = 0;
	else n[i] = atof(tmp);
      }
      break;
    case 'c': c = (int*)malloc(M*sizeof(int));
      tmp = strtok_r(optarg, ",", &saveptr);
      if( tmp==NULL ) c[0] = 0;
      else c[0] = atoi(tmp);
      for( i=1;i<M;i++ ){
	tmp = strtok_r(NULL,",",&saveptr);
	if( tmp==NULL ) c[i] = 0;
	else c[i] = atoi(tmp);
      }
      break;
    case 'e': e = atof(optarg); break;
    case 'i': FIN = fopen(optarg,"r"); break;
    case 'o': FOUT = fopen(optarg,"w+"); break;
    case 'x': dxn = atoi(optarg); break;
    case 'N': N = atoi(optarg); break;
    case 'g': g = atoi(optarg); break;
    }
  }while(opn!=-1);
  if( delim==NULL ){
    delim = malloc(3*sizeof(char));
    strcpy(delim," ");
  }

  double** data = ColumnReadn(FIN,delim,&N,g,M,c,NULL);
  if( N==0 )
    exit(1);
  double pofst,ofst;
  double wp,w;

  ofst = planeoffset(data[0],n,e,M);
  for( i=1;i<N;i++ ){
    pofst = ofst;
    ofst = planeoffset(data[i],n,e,M);
    if( ofst*pofst<0 && ((ofst-pofst)*dxn==fabs(ofst-pofst) ||
			  dxn==0 ) ){ 
      wp = fabs(ofst)/(fabs(ofst)+fabs(pofst));
      w = fabs(pofst)/(fabs(ofst)+fabs(pofst));

      for( k=0;k<M;k++ )
  	fprintf(stdout,"%lf ",
  		data[i-1][k]*wp+data[i][k]*w);
      fprintf(stdout,"\n");
    }
  }
  
  return 0;
}

void usage()
{
  int i;
  fprintf(stderr,"Usage: %s [ options ... ]\n",
	  progname);
  for( i=0;i<nops;i++ )
    fprintf(stderr,"-%c --%s\t %s\n",
	    lops[i].val,lops[i].name,dops[i]);
}

double planeoffset(double* data,double* n,double e,int M)
{
  int i;
  double ret = 0;
  for( i=0;i<M;i++ )
    ret += n[i]*data[i];
  return ret-e;
}
