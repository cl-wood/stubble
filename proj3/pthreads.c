#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>

#define N 4096
#define MAX_THREADS 1000

float x[2][N+2][N+2];
int NN;
float a1, a2, a3, a4, a5, a6;
float MAXDIFF;
int i, j;
int t, t1, t2;
float  maxdiff1;
//int iteration;

int myid[100];

pthread_t tid[200];
int Nthreads;

// Locking
int DONE = 0;
int t_running = 0;
pthread_mutex_t x_mutex[2];
pthread_cond_t x_cond[2];

#define myabs(a) (((a) > 0) ? (a):(-(a)))

// TODO divide for loops up using TID?
// Globals written:
//      maxdiff1
//      x
//      t,t1,t2
// Guaranteed to always have 2n threads
void *jacobi(void *arg)
{
    int i, j;
    int t,t1,t2;

    // Half of threads work on t, other on t1
    int myNum = *(int *)arg;
    if (myNum % 2 == 0) {
        t = 0; 
    }
    else {
        t = 1; 
    }
    t1 = ~t;

    printf("TID %d working on %d\n", myNum, t); fflush(stdout);

    // Wait for other half to finish a matrix
    pthread_mutex_lock(&x_mutex);
    while (t_running != t) {
        pthread_cond_wait(&x_cond, &x_mutex);
    }
    maxdiff1 = -1.0;

    //for (i = NN/Nthreads*myNum+1; i <= NN/Nthreads*(myNum+1); i++) {
    for (i = NN/Nthreads*myNum+1; i <= NN/Nthreads*(myNum+1); i++) {
        for (j=1; j <=NN; j++) {
            x[t][i][j] = a1 * x[t1][i-1][j] + 
                         a2 * x[t1][i][j-1] + 
                         a3 * x[t1][i+1][j] +
                         a4 * x[t1][i][j+1];
            if (myabs(x[t][i][j] - x[t1][i][j]) > maxdiff1) {
                maxdiff1 = myabs(x[t][i][j] - x[t1][i][j]);
            }
        }
    }

    // performing this outside of function with locks
    //t2 = t; t = t1; t1 = t2; 

    //if (myNum == Nthreads - 1 && maxdiff1 > MAXDIFF) {
    if (maxdiff1 > MAXDIFF) {
        DONE = 1;
        printf("DONE HERE %d\n", myNum); fflush(stdout);
    }
    //}

    // if last thread in one of the halves
    if ((Nthreads / 2 + 1 == myNum) || (Nthreads - 1 == myNum)) {
        ~t_running;
        pthread_cond_signal(&x_cond[myNum + 1]);
        pthread_mutex_unlock(&x_mutex);
    }
    printf("DONE %d\n", myNum); fflush(stdout);
    return NULL;

}

int main(int argc, char* argv[]) 
{

    int res = scanf("%d %f %f %f %f %f %f %f", &NN, &a1, &a2, &a3, &a4, &a5, &a6, &MAXDIFF);
    if (res == 0) printf("ERROR\n");
    printf("%d %f %f %f %f %f %f %f\n", NN, a1, a2, a3, a4, a5, a6, MAXDIFF);   

    for (i=1; i<=NN+1; i++) {
        x[0][i][0] = a5*i;
        x[1][i][0] = a5*i;

        x[0][i][NN+1] = a6*i;
        x[1][i][NN+1] = a6*i;

        x[0][0][i] = 0.0;
        x[1][0][i] = 0.0;

        x[0][NN+1][i] = 0.0;
        x[1][NN+1][i] = 0.0; 
    }


    for (i=1; i<=NN; i++) 
        for (j=1; j<=NN; j++) {
            x[0][i][j] = 0.0;
            x[1][i][j] = 0.0;
        }

    printf("maxdiff=%13.12f\n", MAXDIFF);

    t = 0; t1 = 1;
    maxdiff1 = 100000.0;
    //iteration = 0;

    // Set number of threads to use
    if (argc < 2) {
        Nthreads = 4;
    } else {
        Nthreads = atoi(argv[1]);
        printf("Running with %d threads\n", Nthreads);
        // Deal with even number of threads to make things easier
        if (Nthreads % 2 != 0) {
            Nthreads--;
        }
    }

    // Begin parallelization

    // Prepare conditional variables, 1 for each matrix
    pthread_mutex_init(&x_mutex[0], NULL);
    pthread_mutex_init(&x_mutex[1], NULL);
    pthread_cond_init(&x_cond[0], NULL);
    pthread_cond_init(&x_cond[1], NULL);

    // First thread is ready
    //ready[0] = 1;

    // Create threads
    for (int i = 0; i < Nthreads; i++) {
        myid[i] = i;
        if (pthread_create(&tid[i], NULL, &jacobi, &myid[i]) != 0) {
            printf("Failed to create thread #%d\n", i);
            exit(-1);
        }
    } 

    // Join threads
    for (int i = 0; i < Nthreads; i++) {
        pthread_join(tid[i], NULL);
    } 

    // End parallelization
    printf("MAXDIFF = %f, maxdiff = %f\n", MAXDIFF, maxdiff1);

    if ((i=open("pthreads.output", O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) {
        fprintf(stderr, "Cannot open file pthreads.output.\n");
        exit(0);
    }

    for (j = 0; j <= NN + 1; j++) {
        if ((t= write(i, &(x[t1][NN/2][j]),  sizeof(float))) != sizeof(float)) {
            fprintf(stderr, "write error %d %d.\n", j, t);
            exit(0);
        }
    }

    for (j = 0; j <= NN + 1; j++) {
        if ((t = write(i, &(x[t1][j][NN/2]),  sizeof(float))) != sizeof(float)) {
            fprintf(stderr, "write error. %d %d\n", j, t);
            exit(0);
        }
    }
    close(i);
    return 0;
}





