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
    fgets(str, k_str_length, config_file);
    sscanf(str, "%s", user);

    // Read each line to get machine:root pairs
    // TODO currently assuming up to 10 strings of 100 chars
    int num_paths = 0;
    absolute_path paths[32];
    while (fgets(str, k_str_length, config_file) ) {
        sscanf(str, "%s %s", paths[num_paths].machine, paths[num_paths].root);
        printf("%s:%s\n",paths[num_paths].machine, paths[num_paths].root);
        num_paths++;
    }

    // check feof(f) to make sure it wasn't an error, was actually EOF
    fclose(config_file);

    // Set current machine and path
    char current_machine[k_str_length];
    strcpy(current_machine, paths[0].machine);
    char current_root[k_str_length];
    strcpy(current_root, paths[0].root);
    char current_path[k_str_length];
    current_path[0] = '\0';

    // While loop, exec each command
    int command_number = 0;
    printf("%s%d%s ", "<cmd:", command_number++, ">");

    while (fgets(str, k_str_length, stdin) ) {

        int pid, stat;
        int num_machines = 0;
        char delimiter = '/';
        char cmd[k_str_length]; // Don't need to bother with bounds checking?
        char flags[k_str_length];
        char machine1[k_str_length]; 
        char path1[k_str_length]; 
        char root1[k_str_length]; 
        char machine2[k_str_length]; 
        char path2[k_str_length]; 
        char root2[k_str_length]; 

        // Init to NULL string
        cmd[0]      = '\0';
        flags[0]      = '\0';
        machine1[0] = '\0'; 
        path1[0]    = '\0'; 
        root1[0]    = '\0'; 
        machine2[0] = '\0'; 
        path2[0]    = '\0'; 
        root2[0]    = '\0'; 

        // Strip trailing newline
        strtok(str, "\n");

        // quit case
        if (compare(k_quit, str)) {
            return 0;
        }

        // Tokenize based on whitespace
        char* token = strtok(str, " ");

        // Get initial command
        strcpy(cmd, token);

        // Re-prime for rest of user input
        token = strtok (NULL, " ");

        while (token != NULL) {

            // Starts with '-', then flag
            if (token[0] == '-') {
                strcpy(flags, token); 
            }

            // Else, it's machine:path
            else {
                int machine_token = 0;

                // find root path based on machine name
                for (int i = 0; i < num_paths; i++) {
                    if (strstr(token, paths[i].machine) != NULL) {
                        machine_token = 1;
                        sscanf(token, "%[^:]%*c%s", machine1, path1);
                        strcpy(root1, paths[i].root);
                        break;
                    }
                }
            }

            token = strtok (NULL, " ");

        } // end token while

        /* EXEC code
         * There are 4 main cases:
         *      1) ls with no args
         *      2) cd, which changes the current machine:path/root vars
         *      3) anything else but cp, which has 1 arg
         *      4) cp, with 2 args
         */

        // ls command with 0 args
        // TODO ls -al doesn't work
        if (compare(k_ls, cmd) && machine1[0] == '\0') {
            strcpy(machine1, current_machine);
            strcpy(root1, current_root);
            strcpy(path1, current_path);
        } // end ls

        // cd command
        // Behavior is undefined for no args to cd
        if (compare(k_cd, cmd) ) {
            printf("change current directory to %s:%s\n", machine1, path1);

            // Change current_{machine,path,root}, only used for ls without path
            strcpy(current_machine, machine1);
            strcpy(current_path, path1);
            for (int i = 0; i < num_paths; i++) {
                if (strcmp(machine1, paths[i].machine) == 0) {
                    printf("FOUND %s\n", machine1);
                    strcpy(current_root, paths[i].root);
                    break;
                }
            }

            // TODO We don't print the root, but we have to account for it
        } // end cd


        // Now exec command
        if (fork() == 0) {
            // Make user@machine string
            char m1[100];
            strcat(m1, user);
            strcat(m1, "@");
            strcat(m1, machine1);

            // Make absolute path string
            char abs_path[100];
            strcat(abs_path, root1);
            strcat(abs_path, "/");
            strcat(abs_path, path1);

            printf("execute command 'ssh -q %s %s %s %s'\n", 
                m1, cmd, flags, abs_path);

            if (execl("/usr/bin/ssh", "-q", m1, cmd, flags, abs_path, (char*)0) == -1) {
                exit(-1);
            } 
            
        }

        // Now wait for child
        pid = wait(&stat);

        // 
        printf("%s%d%s ", "<cmd:", command_number++, ">");

    } // end of user input while loop

}

