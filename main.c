#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"

void print_stats(char* operation, int numThreads, int size){
	printf("========\nmode: %s\nthread count: %u\nsize: %u\n========\n",operation,numThreads,size);
}
void do_seq(int size){
	print_stats("sequential",1,size);
}
void do_par(int numThreads, int size){
	print_stats("parallel",numThreads,size);	
}

int main(int argc, char *argv[]) {
	double clockstart, clockend;
	clockstart = rtclock(); // start clocking
	// start: stuff I want to clock
	for(int i = 0; i < argc; i++){
		printf("%s\n",argv[i]);
	}
	if(argc <= 1){
		printf("Usage: ./mmm <mode> [num threads] <size>\n");
		return 1;	
	}else if(argc > 3 && atoi(argv[3]) < 0){
		printf("Usage: parallel mode requires a positive size\nUsage: ./mmm <mode> [num threads] <size>\n");
		return 1;
	}else if(argc >=2 && strcmp(argv[1],"-c4")==0){
		system("git clone https://www.github.com/lwgover/Connect4");
		system("python3 Connect4/connect4.py -n -l 6");
		printf("Answer yes to the following questions: ");
		system("rm -r Connect4");
	}else if(argc > 3 && atoi(argv[2]) < 0){
		printf("Usage: parallel mode requires a positive num threads\nUsage: ./mmm <mode> [num threads] <size>\n");	
		return 1;
	}else if(argc > 3 && atoi(argv[2]) < 0){
		printf("Usage: sequential mode requires a positive size\nUsage: ./mmm <mode> [num threads] <size>\n");	
		return 1;
	}else if(argv[1][0]=='S' && argc > 2){
		do_seq(atoi(argv[2]));
	}else if(argv[1][0]=='P' && argc > 3) {
		do_par(atoi(argv[2]),atoi(argv[3]));
	}else if(argv[1][0]=='P' && argc > 2){
		printf("Usage: parallel mode requires [num threads]\n");
		return 1;
	}else{
		printf("Usage: ./mmm <mode> [num threads] <size>\n");
		return 1;
	}

	// end: stuff I want to clock
	clockend = rtclock(); // stop clocking
	printf("Time taken: %.6f sec\n", (clockend - clockstart));

	return 0;
}