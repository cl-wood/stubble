#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const int kStringLength = 256;
#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);

int main(int argc, char* argv[]) 
{
    char* defaultMakefiles[3] = {"makefile1", "makefile2", "makefile3"};

    // If no args, try to make makefile{1,2,3}
    if (argc == 1) {
        struct stat fs;

        // 3 default makefiles
        for (int i = 0; i < 3; i++) {
            int ret = stat(defaultMakefiles[i], &fs);

            if(ret == 0) {
                printf("Trying to build %s\n", defaultMakefiles[i]);
                FILE* makefile = fopen(defaultMakefiles[i], "r");

                while (fgets(str, kStringLength, config_file) ) {
                    sscanf(str, "%s %s", paths[num_paths].machine, paths[num_paths].root);
                    printf("%s:%s\n",paths[num_paths].machine, paths[num_paths].root);
                }
                fclose(config_file);
            }

        }

        // Otherwise, -f and files should be args

    }

}


