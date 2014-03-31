#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


int foo(char *buf)
{
    if (buf[0] != 'A')
        return 0;
    return 1;

}

int main(int ac, char **av)
{
    int fd;
    char *buf;

    if (!(buf = malloc(32)))
        return -1;

    fd = open("./file.txt", O_RDONLY);
    read(fd, buf, 1); 
    close(fd);
    foo(buf);
    printf("%d\n", foo(buf));
}



