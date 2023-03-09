#ifndef MMM_H_
#define MMM_H_

#define MAX 3
#define MIN 0

extern double** A;
extern double** B;
extern double** C;
extern double** D;

extern int SIZE;
extern int num_threads;


/** struct to hold parameters to mmm_par **/
typedef struct thread_args {
  int tid;    // the given thread id (0, 1, ...)
  int start; // where to begin summation
  int end;   // where to end summation
} thread_args;


void mmm_init();
void mmm_reset(double **);
void mmm_freeup();
void mmm_seq();
void *mmm_par(void *args);
double mmm_verify();
void print_matrix(double** matrix);

#endif /* MMM_H_ */
