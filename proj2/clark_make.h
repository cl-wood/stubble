#ifndef __clark_make_h__
#define __clark_make_h__

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const int kStringLength = 256;
const int kNumRules = 16;

#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);

// struct holding makefile rules
typedef struct {
    // 16 64 char length rules
    int macro_rules;
    char macro_variable[16][256];    
    char macro_value[16][256];

} rulesStruct;

// Make a rulesStruct and init properly
rulesStruct rulesFactory(rulesStruct rules) {
    rules.macro_rules = 0;

    return rules;
}


// 
rulesStruct parseMakefileRules(FILE* makefile, rulesStruct rules)
{
    rules.macro_variable[0][0] = '3';

    return rules;
}




#endif

