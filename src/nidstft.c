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
  { "genHamming", 1, NULL, 'H' },
  { "blackman", 0, NULL, 'b' },
  { "truncblackman", 0, NULL, 'T' },
  { "genbBlackman", 1, NULL, 'B' },
  { "WindowLength", 1, NULL, 'N' },
  { "Time-Col", 1, NULL, 't' },
  { "Infile", 1, NULL, 'i' },
  { "Outfile", 1, NULL, 'o' },
  { "Column", 1, NULL, 'c' },
  { "Length", 1, NULL, 'l' },
  { "Ignore", 1, NULL, 'g' },
  { "delim",1,NULL,'d' },
  { NULL, 0, NULL, 0}
};

void PrintUsage(FILE* Stream)
{
  fprintf(Stream,"Usage: %s options [ outfile ... ]\n",progname);
  fprintf(Stream,
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n"
	  "-%c --%s \n" 	  
	  "-%c --%s \n",
	  long_options[0].val,long_options[0].name,
	  long_options[1].val,long_options[1].name,
	  long_options[2].val,long_options[2].name,
	  long_options[3].val,long_options[3].name,
	  long_options[4].val,long_options[4].name,
	  long_options[5].val,long_options[5].name,
	  long_options[6].val,long_options[6].name,
	  long_options[7].val,long_options[7].name,
	  long_options[8].val,long_options[8].name,
	  long_options[9].val,long_options[9].name,
	  long_options[10].val,long_options[10].name,
	  long_options[11].val,long_options[11].name,
	  long_options[12].val,long_options[12].name,
	  long_options[13].val,long_options[13].name,
	  long_options[14].val,long_options[14].name,
	  long_options[15].val,long_options[15].name,
	  long_options[16].val,long_options[16].name
	  );
  exit(1);
}

const char* const short_options = "hrvmnH:bTB:N:t:i:o:c:l:g:d:";
int main(int argn,char* args[])
{
  int next_option,Nw=256;
  double window[3]={0,0,0};
  progname = args[0];
  double freq=1;
  int col=1,L=-1,ignore=0;
  FILE *FIN=stdin,*FOUT=stdout;
  do{
    next_option = getopt_long(argn,args,short_options,long_options,NULL);

    switch(next_option){
    case 'h': PrintUsage(stderr); break;
    case 'r': window[0]=next_option; break;
    case 'v': window[0]=next_option; break;
    case 'm': window[0]=next_option; break;
    case 'n': window[0]=next_option; break;
    case 'H': window[0]=next_option;
      sscanf(optarg,"%lf,%lf",&window[1],&window[2]); break;
    case 'b': window[0]=next_option; break;
    case 'T': window[0]=next_option; break;
    case 'B': window[0]=next_option;
      window[1] = atof(optarg); break;      
    case 'N': Nw = atoi(optarg); break;
    case 't': freq=-atoi(optarg); break;
    case 'i': FIN=fopen(optarg,"r"); break;
    case 'o': FOUT=fopen(optarg,"w+"); break;
    case 'c': col=atoi(optarg); break;
    case 'l': L=atoi(optarg); break;
    case 'g': ignore=atoi(optarg); break;
    case 'd': delim = (strcmp(optarg,"tab")==0)?"\t":(strcmp(optarg,"spc")==0)?" ":optarg; break;
    }
  }while(next_option!=-1);
  if(delim==NULL){
    delim=malloc(3*sizeof(char));
    strcpy(delim," ");}
  if( FIN==stdin )
    fprintf(stderr,"Waiting for input from stdin\n");
  
  int n;
  for( n=-300;n<300;n++ )
    fprintf(stdout,"%d %f %f %f %f %f %f %f\n",n
	    ,(double)Rectangle(n-100,Nw)
	    ,(double)Triangle(n-100,Nw)
	    ,Hann(n-100,Nw)
	    ,Hamming(n-100,Nw)
	    ,TBlackman(n-100,Nw)
	    ,EBlackman(n-100,Nw)
	    ,GBlackman(n-100,Nw,.16));
  return 0;
}
