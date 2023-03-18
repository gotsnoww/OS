/* It 'adds' integers in two binary files and stores in another file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "wrapper.h"

int main(int argc, char *argv[])
{
  int ifd1, ifd2, ofd;
  int data1, data2, data3;
 
  if(argc != 4) { 
	  fprintf(stderr, "Usage: ./hw0 infile1 infile2 outfile");
	  exit(1);
  }

  ifd1 = Open(argv[1], O_RDONLY);
  read(ifd1, &data1, sizeof(data1));
  close(ifd1);

  ifd2 = Open(argv[2], O_RDONLY);
  read(ifd2, &data2, sizeof(data2));
  close(ifd2);

  data3 = data1 + data2;

  ofd = Creat(argv[3], 0644);
  write(ofd, &data3, sizeof(data3));
  close(ofd);

  return(0);
}
