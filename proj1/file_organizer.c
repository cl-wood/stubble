#include<stdio.h>
#include "file_organizer.h"

int main(int argc, char* argv[]) 
{

    // Exit if wrong number of arguments
    if (argc != 2) {
        printf("%s\n", error_arguments);
        return -1;
    }

    // Read first line of input arg, this is just user
    FILE* config_file = fopen(argv[1], "r");
    char user[256];
    //DEBUG
    fscanf(config_file, "%s", &user);

    // Read each line and execute it
    char cmd[256]; // Don't need to bother with bounds checking?
    char* machine = "linprog";
    sprintf(cmd, "%s %s@%s", ssh, user, machine);

    printf("%s\n", cmd);
}

