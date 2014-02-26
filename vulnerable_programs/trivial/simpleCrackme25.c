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
    int win = 0;
    if (buf[300] == 'B' && 
	buf[301] == 'B' && 
	buf[302] == 'C' && 
	buf[303] == 'D' && 
	buf[304] == 'E' && 
	buf[305] == 'F' && 
	buf[306] == 'G' && 
	buf[307] == 'H' && 
	buf[308] == 'I' && 
	buf[309] == 'J' && 
	buf[310] == 'K' && 
	buf[311] == 'L' && 
	buf[312] == 'M' && 
	buf[313] == 'N' && 
	buf[314] == 'O' && 
	buf[315] == '1' && 
	buf[316] == '2' && 
	buf[317] == '3' && 
	buf[318] == '4' && 
	buf[319] == '5' && 
	buf[320] == '6' && 
	buf[321] == '7' && 
	buf[322] == '8' && 
	buf[323] == '9' && 

	buf[324] == '5') {
        	win = 1;
    }

    if (win) {
        printf("you win!\n");
    }
    else {
        printf("you lose!\n");
    }

}
