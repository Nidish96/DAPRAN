#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<getopt.h>
#include<string.h>

const char* progname;
const struct option opl[] = {
  {"help", 0, NULL, 'h'},
  {"seq1", 1, NULL, '1'},
  {"seq2", 1, NULL, '2'},
  {"num", 1, NULL, 'n'},
  {"outfile", 1, NULL, 'o'},
  {NULL, 0, NULL, 0}
};
const char* const opd[] = {
  " - \n\tPrint Help Message\n",
  "str\n\tSequence 1 <filename> [stdin]\n",
  "str\n\tSequence 2 <filename> [stdin]\n",
  "int\n\tLength of data [16]\n",
  "str\n\tFile to output to [stdout]\n"
};
const int opn = 5;
void PrintUsage()
{
  fprintf(stderr,"Usage : %s [ options ... ]\n",progname);
  int i;
  for( i=0;i<opn;i++ )
    fprintf(stderr,"-%c --%s %s\n",opl[i].val,opl[i].name,opd[i]);
}

int main(int nargs,char* sargs[])
{
  const char* const ops = "h1:2:n:o:";
  int opnxt;
  FILE *F1 = stdin,*F2 = stdin;
  FILE* FOUT = stdout;
  int n = 16;
  char* buff;
  
  progname = sargs[0];
  do{
    opnxt = getopt_long(nargs,sargs,ops,opl,NULL);

    switch(opnxt){
    case 'h': PrintUsage(); exit(1);
    case '1': F1 = fopen(optarg,"r"); break;
    case '2': F2 = fopen(optarg,"r"); break;
    case 'n': n = atoi(optarg); break;
    case 'o': FOUT = fopen(optarg,"w+"); break;
    }
  }while(opnxt!=-1);

  double* d1 = calloc(n,sizeof(double));
  double* d2 = calloc(n,sizeof(double));
  int i,k;

  if( F1==stdin ) fprintf(stderr,"Waiting for sequence 1 from stdin\n");
  for( i=0;i<n;i++ )
    if( fscanf(F1,"%lf",&d1[i])==EOF ) break;
  for( ;i<n;i++ )
    d1[i] = 0;
  if( F2==stdin ) fprintf(stderr,"Waiting for sequence 2 from stdin\n");
  for( i=0;i<n;i++ )
    if( fscanf(F2,"%lf",&d2[i])==EOF ) break;
  for( ;i<n;i++ )
    d2[i] = 0;

  double ret;
  for( i=0;i<n;i++ ){
    ret = 0;
    for( k=0;k<n;k++ )
      ret += (i>=k)?d1[i-k]*d2[k]:0;
    fprintf(FOUT,"%d %lf %lf %lf\n",i,d1[i],d2[i],ret);
  }

  free(d1);
  free(d2);
  return 0;
}
