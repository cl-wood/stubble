#ifndef __clark_make_h__
#define __clark_make_h__

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define kNumRules 16
#define kStringLength 256
#define maxRecursiveDepth 32

#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);
#define DBGING 1

// Struct holding makefile rules.
// Each char char used like a key:value pair.
// Macros are arrays of strings, rules arrays of string arrays, 
// since one key maps to a list of instructions.
// Rules must also know how many commands are in each rule
typedef struct {
    int numMacros;
    char macroKeys[kNumRules][kStringLength];    
    char macroValues[kNumRules][kStringLength];    

    int numTargetRules;
    char targetKeys[kNumRules][kStringLength];    
    int numCommandsInTargetKey[kNumRules];
    char targetValues[kNumRules][kNumRules][kStringLength];    

    int numInferenceRules;
    char inferenceKeys[kNumRules][kStringLength];    
    int numCommandsInInferenceKey[kNumRules];
    char inferenceValues[kNumRules][kNumRules][kStringLength];    


} rulesStruct;

// Make a rulesStruct and init properly
rulesStruct rulesFactory(rulesStruct rules) {
    // Should start with 0 macros/rules
    rules.numMacros = 0;
    rules.numTargetRules = 0;
    rules.numInferenceRules = 0;

    // Each rule should start with 0 commands
    memset(rules.numCommandsInTargetKey, 0, kNumRules);
    memset(rules.numCommandsInInferenceKey, 0, kNumRules);

    return rules;
}

// Generic function to get Target or Inference rules
/*
void getRules(FILE* makefile, 
              char* str[kStringLength],
              int* numRules, 
              char* keys[kNumRules][kStringLength],
              int* numCommandsInKey[kNumRules],
              char* values[kNumRules][kNumRules][kStringLength])
{
    // Get target0,1,2... : prereq0,1,2,...
    strcpy(keys[(*numRules)], str);

    // Now get rules
    char c = getc(makefile);
    while (c == '\t') {
        fgets((*values)[(*numRules)] 
                [(*numCommandsInKey)[(*numRules)]],
                kStringLength, 
                makefile);
        (*numCommandsInKey)[(*numRules)]++;
        //fgets(str, kStringLength, makefile);
        //strcpy(rules.targetValues[rules.numCommandsInTargetKey[rules.numTargetRules]], str);
        c = getc(makefile);
    }
    // Put last char back, it isn't tab so it's the start of the next rule
    ungetc(c, makefile);

    // Add rule
    (*numRules)++;

} // end getRules
*/


// 
rulesStruct parseMakefile(FILE* makefile, rulesStruct rules)
{
    rules.numMacros = 0;
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
                // TODO factor this and target case into getRules()
                // Get target0,1,2... : prereq0,1,2,...
                strcpy(rules.inferenceKeys[rules.numInferenceRules], str);

                // Now get rules
                char c = getc(makefile);
                while (c == '\t') {
                    fgets(rules.inferenceValues[rules.numInferenceRules] 
                            [rules.numCommandsInInferenceKey[rules.numInferenceRules]],
                            kStringLength, 
                            makefile);
                    rules.numCommandsInInferenceKey[rules.numInferenceRules]++;
                    //fgets(str, kStringLength, makefile);
                    //strcpy(rules.inferenceValues[rules.numCommandsInInferenceKey[rules.numInferenceRules]], str);
                    c = getc(makefile);
                }

                // Put last char back, it isn't tab so it's the start of the next rule
                ungetc(c, makefile);

                // Add rule
                rules.numInferenceRules++;

                break;

            // Target rule or Macro
            default:

                // No '.' but has ':', then target rule
                if (strchr(str, ':') != NULL) {
                    // Get target0,1,2... : prereq0,1,2,...
                    strcpy(rules.targetKeys[rules.numTargetRules], str);

                    // Now get rules
                    char c = getc(makefile);
                    while (c == '\t') {
                        fgets(rules.targetValues[rules.numTargetRules] 
                                [rules.numCommandsInTargetKey[rules.numTargetRules]],
                                kStringLength, 
                                makefile);
                        rules.numCommandsInTargetKey[rules.numTargetRules]++;
                        //fgets(str, kStringLength, makefile);
                        //strcpy(rules.targetValues[rules.numCommandsInTargetKey[rules.numTargetRules]], str);
                        c = getc(makefile);
                    }

                    // Put last char back, it isn't tab so it's the start of the next rule
                    ungetc(c, makefile);

                    // Add rule
                    rules.numTargetRules++;
                }

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

    printf("Target Rules:\n");
    for (int i = 0; i < rules.numTargetRules; i++) {
        printf("Target %d: %s", i, rules.targetKeys[i]);

        for (int j = 0; j < rules.numCommandsInTargetKey[i]; j++) {
            printf("\t%s", rules.targetValues[i][j]);
        }
    }

    printf("Inference Rules:\n");
    for (int i = 0; i < rules.numInferenceRules; i++) {
        printf("Inference %d: %s", i, rules.inferenceKeys[i]);

        for (int j = 0; j < rules.numCommandsInInferenceKey[i]; j++) {
            printf("\t%s", rules.inferenceValues[i][j]);
        }
    }
#endif


    return rules;

} // end parseMakefileRules

// Recursively resolve key:value pairs of macros
char* resolveMacro(rulesStruct rules, char* key, int recursiveDepth)
{
    // likely we have a loop
    if (recursiveDepth == maxRecursiveDepth) {
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



#endif

