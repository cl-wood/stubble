#define _POSIX_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>

#define N 4096
int check = 0;

float x[N+2][N+2][2];
int iter = 0;

#define myabs(a) (((a) > 0) ? (a):(-(a)))

int pids[1000];

int main(int argc, char* argv[]) 
{
    int NN;
    float a1, a2, a3, a4, a5, a6;
    float MAXDIFF;
    int i, j;
    j = 0;
    int t, t1, t2;
    float  maxdiff1;

    int res = scanf("%d %f %f %f %f %f %f %f", &NN, &a1, &a2, &a3, &a4, &a5, &a6, &MAXDIFF);
    if (res <= 0) {printf("ERROR");}

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
    int inPipe[Nprocesses][2];
    int outPipe[Nprocesses][2];

    printf("Running with %d processes\n", Nprocesses);

    for (int procNum = 0; procNum < Nprocesses; procNum++) {
        pipe(inPipe[procNum]);
        pipe(outPipe[procNum]);

        pid_t pid = fork();

        // Child runs until parent kills it
        if (pid == 0) {

            // Create sub-matrix
            //if (Nprocesses == 1) Nprocesses++;
            float sub_x[NN/Nprocesses + 2][NN + 2];
            float localDiff;

            while (1) {
                localDiff = -1.0;

                // Read in x one float at a time
                close(inPipe[procNum][1]);
                float value;
                int i, j;

                // Read entire section
                for (i = 0; i <= NN + 1; i++) {
                    for (j = 0; j <= NN + 1; j++) {
                        while (read(inPipe[procNum][0], &value, sizeof(float)) != sizeof(float));
                        // 
                        sub_x[i][j] = value;    

                        //fprintf(stderr, "%f ", sub_x[i][j]);fflush(stderr);
                    }
                        //fprintf(stderr, "\n");fflush(stderr);
                }

                // Do sub-jacobi
                //for (i = NN / NProcesses * procNum + 1; i <= NN / Nprocesses * (procNum + 1); i++) {
                // TODO might have to have specific case for last proc

                close(1); dup2(outPipe[procNum][1], 1);
                for (i = 1; i <= NN / Nprocesses; i++) {
                    for (j = 1; j <= NN; j++) {
                        // don't store, write
                        float temp = a1 * sub_x[i-1][j] + 
                                     a2 * sub_x[i][j-1] + 
                                     a3 * sub_x[i+1][j] +
                                     a4 * sub_x[i][j+1];
                        //    fprintf(stderr, "%f %d %d\n", sub_x[i][j][t], i, j);
                        write(outPipe[procNum][1], &temp, sizeof(float));

                        if (myabs(temp - sub_x[i][j]) > localDiff) {
                            localDiff = myabs(temp - sub_x[i][j]);
                        }
                    }
                }
                //fprintf(stderr, "localdiff: %f\n", localDiff);fflush(stderr);
                write(outPipe[procNum][1], &localDiff, sizeof(float));

                if (check == 2)
                    exit(0);
                //check++;
                
            } // End while 1
            } // End if pid is 0

            // Parent grabs pid to kill child later
            else {
                pids[procNum] = pid;
            }
            } // End for loop for forking children


            // Parent process
            while (maxdiff1 > MAXDIFF) {
                maxdiff1 = -1.0;

                // Give sub matrix to each process
                for (int procNum = 0; procNum < Nprocesses; procNum++) {
                    //for (i = NN / Nthreads * myNum + 1; i <= NN / Nthreads * (myNum + 1); i++) {
                    // Prep pipe for write
                    //close(inPipe[procNum][0]);
                    close(1); dup2(inPipe[procNum][1], 1);

                    // Write sub-matrix of x to pipe
                    //for (i = NN / Nprocesses * procNum + 1; i <= NN / Nprocesses * (procNum + 1); i++) {
                    //for (i = NN / Nprocesses * procNum; i <= NN / Nprocesses * (procNum + 1) + 1; i++) {
                    for (i = 0; i <= NN + 1; i++) {
                        for (j = 0; j <= NN + 1; j++) {
                            if (write(inPipe[procNum][1], &(x[i][j][t]), sizeof(float)) != sizeof(float))
                                fprintf(stderr, "write fail\n");fflush(stderr);
                            //fprintf(stderr, "Parent passes: %f\n", x[i][j][t]);fflush(stderr);
                        }
                    }
                }

                // Get localDiff and iteration for each sub matrix
                for (int procNum = 0; procNum < Nprocesses; procNum++) {

                    //close(outPipe[procNum][0]);
                    close(1); dup2(outPipe[procNum][1], 1);

                    // Read localDiffs
                    float localDiff;

                    // Write back x
                    for (i = 1; i <= NN / Nprocesses; i++) {
                        for (j = 1; j <= NN; j++) {
                            read(outPipe[procNum][0], &(x[i][j][t]), sizeof(float));
                     fprintf(stderr,"%f ", x[i][j][t]); fflush(stderr);
                        }
                     fprintf(stderr,"\n"); fflush(stderr);
                    }
                    read(outPipe[procNum][0], &localDiff, sizeof(float));
                    fprintf(stderr, "localdiff: %f\n", localDiff);fflush(stderr);

                    if (localDiff > maxdiff1) {
                        maxdiff1 = localDiff;
                    }

                } // End receive matrix and localDiff, calc maxdiff1

                // Swap t/t1
                t2 = t; t = t1; t1 = t2; 
                //fprintf(stderr,"%f\n", maxdiff1); fflush(stderr);

                } // End while, got to MAXDIFF

                // Done, kill off children
                for (int procNum = 0; procNum < Nprocesses; procNum++) {
                    kill(pids[procNum], SIGTERM);
                }


                printf("MAXDIFF = %f, maxdiff = %f\n", MAXDIFF, maxdiff1);

                if ((i=open("multiproc.output", O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) {
                    fprintf(stderr, "Cannot open file multiproc.output.\n");
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





