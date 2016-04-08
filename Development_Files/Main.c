#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<complex.h>
#include<fftw3.h>
#include<getopt.h>
#include<string.h>

double freq;
char* progname;
char* delim;
const struct option long_options[] = {
  { "Help", 0, NULL, 'h'},
  { "Frequency",1,NULL,'f'},
  { "Length",1,NULL,'l'},
  { "Column",1,NULL,'c'},
  { "delim",1,NULL,'d'},
  { "Time",1,NULL,'t'},
  { "Infile",1,NULL,'i'},
  { "Outfile",1,NULL,'o'}
};

void PrintUsage()
{
  fprintf(stderr,"Usage : %s options [ filename ... ]\n",progname);
  fprintf(stderr,"-%c --%s\t  -  \t Display this help message\n"
	  "-%c --%s\t dbl \t Sampling Frequency [1 Hz]\n"
	  "-%c --%s\t int \t Length to be taken [full file default]\n"
	  "-%c --%s\t int \t Column to be taken [1]\n"
	  "-%c --%s\t int \t Set Delimiter [defaults to space]\n"
	  "-%c --%s\t int \t Column to be taken as time data\n"
	  "-%c --%s\t str \t Input File Name [stdin]\n"
	  "-%c --%s\t str \t Output File Name [stdout]\n"	  
	  ,long_options[0].val,long_options[0].name
	  ,long_options[1].val,long_options[1].name
	  ,long_options[2].val,long_options[2].name
	  ,long_options[3].val,long_options[3].name
	  ,long_options[4].val,long_options[4].name
	  ,long_options[5].val,long_options[5].name
	  ,long_options[6].val,long_options[6].name
	  ,long_options[7].val,long_options[7].name);
  exit(1);
}

int ColumnRead(FILE* F,double** dat,int* n,int c)
{
  int i,j,N=*n;
  rewind(F);
  c=(c<1)?1:c;
  char *line = NULL;
  size_t len = 0;
  char *buff = NULL;

  dat[0] = malloc(sizeof(double));
  double time=0,ptime=0;
  if( N!=-1 ){
    for( i=0;i<N;i++ )
    {
      if(getline(&line,&len,F)==EOF)
      {N=i;	break;}
      if( line[0]=='#' ) continue;
      
      dat[0] = realloc(dat[0],(i+1)*sizeof(double));
      buff = strtok(line,delim);
      ptime = time;
      time = atof(buff);
      for( j=1;j<c;j++ ){
	buff = strtok(NULL,delim);
	if(buff==NULL) break;}
      dat[0][i] = (buff!=NULL)?atof(buff):0;
    }
  }
  else{
    N=0;
    while(getline(&line,&len,F)!=EOF){
      if( line[0]=='#' ) continue;
      N+=1;
      dat[0] = realloc(dat[0],N*sizeof(double));
      buff = strtok(line,delim);
      ptime = time;
      time = atof(buff);
      for( i=1;i<c;i++ ){
	buff = strtok(NULL,delim);
	if(buff==NULL) break;}
      dat[0][N-1] = (buff!=NULL)?atof(buff):0;
    }
  }
  freq = (freq==-1)?1.0/(time-ptime):freq;
  *n = N;

  return 1;
  }

int main(int argc,char* argv[])
{
  progname = argv[0];
  const char* const short_options = "hf:l:c:i:o:t:d:";
  int next_option;
  freq = 1;
  int L = -1,col = 1;
  FILE* FIN = stdin,*FOUT = stdout;
  delim = NULL;
  do{
    next_option = getopt_long(argc,argv,short_options,long_options,NULL);

    switch(next_option){
    case 'h': PrintUsage(); break;
    case 'f': freq = atof(optarg); break;
    case 'l': L = atoi(optarg); break;
    case 'c': col = atoi(optarg); break;
    case 't': freq = -1; break;
    case 'i': FIN = fopen(optarg,"r"); break;
    case 'o': FOUT = fopen(optarg,"w+"); break;
    case 'd': delim = optarg; break;
    }
  }while(next_option!=-1);
  if(delim==NULL){
    delim = malloc(2*sizeof(char));
    strcpy(delim," ");}
  
  if( FIN==stdin )
    fprintf(stderr,"Waiting for Input from stdin\n");

  double* data;
  ColumnRead(FIN,&data,&L,col);
  
  int i;
  /*  fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*L);
  for( i=0;i<L;i++ )
  in[i] = data[i];
  fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*L);*/
  fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*(L/2+1));
  fftw_plan P;

  //  P = fftw_plan_dft_1d(L,in,out,FFTW_FORWARD,FFTW_ESTIMATE);
  P = fftw_plan_dft_r2c_1d(L,data,out,FFTW_ESTIMATE);

  fftw_execute(P);

  L = L/2+1;
  double scale = freq/(2*(L-1));
  for(i=0;i<L;i++)
    fprintf(FOUT,"%lf %lf\n",i*scale,pow(cabs(out[i]),2));

  if( FOUT!=stdout ) fclose(FOUT);
  if( FIN!=stdin ) fclose(FIN);

  fftw_destroy_plan(P);
  fftw_free(out);
  //fftw_free(in);
  free(data);
  return 0;
}
