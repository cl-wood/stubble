#ifndef __clark_make_h__
#define __clark_make_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static const char* MYMAKEPATH = "/home/grads/wood/.bin:/home/grads/wood/.scripts:/usr/local/bin:/usr/local/java/bin:/usr/lang:/bin:/usr/bin:/usr/ucb:/usr/etc:/usr/local/bin/X11:/usr/bin/X11:/usr/openwin/bin:/usr/ccs/bin:/usr/sbin:/opt/sfw/bin:.";

#define kNumRules 16
#define kStringLength 256
#define kMaxRecursiveDepth 32

#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);
#define DBGING 1

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

    // TODO allow multiple targets
    // Name of rule, to the left of the ':'
    char targets[kNumRules][kStringLength];    

    // Prereqs, to the right of the ':'
    char prereqs[kNumRules][kStringLength];    

    // Commands, after the rule, start with '\t'
    char commands[kNumRules][kStringLength];    

} targetRuleStruct;

// Like targetRuleStruct, but no prereqs
// TODO make sure inference rules don't need prereqs
typedef struct {

    // Name of rule, to the left of the ':'
    char targets[2][kStringLength];    

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
    for (int i = 0; i < kNumRules; i++) {
        rules.macros[i].key[0] = '\0';
        rules.targets[i].targets[0][0] = '\0';
        //rules.targets[i].prereqs[0][0] = '\0';
        rules.inferences[i].targets[0][0] = '\0';
        rules.inferences[i].targets[1][0] = '\0';
    }

    return rules;
}

// Parse macro, target, and inference rules into makefile
makefileStruct parseMakefile(FILE* makefile, makefileStruct rules)
{
    // Start with no macros or rules
    int numMacros = 0;
    int numTargets = 0;
    int numInferences = 0;

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
                //strcpy(rules.inferences[numInferences].targets, str);
                sscanf(str, ".%[^:.\n].%[^:.\n]:", rules.inferences[numInferences].targets[0],
                                       rules.inferences[numInferences].targets[1]);

                // Now add commands 
                char c = getc(makefile);
                int i = 0;
                while (c == '\t') {
                    fgets(rules.inferences[numInferences].commands[i],
                            kStringLength, 
                            makefile);
                        strtok(rules.inferences[numInferences].commands[i], "\n");
                    c = getc(makefile);
                    i++;
                }

                // Null terminate last command in each inference rule
                rules.inferences[numInferences].commands[i][0] = '\0';

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
                    // strtok it
                    char before[kStringLength];
                    char after[kStringLength];
                    sscanf(str, "%[^\t\n:]:%[^\t\n]", before, after);

                    // Put before into targets, after into prereqs
                    char *token;
                    int i = 0;
                    token = strtok(before, " ");
                    while (token != NULL)
                    {
                        strcpy(rules.targets[numTargets].targets[i], token);
                        i++;
                        token = strtok (NULL, " ");
                    }
                    rules.targets[numTargets].targets[i][0] = '\0';

                    i = 0;
                    token = strtok(after, " ");
                    while (token != NULL)
                    {
                        strcpy(rules.targets[numTargets].prereqs[i], token);
                        i++;
                        token = strtok (NULL, " ");
                    }
                    rules.targets[numTargets].prereqs[i][0] = '\0';

                    // Now add commands
                    char c = getc(makefile);
                    i = 0;
                    while (c == '\t') {
                        fgets(rules.targets[numTargets].commands[i],
                                kStringLength, 
                                makefile);
                        strtok(rules.targets[numTargets].commands[i], "\n");
                        c = getc(makefile);
                        i++;
                    }
                    // Null terminate last command in each target rule
                    rules.targets[numTargets].commands[i][0] = '\0';

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

    // Null delimit macros and rules
    rules.targets[numTargets].targets[0][0] = '\0'; 
    rules.inferences[numInferences].targets[0][0] = '\0'; 

    // Debugging 
#ifdef DBGING
    printf("Macros:\n");
    int i = 0;
    while(rules.macros[i].key[0] != '\0') {
        printf("Macro %d: %s = %s\n", i, rules.macros[i].key, rules.macros[i].value);
        i++;
    }

    printf("Target Rules:\n");
    i = 0;
    while(rules.targets[i].targets[0][0] != '\0') {
        int j = 0;
        while(rules.targets[i].targets[j][0] != '\0') {
            printf("%s ", rules.targets[i].targets[j]);
            j++;
        }
        printf(": ");
        j = 0;
        while(rules.targets[i].prereqs[j][0] != '\0') {
            printf("%s ", rules.targets[i].prereqs[j]);
            j++;
        }
        printf("\n");
        j = 0;
        while(rules.targets[i].commands[j][0] != '\0') {
            printf("\t%s\n", rules.targets[i].commands[j]);
            j++;
        }

        i++;
    } // End target rules

    printf("Inference Rules:\n");
    i = 0;
    while(rules.inferences[i].targets[0][0] != '\0') {

        printf(".%s.%s:\n", rules.inferences[i].targets[0],
                            rules.inferences[i].targets[1]);

        int j = 0;
        while(rules.inferences[i].commands[j][0] != '\0') {
            printf("\t%s\n", rules.inferences[i].commands[j]);
            j++;
        }

        i++;
    } // End inference rules

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

// Parse commands and execute them, one at a time.
void execTarget(makefileStruct rules, char* target) {

    // TODO default case, make first target
    int pid, stat;

    // Identify target to execute commands of
    int i = 0;
    int j = 0;
    int found = 0;
    while (rules.targets[i].targets[j][0] != '\0') {

        // there are multiple targets possible per line
        while (rules.targets[i].targets[j][0] != '\0') {
            if (strcmp(target, rules.targets[i].targets[j]) == 0) {
                found = 1;
                break;
            }
            j++;
        }
        j = 0;
        if (found) {
            break; // again
        }
        i++;
    }
    if (!found) {
        printf("I don't know that command...");
        return;
    }


    // check 

    // Execute each command 
    int k = 0;
    while (rules.targets[i].commands[k][0] != '\0') {

        // For each command, parse first by ';'
        char* command = strtok(rules.targets[i].commands[k], ";");
        while (command != NULL)
        {
            int args = 0;
            char tempv[16][32];

            char* token = strtok(command, " \t");
            while (token != NULL) {
                // TODO resolve path variable
                
                strcpy(tempv[args], token);
                args++;

                token = strtok(NULL, " \t");
            } // End while loop for tokens

            // Feed into argv
            char* argv[16];
            for (int i = 0; i < args; i++) {
                argv[i] = tempv[i];
                printf(tempv[i]);
            }
            argv[args] = (char*)NULL;
            
            // 
            if (fork() == 0) {
                if (execv(argv[0], argv)) {
                    printf("Command execution failed.\n");
                    exit(-1);
                } 
            }

            // Now wait for child
            pid = wait(&stat);
            

            // TODO check prereqs

            // cd, use chdir()

            // pipes 

            // &

            // < >



            command = strtok(NULL, ";");
        } // End while loop parsing ';' for commands

        k++;
    } // End while loop executing commands




}

#endif


