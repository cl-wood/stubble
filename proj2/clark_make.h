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

// Struct representing a macro. 
// Has:
//     key=value  
// 
typedef struct {
    char key[kStringLength];    
    char value[kStringLength];    
} macroStruct;

// Struct representing one target rule
// Members:
//     target   - an array of strings
//     prereqs  - an array of strings
//     commands - an array of string arrays
// members are treated like the PATH environ,
// in that they are NULL terminated
typedef struct {

    // Name of rule, to the left of the ':'
    char target[kNumRules][kStringLength];    

    // Prereqs, to the right of the ':'
    char prereqs[kNumRules][kStringLength];    

    // Commands, after the rule, start with '\t'
    char commands[kNumRules][kStringLength];    

} targetRuleStruct;

// Like targetRuleStruct, but no prereqs
// TODO make sure inference rules don't need prereqs
typedef struct {

    // Name of rule, to the left of the ':'
    char target[kStringLength];    

    // Commands, after the rule, start with '\t'
    char commands[kNumRules][kStringLength];    

} inferenceRuleStruct;

typedef struct {
    // 
    macroStruct macros[kNumRules];
    targetRuleStruct targets[kNumRules];
    inferenceRuleStruct inferences[kNumRules];

} makefileStruct;

// Make a makefile and init properly
makefileStruct makefileFactory(makefileStruct rules) {
    // Should start with 0 macros/rules
    rules.macros[0].key[0] = '\0';

    //rules.targets = 0;
    //rules.inferences = 0;

    return rules;
}

// Parse macro, target, and inference rules into makefile
makefileStruct parseMakefile(FILE* makefile, makefileStruct rules)
{
    // Start with no macros or rules
    int numMacros = 0;
    int numTargets = 0;
    int numInferences = 0;
    int numCommands = 0;

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

                // Inference rule
            case '.':
                // Add target, can be .s or .s.t
                // TODO parse of .s and .s.t cases, modify inference struct accordingly
                strcpy(rules.inferences[numInferences].target, str);

                // Now add commands 
                char c = getc(makefile);
                numCommands = 0;
                while (c == '\t') {
                    fgets(rules.inferences[numInferences].commands[numCommands],
                            kStringLength, 
                            makefile);
                    c = getc(makefile);
                    numCommands++;
                }

                // Null terminate last command in each inference rule
                rules.inferences[numInferences].commands[numCommands][0] = '\0';

                // Put last char back, it isn't tab so it's the start of the next rule
                ungetc(c, makefile);

                // Add rule
                numInferences++;

                break; // End inference rule case

            // Target rule or Macro, parse and add to rules
            default:

                // No '.' but has ':', then add target rule
                // TODO can target rules have multiple targets? modify parsing/struct
                if (strchr(str, ':') != NULL) {
                    strcpy(rules.targets[numTargets].target, str);

                    // Now add commands
                    char c = getc(makefile);
                    numCommands = 0;
                    while (c == '\t') {
                        fgets(rules.targets[numTargets].commands[numCommands],
                                kStringLength, 
                                makefile);
                        c = getc(makefile);
                        numTargets++;
                    }
                    // Null terminate last command in each target rule
                    rules.targets[numTargets].commands[numCommands][0] = '\0';

                    // Put last char back, it isn't tab so it's the start of the next rule
                    ungetc(c, makefile);

                    // Add rule
                    numTargets++;
                } // End if for target rules

                // Has '=', then macro
                else if (strchr(str, '=') != NULL) {
                    // scan key=value and add macro if correct
                    ret = sscanf(str, "%[^=]%*c%s\n", 
                            rules.macros[numMacros].key, 
                            rules.macros[numMacros].value);

                    // Find 2 strings? Macro syntax correct, add the macro
                    if (ret == 2) {
                        numMacros++;
                    }   
                } // End else if for macros

        } // End switch

    } // End while

    // Debugging 
#ifdef DBGING
    printf("Macros:\n");
    int i = 0;
    while (rules.macros[i].key[0] != '\0') {
        printf("Macro %d: %s = %s\n", i, rules.macros[i].key, rules.macros[i].value);
    }
    //for (int i = 0; i < rules.numMacros; i++) {
    //    printf("Macro %d: %s = %s\n", i, rules.macroKeys[i], rules.macroValues[i]);
    //}

/*
    printf(" Rules:\n");
    for (int i = 0; i < rules.numRules; i++) {
        printf(" %d: %s", i, rules.keys[i]);

        for (int j = 0; j < rules.numCommandsInKey[i]; j++) {
            printf("\t%s", rules.values[i][j]);
        }
    }
    */

#endif

    return rules;

} // end parseMakefileRules

// Recursively resolve key:value pairs of macros
// TODO fix this for new structs
char* resolveMacro(makefileStruct rules, char* key, int recursiveDepth)
{
    // likely we have a loop
    if (recursiveDepth == kMaxRecursiveDepth) {
        printf("Probably encountered a circular macro definition\n");
        return (char*)0;
    }

    //for (int i = 0; i < rules.numMacros; i++) {
    //    if (strcmp(rules.macroKeys[i], key) == 0) {
    //        return resolveMacro(rules, rules.macroValues[i], recursiveDepth + 1);
    //    }
    //}

    return key;

} // end resolveMacro

/*
// check to see if [cmd] is a  rule
void findTarget(makefile rules, char* inputRule)
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


