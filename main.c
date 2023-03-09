#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "rtclock.h"
#include "mmm.h"

#define avgOf 3

/** Global Variables~! */
double** A; // Matrixes that might be multiplied, assuming this code works
double** B; // The good news is that it does work, so they will be multiplied
double** C; 
double** D; // probably

int SIZE; // dimensions
int num_threads; // num threads


//prints statistics about matrix multiplication program
void print_stats(char* operation, int numThreads, int size){
	printf("========\nmode: %s\nthread count: %d\nsize: %d\n========\n",operation,numThreads,size);
}
//does sequential matrix multiplication
void do_seq(int size){
	print_stats("sequential",1,size);
	SIZE = size;
	num_threads = 1;
	mmm_init();
	mmm_seq();

	double start = rtclock();
	for(int i = 0; i < avgOf; i++){mmm_seq();} // does mmm_seq 3 times

	double seq_time = (rtclock() - start) / avgOf; //gets ending time
	printf("Sequential Time (avg of %d runs): %lf sec\n",avgOf,seq_time);
	mmm_freeup();
}

//does parallel matrix multiplication
void do_par(int numThreads, int size){
	print_stats("parallel",numThreads,size); // prints header
	SIZE = size;
	num_threads = numThreads;

	mmm_init();	

	/**
	 * The following code runs parallelization once
	 * This is to warm-up the cache (first run) and to smooth-out the results.
	*/

	/**allocates args*/
	thread_args *args = (thread_args*) malloc(num_threads * sizeof(thread_args)); 
	for (int i = 0; i < num_threads; i++) {
		args[i].tid = i;
		args[i].start = i*(SIZE*SIZE / num_threads) + fmin(SIZE*SIZE % num_threads,i+1);
		args[i].end = (i + 1) * (SIZE*SIZE / num_threads) + fmin(SIZE*SIZE % num_threads,i+1);;
	}
	pthread_t *threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t)); // makes threads

	for (int i = 0; i < num_threads; i++) pthread_create(&threads[i], NULL, mmm_par, &args[i]); // runs pthreads
	for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL); //joins pthreads

	free(args);
	free(threads);

	mmm_seq();//This is to warm-up the cache (first run) and to smooth-out the results.

	/**
	 * Run sequential and time it to get the average
	*/
	double start = rtclock();
	for(int i = 0; i < avgOf; i++){
		mmm_seq();
	}
	double seq_time = (rtclock() - start) / avgOf;

	/**
	 * Run parallelization 3 times and get the average
	*/

	start = rtclock();
	for(int j = 0; j < avgOf; j++){ // 3 times
		args = (thread_args*) malloc(num_threads * sizeof(thread_args)); 
		for (int i = 0; i < num_threads; i++) {
			args[i].tid = i;
			args[i].start = i*(SIZE*SIZE / num_threads) + fmin(SIZE*SIZE % num_threads,i+1);
			args[i].end = (i + 1) * (SIZE*SIZE / num_threads) + fmin(SIZE*SIZE % num_threads,i+1);;
		}
		threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
		for (int i = 0; i < num_threads; i++) pthread_create(&threads[i], NULL, mmm_par, &args[i]);
		for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);
		free(args);
		free(threads);
	}
	double par_time = (rtclock() - start) / avgOf;
	double largErr = mmm_verify(); // verify the error

	printf("Sequential Time (avg of %d runs): %lf sec\n",avgOf,seq_time);
	printf("Parallel Time (avg of %d runs): %lf sec\n",avgOf,par_time);
	printf("Speedup: %lf\n",seq_time/par_time);
	printf("Verifying... ");
	printf("largest error between parallel and sequential matrix: %lf\n",largErr);

	mmm_freeup();
}

int main(int argc, char *argv[]) {

	if(argc <= 1){ // no args
		printf("Usage: ./mmm <mode> [num threads] <size>\n");
		printf("or ./mmm -c4 to play Connect4\n");
		return 1;	
	}else if(argc > 3 && atoi(argv[3]) < 0){ //invalid size parallel mode
		printf("Usage: parallel mode requires a positive size\nUsage: ./mmm <mode> [num threads] <size>\n");
		return 1;
	}else if(argc >=2 && strcmp(argv[1],"-c4")==0){ // play connect 4
		if(system("git clone https://www.github.com/lwgover/Connect4")){} // if statement to surpress unused warning (Sorry Julia)
		if(system("python3 Connect4/connect4.py -n -l 6")){}
		if(system("rm -r Connect4")){}
	}else if(argc > 3 && atoi(argv[2]) < 0){ // invalid num threads
		printf("Usage: parallel mode requires a positive num threads\nUsage: ./mmm <mode> [num threads] <size>\n");	
		return 1;
	}else if(argc > 2 && atoi(argv[2]) < 0){ // invalid size sequential mode
		printf("Usage: sequential mode requires a positive size\nUsage: ./mmm <mode> [num threads] <size>\n");	
		return 1;
	}else if(argv[1][0]=='S' && argc > 2){ // Do sequential!
		do_seq(atoi(argv[2]));
	}else if(argv[1][0]=='P' && argc > 3) { // Do parallel !
		do_par(atoi(argv[2]),atoi(argv[3]));
	}else if(argv[1][0]=='P' && argc > 2){ // not enough arguments!
		printf("Usage: parallel mode requires [num threads]\n");
		return 1;
	}else{
		printf("Usage: ./mmm <mode> [num threads] <size>\n"); // Catch all
		return 1;
	}
	return 0;
}