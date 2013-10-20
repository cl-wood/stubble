#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char **av)
{
    char *buf;
    char c;

    if (!(buf = malloc(32)))
        return -1;

    c = buf[0];           /* UAF not match */
    free(buf);  
    c = buf[0];           /* UAF match     */
    buf = malloc(32);
    c = buf[0];           /* UAF not match */
}
