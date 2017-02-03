#include<stdio.h>
#include<stdlib.h>
#include<ColumnRead.h>
#include<getopt.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include<complex.h>
#include<fftw3.h>
#include<gsl/gsl_vector.h>

char* progname;
const struct option lops[] = {
  { "help", 0, NULL, 'h' },
  { "infile", 1, NULL, 'i' },
  { "ignore", 1, NULL, 'g' },
  { "lines", 1, NULL, 'N' },
  { "delim", 1, NULL, 'd' },
  { "dimensions", 1, NULL, 'M' },
  { "columns", 1, NULL, 'c' },
  { "time", 1, NULL, 't' },
  { "Harmonics", 1, NULL, 'H' },
  { "outfile", 1, NULL, 'o' },
  { NULL, 0, NULL, 0}
};
int nops = 10;
const char* const dops[] = {
  "  -  \t Print this help message",
  " str \t Specify input file (stdin by default)",
  " int \t Specify number of lines to ignore",
  " int \t Specify total number of lines to consider",
  " str \t Set field delimiter <tab,spc>[defaults to spc] - other delimiters may be typed as is",
  " int \t Dimensions of data",
  " i,..\t Specify columns to read from file",
  "  -  \t Enable if first column is time (M includes this) - time scaled to 2pi if not specified",
  " int \t Number of harmonics to output",
  " str \t Specify output file (stdout by default)"
};
const char* const sops = "hd:M:c:i:o:N:g:tH:";

void usage();

int main( int nargs,char* sargs[] )
{
  progname = sargs[0];
  int opn;
  char *delim = NULL,*saveptr,*tmp;
  int M = 2,i,k;
  int* c = (int*)malloc(M*sizeof(int));
  c[0] = 0; c[1] = 1;
  FILE *FIN = stdin,*FOUT = stdout;
  int N = -1,g = 0;
  int t_flag = 0;
  int Nh = -1;

  do{
    opn = getopt_long(nargs,sargs,sops,lops,NULL);

    switch(opn){
    case 'h': usage(); exit(1);
    case 'd': delim = (strcmp(optarg,"tab")==0)?"\t":(strcmp(optarg,"spc")==0)?" ":optarg; break;
    case 'M': M = atoi(optarg); break;
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
    case 'i': FIN = fopen(optarg,"r"); break;
    case 'o': FOUT = fopen(optarg,"w+"); break;
    case 'N': N = atoi(optarg); break;
    case 'g': g = atoi(optarg); break;
    case 't': t_flag = 1; assert(M>1); break;
    case 'H': Nh = atoi(optarg); break;
    }
  }while(opn!=-1);
  if( delim==NULL ){
    delim = malloc(3*sizeof(char));
    strcpy(delim," ");
  }

  double** data = ColumnReadn(FIN,delim,&N,g,M,c,NULL);
  if( N==0 )
    exit(1);
  double** trdata = transpose(data,N,M);
  freedata(data,N,M);

  assert(N>3);
  double w_fnd = (t_flag!=0)?2.*M_PI/(trdata[0][N-1]+trdata[0][N-2]-trdata[0][N-3]):1.0;

  int H = N/2+1;
  Nh = (Nh==-1)?H:Nh;
  fftw_plan P;
  fftw_complex *out,*in;
  out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*H);
  
  gsl_vector* X = gsl_vector_alloc((M-t_flag)*(2*Nh+1));
  gsl_vector_view Xv;

  int j;

  for( i=0;i<M-t_flag;i++ ){
    P = fftw_plan_dft_r2c_1d(N, trdata[i+t_flag], out, FFTW_ESTIMATE);
    fftw_execute(P);
    fftw_destroy_plan(P);

    Xv = gsl_vector_subvector_with_stride(X, i, M-t_flag, 2*Nh+1);
    gsl_vector_set( &Xv.vector,0, creal(out[0])/2. );

    for( j=1;j<=Nh;j++ ){
      gsl_vector_set(&Xv.vector, 2*j-1, creal(out[j]) );
      gsl_vector_set(&Xv.vector, 2*j, -cimag(out[j]) );
    }
  }
  gsl_vector_scale(X, 2.0/N);
  gsl_vector_fprintf(stdout, X, "%lf");
  fprintf(stdout,"%lf\n",w_fnd);

  fftw_free(out);
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
