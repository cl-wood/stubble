#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{

    char buf = getc(stdin);

    // You win if character is X
    int win = 0;
    if (buf == 'X') {
        win = 1;
    }

    if (win) {
        printf("you win!\n");
    }
    else {
        printf("you lose!\n");
    }

}
