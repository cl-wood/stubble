/* strcpy example */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (int argc, char *argv[])
{
    int fd = open(argv[1], O_RDONLY);
    char buf[20];
    char str1[20];
    int res = read(fd, buf, 20);


    // Vuln. condition
    if (buf[10] == 'A') {
        while (read(fd, buf, 50) > 0) {
            strcat(str1, buf);
            printf("%s\n",str1);

        }
        //strcpy(str1, buf);
    }
    else {
        strcpy(str1, "Didn't work");
    }

    printf("%s\n",str1);
    close(fd);
    return 0;
}

