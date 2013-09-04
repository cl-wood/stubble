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
    if (config_file == NULL) {
        printf("Error: %s not a valid file.\n", argv[1]);
        return -1;
    }

    char user[str_length];
    fscanf(config_file, "%s", user);

    // Read each line to get machine:root pairs
    /*
    while (fscanf(config_file, "", ) != EOF ) {

    }*/
    // check feof(f) to make sure it wasn't an error, was actually EOF

    fclose(config_file);


    // While loop, exec each command
    int args;
    char str[str_length];
    char cmd[str_length]; // Don't need to bother with bounds checking?
    char arg[str_length]; 
    char machine1[str_length]; 
    char path1[str_length]; 
    char machine2[str_length]; 
    char path2[str_length]; 

    while (fgets(str, str_length, stdin) ) {

        // quit case
        if (strncmp(str, "quit", 4) == 0) {
            printf("%s\n", "Quitting now.");
            return 0;
        }


        // ls case, may have one optional parameter specifying dir to list.
        if (strncmp(str, "ls", 2) == 0) {

            // TODO do i need to parse a machine:path or just path?
            int ret = sscanf(str, "%s %s", cmd, path1);
            if (ret == 1) {
                printf("Executing %s on home directory\n", cmd);
            } else if (ret == 2) {
                printf("Executing %s on directory: %s\n", cmd, path1);
            } else {
                printf("%s\n", "Error: Used ls incorrectly.");
            }

        }

        
        if (strncmp(str, "cat", 3) == 0) {
            printf("HERE\n");
        }

        if (strncmp(str, "cd", 2) == 0) {
            printf("HERE\n");
        }

        if (strncmp(str, "mkdir", 5) == 0) {
            printf("HERE\n");
        }

        if (strncmp(str, "cp", 2) == 0) {
            printf("HERE\n");
        }

            //sprintf(cmd, "%s %s@%s", ssh, user, machine);

    }

    //DEBUG

}

