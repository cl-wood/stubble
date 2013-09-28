#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFSIZE 512

int main(int argc, char **argv) {
	char *buf;
	buf = (char *) malloc(BUFSIZE);

	free(buf);
	strncpy(buf, argv[1], BUFSIZE-1);
}

