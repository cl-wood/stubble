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
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <unordered_map>
using namespace std;

/* ===================================================================== */
/* Names of malloc and free */
/* ===================================================================== */
#if defined(TARGET_MAC)
#define MALLOC "_malloc"
#define FREE "_free"
#else
#define MALLOC "malloc"
#define FREE "free"
#endif

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

std::ofstream TraceFile;
std::ofstream ControlFlowFile;

// For Mutation 
typedef struct 
{
    vector<UINT32> addresses;
    vector<string> instructions;
} ControlFlowStruct;

ControlFlowStruct ControlFlow;

// For Vulnerability Analysis
unordered_map<ADDRINT, UINT32> MemoryAllocations;

/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "malloctrace.out", "specify trace file name");

/* ===================================================================== */


/* ===================================================================== */
/* Vulnerability Analysis routines                                       */
/* ===================================================================== */
 
// Gets value of Arg1 to malloc call
VOID Arg1Before(CHAR * name, ADDRINT size)
{
    TraceFile << name << "(" << size << ")" << endl;
}

VOID MallocAfter(ADDRINT ret)
{
    TraceFile << "  returns " << ret << endl;
}

// Update map of memory address => size of allocated memory pairs.
// Remove entries when memory freed
VOID MallocInfo(CHAR* name, ADDRINT size, ADDRINT allocationAddress)
{
    // 
    MemoryAllocations[allocationAddress] = size;
    TraceFile << name << "(" << size << ") bytes at " << allocationAddress << endl;
}

VOID FreeInfo(CHAR * name, ADDRINT allocationAddress)
{
    // Memory Freed, set size to 0 to signify this
    MemoryAllocations[allocationAddress] = 0;
    TraceFile << name << "(" << allocationAddress << ")" << endl;
}

/* ===================================================================== */
/* Instrumentation routines                                              */
/* ===================================================================== */

// For Mutation
// At each branch/jump, record the memory address and disassembled instruction.
// Comparisons based on input => ControlFlow mapping will drive mutation.
VOID FindBranchOrJump(INS ins, VOID *v)
{
	// Categories are higher-level semantic descriptions than opcodes.
	// The constant matches all conditional branches
	if (INS_Category(ins) == XED_CATEGORY_COND_BR) {
        
        ControlFlow.addresses.push_back(INS_Address(ins));
        ControlFlow.instructions.push_back(INS_Disassemble(ins));

		ControlFlowFile << INS_Disassemble(ins) << "\t" << INS_Address(ins) << endl;

	} 

}
   
VOID WatchMemoryAllocation(IMG img, VOID *v)
{
    // Instrument the malloc() and free() functions.  Print the input argument
    // of each malloc() or free(), and the return value of malloc().
    //
    //  Find the malloc() function.
    RTN mallocRtn = RTN_FindByName(img, MALLOC);
    if (RTN_Valid(mallocRtn))
    {
        RTN_Open(mallocRtn);

        // Instrument malloc() to print the input argument value and the return value.
        RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)MallocInfo,
                       IARG_ADDRINT, MALLOC,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_FUNCRET_EXITPOINT_VALUE,
                       IARG_END);
        /*
        RTN_InsertCall(mallocRtn, IPOINT_BEFORE, (AFUNPTR)Arg1Before,
                       IARG_ADDRINT, MALLOC,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)MallocAfter,
                       IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);
        */

        RTN_Close(mallocRtn);
    }

    // Find the free() function.
    RTN freeRtn = RTN_FindByName(img, FREE);
    if (RTN_Valid(freeRtn))
    {
        RTN_Open(freeRtn);
        // Instrument free() to print the input argument value.
        //RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)Arg1Before,
        RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)FreeInfo,
                       IARG_ADDRINT, FREE,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_Close(freeRtn);
    }
}

/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    TraceFile.close();
    ControlFlowFile.close();
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
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    // Write to a file since cout and cerr maybe closed by the application
    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << hex;
    TraceFile.setf(ios::showbase);
    ControlFlowFile.open("controlpath.out");
    ControlFlowFile << hex;
    ControlFlowFile.setf(ios::showbase);
    
    // WatchMemoryAllocation to see how malloc/free used
    IMG_AddInstrumentFunction(WatchMemoryAllocation, 0);

    // For Mutation:
    // At every instruction, print if branch or jump
    INS_AddInstrumentFunction(FindBranchOrJump, 0);
    

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
