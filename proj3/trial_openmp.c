#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <omp.h>

#define N 4096

//float x[N+2][N+2][2];
float x[2][N+2][N+2];

#define myabs(a) (((a) > 0) ? (a):(-(a)))

int main(void) 
{
    int NN;
    float a1, a2, a3, a4, a5, a6;
    float MAXDIFF;
    int i, j;
    int t, t1, t2;
    float  maxdiff1;
    int iteration;


    scanf("%d %f %f %f %f %f %f %f", &NN, &a1, &a2, &a3, &a4, &a5, &a6, &MAXDIFF);


    printf("%d %f %f %f %f %f %f %f\n", NN, a1, a2, a3, a4, a5, a6, MAXDIFF);   
    /*  a1 = a2 = a3 = a4 = 0.25; a6 = 0; a5 = 0.1;

        MAXDIFF = 0.0001;
        */

    printf("maxdiff = %13.12f\n", MAXDIFF);

    for (i=1; i<=NN+1; i++) {
        x[0][i][0] = a5*i;
        x[1][i][0] = a5*i;

        x[0][i][NN+1] = a6*i;
        x[1][i][NN+1] = a6*i;

        x[0][i][0] = 0.0;
        x[1][i][0] = 0.0;

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
    iteration = 0;

    //omp_set_num_threads(4);

    // maybe works, but not faster?
    #pragma omp parallel default(shared)
    {
        int pNN = NN;
        int pa1 = a1;
        int pa2 = a2;
        int pa3 = a3;
        int pa4 = a4;

        while (maxdiff1 > MAXDIFF) {
            maxdiff1 = -1.0;

            #pragma omp for schedule(static)
            for (int i = 1; i <= pNN; i++) {
                for (int j = 1; j <= pNN; j++) {
                    //#pragma omp flush(t1)

                    x[t][i][j] = pa1 * x[t1][i-1][j] + 
			    	             pa3 * x[t1][i+1][j] +
				                 pa2 * x[t1][i][j-1] + 
				                 pa4 * x[t1][i][j+1];

                    //#pragma omp critical
                    //{
                    //    #pragma omp flush(maxdiff1)
                    if (myabs(x[t][i][j] - x[t1][i][j]) > maxdiff1) {
                        maxdiff1 = myabs(x[t][i][j] - x[t1][i][j]);
                    }
                    //}
                }
            }

            // swap 
            t2 = t; t = t1; t1 = t2;
            //printf("iteration = %d, maxdiff1 = %f, MAXDIFF = %f\n", iteration++, maxdiff1, MAXDIFF);
        }
    } // End openmp

    printf("MAXDIFF = %f, maxdiff = %f\n", MAXDIFF, maxdiff1);

    if ((i=open("openmp.output", O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) {
        fprintf(stderr, "Cannot open file openmp.output.\n");
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





