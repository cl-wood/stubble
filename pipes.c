#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int numPipes = 5;
    int fds_array[numPipes][2];
    int child_array[numPipes][2];
    char *argv[3];

    for (int i = 0; i < numPipes; i++)
      pipe(fds_array[i]);

    if (fork() == 0) {
	    close(fds_array[numPipes - 1][1]);
	    close(0); dup(fds_array[0]); 
	    argv[0] = "/bin/more";
	    argv[1] = NULL;        
	    execv(argv[0], argv);
    }
    if (fork()== 0) {
	    close(0); dup(fds[numPipes - 1][0]); /* redirect standard input to fds[1] */
	    close(fds[numPipes - 1][1]);
	    for (int i = 0; i < numPipes; i++) {
		    close(fds[i][0]);
		    close(fds[i][1]);
	    }
	    execv(argvs[numPipes][0], argvs[numPipes]);
	    exit(0);
    }


    // close output and redir to i
    // close input and redir to i - 1
    for (int i = 0; i < numPipes; i++) {
	    if (fork() == 0) {
		    close(1); dup(fds_array[i][1]);  
		    close(0); dup(fds_array[i - 1][0]); 
		    close(fds_array[i][1]);
		    close(fds_array[i - 1][1]);
		    argv[0] = "/bin/more";
		    argv[1] = NULL;        
		    execv(argv[0], argv);
	    }
    }

    if (fork() == 0) {
	    close(fds_array[0][0]);
	    close(1); dup(fds_array[1]);  
	    argv[0] = "/bin/ps";
	    argv[1] = NULL;
	    execv(argv[0], argv);
    }

    for (int i = 0; i < numPipes; i++) {
	    close(fds_array[i][1]);
	    wait(&child_array[0]);  
    }
} 
