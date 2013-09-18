#ifndef __clark_make_h__
#define __clark_make_h__

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define kStringLength 256
#define kNumRules 16
#define maxRecursiveDepth 32

#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);
#define DBGING 1

// struct holding makefile rules
typedef struct {
    // 16 64 char length rules
    int numMacroRules;
    char macroKey[kNumRules][kStringLength];    
    char macroValue[kNumRules][kStringLength];    

    // Prevent infinite recursion
    int recursiveDepth;

} rulesStruct;

// Make a rulesStruct and init properly
rulesStruct rulesFactory(rulesStruct rules) {
    rules.numMacroRules = 0;
    rules.recursiveDepth = 0;

    return rules;
}


// 
rulesStruct parseMakefile(FILE* makefile, rulesStruct rules)
{
    rules.numMacroRules = 0;
    char str[kStringLength];
    while (fgets(str, kStringLength, makefile) ) {
       
        int ret;
        switch(str[0]) {
            // Blank line
            case '\n':
                break;

            // Comment line
            case '#':
                break;

            // Inference rule
            case '.':
                break;

            // Macro
            default:
                ret = sscanf(str, "%[^=]%*c%s\n", 
                                rules.macroKey[rules.numMacroRules], 
                                rules.macroValue[rules.numMacroRules]);

                // Find 2 strings? Add the rule
                if (ret == 2) {
                    rules.numMacroRules++;
                }   

        }

    }

    #ifdef DBGING
    for (int i = 0; i < rules.numMacroRules; i++) {
        printf("Rule %d: %s = %s\n", i, rules.macroKey[i], rules.macroValue[i]);
    }
    #endif


    return rules;
} // end parseMakefileRules

// Recursively resolve key:value pairs of macros
char* resolveMacro(rulesStruct rules, char* key, int recursiveDepth)
{
    // likely we have a loop
    if (recursiveDepth == maxRecursiveDepth) {
        printf("Probably encountered a loop\n");
        return (char*)0;
    }

    for (int i = 0; i < rules.numMacroRules; i++) {
        if (strcmp(rules.macroKey[i], key) == 0) {
            return resolveMacro(rules, rules.macroValue[i], recursiveDepth + 1);
        }
    }

    return key;

} // end resolveMacro



#endif

