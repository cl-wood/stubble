/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2013 Intel Corporation. All rights reserved.
 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */

#include "pin.H"
#include <asm/unistd.h>
#include <fstream>
#include <functional> // for hashes
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

bool IN_MAIN = false;

#if defined(TARGET_MAC)
#define MALLOC "_malloc"
//#define FREE "_free"
#define MAIN "_main"
#define LIBCSTART "__libc_start_main"
#define GETCHAR "_getchar"
#define STRCPY "_strcpy"
#else
#define MALLOC "malloc"
//#define FREE "free"
#define MAIN "main"
#define LIBCSTART "__libc_start_main"
#define GETCHAR "getchar"
#define STRCPY "strcpy"
#endif

/* ===================================================================== */
/* User Libraries */
/* ===================================================================== */
//#include "FollowExecution.h"
//#include "DTA.h"
//#include "taint_example_3.h"
#include "FindUseAfterFree.h"
//#include "FindVulnerability.h"


/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "malloctrace.out", "specify trace file name");

/* ===================================================================== */



/* ===================================================================== */
/* Instrumentation routines                                              */
/* ===================================================================== */


// Toggle logging on or off
VOID ToggleLogging(bool status)
{
    IN_MAIN = status; 
    // Global, can be turned off to speed up execution
    //EnableInstrumentation = status;

}

// Instrument main function, this starts the other instrumentation objects.
VOID WatchMain(IMG img, VOID *v)
{
    // TODO might this work better?
    //cout << IMG_IsMainExecutable(img) << endl;

    RTN mainRtn = RTN_FindByName(img, LIBCSTART); 
    if (RTN_Valid(mainRtn))
    {
        RTN_Open(mainRtn);

        // Instrument main() to print the input argument value and the return value.
        RTN_InsertCall(mainRtn, IPOINT_BEFORE, (AFUNPTR)ToggleLogging,
                        IARG_BOOL, true,
                        IARG_END);
        RTN_InsertCall(mainRtn, IPOINT_AFTER, (AFUNPTR)ToggleLogging,
                        IARG_BOOL, false,
                        IARG_END);

        RTN_Close(mainRtn);
    }

}
   

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    //FiniFollowExecution();
    FiniFindUseAfterFree();
    //FiniDTA();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */
   
INT32 Usage()
{
    cerr << "This tool produces a trace of calls to malloc." << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    // Initialize pin & symbol manager
    PIN_InitSymbols();
    PIN_SetSyntaxIntel();

    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    // Init functions for modules
    //InitFollowExecution();
    InitFindUseAfterFree();
    //InitDTA();

//TaintFile.open("taint.out");
//TaintFile << hex;
//TaintFile.setf(ios::showbase);

    // Watch main function
    //IMG_AddInstrumentFunction(WatchMain, 0);
    INS_AddInstrumentFunction(Instruction, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
