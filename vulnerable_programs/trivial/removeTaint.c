#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    // Open and read user input
    //int fd = open("file.txt", O_RDONLY);
    int fd = open(argv[1], O_RDONLY);
    char buf[500];
    int res = read(fd, buf, 500);
    close(fd);

    // You win if 300th character is X

    // remove taint
    int i;
    for (i = 0; i < 500; i++) {
        buf[i] = '\0';
    }

    int win = 0;
    if (buf[300] == 'X') {
        win = 1;
    }

    if (win) {
        printf("you win!\n");
    }
    else {
        printf("you lose!\n");
    }

}