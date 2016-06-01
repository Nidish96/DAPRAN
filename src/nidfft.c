#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<complex.h>
#include<fftw3.h>
#include<getopt.h>
#include<string.h>
#include<ColumnRead.h>

char* progname;

const struct option long_options[] = {
  { "Help", 0, NULL, 'h'},
  { "Reverse",0,NULL, 'r'},
  { "Samp-Freq",1,NULL,'f'},
  { "Freq-Col",1,NULL,'F'},
  { "Length",1,NULL,'l'},
  { "Ignore",1,NULL,'g'},
  { "Column",1,NULL,'c'},
  { "delim",1,NULL,'d'},
  { "Time-Col",1,NULL,'t'},
  { "Infile",1,NULL,'i'},
  { "Outfile",1,NULL,'o'},
  { "Complex",1,NULL,'I'},
  { "OP_W",0,NULL,'w' }
};

void PrintUsage()
{
  fprintf(stderr,"Usage : %s options [ filename ... ]\n",progname);
  fprintf(stderr,"-%c --%s\t  -  \t Display this help message\n"
	  "-%c --%s\t i,i  \t Enable for Reverse Transform - Needs output with <real,imag> columns <i1,i2>\n"
	  "-%c --%s\t dbl \t Sampling Frequency [1 Hz]\n"
	  "-%c --%s\t int \t Frequency Column (only for -r mode)\n"
	  "-%c --%s\t int \t Length to be taken [full file default]\n"
	  "-%c --%s\t int \t Number of Lines to be ignored[0]\n"
	  "-%c --%s\t int \t Column to be taken [1]\n"
	  "-%c --%s\t str \t Set Delimiter <tab,spc>[defaults to spc] Other delimiters may just be typed\n"
	  "-%c --%s\t int \t Column to be taken as time data\n"
	  "-%c --%s\t str \t Input File Name [stdin]\n"
	  "-%c --%s\t str \t Output File Name [stdout]\n"
	  "-%c --%s\t  -  \t Enable Complex output\n"
	  "-%c --%s\t  -  \t Output W(omega) instead of f(frequency) (only for fwd trnsfrm)\n"	  
	  ,long_options[0].val,long_options[0].name
	  ,long_options[1].val,long_options[1].name
	  ,long_options[2].val,long_options[2].name
	  ,long_options[3].val,long_options[3].name
	  ,long_options[4].val,long_options[4].name
	  ,long_options[5].val,long_options[5].name
	  ,long_options[6].val,long_options[6].name
	  ,long_options[7].val,long_options[7].name
	  ,long_options[8].val,long_options[8].name
	  ,long_options[9].val,long_options[9].name
	  ,long_options[10].val,long_options[10].name
	  ,long_options[11].val,long_options[11].name
	  ,long_options[12].val,long_options[12].name	  
	  );

  fprintf(stderr,"\nOutput :\n"
	  "FORWARD TRANSFORM\n"
	  "default: <frequency,spectral power>\n"
	  "-I     : <frequency,real(tfm),imag(tfm)>\n"
	  "REVERSE TRANSFORM (-r)\n"
	  "default: <time,value>\n");

  exit(1);
}

int main(int argc,char* argv[])
{
  progname = argv[0];
  const char* const short_options = "hf:l:c:i:o:t:d:Ig:r:F:w";
  int next_option;
  double freq = 1,w=0;
  int L = -1,col = 1;
  FILE* FIN = stdin,*FOUT = stdout;
  char* delim;
  delim = NULL;
  int cmp=0;
  int ignore=0;
  int inv=0;
  int icol1,icol2;
  do{
    next_option = getopt_long(argc,argv,short_options,long_options,NULL);

    switch(next_option){
    case 'h': PrintUsage(); break;
    case 'f': freq = atof(optarg); break;
    case 'l': L = atoi(optarg); break;
    case 'c': col = atoi(optarg); break;
    case 't': freq = -atoi(optarg); break;
    case 'i': FIN = fopen(optarg,"r"); break;
    case 'o': FOUT = fopen(optarg,"w+"); break;
    case 'd': delim = (strcmp(optarg,"tab")==0)?"\t":(strcmp(optarg,"spc")==0)?" ":optarg; break;
    case 'I': cmp=1; break;
    case 'g': ignore=atoi(optarg); break;
    case 'r': inv=1; icol1=atoi(strtok(optarg,",")); icol2=atoi(strtok(NULL,",")); freq=0; break;
    case 'F': freq=-atoi(optarg); break;
    case 'w': w = 1; break;
    }
  }while(next_option!=-1);
  if(delim==NULL){
    delim = malloc(3*sizeof(char));
    strcpy(delim," ");}

  if( FIN==stdin )
    fprintf(stderr,"Waiting for Input from stdin\n");

  int i;
  fftw_complex *out,*in;
  double *data,scale,**data2,*OUT;
  fftw_plan P;
  if( inv==0 ){
    data = ColumnRead(FIN,delim,&L,ignore,col,&freq);

    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*(L/2+1));

    //  P = fftw_plan_dft_1d(L,in,out,FFTW_FORWARD,FFTW_ESTIMATE);
    P = fftw_plan_dft_r2c_1d(L,data,out,FFTW_ESTIMATE);

    fftw_execute(P);

    L = L/2+1;
    scale = freq/(2*(L-1));
    scale = (w)?scale*2*M_PI:scale;
    if( cmp==0 ){
      for(i=0;i<L;i++)
	fprintf(FOUT,"%lf %.4e\n",i*scale,pow(cabs(out[i]),2)/(L-1));}
     else{
      for(i=0;i<L;i++)
	fprintf(FOUT,"%lf %.4e %.4e\n",i*scale,creal(out[i])/(L-1),cimag(out[i])/(L-1));}

    fftw_destroy_plan(P);
    free(data);
    fftw_free(out);
   }
   else{
    data2 = ColumnRead2(FIN,delim,&L,ignore,icol1,icol2,&freq);
    L = 2*(L-1);
    OUT = (double*)malloc(L*sizeof(double));
    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*(L/2+1));
     for( i=0;i<L/2;i++ )
       in[i] = data2[0][i]+I*data2[1][i];
    free(data2[0]); free(data2[1]); free(data2);

    P = fftw_plan_dft_c2r_1d(L,in,OUT,FFTW_ESTIMATE);
    fftw_execute(P);

    for( i=0;i<L;i++ )
      fprintf(FOUT,"%lf %.4e\n",(double)i/freq,OUT[i]);

    free(OUT);
    fftw_free(in);
    fftw_destroy_plan(P);
  }

  if( FOUT!=stdout ) fclose(FOUT);
  if( FIN!=stdin ) fclose(FIN);

  if( delim!=NULL ) free(delim);
  return 0;
}
