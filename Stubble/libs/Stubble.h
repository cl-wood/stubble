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
#include <list>
#include <tuple>
#include <unordered_map>
#include <vector>

/* Globals for Stubble */

const string results_dir = "./results/currentrun/";
const int UNTAINTED = -1;
unsigned long INS_COUNT;

// For taint tracking EFLAGS register's carry/overflow/parity/sign/zero flags
enum {
    CF = 1,
    PF = 4,
    ZF = 64,
    SF = 128,
    OF = 2048
};

// Tainted registers and memory
unordered_map<string, tuple<UINT32, string> > tainted_registers; // TODO make registers an enumerated array of tuple(byteLocation, fileName), if untainted byteLocation = -1
extern unordered_map<ADDRINT, tuple<UINT32, string> > tainted_memory;
ADDRINT eflags_value;

// Set of all registers and one for each funct that needs to manipulate regs
REGSET instruction_registers; 

// Files for recording various information
ofstream CondBranchFile;
ofstream TaintFile;
ofstream EflagsFile;
ofstream UnhandledFile;
ofstream NotUsedOpsFile;

/* 
 * Example: EAX is tainted...AX, AH, and AL should also be tainted
 * Use this whenver tainting or untainting registers
 *
 */
list<REG> add_partial_registers(REG reg)
{
    list<REG> regs;
    
    switch(reg){

        // TODO can we mov to just AH, thus making AH-> AH and AL not true?
        case REG_EAX:  regs.push_front(REG_EAX); 
        case REG_AX:   regs.push_front(REG_AX); 
        case REG_AH:   regs.push_front(REG_AH); 
        case REG_AL:   regs.push_front(REG_AL); 
                       break;

        case REG_EBX:  regs.push_front(REG_EBX);
        case REG_BX:   regs.push_front(REG_BX);
        case REG_BH:   regs.push_front(REG_BH);
        case REG_BL:   regs.push_front(REG_BL);
                       break;

        case REG_ECX:  regs.push_front(REG_ECX);
        case REG_CX:   regs.push_front(REG_CX);
        case REG_CH:   regs.push_front(REG_CH);
        case REG_CL:   regs.push_front(REG_CL);
                       break;

        case REG_EDX:  regs.push_front(REG_EDX); 
        case REG_DX:   regs.push_front(REG_DX); 
        case REG_DH:   regs.push_front(REG_DH); 
        case REG_DL:   regs.push_front(REG_DL); 
                       break;

        case REG_EDI:  regs.push_front(REG_EDI); 
        case REG_DI:   regs.push_front(REG_DI); 
                       break;

        case REG_ESI:  regs.push_front(REG_ESI); 
        case REG_SI:   regs.push_front(REG_SI); 
                       break;

        default:
                       break;
    }

    return regs;

} // End add_partial_registers

/* Removes taint from a register and all of its partial registers
 *
 * ARGS-    REG:   register to remove taint from
 */
VOID untaint_register(REG reg, string insDis)
{
    // Create a list containing the register and any of its partial registers
    list<REG> regs = add_partial_registers(reg);

    // For the register and its partial registers, remove taint
    for (list<REG>::iterator it=regs.begin(); it != regs.end(); ++it) {

        string regString = REG_StringShort(*it);
        UINT32 userInputByte = get<0>(tainted_registers[regString]);

        tainted_registers.erase(regString);
        TaintFile << INS_COUNT << ":" << userInputByte << "\t" 
                  << "UNTAINT REG\t" << regString << "\t" << insDis << endl;
    }

} // end untaint_register

/* Taints a register and all of its partial registers. TODO should AL tainted -> EAX tainted? Do we taint parent regs?
 *
 * ARGS- REG:       register to be tainted
 *       string:    source of taint
 *       string:    whether source of taint is a register or memory location
 */
VOID taint_register(REG reg, string source, string source_type, string insDis)
{
    // Create a set containing the register and any of its partial registers
    list<REG> regs = add_partial_registers(reg);
    UINT32 userInputByte;

    // Decide where to get taint from: need to get cast source to ADDRINT for tainted_memory
    if (source_type == "REGISTER") {
        userInputByte = get<0>(tainted_registers[source]);
    } else if (source_type == "MEMORY") {
        ADDRINT mem_addr = AddrintFromString(source);
        userInputByte = get<0>(tainted_memory[mem_addr]);
    }

    // For the register and its partial registers, propagate taint
    for (list<REG>::iterator it=regs.begin(); it != regs.end(); ++it) {
        string regString = REG_StringShort(*it);

        tainted_registers[regString] = tuple<UINT32, string>(userInputByte, "");
        TaintFile << INS_COUNT << ":" << userInputByte << "\t"
                  << "TAINT REG\t" << regString << "\t" << insDis << endl;
    }

} // end taint_register

/* Given a register
 *
 */
BOOL reg_is_tainted(REG reg)
{
//        if (tainted_registers.count() > 0) {

    return false;

}

// Remove memory from  tainted_memory
/* Removes taint from memory.
 *
 * ARGS-    
 */
