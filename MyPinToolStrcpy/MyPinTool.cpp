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
//#include "FollowExecution.h"
//#include "buffer_linux.h"
//#include "DTA.h"
//#include "FindUseAfterFree.h"


/* ===================================================================== */
/* Instrumentation routines                                              */
/* ===================================================================== */

// Instrument strcpy

CHAR_PTR (*fptrstrcpy)(CHAR_PTR __dest, const CHAR_PTR __src);

CHAR_PTR  mystrcpy(CHAR_PTR __dest, const CHAR_PTR __src) 
{
    OutFile << CurrentTime() << "mystrcpy called " << endl;
    OutFile.flush();
    CHAR_PTR  res = fptrstrcpy(__dest, __src);

    return res;
}

// Image load callback - inserts the probes.
void ImgLoad(IMG img, VOID_PTR v) {
    // Called every time a new image is loaded
    if ((IMG_Name(img).find("libc.so") != string::npos)
            || (IMG_Name(img).find("LIBC.SO") != string::npos)
            || (IMG_Name(img).find("LIBC.so") != string::npos)) 
    {

        RTN rtnstrcpy = RTN_FindByName(img, "strcpy");
        if (RTN_Valid(rtnstrcpy) && RTN_IsSafeForProbedReplacement(rtnstrcpy))
        {
            OutFile << CurrentTime() << "Inserting probe for strcpy at " << RTN_Address(rtnstrcpy) << endl;
            OutFile.flush();
            AFUNPTR fptr = (RTN_ReplaceProbed(rtnstrcpy, AFUNPTR(mystrcpy)));
            fptrstrcpy = (CHAR_PTR  (*)(CHAR_PTR , const CHAR_PTR ))fptr;
        }

    }

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    //FiniFollowExecution();
    //FiniFindUseAfterFree();
    //FiniDTA();
    //FiniBufferLinux();
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

    IMG_AddInstrumentFunction(ImgLoad, 0);

    // Init functions for modules
    //InitFollowExecution();
    //InitFindUseAfterFree();
    //InitDTA();
    //InitBufferLinux();

    // Watch main function
    //IMG_AddInstrumentFunction(WatchMain, 0);
    //INS_AddInstrumentFunction(Instruction, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    //PIN_StartProgram();
    PIN_StartProgramProbed(); // never returns

    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */

