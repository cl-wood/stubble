#ifndef __STUBBLE_H__ 
#define __STUBBLE_H__ 

// Stubble.h
// http://en.wikibooks.org/wiki/X86_Assembly/Control_Flow and the book in general is very helpful

#include "syscalls.h"
#include "intercept_signals.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>
#include <tuple>
#include <unordered_map>
#include <vector>
//#include <xed-flag-enum.h>

/* Globals for Stubble */
string results_dir = "./results/currentrun/";
int UNTAINTED = -1;

// Tainted registers and memory
unordered_map<string, tuple<UINT32, string> > tainted_registers; // TODO make registers an enumerated array of tuple(byteLocation, fileName), if untainted byteLocation = -1
extern unordered_map<UINT32, tuple<UINT32, string> > tainted_memory;




// Array of registers to check for tainted writes
REG registers[] = { 
                    REG_EFLAGS,
                    REG_EDI, 
                    REG_ESI, 
                    REG_EBP, 
                    REG_ESP, 
                    REG_EDX, 
                    REG_ECX, 
                    REG_EBX, 
                    REG_EAX,  
                    REG_EIP,
                    REG_AL, 
                    REG_AH, 
                    REG_AX, 
                    REG_CL, 
                    REG_CH, 
                    REG_CX, 
                    REG_DL, 
                    REG_DH, 
                    REG_DX, 
                    REG_BL, 
                    REG_BH, 
                    REG_BX, 
                    REG_BP, 
                    REG_SI, 
                    REG_DI, 
                    REG_SP 
                   };

// File for recording taint propagation
ofstream CondBranchFile;
ofstream TaintRegsFile;
ofstream EflagsFile;
ofstream UnhandledFile;

// CATEGORIES and ICLASSes to consider
//XED_CATEGORY_CMOV all cond. move instructions

        /*
        // Check to see if carry/offset/parity/sign/zero flags changed
        if (tmp_flags & XED_FLAG_cf) {
            FlagsFile << insDis << ":\t" << PIN_GetContextReg(context, REG_EFLAGS) << endl;
        }*/

/*    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)ReadMem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandCount(ins),
                IARG_UINT32, INS_OperandReg(ins, 0),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);
        */
//Calls taintReg(reg) VOID ReadMem(UINT32 insAddr, std::string insDis, UINT32 opCount, REG reg_r, UINT32 memOp, UINT32 sp)

// Handles taint tracking for EFLAGS register's carry/overflow/parity/sign/zero flags
VOID taint_eflags()
{

}

// When tainted data is written to a register
//      1) Record in TaintRegsFile
//      2) Update tainted_registers data structure
VOID taint_register(ADDRINT ins, string insDis, string regName, string category, CONTEXT* context, REG reg) {
    
    TaintRegsFile << insDis << "\t[REG]\t" << regName << endl;

    // If EFLAGS tainted, figure out which flag
    // TODO does this have to be instruction independent?
    if (regName == "eflags") {
        EflagsFile << insDis << "\t[CATEGORY]\t" << category << endl;
    }

    // Update tainted_registers
    // tuple should be (memLoc, fileName)?
    // TODO make the "0" memLoc
    tainted_registers[regName] = tuple<UINT32, string>(0, insDis);

}

// Used when:
//  1) untainted data written to register
//  2) xor r1, r1
VOID untaint_register(REG reg) {

    tainted_registers[REG_StringShort(reg)] = tuple<UINT32, string>(UNTAINTED, "UNTAINTED");

}

VOID taint_mem2reg(ADDRINT ins, string insDis, string regName, UINT32 mem, CONTEXT* context) 
{
    if (tainted_memory.count(mem) > 0) {
        tainted_registers[regName] = tuple<UINT32, string>(0, ""); // TODO should the 0 be memory-loc or loc in user input?
        TaintRegsFile << insDis << "\t[FLOW]\t" << mem << " to " << regName <<  endl;

    }

}

