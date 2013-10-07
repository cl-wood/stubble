#include "clark_make.h"


int main(int argc, char* argv[]) 
{
    char* dummy = "dummyValueForArgumentOne";
    char* defaultMakefiles[7] = {argv[0], dummy, "-f", "makefile1", "makefile2", "makefile3", NULL};
    char** makefiles;

    struct stat fs;

    // Case: clark_make
    if (argc == 1) {
        //DEBUG
        makefiles = (char**)defaultMakefiles;
        argc = 6;
    } 

    // Case: clark_make cmd
    else if (argc == 2 && argv[1][0] != '-') {
        //DEBUG
        // TODO strcpy segfaults why??
        defaultMakefiles[1] = argv[1];
        makefiles = (char**)defaultMakefiles;
        argc = 5;
    }
    
    // Case: clark_make -f m1,m2,...
    else if (argv[1][0] == '-') {
        //DEBUG
        for (int i = 1; i < argc; i++) {
           defaultMakefiles[i + 1] = argv[i];
        }
        defaultMakefiles[argc + 1] = NULL;
        makefiles = (char**)defaultMakefiles;
        //argc = 5;
        argc = argc + 1;
    }

    // Case: clark_make cmd -f m1,m2,...
    else {
        //DEBUG
        makefiles = argv;

    }


    for (int i = 2; i < argc; i++) {

        // Skip -f flag
        if (makefiles[i][0] == '-') {
            continue;
        }

        // Setup rules data structure
        makefileStruct rules;
        rules = makefileFactory(rules);

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

        // Make first target
        if (strcmp(makefiles[1], dummy) == 0) {
            //DEBUG
            makefiles[1] = rules.targets[0].targets[0];
        }

        printf("Making %s\n", makefiles[1]);
        execTarget(rules, makefiles[1]);

    }


}

//

