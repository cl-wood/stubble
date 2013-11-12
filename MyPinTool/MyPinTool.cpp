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
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
using namespace std;

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

bool IN_MAIN = false;

/* ===================================================================== */
/* User Libraries */
/* ===================================================================== */
#include "FollowExecution.h"
//#include "DTA.h"
//#include "FindUseAfterFree.h"


/* ===================================================================== */
/* Instrumentation routines                                              */
/* ===================================================================== */

// Instrument main function, this starts the other instrumentation objects.
/*
VOID WatchMain(IMG img, VOID *v)
{
    // TODO might this work better?
    for (IMG img = APP_ImgHead(); IMG_Valid(img); img = IMG_Next(img)) {
        if (IMG_IsMainExecutable(img)) {
            IMG_AddInstrumentFunction(LogMain, 0);
        }
    }

}
*/


/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    FiniFollowExecution();
    //FiniFindUseAfterFree();
    //FiniDTA();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool DTA and backsolving to explore paths within a binary." << endl;
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

    if (PIN_Init(argc,argv) )
    {
        return Usage();
    }

    TaintFile.open("taint.out");
    TaintFile << hex;
    TaintFile.setf(ios::showbase);

    // Init functions for modules
    InitFollowExecution();
    //InitFindUseAfterFree();
    //InitDTA();

    // Watch main function
    //IMG_AddInstrumentFunction(WatchMain, 0);
    //INS_AddInstrumentFunction(Instruction, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */

