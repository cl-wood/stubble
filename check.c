int main() {

	for (int i = 0; i < numPipes; i++) {
		pipe(fds[i]);
	}

	//Last pipe always the same
	if (fork()== 0) {
		printf("end pipe: %d\n", numPipes - 1);
		close(0); dup(fds[numPipes - 1][0]); /* redirect standard input to fds[1] */
		close(fds[numPipes - 1][1]);
		for (int i = 0; i < numPipes; i++) {
			close(fds[i][0]);
			close(fds[i][1]);
		}
		execv(argvs[numPipes][0], argvs[numPipes]);
		exit(0);
	}

	for (int i = numPipes - 1; i > 0; i--) {
		if (fork()== 0) {
			printf("middle pipe out: %d\n", i);
			close(1); dup(fds[i][1]);
			printf("middle pipe in: %d\n", i - 1);
			close(0); dup(fds[i - 1][0]); 
			for (int i = 0; i < numPipes; i++) {
				close(fds[i][0]);
				close(fds[i][1]);
			}
			//having this makes it stop?
			execv(argvs[i][0], argvs[i]);
			exit(0);
		}
	}


	if (fork() == 0) {
		close(1); dup(fds[0][1]);  /* redirect standard output to fds[0] */
		for (int i = 0; i < numPipes; i++) {
			close(fds[i][0]);
			close(fds[i][1]);
		}
		execv(argvs[0][0], argvs[0]);
		exit(0);

	}

	for (int i = 0; i < numPipes; i++) {
		close(fds[i][0]);
		close(fds[i][1]);
	}
}
