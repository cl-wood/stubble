#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
    //FILE * pFile = fopen(argv[1], "r");
    int fd = open("file.txt", O_RDONLY);
    char buf[500];
    int res = read(fd, buf, 500);
    close(fd);

    // Loop through password, creating a bunch of branches.
    char *password = "g";
    int counter = 0;
    int i = 300;

    // put switches here
    switch(buf[i]) {
        case 'a':
            printf("Not a\n");
            counter += 97;
            i++;
            break;
        case 'b':
            printf("Not b\n");
            counter += 98;
            i++;
            break;
        case 'c':
            printf("Not c\n");
            counter += 99;
            i++;
            break;
        case 'd':
            printf("Not d\n");
            counter += 100;
            i++;
            break;
        case 'e':
            printf("Not e\n");
            counter += 101;
            i++;
            break;
        case 'f':
            printf("Not f\n");
            counter += 102;
            i++;
            break;
        case 'g':
            i++;
            break;
        case 'h':
            printf("Not h\n");
            counter += 104;
            i++;
            break;
        case 'i':
            printf("Not i\n");
            counter += 105;
            i++;
            break;
        case 'j':
            printf("Not j\n");
            counter += 106;
            i++;
            break;
        case 'k':
            printf("Not k\n");
            counter += 107;
            i++;
            break;
        case 'l':
            printf("Not l\n");
            counter += 108;
            i++;
            break;
        case 'm':
            printf("Not m\n");
            counter += 109;
            i++;
            break;
        case 'n':
            printf("Not n\n");
            counter += 110;
            i++;
            break;
        case 'o':
            printf("Not o\n");
            counter += 111;
            i++;
            break;
        case 'p':
            printf("Not p\n");
            counter += 112;
            i++;
            break;
        case 'q':
            printf("Not q\n");
            counter += 113;
            i++;
            break;
        case 'r':
            printf("Not r\n");
            counter += 114;
            i++;
            break;
        case 's':
            printf("Not s\n");
            counter += 115;
            i++;
            break;
        case 't':
            printf("Not t\n");
            counter += 116;
            i++;
            break;
        case 'u':
            printf("Not u\n");
            counter += 117;
            i++;
            break;
        case 'v':
            printf("Not v\n");
            counter += 118;
            i++;
            break;
        case 'w':
            printf("Not w\n");
            counter += 119;
            i++;
            break;
        case 'x':
            printf("Not x\n");
            counter += 120;
            i++;
            break;
        case 'y':
            printf("Not y\n");
            counter += 121;
            i++;
            break;
        case 'z':
            printf("Not z\n");
            counter += 122;
            i++;
            break;
        case 'A':
            printf("Not A\n");
            counter += 65;
            i++;
            break;
        case 'B':
            printf("Not B\n");
            counter += 66;
            i++;
            break;
        case 'C':
            printf("Not C\n");
            counter += 67;
            i++;
            break;
        case 'D':
            printf("Not D\n");
            counter += 68;
            i++;
            break;
        case 'E':
            printf("Not E\n");
            counter += 69;
            i++;
            break;
        case 'F':
            printf("Not F\n");
            counter += 70;
            i++;
            break;
        case 'G':
            printf("Not G\n");
            counter += 71;
            i++;
            break;
        case 'H':
            printf("Not H\n");
            counter += 72;
            i++;
            break;
        case 'I':
            printf("Not I\n");
            counter += 73;
            i++;
            break;
        case 'J':
            printf("Not J\n");
            counter += 74;
            i++;
            break;
        case 'K':
            printf("Not K\n");
            counter += 75;
            i++;
            break;
        case 'L':
            printf("Not L\n");
            counter += 76;
            i++;
            break;
        case 'M':
            printf("Not M\n");
            counter += 77;
            i++;
            break;
        case 'N':
            printf("Not N\n");
            counter += 78;
            i++;
            break;
        case 'O':
            printf("Not O\n");
            counter += 79;
            i++;
            break;
        case 'P':
            printf("Not P\n");
            counter += 80;
            i++;
            break;
        case 'Q':
            printf("Not Q\n");
            counter += 81;
            i++;
            break;
        case 'R':
            printf("Not R\n");
            counter += 82;
            i++;
            break;
        case 'S':
            printf("Not S\n");
            counter += 83;
            i++;
            break;
        case 'T':
            printf("Not T\n");
            counter += 84;
            i++;
            break;
        case 'U':
            printf("Not U\n");
            counter += 85;
            i++;
            break;
        case 'V':
            printf("Not V\n");
            counter += 86;
            i++;
            break;
        case 'W':
            printf("Not W\n");
            counter += 87;
            i++;
            break;
        case 'X':
            printf("Not X\n");
            counter += 88;
            i++;
            break;
        case 'Y':
            printf("Not Y\n");
            counter += 89;
            i++;
            break;
        case 'Z':
            printf("Not Z\n");
            counter += 90;
            i++;
            break;
        case '0':
            printf("Not 0\n");
            counter += 48;
            i++;
            break;
        case '1':
            printf("Not 1\n");
            counter += 49;
            i++;
            break;
        case '2':
            printf("Not 2\n");
            counter += 50;
            i++;
            break;
        case '3':
            printf("Not 3\n");
            counter += 51;
            i++;
            break;
        case '4':
            printf("Not 4\n");
            counter += 52;
            i++;
            break;
        case '5':
            printf("Not 5\n");
            counter += 53;
            i++;
            break;
        case '6':
            printf("Not 6\n");
            counter += 54;
            i++;
            break;
        case '7':
            printf("Not 7\n");
            counter += 55;
            i++;
            break;
        case '8':
            printf("Not 8\n");
            counter += 56;
            i++;
            break;
        case '9':
            printf("Not 9\n");
            counter += 57;
            i++;
            break;
        default:
            break;
    }

    if (counter == 0) {
        printf("you win!\n");
    }
    else {
        printf("you lose!\n");
    }

}