/* ===================================================================== */
/* Check conditional branch instructions                                 */
/* ===================================================================== */
VOID conditional_branch(ADDRINT ins, INT32 branchTaken, string insDis) {

    CondBranchFile << insDis << endl;
}

/*
    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
    else if ( (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsWritten(ins, 0) ) ) {
    else if (INS_IsLea(ins) ) {
    else if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)) {
    if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0) 
        && INS_OperandIsImmediate(ins, 1) ) { 
*/

/* 
 * Instrument instructions to track taint.
 * If an instruction is not instrumented, record it in unhandled_instructions.out
 */
VOID Instructions(INS ins, VOID *v)
{

    // Conditional Branches
    if (INS_Category(ins) == XED_CATEGORY_COND_BR) { 
        
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(conditional_branch),
                       IARG_INST_PTR,
                       IARG_BRANCH_TAKEN,
                       IARG_PTR, new string(INS_Disassemble(ins)),
                       IARG_END);
    }

    // TODO taint_memory can use INS_IsMemoryWrite(ins)
    // INS_IsStackWrite also could be useful

    // TODO check for taint here instead of in function??
    // TODO figure out why this doesn't work without these 3
    // Handled tainting and untainting of registers
    else if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {

                INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(taint_mem2reg),
                        IARG_INST_PTR,
                        IARG_PTR, new string(INS_Disassemble(ins)),
                        IARG_PTR, new string(REG_StringShort(INS_OperandReg(ins, 0) ) ),
                        //IARG_UINT32, INS_OperandReg(ins, 1),
                        IARG_MEMORYOP_EA, 0,
                        IARG_CONST_CONTEXT,
                        IARG_END);
        }

        /*
        for (REG reg : registers) {
            // Writes to a register and is NOT 
            if (INS_RegWContain(ins, reg) && INS_HasFallThrough(ins) ) {
                INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(taint_register),
                        IARG_INST_PTR,
                        IARG_PTR, new string(INS_Disassemble(ins)),
                        IARG_PTR, new string(REG_StringShort(reg)),
                        IARG_PTR, new string(CATEGORY_StringShort(INS_Category(ins) )),
                        IARG_CONST_CONTEXT,
                        IARG_UINT32, INS_OperandReg(ins, 0),
                        IARG_END);
                //break; // Not sure...but only one register should be written per instruction
            }
        } 
        */
    

    // Put in "unhandle_instruction.out" for debugging
    else {

        UnhandledFile << INS_Disassemble(ins) << endl;

    }

}

// Instructions that modify flags register
//if (INS_Category(ins) == XED_CATEGORY_CMOV) { 
//if (INS_Category(ins) == XED_CATEGORY_FLAGOP ||
//    INS_Category(ins) == XED_CATEGORY_) { 
/*
   else if (INS_HasFallThrough(ins) ) {
   INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(CheckEflags),
   IARG_INST_PTR,
   IARG_PTR, new string(INS_Disassemble(ins)),
   IARG_CONST_CONTEXT,
   IARG_END);
   }
 */



VOID init_stubble()
{
    //ControlFlowFile << hex;
    //ControlFlowFile.setf(ios::showbase);

    CondBranchFile.open(results_dir + "branches.out");
    TaintRegsFile.open(results_dir + "taint_regs.out");
    EflagsFile.open(results_dir + "eflags.out");
    UnhandledFile.open(results_dir + "unhandled_instructions.out");

    INS_AddInstrumentFunction(Instructions, 0);

    // Init libraries
    init_intercept_signals();
    init_syscalls();

}

VOID fini_stubble()
{
    // iterate over DS to check
    //for (string s : tainted_registers) {
    //    cout << s << "\t" << get<0>(tainted_registers[s]) << endl;
    //}

    CondBranchFile.close();
    TaintRegsFile.close();
    EflagsFile.close();
    UnhandledFile.close();

    fini_intercept_signals();
    fini_syscalls();
}

#endif /* __STUBBLE_H__ */

