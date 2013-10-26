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
float l_maxdiff1s[MAX_THREADS]; // global, use to reduce maxdiff1 after threads done

pthread_t tid[200];
int Nthreads;

// Locking
int threadsDone = 0;
int done = 0;
pthread_mutex_t x_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t x_cond = PTHREAD_COND_INITIALIZER;

#define myabs(a) (((a) > 0) ? (a):(-(a)))

/* cool point about mutex vs. sema http://stackoverflow.com/questions/2065747/pthreads-mutex-vs-semaphore/13222180#13222180 */
void *jacobi(void *arg)
{
    int i, j;

    int myNum = (int) arg;
    //printf("Thread %d\n", myNum); fflush(stdout);

    while (!done) {
        l_maxdiff1s[myNum] = -1.0;
        for (i = NN / Nthreads * myNum + 1; i <= NN / Nthreads * (myNum + 1); i++) {
            for (j = 1; j <= NN; j++) {
                x[t][i][j] = a1 * x[t1][i-1][j] + 
                             a2 * x[t1][i][j-1] + 
                             a3 * x[t1][i+1][j] +
                             a4 * x[t1][i][j+1];
                // Update personal maxdiff, reduce later
                if (myabs(x[t][i][j] - x[t1][i][j]) > l_maxdiff1s[myNum]) {
                    l_maxdiff1s[myNum] = myabs(x[t][i][j] - x[t1][i][j]);
                }
            }
        }

        // LOCK
        pthread_mutex_lock(&x_mutex);

        threadsDone++;

        // Last thread done. Switch x's and go again unless at MAXDIFF
        if (threadsDone == Nthreads) {
            threadsDone = 0;

            float maxdiffCheck = -1.0;

            for (int k = 0; k < Nthreads; k++) {
                if (l_maxdiff1s[k] > maxdiffCheck) {
                    maxdiffCheck = l_maxdiff1s[k];
                }
            }

            //printf("maxdiff1 = %f\n", maxdiffCheck); fflush(stdout);

            // Done if hit MAXDIFF
            if (maxdiffCheck <= MAXDIFF) {
                done = 1;
                //printf("HIT MAXDIFF\n"); fflush(stdout);
            }

            t2 = t; t = t1; t1 = t2; 

            pthread_cond_broadcast(&x_cond);
        } 

        // Not last thread finished, wait for them to finish matrix
        else {
            //printf("Waiting %d\n", myNum); fflush(stdout);
            pthread_cond_wait(&x_cond, &x_mutex);
        }

        // UNLOCK
        pthread_mutex_unlock(&x_mutex);

    } // End while !done

    //printf("DONE %d\n", myNum); fflush(stdout);
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

    // Set number of threads to use
    // TODO seems to mess up with non-power of 2 thread count
    if (argc < 2) {
        Nthreads = 4;
    } else {
        Nthreads = atoi(argv[1]);
        printf("Running with %d threads\n", Nthreads);
    }




    // Begin parallelization
    // Create threads
    for (int i = 0; i < Nthreads; i++) {
        if (pthread_create(&tid[i], NULL, &jacobi, (void *)i) != 0) {
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


