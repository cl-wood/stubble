#include "clark_make.h"


int main(int argc, char* argv[]) 
{
    char* defaultMakefiles[7] = {argv[0], "dummyValueForArgumentOne", "-f", "makefile1", "makefile2", "makefile3", NULL};
    char** makefiles;

    struct stat fs;

    // Case: no args
    if (argc == 1) {
        makefiles = (char**)defaultMakefiles;
        argc = 5;
    } 

    // Case: ./a.out cmd
    else if (argc == 2) {
        // TODO strcpy segfaults why??
        defaultMakefiles[1] = argv[1];
        makefiles = (char**)defaultMakefiles;
        argc = 5;
    }
    
    // Case: ./a.out [cmd] -f m1[,m2,...]
    else {
        DEBUG
        makefiles = argv;
    }

    for (int i = 2; i < argc; i++) {
        printf("%s\n", makefiles[i]);

        // Skip -f flag
        if (makefiles[i][0] == '-') {
            continue;
        }

        // Setup rules data structure
        makefileStruct rules;
        //rules = makefileFactory(rules);

        // Try to open file
        int ret = stat(makefiles[i], &fs);
        if(ret != 0) {
            printf("Error trying to stat %s\n", makefiles[i]);
            break;
        }

        // Build rulesStruct 
        printf("Trying to build %s\n", makefiles[i]);
        FILE* makefile = fopen(makefiles[i], "r");
        rules = parseMakefile(makefile, rules);

        printf("Making %s\n", makefiles[1]);
        execTarget(rules, makefiles[1]);

    }


}

//

