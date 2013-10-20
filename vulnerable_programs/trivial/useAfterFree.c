#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char *buf = malloc(32);
    char c;

    // User input
    char *userInput = malloc(32);
    gets(userInput);

    c = buf[0];             /* UAF not match */
    if (userInput[0] == 'A') {
        printf("Running UAF branch\n");
        free(buf);  
        c = buf[0];         /* UAF match     */
        buf = malloc(32);
        c = buf[0];         /* UAF not match */
    }
    if (userInput[0] != 'A') {
        printf("NOT running UAF branch\n");
    }

}
