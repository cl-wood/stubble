/* strcpy example */
#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[])
{
    char str1[40];
    strcpy(str1,argv[1]);
    printf("str1: %s\n",str1);
    return 0;
}
