#ifndef __file_organizer_h
#define __file_organizer_h

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>

#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);

const char* k_ssh = "ssh -q";
const char* k_error_arguments = "Usage: a.out expects 1 argument, a config file of commands";
const int k_str_length = 256;

// Commands
const char* k_quit = "quit";
const char* k_ls = "ls";
const char* k_cat = "cat";
const char* k_cd = "cd";
const char* k_mkdir = "mkdir";
const char* k_cp = "cp";

// String compare function
int compare(const char* s, const char* input)
{
    return strncmp(input, s, strlen(s)) == 0 && 
           strlen(s) == strlen(input);
}

//cp
//cat
//mkdir

void cd() 
{
    printf("stub!\n");    
}

//ls


//ls dir

#endif

