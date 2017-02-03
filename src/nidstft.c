#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<complex.h>
#include<fftw3.h>
#include<getopt.h>
#include<string.h>
#include<ColumnRead.h>
#include<Windows.h>

const char* progname;
const struct option long_options[]={
  { "help", 0, NULL, 'h' },
  { "rectangle", 0, NULL, 'r' },
  { "triangle", 0, NULL, 'v' },
  { "hamming", 0, NULL, 'm' },
  { "hann", 0, NULL, 'n' },
  { "Ghamming", 1, NULL, 'H' },
  { "blackman", 0, NULL, 'b' },
  { "Tblackman", 0, NULL, 'T' },
  { "Gblackman", 1, NULL, 'B' },
  { "wndwlngth", 1, NULL, 'N' },
  { "Sampl-Freq", 1, NULL, 'f' },
  { "Time-Col", 1, NULL, 't' },
  { "Infile", 1, NULL, 'i' },
  { "Outfile", 1, NULL, 'o' },
  { "Column", 1, NULL, 'c' },
  { "Length", 1, NULL, 'l' },
  { "Ignore", 1, NULL, 'g' },
  { "delim",1,NULL,'d' },
  { "stft-Inc", 1, NULL, 'I' },
  { "Complex", 0, NULL, 'C' },
  { "OP_Omega", 0, NULL, 'w' },
  { NULL, 0, NULL, 0}
};
const char* const description[]={
  " -  Display this Help Message",
  " -  Window - Rectangle",
  " -  Window - Triangle",
  " -  Window - Hamming",
  " -  Window - Hann",
  "dbl Window - Generalized Hamming",
  " -  Window - Blackman",
  " -  Window - Truncated Blackman (alpha=.16)",
  "dbl Window - Generalized Blackman",
  "int Window Length [defaults to 80]",
  "dbl Sampling Frequency [defaults to 1Hz]",
  "int Column to take time data from [sampling frequency 1Hz if not set]",
  "str File to take input from [defaults to stdin]",
  "str File to output stft data to [defaults to stdout]",
  "int Column to read [defaults to 1]",
  "int Length of file to be read [defaults to full file]",
  "int Number of lines to ignore [defaults to 0]",
  "str Set delimiter <tab,spc>[defaults to spc]; Other string delimiters may be typed",
  "int Set increment for STFT [defaults to 24]",
  " -  Enable Complex Output <real,imag> [defaults to power]",
  " -  Output in terms of W(Omega, rad s^-1) instead of f(Frequency, Hz)"
};
int args_tot=21;
void PrintUsage(FILE* Stream)
{
  fprintf(Stream,"Usage: %s options [ outfile ... ]\n",progname);
  int i;
  for( i=0;i<args_tot;i++ )
    fprintf(Stream,"-%c --%s\t %s\n",
	    long_options[i].val,long_options[i].name,description[i]);
  fprintf(Stream,"\nOutput : \n"
"default: <time,frequency,spectral power>\n"
"-C     : <time,frequency,real(tfm),imag(tfm)>\n");
  exit(1);
}

const char* const short_options = "hrvmnH:bTB:N:f:t:i:o:c:l:g:d:F:CI:w";
int main(int argn,char* args[])
{
  int next_option,Nw=80,Inc=24;
  window W={'b',0,0};
  progname = args[0];
  double freq=1,w=0;
  int col=1,L=-1,ignore=0,cmp=0;
  char* delim;
  delim=NULL;
  FILE *FIN=stdin,*FOUT=stdout;
  do{
    next_option = getopt_long(argn,args,short_options,long_options,NULL);

    switch(next_option){
    case 'h': PrintUsage(stderr); break;
    case 'r': 
    case 'v': 
    case 'm': 
    case 'n': W.w=next_option; break;
    case 'H': W.w=next_option;
      sscanf(optarg,"%lf,%lf",&W.a,&W.b); break;
    case 'b': 
    case 'T': W.w=next_option; break;
    case 'B': W.w=next_option;
      W.a = atof(optarg); break;      
    case 'N': Nw = atoi(optarg); break;
    case 't': freq=-atoi(optarg); break;
    case 'i': FIN=fopen(optarg,"r"); break;
    case 'o': FOUT=fopen(optarg,"w+"); break;
    case 'c': col=atoi(optarg); break;
    case 'l': L=atoi(optarg); break;
    case 'g': ignore=atoi(optarg); break;
    case 'd': delim=malloc(6*sizeof(char));
      delim = (strcmp(optarg,"tab")==0)?"\t":(strcmp(optarg,"spc")==0)?" ":optarg; break;
    case 'f': freq = atof(optarg); break;
    case 'I': Inc = atoi(optarg); break;
    case 'C': cmp = 1; break;
    case 'w': w = 1; break;
    }
  }while(next_option!=-1);
  if(delim==NULL){
    delim = malloc(3*sizeof(char));
    strcpy(delim," "); }
  
  if( FIN==stdin )
    fprintf(stderr,"Waiting for input from stdin\n");
  
  double *data = ColumnRead(FIN,delim,&L,ignore,col,&freq);
  double *windata = malloc(L*sizeof(double));

  fftw_complex *out=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*(L/2+1));
  fftw_plan P = fftw_plan_dft_r2c_1d(L,windata,out,FFTW_ESTIMATE);  
  int i,k,Lf=(L/2+1);
  double fscale = freq/(2*(Lf-1));
  fscale = (w)?fscale*2*M_PI:fscale;

  for( k=0;k<L;k+=Inc ){
    Windowed(data,W,k,L,Nw,windata);
    fftw_execute(P);

    if( cmp==0 ){
      for( i=0;i<Lf;i++ )
	fprintf(FOUT,"%lf %lf %lf\n",
		(double)k/freq,i*fscale,pow(cabs(out[i]),2));
      fprintf(FOUT,"\n");
    }
    else{
      for( i=0;i<Lf;i++ )
	fprintf(FOUT,"%lf %lf %lf %lf\n",
		(double)k/freq,i*fscale,creal(out[i]),cimag(out[i]));
      fprintf(FOUT,"\n");
    }
  }

  if( FOUT!=stdout ) fclose(FOUT);
  if( FIN!=stdin ) fclose(FIN);
  fftw_destroy_plan(P);
  fftw_free(out);
  free(data);
  free(windata);
  free(delim);
  /* if(delim!=NULL) free(delim); */
  
  return 0;
}
