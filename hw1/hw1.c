/* It computes matrix multiplication. Reads two matrix, multiply them, stores the result matrix */
/* Usage: ./hw1 data1.bin data2.bin res.bin */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wrapper.h"
#include "timeft.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <math.h>

int main(int argc, char *argv[])
{
  int ifd1, ifd2, ofd, n;
  double* data1;
  double* data2;
  struct stat finfo1, finfo2;

  if(argc != 4) {
	  fprintf(stderr, "Usage: ./hw1 infile1 infile2 outfile");
	  exit(1);
  }

  ifd1 = Open(argv[1], O_RDONLY);
  fstat(ifd1, &finfo1);
  void *addr1 = mmap(NULL, finfo1.st_size, PROT_READ, MAP_PRIVATE, ifd1, 0);
  data1 = (double *)addr1;  
  close(ifd1);
  
  double temp = sqrt((finfo1.st_size / 8));
  if(((temp - (int)temp) != 0)){
	  fprintf(stderr, "Error: The 1st input file size is not n*n");
	  exit(1);	
  }

  ifd2 = Open(argv[2], O_RDONLY);
  fstat(ifd2, &finfo2);
  void *addr2 = mmap(NULL, finfo2.st_size, PROT_READ, MAP_PRIVATE, ifd2, 0);
  data2 = (double *)addr2;
  close(ifd2);


  if(finfo1.st_size != finfo2.st_size){
	  fprintf(stderr, "Error: The 2nd input file size is not the same as the 1st one");
	  exit(1);
  }

  n = sqrt(finfo1.st_size / sizeof(double));	

  ofd = Creat(argv[3], 0644);
  double* result = (double*)malloc(n * n * sizeof(double));

  init_timelog(1);
  start_timelog(0);

  for(int i = 0; i < n; i++){
	  for(int j = 0; j < n; j++){
		  double res = 0;
		  for(int k = 0; k < n; k++){
			  res += data1[i * n + k] * data2[k * n + j];
		  }
		  result[i * n + j] = res;
          }
  }

  finish_timelog(0);
  close_timelog();

  write(ofd, result, n * n * sizeof(double));
  close(ofd);
  	
  munmap(addr1, finfo1.st_size);
  munmap(addr2, finfo1.st_size);

  return(0);
}
