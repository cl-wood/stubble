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
    int num_paths = 0;
    char machine[k_str_length]; 
    char root[k_str_length]; 
    absolute_path paths[100];

    // TODO getting a duplicate on last entry?
    // TODO can i fscanf directly into paths?
    while (fgets(str, k_str_length, config_file) ) {

        fscanf(config_file, "%s %s", machine, root);
        strncpy(paths[num_paths].machine, machine, strlen(machine) + 1);
        strncpy(paths[num_paths].root, root, strlen(root) + 1);
        num_paths++;
    }

    /*
    for (int j = 0; j < num_paths; j++) {
        printf("%s %s\n", paths[j].machine, paths[j].root);
    }
    */
   
    // check feof(f) to make sure it wasn't an error, was actually EOF

    fclose(config_file);

    // While loop, exec each command
    int command_number = 0;
    int num_machines = 0;
    printf("%s%d%s ", "<cmd:", command_number++, ">");

    while (fgets(str, k_str_length, stdin) ) {

        char cmd[k_str_length]; // Don't need to bother with bounds checking?
        char machine1[k_str_length]; 
        char path1[k_str_length]; 
        char machine2[k_str_length]; 
        char path2[k_str_length]; 

        // Strip trailing newline
        strtok(str, "\n");

        // quit case
        if (compare(k_quit, str)) {
            return 0;
        }

        else {

            // Tokenize based on whitespace
            char* token = strtok(str, " ");

            // Get initial command
            strcpy(cmd, token);
            printf("cmd: %s\n", cmd);

            // Re-prime for rest of user input
            token = strtok (NULL, " ");
            
            while (token != NULL) {
                    
                // Check if this token contains a machine name
                int contains_machine = 0;
                for (int i = 0; i < num_paths; i++) {

                    if (strstr(token, paths[i].machine) != NULL) {
                        contains_machine = 1; 

                        // Take care of machine1 first
                        if (num_machines == 0) {
                            num_machines++;
                            sscanf(token, "%[^:]%*c%s", machine1, path1);
                            printf("m: %s\n", machine1);
                            printf("p: %s\n", path1);
                        }

                        // Now handle machine2
                        else {
                            sscanf(token, "%[^:]%*c%s", machine2, path2);
                        }
                        
                        break;
                    }
                } // end machine name for loop

                // Must be done to handle flags that can occur before or after args
                if (!contains_machine) {
                    strcat(cmd, " "); 
                    strcat(cmd, token); 
                    printf("cmd: %s\n", cmd);
                }

                token = strtok (NULL, " ");

           } // end token while

            printf("execute command ssh -q %s@%s %s \n", user, machine1, cmd);
        }


        // End of while loop, print next line
        printf("%s%d%s ", "<cmd:", command_number++, ">");
    }

    //DEBUG

}

