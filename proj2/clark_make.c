#include "clark_make.h"

int main(int argc, char* argv[]) 
{
    char* defaultMakefiles[6] = {"dummy", "-f", "makefile1", "makefile2", "makefile3", NULL};
    char** makefiles;

    struct stat fs;

    // If no args, try to make makefile{1,2,3}
    if (argc == 1) {
        makefiles = (char**)defaultMakefiles;
        argc = 5;
    } else {
        makefiles = argv;
    }

    for (int i = 2; i < argc; i++) {
        //printf("%s\n", makefiles[i]);

        // Setup rules data structure
        rulesStruct rules;
        rules = rulesFactory(rules);

        // Try to open file
        int ret = stat(argv[i], &fs);
        if(ret != 0) {
            printf("Error trying to stat %s\n", makefiles[i]);
            break;
        }

        printf("Trying to build %s\n", makefiles[i]);
        FILE* makefile = fopen(makefiles[i], "r");
        rules = parseMakefile(makefile, rules);
        printf("String1 is: %s\n", resolveMacro(rules, "string1", 0));


    }


}


