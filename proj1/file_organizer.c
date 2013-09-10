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

    // While loop, exec each command
    int command_number = 0;
    int num_machines = 0;
    printf("%s%d%s ", "<cmd:", command_number++, ">");

    while (fgets(str, k_str_length, stdin) ) {

        char cmd[k_str_length]; // Don't need to bother with bounds checking?
        char machine1[k_str_length]; 
        char path1[k_str_length]; 
        char root1[k_str_length]; 
        char machine2[k_str_length]; 
        char path2[k_str_length]; 
        char root2[k_str_length]; 

        // Init to NULL string
        machine1[0] = '\0'; 
        path1[0] = '\0'; 
        root1[0] = '\0'; 
        machine2[0] = '\0'; 
        path2[0] = '\0'; 
        root2[0] = '\0'; 

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
                    printf("cmd: %s\n", cmd);
                }

                token = strtok (NULL, " ");

           } // end token while


            // Handle commands
            //<cmd:1> cp -r linprog1:/a linprog2:/a
            //printf("execute command 'ssh -q wood@linprog1 scp -q /home/grads/wood/tmp//a wood@linprog2:/tmp/wood///a'\n");
            /*
            printf("execute command 'ssh -q %s@%s %s -q %s/%s %s@%s:%s/%s'\n", 
                user, 
                machine1, 
                cmd,
                root1,
                path1,
                user,
                machine2,
                root2,
                path2
            );
            */

            // ls command
            // Either 1) "ls"
            //        2) "ls machine"
            //        3) "ls machine:dir"
            if (compare(k_ls, cmd) ) {

                char delimiter;
                
                if (machine1[0] == '\0' && path1[0] == '\0') {
                    delimiter = '\0';
                    strcpy(machine1, paths[0].machine);
                    strcpy(root1, paths[0].root);
                }

                else if (machine1[0] != '\0' && path1[0] == '\0') {
                    delimiter = '\0';
                    strcpy(root1, paths[0].root);
                }

                else if (machine1[0] != '\0' && path1[0] != '\0') {
                    delimiter = '/';
                } 


                printf("execute command 'ssh -q %s@%s %s %s%c%s'\n", 
                    user,  // @
                    machine1, // SPACE
                    cmd, // 
                    root1,
                    delimiter,
                    path1 
                );

            }

                /*
                if (machine1[0] == NULL) {
                    printf("execute command 'ssh -q %s@%s %s -q %s/%s %s@%s:%s/%s'\n", 
                    user, 
                    machine1, 
                    cmd,
                    root1,
                    path1,
                    user,
                    machine2,
                    root2,
                    path2
                    );
                    */
                

                //execute command 'ssh -q wood@linprog1 ls  /home/grads/wood/tmp'
                //execute command 'ssh -q wood@linprog1 ls  /home/grads/wood/tmp/a'
                /*
                execute command 'ssh -q wood@ ls -q / wood@:/'
                <cmd:1> ls linprog1:a
                cmd: ls
                execute command 'ssh -q wood@linprog1 ls -q /home/grads/wood/tmp/a wood@:/'
                <cmd:2> ls linprog1
                cmd: ls
                execute command 'ssh -q wood@ ls -q / wood@linprog1:/home/grads/wood/tmp/'
                */

            


            // cp command 
            //if (compare(k_cp, cmd) ) {
            //    
            //}





        } // end user input while


        // 
        printf("%s%d%s ", "<cmd:", command_number++, ">");
    }

    //DEBUG

}

