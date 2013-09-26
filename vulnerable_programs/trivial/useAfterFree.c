#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
	char *buf;
	// Allocate memory
	buf = (char *) malloc(512);

	// Free memory
	free(buf);

	// Get used input, taint source
	char string[32];
	printf("%s", "Enter string: ");
  	gets(string);	

	// Use memory after freed
	strcpy(buf, string);

}