VOID untaint_memory(string source, ADDRINT sink, string insDis)
{
    //ADDRINT mem_addr = AddrintFromString(source);
    UINT32 userInputByte = get<0>(tainted_registers[source]);

    TaintFile << INS_COUNT << ":" << userInputByte << "\t"
              << "UNTAINT MEM\t" << StringFromAddrint(sink) << "\t" << insDis << endl;
    tainted_memory.erase(sink);

}

/* Adds taint to memory.
 * NB: Memory cannot be tainted by other memory or immediate values, so source must be a register.
 *
 * ARGS-
 *
 */
VOID taint_memory(string source, ADDRINT sink, string insDis)
{
    // Get memory address from string and
    //ADDRINT mem_addr = AddrintFromString(sink);
    UINT32 userInputByte = get<0>(tainted_registers[source]);

    tainted_memory[sink] = tuple<UINT32, string>(userInputByte, insDis);
    TaintFile << INS_COUNT << ":" << userInputByte << "\t"
    << "TAINT MEM\t" << StringFromAddrint(sink) << "\t" << insDis << endl;

}

// For register read and register written, create set with those registers and all of their partial registers
// Case 1) If reg_r is tainted -> reg_w is now tainted.
// Case 2) If reg_r is not tainted and reg_w is tainted -> reg_w is now NOT tainted.
VOID reg2reg(UINT32 insAddr, string insDis, REG reg_r, REG reg_w)
{
    // For read and write registers. Add register and its partial registers to regsets
    list<REG> regs_r = add_partial_registers(reg_r);
    //list<REG> regs_w = add_partial_registers(reg_w);

    bool reg_r_tainted = false;

    // Any tainted registers in regset_r -> taint propagates to all equivalent registers in regset_w
    for (list<REG>::iterator it=regs_r.begin(); it != regs_r.end(); ++it) {
        string regString = REG_StringShort(*it);
        if (tainted_registers.count(regString) > 0) {
            reg_r_tainted = true;

            // TODO between registers is complicated
            taint_register(reg_w, regString, "REGISTER", insDis);

            break;
        }
    }

    // If reg_r is NOT tainted and reg_w IS tainted, remove taint from reg_w
    if (!reg_r_tainted) {
        untaint_register(reg_w, insDis);
    }

} // end reg2reg


VOID reg2mem(UINT32 insAddr, std::string insDis, REG reg_r, UINT32 memOp, UINT32 sp)
{

    string regString = REG_StringShort(reg_r);

    if (tainted_memory.count(memOp) > 0) {
        if (!REG_valid(reg_r) || tainted_registers.count(regString) == 0) {
            untaint_memory(regString, memOp, insDis);
        }
    }
    else { // memory not tainted, register is tainted
        taint_memory(regString, memOp, insDis);
    } 

}

VOID ReadMem(UINT32 insAddr, string insDis, REG reg_r, UINT32 memOp, UINT32 sp)
{
    string regString = REG_StringShort(reg_r);
    // Mem tainted, so taint propagates to register
    if (tainted_memory.count(memOp) > 0) {
        taint_register(reg_r, StringFromAddrint(memOp), "MEMORY", insDis);
    }

    // Mem ~tainted, but register is, so remove taint
    else if (tainted_registers.count(regString) > 0) {
        untaint_register(reg_r, insDis);   
    }

}

VOID followData(UINT32 insAddr, string insDis, REG reg)
{
    if (!REG_valid(reg)) {
        return;
    }

    string regString = REG_StringShort(reg);
    if (tainted_registers.count(regString) > 0) {
        TaintFile << INS_COUNT << ":" << get<0>(tainted_registers[regString]) << "\t"
        << "[FOLLOW]\t\t" << insDis << endl;
    }
}


// TODO these two might need to be expanded to handle when it isn't "cmp reg, imm"
VOID save_eflags(INS ins, string insDis, REG reg, CONTEXT* context)
{

    eflags_value = PIN_GetContextReg(context, REG_EFLAGS);
    EflagsFile << eflags_value << "\t" << insDis << endl;

}

VOID diff_eflags(INS ins, string insDis, REG reg, CONTEXT* context)
{
    string regString = REG_StringShort(reg);

    if (tainted_registers.count(regString) > 0) 
    {
        ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
        if (eflags && ZF != eflags_value && ZF) 
        {
            UINT32 userInputByte = get<0>(tainted_registers[regString]);
            EflagsFile << eflags << "\t" << insDis << endl;
            TaintFile << INS_COUNT << ":" << userInputByte << "\t"
                      << "TAINT ZF\t" << insDis << endl;
        }
    }

}

