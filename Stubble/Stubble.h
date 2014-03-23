// Stubble.h
// http://en.wikibooks.org/wiki/X86_Assembly/Control_Flow and the book in general is very helpful

#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <list>
#include <unordered_map>
#include <xed-flag-enum.h>

int insCount = 0;
list<UINT32> addressTainted;
list<REG> regsTainted;

UINT32 EFLAGS;
//INS last_instruction;
//BOOL taintedCmp = false;

// File for recording taint propagation
ofstream CondBranchFile;
ofstream FlagsFile;

// CATEGORIES and ICLASSes to consider
//XED_CATEGORY_CMOV all cond. move instructions

/* ===================================================================== */
/* Check instructions that modify EFLAGS                                 */
/* ===================================================================== */
// NOTE: this catches the initial change from EFLAGS is 0 to a valid EFLAGS, is it always 514?
VOID ModifiesEflags(ADDRINT ins, string insDis, CONTEXT* context) {

    UINT32 tmp_flags = PIN_GetContextReg(context, REG_EFLAGS);

    //if (EFLAGS != tmp_flags && tmp_flags & XED_FLAG_df) {
    if (EFLAGS != tmp_flags) {

        // overflow, sign, zero, carry, and parity flags

        if (tmp_flags & XED_FLAG_of) {
            FlagsFile << insDis << ":\t" << PIN_GetContextReg(context, REG_EFLAGS) << endl;
        }

        EFLAGS = tmp_flags;
    }
}

/* ===================================================================== */
/* Check conditional branch instructions                                 */
/* ===================================================================== */
VOID ConditionalBranch(ADDRINT ins, INT32 branchTaken, string insDis) {

    CondBranchFile << insDis << endl;
}

/*
    // TODO PREDICATE_ZERO to get ZF == 0 or cases for all jumps and backsolve
    //if (INS_Opcode(ins) == XED_ICLASS_JNZ) {
        
    //}
    if (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
    else if ( (INS_OperandCount(ins) > 1 && INS_MemoryOperandIsWritten(ins, 0) ) ) {
    else if (INS_IsLea(ins) ) {
    else if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)) {
    if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0) 
        && INS_OperandIsImmediate(ins, 1) ) { 
*/

VOID Instructions(INS ins, VOID *v)
{

    // Conditional Branches
    if (INS_Category(ins) == XED_CATEGORY_COND_BR) { 
        
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(ConditionalBranch),
                       IARG_INST_PTR,
                       IARG_BRANCH_TAKEN,
                       IARG_PTR, new string(INS_Disassemble(ins)),
                       IARG_END);
    }

    // Instructions that modify flags register
    //if (INS_Category(ins) == XED_CATEGORY_CMOV) { 
    //if (INS_Category(ins) == XED_CATEGORY_FLAGOP ||
    //    INS_Category(ins) == XED_CATEGORY_) { 
    else if (INS_HasFallThrough(ins) ) {
        INS_InsertCall(ins, IPOINT_AFTER, AFUNPTR(ModifiesEflags),
                       IARG_INST_PTR,
                       IARG_PTR, new string(INS_Disassemble(ins)),
                        IARG_CONST_CONTEXT,
                       IARG_END);
    }

}

VOID InitStubble()
{
    //ControlFlowFile.open("controlpath.out");
    //ControlFlowFile << hex;
    //ControlFlowFile.setf(ios::showbase);

    // Initialize EFLAGS
    EFLAGS = 0;

    CondBranchFile.open("branches.out");
    FlagsFile.open("flags.out");

    INS_AddInstrumentFunction(Instructions, 0);
    //PIN_AddSyscallEntryFunction(Syscall_entry, 0);

}

VOID FiniStubble()
{
    CondBranchFile.close();
    FlagsFile.close();
}

// End Stubble.h

