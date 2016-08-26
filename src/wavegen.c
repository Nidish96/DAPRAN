#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<math.h>
#include<string.h>
#define d2r 0.017453292

const char* progname;
const struct option op_long[] = {
  {"help", 0, NULL, 'h'},
  {"Waveatrributes", 1, NULL, 'W'},
  {"Sampling", 1, NULL, 'f'},
  {"Time", 1, NULL, 't'},
  {"Points", 1, NULL, 'p'},
  {"skiptime", 0, NULL, 's'},
  {"Outfile", 1, NULL, 'o'},
  {NULL, 0, NULL, 0}
};
const char* const op_desc[] = {
  "---\n\tPrint this help message\n",
  "dbl_dbl_dbl_dbl,dbl_dbl_dbl_dbl,...\n\tSpecify wave by <amp_freq(Hz)_phase(deg)_power>. Power denotes the power to which the particular\n\twave has to be raised\n",
  "dbl\n\tSpecify Sampling Frequency (in Hz)\n",
  "dbl\n\tSpecify how long to generate the wave (in seconds)\n",
  "int\n\tSpecify how data points to generate (resets the sampling frequency accordingly)\n",
  " - \n\tSet option to skip time column in output\n",
  "str\n\tSpecify output file (stdout by default)\n",  
};
const int op_num =  7;
void PrintUsage()
{
  fprintf(stderr,"Usage : %s options [ outfile ... ]\n",progname);
  int i;
  for( i=0;i<op_num;i++ )
    fprintf(stderr,"-%c --%s\t%s\n",op_long[i].val,op_long[i].name,op_desc[i]);

  fprintf(stderr,"OUTPUT FORMAT :\nt y\n");

  exit(1);
}

typedef struct{ double A,w,P,pwr; }WAVE;

int main(int argn,char* args[])
{
  const char* const op_short = "hW:f:t:p:so:";
  char* buff;
  int op_choice;
  int nw = 1;
  WAVE *W = (WAVE*)malloc(nw*sizeof(WAVE));
  int points = 256;
  double freq=10,Tmax=points/freq;
  points = -1;
  int tflag = 1;

  FILE* FOUT = stdout;

  progname = args[0];
  do{
    op_choice = getopt_long(argn,args,op_short,op_long,NULL);

    switch(op_choice){
    case 'h': PrintUsage(); break;
    case 'W': nw=0;
      buff = strtok(optarg,",");
      do{
	nw++;
	W = (WAVE*)realloc(W,nw*sizeof(WAVE));
	sscanf(buff,"%lf_%lf_%lf_%lf",&W[nw-1].A,&W[nw-1].w,&W[nw-1].P,&W[nw-1].pwr);
	W[nw-1].pwr = (!W[nw-1].pwr)?1:W[nw-1].pwr;
	W[nw-1].w *= 2*M_PI;
	W[nw-1].P *= d2r;
      }while(buff=strtok(NULL,","));
      break;
    case 'f': freq = atof(optarg); break;
    case 't': Tmax = atof(optarg); break;
    case 'p': points = atoi(optarg); break;
    case 'o': FOUT = fopen(optarg,"w+"); break;
    case 's': tflag = 0; break;
    }    
  }while(op_choice!=-1);

  /* Tmax = (points!=-1)?points/freq:Tmax; */
  freq = (points!=-1)?points/Tmax:freq;

  double t,step=1./freq,y;
  int i;
  for( t=0;t<=Tmax;t+=step){
    y = 0;
    for( i=0;i<nw;i++ )
      y += pow(W[i].A*cos(W[i].w*t+W[i].P),W[i].pwr);
    if( tflag ) fprintf(FOUT,"%lf ",t);
    fprintf(FOUT,"%lf\n",y);
  }

  free(W);
  if( FOUT!=stdout ) fclose(FOUT);
  return 0;
}
