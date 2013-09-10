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

// Holds machine names and file root paths
typedef struct  {
    char machine[64];    
    char root[64];    
} absolute_path;

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

// cp machine1:path1 machine2:path2 
// ssh -q machine1 scp -q root1/path1 username@machine2:root2/path2
//void cp(char* str, struct absolute_path paths)
//{
//}

//cat
//mkdir

void cd() 
{
    printf("stub!\n");    
}

// ssh -q linprog1 ls /root1/path1
void ls(char* dir, absolute_path paths)
{

//k_ssh + machine + k_ls + path

}


#endif

