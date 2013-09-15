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

    // Set default machine and path
    char default_machine[k_str_length];
    strcpy(default_machine, paths[0].machine);
    char default_root[k_str_length];
    strcpy(default_root, paths[0].root);
    char default_path[k_str_length];
    default_path[0] = '\0';

    // While loop, exec each command
    int command_number = 0;
    printf("%s%d%s ", "<cmd:", command_number++, ">");

    while (fgets(str, k_str_length, stdin) ) {

        int pid, stat;
        int num_machines = 0;
        char delimiter = '/';
        char cmd[k_str_length]; // Don't need to bother with bounds checking?
        char machine1[k_str_length]; 
        char path1[k_str_length]; 
        char root1[k_str_length]; 
        char machine2[k_str_length]; 
        char path2[k_str_length]; 
        char root2[k_str_length]; 

        // Init to NULL string
        cmd[0]      = '\0';
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

            for (int i = 0; i < num_paths; i++) {
                if (strstr(token, paths[i].machine) != NULL) {
                    sscanf(token, "%[^:]%*c%s", machine1, path1);
                    strcpy(root1, paths[i].root);
                    break;
                }
            }
            
            // Check if this token contains a machine name
            /*
            int contains_machine = 0;
            for (int i = 0; i < num_paths; i++) {

                if (strstr(token, paths[i].machine) != NULL) {
                    contains_machine = 1; 

                    // Take care of machine1 first
                    if (num_machines == 0) {
                        num_machines++;
                        sscanf(token, "%[^:]%*c%s", machine1, path1);
                        strcpy(root1, paths[i].root);
                    }

                    // Now handle machine2
                    else {
                        sscanf(token, "%[^:]%*c%s", machine2, path2);
                        strcpy(root2, paths[i].root);
                    }

                    break;
                }
            } // end machine name for loop

            // Must be done to handle flags that can occur before or after args
            if (!contains_machine) {
                strcat(cmd, " "); 
                strcat(cmd, token); 
                //printf("cmd: %s\n", cmd);
            }
            */

            token = strtok (NULL, " ");

        } // end token while

        /* EXEC code
         * There are 3 main cases:
         *      1) ls with no args
         *      2) anything else but cp, which has 1 arg
         *      3) cp, with 2 args
         */

        // ls command with 0 args
        // TODO ls -al doesn't work
        if (compare(k_ls, cmd) && machine1[0] == '\0') {

            strcpy(machine1, default_machine);
            strcpy(root1, default_root);
        } // end ls

        // cd command
        if (compare(k_cd, cmd) ) {
            printf("change current directory to %s:%s\n", machine1, path1);
            strcpy(default_machine, machine1 );
            //strcpy(default_root, path1);
            strcpy(default_path, path1);

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

            printf("execute command 'ssh -q %s %s %s'\n", 
                m1, cmd, abs_path);

            if (execl("/usr/bin/ssh", "-q", m1, cmd, abs_path, (char*)0) == -1) {
                exit(-1);
            } 
            
        }

        // Now wait for child
        pid = wait(&stat);

        // 
        printf("%s%d%s ", "<cmd:", command_number++, ">");

    } // end of user input while loop

}

