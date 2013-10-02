#include "clark_make.h"


int main(int argc, char* argv[]) 
{
    char* defaultMakefiles[6] = {"dummyValueForArgumentOne", "-f", "makefile1", "makefile2", "makefile3", NULL};
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
        defaultMakefiles[0] = argv[1];
        makefiles = (char**)defaultMakefiles;
        argc = 5;
    }
    
    // Case: ./a.out [cmd] -f m1[,m2,...]
    else {
        makefiles = argv;
    }

    for (int i = 1; i < argc; i++) {

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

        printf("%s\n", findInPath("ls"));
        //execTarget(rules, "demo");

        //printf("String1 is: %s\n", resolveMacro(rules, "string1", 0));

    }


}

//

