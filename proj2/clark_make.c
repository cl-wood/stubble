#include "clark_make.h"

static const char* MYMAKEPATH = "/home/grads/wood/.bin:/home/grads/wood/.scripts:/usr/local/bin:/usr/local/java/bin:/usr/lang:/bin:/usr/bin:/usr/ucb:/usr/etc:/usr/local/bin/X11:/usr/bin/X11:/usr/openwin/bin:/usr/ccs/bin:/usr/sbin:/opt/sfw/bin:.";

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
        //rules = parseMakefile(makefile, rules);
        //printf("String1 is: %s\n", resolveMacro(rules, "string1", 0));

    }


}

//

