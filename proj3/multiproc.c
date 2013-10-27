#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>

#define N 4096

float x[N+2][N+2][2];

#define myabs(a) (((a) > 0) ? (a):(-(a)))

int pids[1000];

int main(int argc, char* argv[]) 
{
    int NN;
    float a1, a2, a3, a4, a5, a6;
    float MAXDIFF;
    int i, j;
    int t, t1, t2;
    float  maxdiff1;

    int res = scanf("%d %f %f %f %f %f %f %f", &NN, &a1, &a2, &a3, &a4, &a5, &a6, &MAXDIFF);
    if (res!=0) {printf("ERROR");}

    printf("%d %f %f %f %f %f %f %f\n", NN, a1, a2, a3, a4, a5, a6, MAXDIFF);   

    printf("maxdiff = %13.12f\n", MAXDIFF);

    for (i=1; i<=NN+1; i++) {
        x[i][0][0] = a5*i;
        x[i][0][1] = a5*i;

        x[i][NN+1][0] = a6*i;
        x[i][NN+1][1] = a6*i;

        x[0][i][0] = 0.0;
        x[0][i][1] = 0.0;

        x[NN+1][i][0] = 0.0;
        x[NN+1][i][1] = 0.0; 
    }


    for (i=1; i<=NN; i++) 
        for (j=1; j<=NN; j++) {
            x[i][j][0] = 0.0;
            x[i][j][1] = 0.0;
        }

    printf("maxdiff=%13.12f\n", MAXDIFF);

    t = 0; t1 = 1;
    maxdiff1 = 100000.0;
    //iteration = 0;

    int Nprocesses;
    if (argc < 2) {
        Nprocesses = 4;
    } else {
        Nprocesses = atoi(argv[1]);
    }

    // Pipes for giving and returning sub-matrices
    int subMatrixPipe[Nprocesses][2];
    int resultsPipe[Nprocesses][2];

    printf("Running with %d processes\n", Nprocesses);

    for (int procNum = 0; procNum < Nprocesses; procNum++) {
        pid_t pid = fork();

        // Child runs until parent kills it
        if (pid == 0) {

            // Create sub-matrix
            float sub_x[N+2][N+2][2];

            while (1) {
                // Prep pipe for read
                pipe(subMatrixPipe[procNum]);
                close(subMatrixPipe[procNum][1]);
                close(0); dup2(subMatrixPipe[procNum][0], 0);
                
                // Read in x
                int n;
                float value;
                float x[][];
                while (n = read(subMatrixPipe[procNum][0], &value, sizeof(float)) > 0) {
                    
                }

                // Do sub-jacobi
                for (i = NN / NProcesses * procNum + 1; i <= NN / Nprocesses * (procNum + 1); i++) {
                    for (j = 1; j <= NN; j++) {
                        val = a1 * x[i-1][j] + 
                              a2 * x[i][j-1] + 
                              a3 * x[i+1][j] +
                              a4 * x[i][j+1];
                        if (myabs(val - x[i][j]) > maxdiff1) {
                            maxdiff1 = myabs(val - x[i][j]);
                        }
                    }
                }
        //if ((t= write(i, &(x[NN/2][j][t1]),  sizeof(float))) != sizeof(float)) {

                // Prep pipe for write
                pipe(resultsPipe[procNum]);
                close(resultsPipe[procNum][0]);
                close(1); dup2(resultsPipe[procNum][1], 1);

                // Write back x
                writePipe res; // and localDiff
            }
        }

        // Parent grabs pid to kill child later
        else {
            pids[procNum] = pid;
        }
    }

    // Parent process
    while (maxdiff1 > MAXDIFF) {
        for (int procNum = 0; procNum < Nprocesses; procNum++) {
            //for (i = NN / Nthreads * myNum + 1; i <= NN / Nthreads * (myNum + 1); i++) {
            writePipe submatrix
        }

        for (int procNum = 0; procNum < Nprocesses; procNum++) {
            receiveIteration data
            get max(localDiffs)
            writeIterationData
        }

        swap(t,t1)
    }

    // Done, kill off children
    for (int procNum = 0; procNum < Nprocesses; procNum++) {
        kill(pids[procNum], SIGTERM);
    }


            /*
               for (i = 1; i <= NN; i++) {
               for (j = 1; j <=NN; j++) {
               x[i][j][t] = a1 * x[i-1][j][t1] + 
               a2 * x[i][j-1][t1] + 
               a3 * x[i+1][j][t1] +
               a4 * x[i][j+1][t1];
               if (myabs(x[i][j][t] - x[i][j][t1]) > maxdiff1) {
               maxdiff1 = myabs(x[i][j][t] - x[i][j][t1]);
               }
               }
               }
             */


            printf("MAXDIFF = %f, maxdiff = %f\n", MAXDIFF, maxdiff1);

    if ((i=open("multiproc.output", O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) {
        fprintf(stderr, "Cannot open file proj3.output.\n");
        exit(0);
    }

    for (j = 0; j <= NN + 1; j++) {
        if ((t= write(i, &(x[NN/2][j][t1]),  sizeof(float))) != sizeof(float)) {
            fprintf(stderr, "write error %d %d.\n", j, t);
            exit(0);
        }
    }

    for (j = 0; j <= NN + 1; j++) {
        if ((t = write(i, &(x[j][NN/2][t1]),  sizeof(float))) != sizeof(float)) {
            fprintf(stderr, "write error. %d %d\n", j, t);
            exit(0);
        }
    }
    close(i);
    return 0;
    }





