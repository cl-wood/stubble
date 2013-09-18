#include "clark_make.h"

int main(int argc, char* argv[]) 
{
    char* defaultMakefiles[3] = {"makefile1", "makefile2", "makefile3"};

    struct stat fs;
    // If no args, try to make makefile{1,2,3}
    if (argc == 1) {
    /*

        //TODO different types of rules, do we have to parse the whole makefile first?
        // 3 default makefiles
        for (int i = 0; i < 3; i++) {
            int ret = stat(defaultMakefiles[i], &fs);

            if(ret == 0) {
                printf("Trying to build %s\n", defaultMakefiles[i]);
                FILE* makefile = fopen(defaultMakefiles[i], "r");

                while (fgets(str, kStringLength, config_file) ) {
                    sscanf(str, "%s %s", );
                    printf("%s:%s\n",paths[num_paths].machine, paths[num_paths].root);
                }

                fclose(config_file);
            }

        }
    */
    }

    // Otherwise, -f and files should be args
    else if (argc > 1) {
        
        rulesStruct rules;
        rules = rulesFactory(rules);
        
        for (int i = 2; i < argc; i++) {
            int ret = stat(argv[i], &fs);

            if(ret != 0) {
                printf("Error trying to stat %s\n", argv[i]);
                break;
            }

            printf("Trying to build %s\n", argv[i]);
            FILE* makefile = fopen(argv[i], "r");
            rules = parseMakefile(makefile, rules);
            printf("String1 is: %s\n", resolveMacro(rules, "string1", 0));
            
        }
    }

}


