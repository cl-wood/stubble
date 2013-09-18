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
    char macro_variable[kNumRules][kStringLength];    
    char macro_value[kNumRules][kStringLength];    

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
rulesStruct parseMakefileRules(FILE* makefile, rulesStruct rules)
{
    rules.numMacroRules = 0;
    char str[kStringLength];
    while (fgets(str, kStringLength, makefile) ) {
       
        int ret;
        switch(str[0]) {

            // Comment line
            case '#':
                break;

            // Macro
            default:
                ret = sscanf(str, "%[^=]%*c%s\n", 
                                rules.macro_variable[rules.numMacroRules], 
                                rules.macro_value[rules.numMacroRules]);

                // Find 2 strings? Add the rule
                if (ret == 2) {
                    rules.numMacroRules++;
                }   

        }

    }

    #ifdef DBGING
    for (int i = 0; i < rules.numMacroRules; i++) {
        printf("Rule %d: %s = %s\n", i, rules.macro_variable[i], rules.macro_value[i]);
    }
    #endif


    return rules;
}




#endif

