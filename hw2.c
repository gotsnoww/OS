/* It computes matrix multiplication. Reads two matrix, multiply them, stores the result matrix */
/* Usage: ./hw2 data1.bin data2.bin res.bin N */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>
#include <signal.h>
#include <sys/wait.h>
#include "wrapper.h"
#include "timeft.h"

int main(int argc, char *argv[])
{
  int ifd1, ifd2, ofd;
  double* data1;
  double* data2;
  struct stat finfo1, finfo2;

  //hw2가 4개의 인자를 받지 못할시 에러메시지 출력 후 종료
  if (argc != 5) {
	fprintf(stderr, "Usage: ./hw2 infile1 infile2 outfile <p>");
	exit(1);	
  }

  int p = atoi(argv[4]); //프로세스 개수
  if (p <= 0) {
	  fprintf(stderr, "Error: The 4th parameter should be an integer greater than zero");
	  exit(1);
  }

  ifd1 = Open(argv[1], O_RDONLY);
  Fstat(ifd1, &finfo1);
  //첫 번째 파일을 메모리 공간에 매핑시킴
  void *addr1 = mmap(NULL, finfo1.st_size, PROT_READ, MAP_PRIVATE, ifd1, 0); 
  data1 = (double *)addr1;
  close(ifd1);

  //첫 번째 인자 파일의 크기가 n*n double이 아닌 경우
  double n = sqrt((finfo1.st_size / 8)); //행렬의 한 면의 길이 n
  if (((n - (int)n) != 0)){
	  fprintf(stderr, "Error: The 1st input file size is not n*n");
	  exit(1);
  }

  ifd2 = Open(argv[2], O_RDONLY);
  Fstat(ifd2, &finfo2);
  void *addr2 = mmap(NULL, finfo2.st_size, PROT_READ, MAP_PRIVATE, ifd2, 0);
  data2 = (double *)addr2;
  close(ifd2);

  //두 번째 인자 파일의 크기가 첫 번째 인자 파일의 크기와 같지 않은 경우
  if (finfo1.st_size != finfo2.st_size){
	  fprintf(stderr, "Error: The 2nd input file size is not the same as the 1st one");
	  exit(1);
  }

  int M = (int)n;

  //네 번째 인자의 값이 행렬의 한쪽 면의 크기인 n보다 클 경우
  if (p > M)
	  p = M;

  int startIdx, endIdx;
  int r = M % p;
  //pid_t pid_temp[10];

  double* result = (double*)malloc(M * M * sizeof(double));

  init_timelog(1);
  start_timelog(0);
  
  for (int i = 0; i < p; i++){  
	pid_t pid = fork();
	//pid_temp[i] = getpid();

	if(r == 0) //행렬의 n과 프로세스의 개수의 나머지가 0이면
	{
		startIdx = i * (M/p);
		endIdx = (i+1) * (M/p) - 1;
	}
	else { //나머지가 0이 아니면
		if(i < r){ //자식 프로세스가 담당할 영역 설정
			startIdx = i * (M/p + 1);
			endIdx = (i+1) * (M/p + 1) - 1;
	        }
		else{
			startIdx = r + i * (M/p);
	        	endIdx = r + (i+1) * (M/p) - 1;	
	        }
	}

	if(pid == 0){ //자식 프로세스
		//해당 자식 프로세스가 담당한 영역의 행렬 곱 구하기
		for(int j = startIdx; j <= endIdx; j++){
			for(int k = 0; k < M; k++){
				double res = 0;
				for(int l = 0; l < M; l++){
					res += data1[j * M + l] * data2[l * M + k];
				}
				result[j * M + k] = res;
			}
		}		
	}
  }
  
  finish_timelog(0);
  close_timelog();
   
  ofd = Creat(argv[3], 0644); //행렬의 곱을 저장할 세 번째 인자 파일 생성
  write(ofd, result, M * M * sizeof(double)); //자식 프로세스에서 구한 행렬의 곱 저장
  close(ofd);

  /* 자식 프로세스 종료
  for (int i = 0; i < p; i++) {
	if (kill(pid_temp[i], SIGTERM) < 0) {
		fprintf(stderr, "Failed kill process");
	}
	else { //자식 프로세스가 종료될 때까지 대기
		int status;
		waitpid(pid_temp[i], &status, 0);
	}
  } */

  munmap(addr1, finfo1.st_size);
  munmap(addr2, finfo1.st_size);
  free(result);

  return(0);
}
