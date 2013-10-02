#ifndef __clark_make_h__
#define __clark_make_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

static const char* MYMAKEPATH = "/home/grads/wood/.bin:/home/grads/wood/.scripts:/usr/local/bin:/usr/local/java/bin:/usr/lang:/bin:/usr/bin:/usr/ucb:/usr/etc:/usr/local/bin/X11:/usr/bin/X11:/usr/openwin/bin:/usr/ccs/bin:/usr/sbin:/opt/sfw/bin:.";

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

void handlePipes(char* command) {
    int maxPipes = 5;

    // Count number of pipes
    int numPipes = 0;
    int fds[maxPipes][2];
    //int child[maxPipes];
    char tempArgv[maxPipes][5][kStringLength];

    char* token = strtok(command, "|");
    while (token != NULL)
    {
        int args = sscanf(token, "%s %s %s %s", tempArgv[numPipes][0],
                                                tempArgv[numPipes][1],
                                                tempArgv[numPipes][2],
                                                tempArgv[numPipes][3]);
        tempArgv[numPipes][args][0] = '\0';
        numPipes++;
        token = strtok(NULL , "|");
    }
    numPipes--;

    // Convert char x[][][] to char* x[][]
    char* argvs[maxPipes][5];
    for (int i = 0; i < maxPipes; i++) {
        for (int j = 0; tempArgv[i][j][0] != '\0'; j++) {
            argvs[i][j] = tempArgv[i][j];
        }
    }


    // Create enough pipes
    for (int i = 0; i < numPipes; i++) {
        pipe(fds[i]);
    }

    // Last pipe always the same
    if (fork()== 0) {
        printf("end pipe: %d\n", numPipes - 1);
        close(0); dup(fds[numPipes - 1][0]); /* redirect standard input to fds[1] */
        close(fds[numPipes - 1][1]);
        for (int i = 0; i < numPipes; i++) {
            close(fds[i][0]);
            close(fds[i][1]);
        }
        execv(argvs[numPipes][0], argvs[numPipes]);
        exit(0);
    }

    // TODO how should this middle one work?
    // If intermediate pipes, count down
    for (int i = numPipes - 1; i > 0; i--) {
        if (fork()== 0) {
            printf("middle pipe out: %d\n", i);
            close(1); dup(fds[i][1]);
            printf("middle pipe in: %d\n", i - 1);
            close(0); dup(fds[i - 1][0]); 
            for (int i = 0; i < numPipes; i++) {
                close(fds[i][0]);
                close(fds[i][1]);
            }
            // having this makes it stop?
            execv(argvs[i][0], argvs[i]);
            exit(0);
        }
    }


    // First pipe always the same
    if (fork() == 0) {
        close(1); dup(fds[0][1]);  /* redirect standard output to fds[0] */
        for (int i = 0; i < numPipes; i++) {
            close(fds[i][0]);
            close(fds[i][1]);
        }
        execv(argvs[0][0], argvs[0]);
        exit(0);

    }

    for (int i = 0; i < numPipes; i++) {
        close(fds[i][0]);
        close(fds[i][1]);
    }

    // Wait for first and last, then middle children
    waitpid(-1, NULL, 0);
    waitpid(-1, NULL, 0);
    //waitpid(-1, NULL, 0);
    for (int i = 0; i < numPipes - 2; i++) {
        waitpid(-1, NULL, 0);
    }

} // End handlePipes

void handleMultiple(char* command)
{
    //TODO handle cd persistence
    char argv[5][64];

    char* token = strtok(command, ";");
    while (token != NULL)
    {
        int args = sscanf(token, "%s %s %s %s", argv[0],
                                                argv[1],
                                                argv[2],
                                                argv[3]);

        // Convert char x[][] to char* x[]
        char* temp[5];
        for (int i = 0; i < args; i++) {
            temp[i] = argv[i];
        }
        temp[args]= NULL;

        // Fork and wait, re-tokenize
        if (fork()== 0) {
            execv(temp[0], temp);
            exit(0);
        }

        waitpid(-1, NULL, 0);
        token = strtok(NULL , ";");
    }

} // End handleMultiple

