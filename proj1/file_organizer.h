#ifndef __file_organizer_h
#define __file_organizer_h

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>

#define DEBUG printf("HERE %d\n", __LINE__);fflush(stdout);

const char* ssh = "ssh -q";
const char* error_arguments = "Usage: a.out expects 1 argument, a config file of commands";
const int str_length = 256;

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

