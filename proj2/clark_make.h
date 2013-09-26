#ifndef __clark_make_h__
#define __clark_make_h__

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define kNumRules 16
#define kStringLength 256
#define kMaxRecursiveDepth 32

#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);
//#define DBGING 1

// Struct holding makefile rules.
// Each char char used like a key:value pair.
// Macros are arrays of strings, rules arrays of string arrays, 
// since one key maps to a list of instructions.
// Rules must also know how many commands are in each rule
typedef struct {
    int numMacros;
    char macroKeys[kNumRules][kStringLength];    
    char macroValues[kNumRules][kStringLength];    

    int numRules;
    char keys[kNumRules][kStringLength];    
    char values[kNumRules][kNumRules][kStringLength];    
    int numCommandsInKey[kNumRules];

} rulesStruct;

// Make a rulesStruct and init properly
rulesStruct rulesFactory(rulesStruct rules) {
    // Should start with 0 macros/rules
    rules.numMacros = 0;
    rules.numRules = 0;

    // Each rule should start with 0 commands
    memset(rules.numCommandsInKey, 0, kNumRules);

    return rules;
}

// Parse macro, target, and inference rules into rulesStruct
rulesStruct parseMakefile(FILE* makefile, rulesStruct rules)
{
    rules.numMacros = 0;
    char str[kStringLength];
    while (fgets(str, kStringLength, makefile) ) {

        int ret;
        switch(str[0]) {
            // Blank line
            case '\n':
            case ' ':
                break;

            // Comment line
            case '#':
                break;

            // Rule or Macro
            default:

                // No '.' but has ':', then  rule
                if (strchr(str, ':') != NULL) {
                    // Get target or inference rule
                    strcpy(rules.keys[rules.numRules], str);

                    // Now get rules
                    char c = getc(makefile);
                    while (c == '\t') {
                        fgets(rules.values[rules.numRules] 
                                [rules.numCommandsInKey[rules.numRules]],
                                kStringLength, 
                                makefile);
                        rules.numCommandsInKey[rules.numRules]++;
                        //fgets(str, kStringLength, makefile);
                        //strcpy(rules.Values[rules.numCommandsInKey[rules.numRules]], str);
                        c = getc(makefile);
                    }

                    // Put last char back, it isn't tab so it's the start of the next rule
                    ungetc(c, makefile);

                    // Add rule
                    rules.numRules++;
                } // End if

                // Has '=', then macro
                else if (strchr(str, '=') != NULL) {
                    ret = sscanf(str, "%[^=]%*c%s\n", 
                            rules.macroKeys[rules.numMacros], 
                            rules.macroValues[rules.numMacros]);

                    // Find 2 strings? Macro syntax correct, add the rule
                    if (ret == 2) {
                        rules.numMacros++;
                    }   
                }

        }

    }

#ifdef DBGING
    printf("Macros:\n");
    for (int i = 0; i < rules.numMacros; i++) {
        printf("Macro %d: %s = %s\n", i, rules.macroKeys[i], rules.macroValues[i]);
    }

    printf(" Rules:\n");
    for (int i = 0; i < rules.numRules; i++) {
        printf(" %d: %s", i, rules.keys[i]);

        for (int j = 0; j < rules.numCommandsInKey[i]; j++) {
            printf("\t%s", rules.values[i][j]);
        }
    }

#endif


    return rules;

} // end parseMakefileRules

// Recursively resolve key:value pairs of macros
char* resolveMacro(rulesStruct rules, char* key, int recursiveDepth)
{
    // likely we have a loop
    if (recursiveDepth == kMaxRecursiveDepth) {
        printf("Probably encountered a circular macro definition\n");
        return (char*)0;
    }

    for (int i = 0; i < rules.numMacros; i++) {
        if (strcmp(rules.macroKeys[i], key) == 0) {
            return resolveMacro(rules, rules.macroValues[i], recursiveDepth + 1);
        }
    }

    return key;

} // end resolveMacro

/*
// check to see if [cmd] is a  rule
void findTarget(rulesStruct rules, char* inputRule)
{
    for (int i = 0; i < rules.numRules; i++) {
        char x = strtok(rules.keys[i], ":");
        printf("%s\n", x);

        // Make sure it's a target rule, not inference
        if (strchr(rules.keys[i], '.') != NULL) {
            continue;
        }

        // Now make sure it's the rule we're looking for
        if (strcmp(rules.keys[i], inputRule) != 0) {
            continue;
        }

        // Strip trailing newline
        //strtok(str, "\n");
            
        for (int j = 0; j < rules.numCommandsInKey[i]; j++) {

            // Re-prime for rest of user input
            //char* token = strtok(str, " ");
            //while (token != NULL) {
            
                printf("Token %d: %s", j, rules.values[i][j]);
            //}
        }

    } // end for loop over rules


} // End findTarget()
*/



#endif


