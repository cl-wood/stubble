#include "file_organizer.h"

int main(int argc, char* argv[]) 
{

    // Exit if wrong number of arguments
    if (argc != 2) {
        printf("%s\n", k_error_arguments);
        return -1;
    }

    // Read first line of input arg, this is just user
    FILE* config_file = fopen(argv[1], "r");
    if (config_file == NULL) {
        printf("Error: %s not a valid file.\n", argv[1]);
        return -1;
    }

    char str[k_str_length];
    char user[k_str_length];
    fscanf(config_file, "%s", user);

    // Read each line to get machine:root pairs
    // TODO currently assuming up to 10 strings of 100 chars
    int i = 0;
    char machine[k_str_length]; 
    char machines[10][100];
    memset(machines, 0, 10 * 100);
    char root[k_str_length]; 
    char roots[10][100];
    memset(roots, 0, 10 * 100);

    // TODO getting a duplicate on last entry?
    while (fgets(str, k_str_length, config_file) ) {

        fscanf(config_file, "%s %s", machine, root);
        strncpy(machines[i], machine, strlen(machine) + 1);
        strncpy(roots[i], root, strlen(root) + 1);
        i++;
    }

    for (int j = 0; j < i; j++) {
        printf("%s %s\n", machines[j], roots[j]);
    }
    // check feof(f) to make sure it wasn't an error, was actually EOF

    fclose(config_file);

/*

    // While loop, exec each command
    int command_number = 1;
    int args;
    char cmd[k_str_length]; // Don't need to bother with bounds checking?
    char arg[k_str_length]; 
    char machine1[k_str_length]; 
    char path1[k_str_length]; 
    char machine2[k_str_length]; 
    char path2[k_str_length]; 
    printf("%s%d%s ", "<cmd:", command_number++, ">");

    while (fgets(str, k_str_length, stdin) ) {

        // Strip trailing newline
        strtok(str, "\n");

        // quit case
        if (compare(k_quit, str)) {
            printf("%s\n", "Quitting now.");
            return 0;
        }


        // ls case, may have one optional parameter specifying dir to list.
        if (compare(k_ls, str)) {

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

        
        if (compare(k_cat, str)) {
            printf("HERE\n");
        }

        if (compare(k_cd, str)) {
            printf("HERE\n");
        }

        if (compare(k_mkdir, str)) {
            printf("HERE\n");
        }

        if (compare(k_cp, str)) {
            printf("HERE\n");
        }


        // End of while loop, print next line
        printf("%s%d%s ", "<cmd:", command_number++, ">");
    }

*/
    //DEBUG

}

