#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<fcntl.h>

int foo2(char a, char b, char c)
{
    a = 1;
    b = 2;
    c = 3;
    return 0;
}

int foo(char *buf)
{
    char c, b, a;

    c = buf[0];
    b = c;
    a = buf[8];
    foo2(a, b, c);
    return true;
}

int main(int ac, char **av)
{
    int fd;
    char *buf;

    fd = open("./file.txt", O_RDONLY);
    read(fd, buf, 32), close(fd);
    foo(buf);
}
