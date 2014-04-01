#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char buf[32];

int func1()
{
    if (buf[0] == 'A')
        return 1;
    return 0;

}

int main(int ac, char **av)
{
    int fd;
    //char* secret = "AAAAAAAAAA";

    fgets(buf, 32, stdin); 
        
    if (func1() == 0) 
    {
        printf("%s\n", "FAIL!");
        return;
    }

    printf("%s\n", "WIN!");

}



