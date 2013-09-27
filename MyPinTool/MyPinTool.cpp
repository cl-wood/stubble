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
#define MAIN "_main"
#else
#define MALLOC "malloc"
#define FREE "free"
#define MAIN "main"
#endif

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

bool LOGGING = false;

std::ofstream TraceFile;
std::ofstream TaintFile;
std::ofstream ControlFlowFile;

// For Mutation 
typedef struct 
{
    vector<UINT32> addresses;
    vector<string> instructions;
} ControlFlowStruct;

ControlFlowStruct ControlFlow;

// For Vulnerability Analysis
//unordered_map<ADDRINT, UINT32> MemoryAllocations;
unordered_map<ADDRINT, UINT32> TaintMap;

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
    if (LOGGING) {
        TraceFile << name << "(" << size << ")" << endl;
    }
}

VOID MallocAfter(ADDRINT ret)
{
    if (LOGGING) {
        TraceFile << "  returns " << ret << endl;
    }
}

// Update map of memory address => size of allocated memory pairs.
// Remove entries when memory freed
/*
VOID MallocInfo(CHAR* name, ADDRINT size, ADDRINT allocationAddress)
{
    // 
    if (LOGGING) {
        MemoryAllocations[allocationAddress] = size;
        TraceFile << name << "(" << size << ") bytes at " << allocationAddress << endl;
    }
}

VOID FreeInfo(CHAR * name, ADDRINT allocationAddress)
{
    // Memory Freed, set size to 0 to signify this
    if (LOGGING) {
        MemoryAllocations[allocationAddress] = 0;
        TraceFile << name << "(" << allocationAddress << ")" << endl;
    }
}
*/

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
	if (INS_Category(ins) == XED_CATEGORY_COND_BR && LOGGING) {
        
        ControlFlow.addresses.push_back(INS_Address(ins));
        ControlFlow.instructions.push_back(INS_Disassemble(ins));

		ControlFlowFile << INS_Disassemble(ins) << "\t" << INS_Address(ins) << endl;

	} 

}

// Follow each instruction in each bbl of each trace.
// basic block is single entrace, single exit block of sequential instructions.
// trace is single entrace, multiple exits, and generated at entrace during runtime.
VOID FollowTraces(TRACE trace, VOID *v)
{
    ControlFlowFile << "START: " << TRACE_Address(trace) << endl;
    // For each basic block, track each instruction
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins) ) {
		    ControlFlowFile << INS_Disassemble(ins) << "\t"; //<< INS_Address(ins) << endl;

            // Get routine name if it's a call instruction
            if (INS_IsDirectCall(ins) && INS_IsDirectBranchOrCall(ins) ) {
                ADDRINT addrint = INS_DirectBranchOrCallTargetAddress(ins);
                ControlFlowFile << RTN_FindNameByAddress(addrint);
            }

            ControlFlowFile << endl;

        }
    }
} // End FollowTraces

// Toggle logging on or off
VOID ToggleLogging(bool status)
{
    LOGGING = status; 
    // Global, can be turned off to speed up execution
    //EnableInstrumentation = status;

}

// Instrument main function, this starts the other instrumentation objects.
VOID WatchMain(IMG img, VOID *v)
{
    //LOGGING = IMG_IsMainExecutable(img);

    RTN mainRtn = RTN_FindByName(img, MAIN);
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

        /*
        // Instrument malloc() to print the input argument value and the return value.
        RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)MallocInfo,
                       IARG_ADDRINT, MALLOC,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_FUNCRET_EXITPOINT_VALUE,
                       IARG_END);
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
        /*
        RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)FreeInfo,
                       IARG_ADDRINT, FREE,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        */
        RTN_Close(freeRtn);
    }
}

// Catch all read syscalls since they are taint sources.
VOID CatchReadSyscalls(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, VOID *v)
{
    if (PIN_GetSyscallNumber(ctxt, std) == __NR_read) {

        //ssize_t read(int fd, void *buf, size_t count);
        ADDRINT buf   = ((PIN_GetSyscallArgument(ctxt, std, 1)));
        ADDRINT count = ((PIN_GetSyscallArgument(ctxt, std, 2)));

        for (int i = 0; i < (int)count; i++) {
            // TODO use API instead of sizeof char to get bytes?
            pair<ADDRINT, UINT32> taintLocation(buf + i * sizeof(char), 0);
            TaintMap.insert(taintLocation); 
            TaintFile << get<0>(taintLocation) << "\t" << count << endl;
        }
    }

}


/* ===================================================================== */

VOID Fini(INT32 code, VOID *v)
{
    TraceFile.close();
    TaintFile.close();
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
    TaintFile.open("taint.out");
    TaintFile << hex;
    TaintFile.setf(ios::showbase);
    ControlFlowFile.open("controlpath.out");
    ControlFlowFile << hex;
    ControlFlowFile.setf(ios::showbase);
    
    // WatchMemoryAllocation to see how malloc/free used
    //IMG_AddInstrumentFunction(WatchMemoryAllocation, 0);

    // Watch main function
    //IMG_AddInstrumentFunction(WatchMain, 0);

    // For Mutation:
    // At every instruction, print if branch or jump
    //INS_AddInstrumentFunction(FindBranchOrJump, 0);

    // Perform work at BBL or Trace level
    TRACE_AddInstrumentFunction(FollowTraces, 0);

    // Instrument read syscall to get taint sources
    PIN_AddSyscallEntryFunction(CatchReadSyscalls, 0);
    

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