/*
// Special case since cmp instructions and the like modify flags for branch instructions
// Handles taint tracking for EFLAGS register's carry/overflow/parity/sign/zero flags
VOID save_eflags_2regs(INS ins, string insDis, string reg1, string reg2, CONTEXT* context)
{

if (tainted_registers.count(reg1) > 0 || tainted_registers.count(reg2) > 0) {
ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
EflagsFile << eflags << "\t" << insDis << endl;
}

}

VOID handle_eflags_2regs(INS ins, string insDis, string reg1, string reg2, CONTEXT* context)
{

if (tainted_registers.count(reg1) > 0 || tainted_registers.count(reg2) > 0) {
ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
EflagsFile << eflags << "\t" << insDis << endl;
}

}

VOID save_eflags_reg_imm(INS ins, string insDis, string reg1, UINT32 imm, CONTEXT* context)
{

if (tainted_registers.count(reg1) > 0) {
ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
EflagsFile << eflags << "\t" << insDis << "\t" << imm << endl;
}

}

VOID handle_eflags_reg_imm(INS ins, string insDis, string reg1, UINT32 imm, CONTEXT* context)
{

if (tainted_registers.count(reg1) > 0) {
ADDRINT eflags = PIN_GetContextReg(context, REG_EFLAGS);
EflagsFile << eflags << "\t" << insDis << "\t" << imm << endl;
}

}
 */


/* ===================================================================== */
/* Check conditional branch instructions                                 */
/* ===================================================================== */
VOID conditional_branch(ADDRINT ins, INT32 branchTaken, string insDis) 
{

    CondBranchFile << insDis << endl;

}

/* 
 * Instrument instructions to track taint.
 * If an instruction is not instrumented, record it in unhandled_instructions.out
 */
VOID Instructions(INS ins, VOID *v)
{
    // Handle instructions which write EFLAGS register
    // Where this goes is up in the air atm, but it should be independent of other callbacks and probably after the instruction executed
    if (INS_RegWContain(ins, REG_EFLAGS) && INS_HasFallThrough(ins) && INS_OperandIsReg(ins, 0) ) {

        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)save_eflags,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_CONST_CONTEXT,
                IARG_END);

        INS_InsertCall(
                ins, IPOINT_AFTER, (AFUNPTR)diff_eflags,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_CONST_CONTEXT,
                IARG_END);
    }

    // Handle tainted conditional branches
    if (INS_Category(ins) == XED_CATEGORY_COND_BR) { 

/*
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)conditional_branch,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandReg(ins, 0),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);
        */

    }

    // Operations with 0 or 1 operands
    if (INS_OperandCount(ins) == 0 || INS_OperandCount(ins) == 1) {
        NotUsedOpsFile << INS_Disassemble(ins) << endl;
    }

    // TODO also need to handle immediates being op1 and op0 being written by it to untaint?

    // Taint can spread in 3 ways:
    //  Case 1) op reg, mem 
    else if (INS_MemoryOperandIsRead(ins, 0) && INS_OperandIsReg(ins, 0)) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)ReadMem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandReg(ins, 0),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);

    }
    //  Case 2) op mem, reg 
    else if (INS_MemoryOperandIsWritten(ins, 0) ) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)reg2mem,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_OperandReg(ins, 1),
                IARG_MEMORYOP_EA, 0,
                IARG_REG_VALUE, REG_STACK_PTR,
                IARG_END);

    }

    //  Case 3) op reg, reg 
    else if (INS_OperandIsReg(ins, 0) ) {
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)reg2reg,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_UINT32, INS_RegW(ins, 0),
                IARG_END);
    }

    if (INS_OperandCount(ins) > 1 && INS_OperandIsReg(ins, 0)){
        INS_InsertCall(
                ins, IPOINT_BEFORE, (AFUNPTR)followData,
                IARG_ADDRINT, INS_Address(ins),
                IARG_PTR, new string(INS_Disassemble(ins)),
                IARG_UINT32, INS_RegR(ins, 0),
                IARG_END);
    }


    // Put in "unhandle_instruction.out" for debugging
    else {
        UnhandledFile << INS_Disassemble(ins) << endl;
    }

    INS_COUNT++;

} // End Instructions


VOID init_stubble()
{
    //ControlFlowFile << hex;
    //ControlFlowFile.setf(ios::showbase);

    CondBranchFile.open(results_dir + "branches.out");
    TaintFile.open(results_dir + "taint.out");
    EflagsFile.open(results_dir + "eflags.out");
    UnhandledFile.open(results_dir + "unhandled_instructions.out");
    NotUsedOpsFile.open(results_dir + "not_used_ops.out");

    INS_AddInstrumentFunction(Instructions, 0);


    // Init libraries
    init_intercept_signals();
    init_syscalls();

    /*
       REGSET_Clear(instruction_registers);
       REGSET_Insert(instruction_registers, REG_EAX);
       while (!REGSET_PopCountIsZero(instruction_registers) ) {
       REG reg = REGSET_PopNext(instruction_registers); 
       REGSET_Remove(instruction_registers, reg);
       string r = REG_StringShort(reg); 

       cout << r << endl;
       }
     */

    INS_COUNT = 0;

}

VOID fini_stubble()
{

    CondBranchFile.close();
    TaintFile.close();
    EflagsFile.close();
    UnhandledFile.close();
    NotUsedOpsFile.close();

    fini_intercept_signals();
    fini_syscalls();
}

#endif /* __STUBBLE_H__ */