// TODO handle more possible args before redirs
void handleRedirection(char* command)
{
    char argv[5][64];
    char inFile[kStringLength];
    char outFile[kStringLength];
    int fd0, fd1;
    int hasInputRedirection = 0;
    int hasOutputRedirection = 0;

    // Tokenize on whitespace, handling < and > appropriately
    char* token = strtok(command, " ");
    int argc = 0;
    while (token != NULL)
    {
        if (strchr(token, '<') == NULL && (strchr(token, '>') == NULL)) {
            strcpy(argv[argc], token);
            //printf("%s\n", token);
            argc++;
        }
        else { // has <, >, or both
            if (strchr(token, '<') != NULL) {
                hasInputRedirection = 1;
                token = strtok(NULL, " ");
            //printf("IN: %s\n", token);
                strcpy(inFile, token);
                token = strtok(NULL, " ");
            }
            if (strchr(token, '>') != NULL) {
                hasOutputRedirection = 1;
                token = strtok(NULL, " ");
            //printf("OUT: %s\n", token);
                strcpy(outFile, token);
            }
            break;
        }

        token = strtok(NULL, " ");
    } // End token while

    if (fork() == 0) {
       
        // Convert char x[][] to char* x[]
        char* temp[5];
        for (int i = 0; i < argc; i++) {
            temp[i] = argv[i];
        }
        temp[argc]= NULL;

        // Handle file I/O redirection
        if (hasInputRedirection) {
            fd0 = open(inFile, O_RDONLY);
            close(0);
            dup(fd0);
        }
        if (hasOutputRedirection) {
            fd1 = open(outFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
            close(1);
            dup(fd1);
        }

        execv(temp[0], temp);
        exit(0);

    } // End fork

    waitpid(-1, NULL, 0);

} // End handleRedirection

void handleBackground(char* command)
{
    char argv[5][64];

    int argc = sscanf(command, "%s %s %s %s", 
            argv[0],
            argv[1],
            argv[2],
            argv[3]);

    // Convert char x[][] to char* x[]
    char* temp[5];
    for (int i = 0; i < argc; i++) {
        if (strchr(argv[i], '&') == NULL) {
            temp[i] = argv[i];
        }
    }
    // Account for not including '&'
    temp[argc - 1]= NULL;

    // Fork and wait, re-tokenize
    if (fork()== 0) {
        execv(temp[0], temp);
        exit(0);
    }

    // Not waiting puts it in bg
    //waitpid(-1, NULL, 0);

} // End handleBackground

// Parse commands and execute them, one at a time.
void execTarget(makefileStruct rules, char* target) {

    // TODO default case, make first target
    //int pid, stat;
    char* pwd = getcwd(NULL, kStringLength);

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

        // Check what type of command it is
        // TODO add cd
        if (strchr(rules.targets[i].commands[k], '&') != NULL) {
            handleBackground(rules.targets[i].commands[k]);
        }
        else if (strchr(rules.targets[i].commands[k], '<') != NULL ||
                strchr(rules.targets[i].commands[k], '>') != NULL) {
            handleRedirection(rules.targets[i].commands[k]);
        }
        else if (strchr(rules.targets[i].commands[k], '|') != NULL) {
            handlePipes(rules.targets[i].commands[k]);
        }
        else if (strchr(rules.targets[i].commands[k], ';') != NULL) {
            handleMultiple(rules.targets[i].commands[k]);
        }
        else {
            printf("Regular\n");
            // TODO will this work? it should...
            handleMultiple(rules.targets[i].commands[k]);
        }

        // TODO check prereqs

        k++;
    } // End while loop executing commands



    free(pwd); // Remember to free!

}

#endif


